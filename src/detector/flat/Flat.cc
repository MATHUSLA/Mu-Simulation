#include "detector/Flat.hh"

namespace MATHUSLA { namespace MU {

namespace Flat { ///////////////////////////////////////////////////////////////////////////////


Detector::Detector() : G4VSensitiveDetector("MATHUSLA/MU/Flat") {
  collectionName.insert("Flat_HC");
}

void Detector::Initialize(G4HCofThisEvent* eventHC) {

}

G4bool Detector::ProcessHits(G4Step* step, G4TouchableHistory*) {
  return false;
}

void Detector::EndOfEvent(G4HCofThisEvent*) {

}

G4VPhysicalVolume* Detector::Construct(G4LogicalVolume* world) {
  return nullptr;
}

} /* namespace Flat */ /////////////////////////////////////////////////////////////////////////

} } /* namespace MATHUSLA::MU */
