/* src/tracking.cc
 *
 * Copyright 2018 Brandon Gomes
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "tracking.hh"

#include <iomanip>

#include <Geant4/G4SDManager.hh>
#include <Geant4/G4RunManager.hh>
#include <Geant4/tls.hh>

#include "physics/Units.hh"
#include "ui.hh"

namespace MATHUSLA { namespace MU {

namespace Tracking { ///////////////////////////////////////////////////////////////////////////

//__Hit Allocator_______________________________________________________________________________
G4ThreadLocal G4Allocator<Hit>* HitAllocator = nullptr;
//----------------------------------------------------------------------------------------------

//__Hit Constructor_____________________________________________________________________________
Hit::Hit(const G4ParticleDefinition* particle,
         const int track,
         const int parent,
         const std::string& chamber,
         const double deposit,
         const G4LorentzVector position,
         const G4LorentzVector momentum)
    : G4VHit(), _particle(particle), _trackID(track), _parentID(parent),
      _chamberID(chamber), _deposit(deposit), _position(position),
      _momentum(momentum) {}
//----------------------------------------------------------------------------------------------

//__Hit Constructor_____________________________________________________________________________
Hit::Hit(const G4Step* step,
         bool post) {
  if (!step) return;

  const auto track = step->GetTrack();
  const auto step_point = post ? step->GetPostStepPoint()
                               : step->GetPreStepPoint();

  _particle  = track->GetParticleDefinition();
  _trackID   = track->GetTrackID();
  _parentID  = track->GetParentID();
  _chamberID = track->GetTouchable()->GetHistory()->GetTopVolume()->GetName();
  _deposit   = step->GetTotalEnergyDeposit()                / Units::Energy;
  _position  = G4LorentzVector(step_point->GetGlobalTime()  / Units::Time,
                               step_point->GetPosition()    / Units::Length);
  _momentum  = G4LorentzVector(step_point->GetTotalEnergy() / Units::Energy,
                               step_point->GetMomentum()    / Units::Momentum);
}
//----------------------------------------------------------------------------------------------

//__Draw Hit in World___________________________________________________________________________
void Hit::Draw() {
  Vis::Draw(Vis::Circle(_position.vect() * Units::Length, 4, G4Color::White()));
}
//----------------------------------------------------------------------------------------------

//__Print Hit Data______________________________________________________________________________
void Hit::Print(std::ostream& os) const {
  constexpr static auto WIDTH = 10;
  constexpr static auto DECIMAL_PLACES = 4;
  os.precision(DECIMAL_PLACES);
  os << " "            << GetParticleName()
     << " | "          << _trackID
     << " | "          << _parentID
     << " | "          << _chamberID
     << " | Deposit: " << std::setw(WIDTH) << G4BestUnit(_deposit * Units::Energy, "Energy")
     << " | ["
      << std::setw(WIDTH) << G4BestUnit(_position.t() * Units::Time, "Time") << " "
      << std::setw(WIDTH) << G4BestUnit(_position.x() * Units::Length, "Length")
      << std::setw(WIDTH) << G4BestUnit(_position.y() * Units::Length, "Length")
      << std::setw(WIDTH) << G4BestUnit(_position.z() * Units::Length, "Length")
    << "] | ["
      << std::setw(WIDTH) << G4BestUnit(_momentum.e()  * Units::Energy,  "Energy")
      << std::setw(WIDTH) << G4BestUnit(_momentum.px() * Units::Momentum, "Momentum")
      << std::setw(WIDTH) << G4BestUnit(_momentum.py() * Units::Momentum, "Momentum")
      << std::setw(WIDTH) << G4BestUnit(_momentum.pz() * Units::Momentum, "Momentum")
    << " ]"
    << "\n";
}
//----------------------------------------------------------------------------------------------

//__Print Hit Data______________________________________________________________________________
void Hit::Print() {
  Print(std::cout);
}
//----------------------------------------------------------------------------------------------

//__Stream Hit__________________________________________________________________________________
std::ostream& operator<<(std::ostream& os,
                         const Hit& hit) {
  hit.Print(os);
  return os;
}
//----------------------------------------------------------------------------------------------

//__Stream Hit Collection_______________________________________________________________________
std::ostream& operator<<(std::ostream& os,
                         const HitCollection& hits) {
  const auto event_id = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();
  const auto count = hits.entries();
  if (!count)
    return os;

  const auto boxside = std::string(25 + std::to_string(event_id).length()
                                      + std::to_string(count).length(), '-');

  const auto box = boxside
                 + "\n| Event: "     + std::to_string(event_id)
                 +  " | Hit Count: " + std::to_string(count)
                 +  " |\n" + boxside + '\n';

  os << "\n\n" << box;

  auto trackID = -1;
  for (auto i = 0; i < count; ++i) {
    const auto hit = dynamic_cast<Tracking::Hit*>(hits.GetHit(i));
    const auto new_trackID = hit->GetTrackID();

    if (i != 0 && trackID != new_trackID) {
      const auto barlength = 162
        + hit->GetParticleName().length()
        + std::to_string(new_trackID).length()
        + std::to_string(hit->GetParentID()).length()
        + hit->GetChamberID().length();
      os << std::string(barlength, '-') << '\n';
    }

    trackID = new_trackID;

    os << *hit;
  }
  return os << '\n';
}
//----------------------------------------------------------------------------------------------

//__Add Hit Collection to Detector______________________________________________________________
HitCollection* GenerateHitCollection(G4VSensitiveDetector* detector,
                                     G4HCofThisEvent* event) {
  const auto& collection_name = detector->GetCollectionName(0);
  auto out = new HitCollection(detector->GetName(), collection_name);
  event->AddHitsCollection(G4SDManager::GetSDMpointer()->GetCollectionID(collection_name), out);
  return out;
}
//----------------------------------------------------------------------------------------------

namespace { ////////////////////////////////////////////////////////////////////////////////////

//__Convert HitCollection to Analysis Form______________________________________________________
template<class NameMap>
const Analysis::ROOT::DataEntryList _convert_to_analysis(const HitCollection* collection,
                                                         NameMap name_map) {
  constexpr const std::size_t column_count = 13UL;

  Analysis::ROOT::DataEntryList out;
  out.reserve(column_count);

  const auto size = collection->GetSize();
  for (std::size_t i = 0; i < column_count; ++i) {
    Analysis::ROOT::DataEntry entry;
    entry.reserve(size);
    out.push_back(entry);
  }

  for (std::size_t i = 0; i < size; ++i) {
    const auto hit = dynamic_cast<Hit*>(collection->GetHit(i));
    out[0].push_back(hit->GetPDGEncoding());
    out[1].push_back(hit->GetTrackID());
    out[2].push_back(hit->GetParentID());
    out[3].push_back(name_map(hit->GetChamberID()));
    out[4].push_back(hit->GetDeposit());
    out[5].push_back(hit->GetPosition().t());
    out[6].push_back(hit->GetPosition().x());
    out[7].push_back(hit->GetPosition().y());
    out[8].push_back(hit->GetPosition().z());
    out[9].push_back(hit->GetMomentum().e());
    out[10].push_back(hit->GetMomentum().px());
    out[11].push_back(hit->GetMomentum().py());
    out[12].push_back(hit->GetMomentum().pz());
  }

  return out;
}
//----------------------------------------------------------------------------------------------

} /* anonymous namespace */ ////////////////////////////////////////////////////////////////////

