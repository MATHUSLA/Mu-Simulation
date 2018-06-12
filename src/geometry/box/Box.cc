#include "geometry/Box.hh"

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

} /* anonymous namespace */ ////////////////////////////////////////////////////////////////////

//__Box Data Variables__________________________________________________________________________
const std::string& Detector::DataPrefix = "event";
const std::vector<std::string>& Detector::DataKeys = {
  "Deposit", "Time", "Detector",
  "PDG", "Track", "X", "Y", "Z", "E", "PX", "PY", "PZ", "D_PMT",
  "TimestampDate", "TimestampTime"};
//----------------------------------------------------------------------------------------------

constexpr auto displacement = 100*m;
constexpr auto edge_length = 200*m;
constexpr auto steel_height = 3*cm;
constexpr auto scintillator_height = 1*cm;
constexpr auto scintillator_x_width = 0.25*m;
constexpr auto scintillator_y_width = 0.25*m;
constexpr auto scintillator_casing_thickness = 0.1*cm;
constexpr auto layer_spacing = 1.5*m;
constexpr auto layer_count = 3;
constexpr auto full_detector_height = layer_count * (layer_spacing + scintillator_height) + steel_height;
constexpr auto x_count = edge_length / scintillator_x_width;
constexpr auto y_count = edge_length / scintillator_y_width;

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
  const auto track = step->GetTrack();
  const auto step_point = step->GetPostStepPoint();
  const auto particle  = track->GetParticleDefinition()->GetParticleName();
  const auto trackID   = track->GetTrackID();
  const auto parentID  = track->GetParentID();
  const auto position  = G4LorentzVector(step_point->GetGlobalTime(), step_point->GetPosition());
  const auto momentum  = G4LorentzVector(step_point->GetTotalEnergy(), step_point->GetMomentum());
  const auto deposit = step->GetTotalEnergyDeposit();

  const auto new_x = position.x() - (0.5 * edge_length + displacement) + 0.5 * edge_length;
  const auto new_y = position.y() + 0.5 * edge_length;
  const auto new_z = position.z() + (0.5 * full_detector_height - steel_height) - (0.5 * full_detector_height);
  const auto layer_z = new_z + steel_height + layer_count * scintillator_height - layer_spacing;
  const auto x_index = static_cast<size_t>(std::ceil((new_x / edge_length) * x_count));
  const auto y_index = static_cast<size_t>(std::ceil((new_y / edge_length) * y_count));
  const auto z_index = 1 + static_cast<size_t>(std::abs(std::ceil(layer_z / layer_spacing)));

  const auto x_name = std::to_string(x_index);
  const auto a = x_index < 10 ? "00" + x_name : (x_index < 100 ? "0" + x_name : x_name);
  const auto y_name = std::to_string(y_index);
  const auto b = y_index < 10 ? "00" + y_name : (y_index < 100 ? "0" + y_name : y_name);
  const auto name = std::to_string(z_index) + a + b;

  _hit_collection->insert(new Tracking::Hit(
     particle,
     trackID,
     parentID,
     name,
     deposit,
     position,
     momentum));

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

  for (size_t k = 1; k <= layer_count; ++k) {
    const auto k_name = std::to_string(k);
    const auto name = "s" + k_name;
    auto current = new Scintillator(name ,edge_length, edge_length, scintillator_height, scintillator_casing_thickness);
    current->PlaceIn(DetectorVolume, Construction::Transform(
      0,
      0,
      0.5*full_detector_height-steel_height-0.5*scintillator_height-((k-1)*(scintillator_height+layer_spacing))));
    _scintillators.push_back(current);
  }

  _steel = Construction::BoxVolume("SteelPlate",
    edge_length, edge_length, steel_height,
    Construction::Material::Iron,
    Construction::CasingAttributes());
  Construction::PlaceVolume(_steel, DetectorVolume, Construction::Transform(0, 0, 0.5*full_detector_height-0.5*steel_height));
  return Construction::PlaceVolume(DetectorVolume, world,
    G4Translate3D(0.5*edge_length + displacement, 0, -0.5*full_detector_height + steel_height));
}
//----------------------------------------------------------------------------------------------

} /* namespace Box */ //////////////////////////////////////////////////////////////////////////

} } /* namespace MATHUSLA::MU */
