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
  _hit_collection->insert(new Tracking::Hit(step));
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

  constexpr auto displacement = 100*m;
  constexpr auto edge_length = 200*m;
  constexpr auto steel_height = 3*cm;
  constexpr auto scintillator_height = 1*cm;
  constexpr auto scintillator_x_width = 0.25*m;
  constexpr auto scintillator_y_width = 0.25*m;
  constexpr auto scintillator_casing_thickness = 0.1*cm;
  constexpr auto layer_spacing = 1.5*m;
  constexpr auto full_detector_height = 3 * (layer_spacing + scintillator_height) + steel_height;

  auto DetectorVolume = Construction::BoxVolume("Box", edge_length, edge_length, full_detector_height);

  constexpr auto x_count = edge_length / scintillator_x_width;
  constexpr auto y_count = edge_length / scintillator_y_width;
  for (size_t i = 0; i < x_count; ++i) {
    for (size_t j = 0; j < y_count; ++j) {
      // naming scheme-> _ ___ ___
      // SCINTILLATOR CONSTRUCTOR: new Scintillator("name!", scintillator_x_width, scintillator_y_width, scintillator_height, scintillator_casing_thickness);
      // STEPS: 1 construct scintillator like above
      //        2 add to "_scintillators" vector
      //        3 place into world with "Construction::Transform(x, y, z)" depending on index (i,j)
    }
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