//__Convert HitCollection to Analysis Form______________________________________________________
const Analysis::ROOT::DataEntryList ConvertToAnalysis(const HitCollection* collection) {
  return _convert_to_analysis(collection, [](const auto& id) { return std::stold(id); });
}
//----------------------------------------------------------------------------------------------

//__Convert HitCollection to Analysis Form______________________________________________________
const Analysis::ROOT::DataEntryList ConvertToAnalysis(const HitCollection* collection,
                                                      const Analysis::ROOT::NameToDataMap& map) {
  const auto map_end = map.cend();
  return _convert_to_analysis(collection, [&](const auto& id) {
    const auto search = map.find(id);
    return search != map_end ? search->second : -1;
  });
}
//----------------------------------------------------------------------------------------------

//__Convert G4Event to Analysis Form____________________________________________________________
const Analysis::ROOT::DataEntryList ConvertToAnalysis(const G4Event* event) {
  constexpr const std::size_t column_count = 11UL;

  Analysis::ROOT::DataEntryList out;
  out.reserve(column_count);

  std::size_t size{};
  const auto vertex_count = event->GetNumberOfPrimaryVertex();
  for (auto i = 0; i < vertex_count; ++i)
    size += event->GetPrimaryVertex(i)->GetNumberOfParticle();

  for (std::size_t i{}; i < column_count; ++i) {
    Analysis::ROOT::DataEntry entry;
    entry.reserve(size);
    out.push_back(entry);
  }

  for (auto i = 0; i < vertex_count; ++i) {
    const auto vertex = event->GetPrimaryVertex(i);
    const auto vertex_size = vertex->GetNumberOfParticle();
    for (auto j = 0; j < vertex_size; ++j) {
      const auto primary = vertex->GetPrimary(j);

      out[0].push_back(primary->GetPDGcode());
      out[1].push_back(primary->GetTrackID());
      out[2].push_back(0);

      out[3].push_back(vertex->GetT0() / Units::Time);
      out[4].push_back(vertex->GetX0() / Units::Length);
      out[5].push_back(vertex->GetY0() / Units::Length);
      out[6].push_back(vertex->GetZ0() / Units::Length);
      out[7].push_back(primary->GetTotalEnergy() / Units::Energy);

      const auto momentum = primary->GetMomentum();
      out[8].push_back(momentum.x() / Units::Momentum);
      out[9].push_back(momentum.y() / Units::Momentum);
      out[10].push_back(momentum.z() / Units::Momentum);

    }
  }

  return out;
}
//----------------------------------------------------------------------------------------------

} /* namespace Tracking */ /////////////////////////////////////////////////////////////////////

} } /* namespace MATHUSLA::MU */
