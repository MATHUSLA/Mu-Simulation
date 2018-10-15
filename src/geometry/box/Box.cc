#include "geometry/Box.hh"

#include <Geant4/G4SubtractionSolid.hh>
#include <Geant4/tls.hh>

#include "action.hh"
#include "analysis.hh"
#include "geometry/Earth.hh"
#include "physics/Units.hh"
#include "tracking.hh"

namespace MATHUSLA { namespace MU {

namespace Box { ////////////////////////////////////////////////////////////////////////////////

namespace { ////////////////////////////////////////////////////////////////////////////////////

//__Box Sensitive Material______________________________________________________________________
std::vector<Scintillator*> _scintillators;
G4LogicalVolume* _steel;
//----------------------------------------------------------------------------------------------

//__Box Hit Collection__________________________________________________________________________
G4ThreadLocal Tracking::HitCollection* _hit_collection;
//----------------------------------------------------------------------------------------------

//__Box Specification Variables_________________________________________________________________
constexpr auto x_edge_length = 100*m;
constexpr auto y_edge_length = 100*m;
constexpr auto x_displacement = 100*m;
constexpr auto y_displacement = -50*m;

constexpr auto steel_height = 3*cm;
constexpr auto air_gap = 25*m;

constexpr auto scintillator_x_width 	     = 0.25*m;
constexpr auto scintillator_y_width 	     = 0.25*m;
constexpr auto scintillator_height           = 1*cm;
constexpr auto scintillator_casing_thickness = 0.1*cm;

constexpr auto layer_spacing = 1.5L*m;
constexpr auto layer_count   = 5UL;

constexpr auto full_detector_height = steel_height + layer_count * (layer_spacing + scintillator_height) - layer_spacing;
constexpr auto half_detector_height = 0.5L * full_detector_height;
//----------------------------------------------------------------------------------------------

} /* anonymous namespace */ ////////////////////////////////////////////////////////////////////

//__Box Data Variables__________________________________________________________________________
const std::string& Detector::DataName = "box_run";
const Analysis::ROOT::DataKeyList Detector::DataKeys = Analysis::ROOT::DefaultDataKeyList;
const Analysis::ROOT::DataKeyTypeList Detector::DataKeyTypes = Analysis::ROOT::DefaultDataKeyTypeList;
//----------------------------------------------------------------------------------------------

//__Detector Constructor________________________________________________________________________
Detector::Detector() : G4VSensitiveDetector("MATHUSLA/MU/Box") {
  collectionName.insert("Box_HC");
  for (auto& scintillator : _scintillators)
    scintillator->Register(this);
}
//----------------------------------------------------------------------------------------------

//__Initalize Event_____________________________________________________________________________
void Detector::Initialize(G4HCofThisEvent* event) {
  _hit_collection = Tracking::GenerateHitCollection(this, event);
}
//----------------------------------------------------------------------------------------------

//__Hit Processing______________________________________________________________________________
G4bool Detector::ProcessHits(G4Step* step, G4TouchableHistory*) {
  const auto deposit = step->GetTotalEnergyDeposit();

  if (deposit == 0.0L)
    return false;

  const auto track      = step->GetTrack();
  const auto step_point = step->GetPostStepPoint();
  const auto particle   = track->GetParticleDefinition();
  const auto trackID    = track->GetTrackID();
  const auto parentID   = track->GetParentID();
  const auto position   = G4LorentzVector(step_point->GetGlobalTime(), step_point->GetPosition());
  const auto momentum   = G4LorentzVector(step_point->GetTotalEnergy(), step_point->GetMomentum());

  const auto local_position = position.vect() - G4ThreeVector(x_displacement, y_displacement, 0);

  const auto x_index = static_cast<size_t>(std::floor(+local_position.x() / scintillator_x_width));
  const auto y_index = static_cast<size_t>(std::floor(+local_position.y() / scintillator_y_width));
  const auto z_index = static_cast<size_t>(std::floor(-local_position.z() / (layer_spacing + scintillator_height)));

  const auto x_name = std::to_string(x_index);
  const auto y_name = std::to_string(y_index);

  _hit_collection->insert(new Tracking::Hit(
    particle,
    trackID,
    parentID,
    std::to_string(1UL + z_index)
      + (x_index < 10UL ? "00" + x_name : (x_index < 100UL ? "0" + x_name : x_name))
      + (y_index < 10UL ? "00" + y_name : (y_index < 100UL ? "0" + y_name : y_name)),
    deposit / Units::Energy,
    G4LorentzVector(position.t() / Units::Time,   position.vect() / Units::Length),
    G4LorentzVector(momentum.e() / Units::Energy, momentum.vect() / Units::Momentum)));

  return true;
}
//----------------------------------------------------------------------------------------------

//__Post-Event Processing_______________________________________________________________________
void Detector::EndOfEvent(G4HCofThisEvent*) {
  if (_hit_collection->GetSize() == 0)
    return;

  const auto collection_data = Tracking::ConvertToAnalysis(_hit_collection);

  Analysis::ROOT::DataEntryList root_data;
  root_data.reserve(24UL);
  root_data.push_back(collection_data[4]);
  root_data.push_back(collection_data[5]);
  root_data.push_back(collection_data[3]);
  root_data.push_back(collection_data[0]);
  root_data.push_back(collection_data[1]);
  root_data.push_back(collection_data[2]);
  root_data.push_back(collection_data[6]);
  root_data.push_back(collection_data[7]);
  root_data.push_back(collection_data[8]);
  root_data.push_back(collection_data[9]);
  root_data.push_back(collection_data[10]);
  root_data.push_back(collection_data[11]);
  root_data.push_back(collection_data[12]);

  const auto gen_particle_data = Tracking::ConvertToAnalysis(EventAction::GetEvent());
  root_data.insert(root_data.cend(), gen_particle_data.cbegin(), gen_particle_data.cend());

  Analysis::ROOT::DataEntry metadata;
  metadata.reserve(2UL);
  metadata.push_back(collection_data[0UL].size());
  metadata.push_back(gen_particle_data[0UL].size());

  Analysis::ROOT::FillNTuple(DataName, Detector::DataKeyTypes, metadata, root_data);
  if (verboseLevel >= 2 && _hit_collection)
    std::cout << *_hit_collection;
}
//----------------------------------------------------------------------------------------------

//__Build Detector______________________________________________________________________________
G4VPhysicalVolume* Detector::Construct(G4LogicalVolume* world) {
  Scintillator::Material::Define();
  _scintillators.clear();

  auto DetectorVolume = Construction::BoxVolume("Box", x_edge_length, y_edge_length, full_detector_height);

  for (std::size_t layer{}; layer < layer_count; ++layer) {
    auto current = new Scintillator("S" + std::to_string(1UL + layer),
      x_edge_length,
      y_edge_length,
      scintillator_height,
      scintillator_casing_thickness);
    current->PlaceIn(DetectorVolume, Construction::Transform(
      0,
      0,
      half_detector_height
        - steel_height
        - 0.5L*scintillator_height
        - layer * (scintillator_height + layer_spacing)));
    _scintillators.push_back(current);
  }

  _steel = Construction::BoxVolume("SteelPlate",
    x_edge_length, y_edge_length, steel_height,
    Construction::Material::Iron,
    Construction::CasingAttributes());
  Construction::PlaceVolume(_steel, DetectorVolume, Construction::Transform(0, 0, half_detector_height-0.5L*steel_height));
  return Construction::PlaceVolume(DetectorVolume, world,
    G4Translate3D(0.5L*x_edge_length + x_displacement, 0.5L*y_edge_length + y_displacement, -half_detector_height + steel_height));
}
//----------------------------------------------------------------------------------------------

//__Build Earth for Detector____________________________________________________________________
G4VPhysicalVolume* Detector::ConstructEarth(G4LogicalVolume* world) {
  Earth::Material::Define();

  auto earth = Earth::Volume();
  auto modified = Construction::Volume(new G4SubtractionSolid("ModifiedSandstone",
    Earth::SandstoneVolume()->GetSolid(),
    Construction::Box("AirBox", x_edge_length, y_edge_length, air_gap),
    Construction::Transform(0.5L*x_edge_length + x_displacement,
                            0.5L*y_edge_length + y_displacement,
                            0.5L*(air_gap-Earth::SandstoneDepth))),
    Earth::Material::SiO2);

  Construction::PlaceVolume(modified, earth, Earth::SandstoneTransform());
  Construction::PlaceVolume(Earth::MarlVolume(), earth, Earth::MarlTransform());
  Construction::PlaceVolume(Earth::MixVolume(), earth, Earth::MixTransform());
  return Construction::PlaceVolume(earth, world, Earth::Transform());
}
//----------------------------------------------------------------------------------------------

} /* namespace Box */ //////////////////////////////////////////////////////////////////////////

} } /* namespace MATHUSLA::MU */
