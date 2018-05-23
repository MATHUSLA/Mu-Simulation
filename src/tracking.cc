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

#include "Geant4/G4SDManager.hh"
#include "Geant4/G4RunManager.hh"

#include "physics/Units.hh"
#include "ui.hh"

namespace MATHUSLA { namespace MU {

namespace Tracking { ///////////////////////////////////////////////////////////////////////////

//__Hit Allocator_______________________________________________________________________________
G4ThreadLocal G4Allocator<Hit>* HitAllocator = nullptr;
//----------------------------------------------------------------------------------------------

//__Hit Constructor_____________________________________________________________________________
Hit::Hit(const std::string& particle,
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
Hit::Hit(const G4Step* step) {
  if (!step) return;

  const auto track     = step->GetTrack();
  const auto post_step = step->GetPostStepPoint();

  _particle  = track->GetParticleDefinition()->GetParticleName();
  _trackID   = track->GetTrackID();
  _parentID  = track->GetParentID();
  _chamberID = track->GetTouchable()->GetHistory()->GetTopVolume()->GetName();
  _deposit   = step->GetTotalEnergyDeposit();
  _position  = G4LorentzVector(post_step->GetGlobalTime(), post_step->GetPosition());
  _momentum  = G4LorentzVector(post_step->GetTotalEnergy(), post_step->GetMomentum());
}
//----------------------------------------------------------------------------------------------

//__Draw Hit in World___________________________________________________________________________
void Hit::Draw() {
  Vis::Draw(Vis::Circle(_position.vect(), 4, G4Color::White()));
}
//----------------------------------------------------------------------------------------------

//__Print Hit Data______________________________________________________________________________
void Hit::Print(std::ostream& os) const {
  constexpr static auto WIDTH = 10;
  constexpr static auto DECIMAL_PLACES = 4;
  os.precision(DECIMAL_PLACES);
  os << " "            << _particle
     << " | "          << _trackID
     << " | "          << _parentID
     << " | "          << _chamberID
     << " | Deposit: " << std::setw(WIDTH) << G4BestUnit(_deposit, "Energy")
     << " | ["
      << std::setw(WIDTH) << G4BestUnit(_position.t(), "Time") << " "
      << std::setw(WIDTH) << G4BestUnit(_position.x(), "Length")
      << std::setw(WIDTH) << G4BestUnit(_position.y(), "Length")
      << std::setw(WIDTH) << G4BestUnit(_position.z(), "Length")
    << "] | ["
      << std::setw(WIDTH) << G4BestUnit(_momentum.e(),  "Energy")
      << std::setw(WIDTH) << G4BestUnit(_momentum.px(), "Momentum")
      << std::setw(WIDTH) << G4BestUnit(_momentum.py(), "Momentum")
      << std::setw(WIDTH) << G4BestUnit(_momentum.pz(), "Momentum")
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
  const auto&& event_id = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();
  const auto&& count = hits.entries();
  if (!count) return os;

  const auto boxside = std::string(25 + std::to_string(event_id).length()
                                      + std::to_string(count).length(), '-');

  const auto box = boxside
                 + "\n| Event: "     + std::to_string(event_id)
                 +  " | Hit Count: " + std::to_string(count)
                 +  " |\n" + boxside + '\n';

  os << "\n\n" << box;

  auto trackID = -1;
  std::string chamberID;
  for (auto i = 0; i < count; ++i) {
    const auto hit = dynamic_cast<Tracking::Hit*>(hits.GetHit(i));
    const auto new_chamberID = hit->GetChamberID();
    const auto new_trackID   = hit->GetTrackID();

    if (i != 0 && trackID != new_trackID) {
      const auto barlength = 162
        + hit->GetParticleName().length()
        + std::to_string(new_trackID).length()
        + std::to_string(hit->GetParentID()).length()
        + new_chamberID.length();
      os << std::string(barlength, '-') << '\n';
    }

    chamberID = new_chamberID;
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

} /* namespace Tracking */ /////////////////////////////////////////////////////////////////////

} } /* namespace MATHUSLA::MU */
