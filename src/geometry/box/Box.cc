#include "geometry/Box.hh"

#include "tracking.hh"

namespace MATHUSLA { namespace MU {

namespace Box { ////////////////////////////////////////////////////////////////////////////////

namespace { ////////////////////////////////////////////////////////////////////////////////////

//__Box Sensitive Material______________________________________________________________________
std::vector<Scintillator*> _scintillators;
G4LogicalVolume* _box;
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
  _box->SetSensitiveDetector(this);
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

  constexpr double box_height = 25*m;
  constexpr double outer_gap = 2*cm;
  constexpr double inner_gap = 10*cm;
  auto DetectorVolume = Construction::BoxVolume("Box", 200*m, 200*m, box_height);

  _box = Construction::OpenBoxVolume("BoxOuterLayer",
    200*m - outer_gap, 200*m - outer_gap, box_height - outer_gap, inner_gap,
    Scintillator::Material::Scintillator,
    Construction::CasingAttributes());

  Construction::PlaceVolume(_box, DetectorVolume);

  return Construction::PlaceVolume(DetectorVolume, world,
    G4Translate3D(100*m, 100*m, -0.5*box_height));
}
//----------------------------------------------------------------------------------------------

} /* namespace Box */ //////////////////////////////////////////////////////////////////////////

} } /* namespace MATHUSLA::MU */
