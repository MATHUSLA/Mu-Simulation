#include "geometry/Box.hh"

#include <Geant4/G4SubtractionSolid.hh>

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
constexpr auto edge_length  = 200*m;
constexpr auto displacement = 100*m;

constexpr auto steel_height = 3*cm;

constexpr auto air_gap = 20*m;

constexpr auto scintillator_x_width 	     = 0.25*m;
constexpr auto scintillator_y_width 	     = 0.25*m;
constexpr auto scintillator_height           = 1*cm;
constexpr auto scintillator_casing_thickness = 0.1*cm;

constexpr auto layer_spacing = 1.5*m;
constexpr auto layer_count   = 3;

constexpr auto full_detector_height = steel_height + layer_count * (layer_spacing + scintillator_height);
constexpr auto half_detector_height = 0.5 * full_detector_height;
//----------------------------------------------------------------------------------------------

} /* anonymous namespace */ ////////////////////////////////////////////////////////////////////

//__Box Data Variables__________________________________________________________________________
const std::string& Detector::DataPrefix = "event";
const std::vector<std::string>& Detector::DataKeys = {
  "Deposit", "Time", "Detector",
  "PDG", "Track", "X", "Y", "Z", "E", "PX", "PY", "PZ"};
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

  const auto local_position = position.vect() - G4ThreeVector(displacement, -0.5 * edge_length, steel_height);

  const auto layer_z = local_position.z() + steel_height + layer_count * scintillator_height - layer_spacing;
  const auto x_index = static_cast<size_t>(std::ceil(local_position.x() / scintillator_x_width));
  const auto y_index = static_cast<size_t>(std::ceil(local_position.y() / scintillator_y_width));
  const auto z_index = static_cast<size_t>(std::abs(std::ceil(layer_z / layer_spacing)));

  const auto x_name = std::to_string(x_index);
  const auto x_fullname = x_index < 10 ? "00" + x_name : (x_index < 100 ? "0" + x_name : x_name);
  const auto y_name = std::to_string(y_index);
  const auto y_fullname = y_index < 10 ? "00" + y_name : (y_index < 100 ? "0" + y_name : y_name);
  const auto name = std::to_string(1 + z_index) + x_fullname + y_fullname;

  _hit_collection->insert(new Tracking::Hit(
    particle->GetParticleName(), trackID, parentID, name, deposit, position, momentum));

  Analysis::FillNTuple(DataPrefix, EventAction::EventID(), {
    deposit       / Units::Energy,
    position.t()  / Units::Time,
    std::stod(name),
    static_cast<double>(particle->GetPDGEncoding()),
    static_cast<double>(trackID),
    position.x() / Units::Length,
    position.y() / Units::Length,
    position.z() / Units::Length,
    momentum.t() / Units::Energy,
    momentum.x() / Units::Momentum,
    momentum.y() / Units::Momentum,
    momentum.z() / Units::Momentum});

  return true;
}
//----------------------------------------------------------------------------------------------

//__Post-Event Processing_______________________________________________________________________
void Detector::EndOfEvent(G4HCofThisEvent*) {
  if (verboseLevel >= 2 && _hit_collection)
    std::cout << *_hit_collection;
}
//----------------------------------------------------------------------------------------------

//__Build Detector______________________________________________________________________________
G4VPhysicalVolume* Detector::Construct(G4LogicalVolume* world) {
  Scintillator::Material::Define();
  _scintillators.clear();

  auto DetectorVolume = Construction::BoxVolume("Box", edge_length, edge_length, full_detector_height);

  for (size_t layer = 0; layer < layer_count; ++layer) {
    auto current = new Scintillator("S" + std::to_string(1+layer),
      edge_length,
      edge_length,
      scintillator_height,
      scintillator_casing_thickness);
    current->PlaceIn(DetectorVolume, Construction::Transform(
      0,
      0,
      half_detector_height
        - steel_height
        - 0.5*scintillator_height
        - layer * (scintillator_height + layer_spacing)));
    _scintillators.push_back(current);
  }

  _steel = Construction::BoxVolume("SteelPlate",
    edge_length, edge_length, steel_height,
    Construction::Material::Iron,
    Construction::CasingAttributes());
  Construction::PlaceVolume(_steel, DetectorVolume, Construction::Transform(0, 0, half_detector_height-0.5*steel_height));
  return Construction::PlaceVolume(DetectorVolume, world,
    G4Translate3D(0.5*edge_length + displacement, 0, -half_detector_height + steel_height));
}
//----------------------------------------------------------------------------------------------

//__Build Earth for Detector____________________________________________________________________
G4VPhysicalVolume* Detector::ConstructEarth(G4LogicalVolume* world) {
  Earth::Material::Define();

  auto earth = Earth::Volume();
  auto modified = Construction::Volume(new G4SubtractionSolid("ModifiedSandstone",
    Earth::SandstoneVolume()->GetSolid(),
    Construction::Box("AirBox", edge_length, edge_length, air_gap),
    Construction::Transform(0.5L*edge_length + displacement, 0, 0.5L*(air_gap-Earth::SandstoneDepth))));

  Construction::PlaceVolume(modified, earth, Earth::SandstoneTransform());
  Construction::PlaceVolume(Earth::MarlVolume(), earth, Earth::MarlTransform());
  Construction::PlaceVolume(Earth::MixVolume(), earth, Earth::MixTransform());
  return Construction::PlaceVolume(earth, world, Earth::Transform());
}
//----------------------------------------------------------------------------------------------

} /* namespace Box */ //////////////////////////////////////////////////////////////////////////

} } /* namespace MATHUSLA::MU */
