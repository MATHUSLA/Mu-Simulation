#ifndef MU__DETECTOR_FLAT_HH
#define MU__DETECTOR_FLAT_HH
#pragma once

#include "Geant4/G4VSensitiveDetector.hh"

#include "detector/Construction.hh"
#include "tracking.hh"

namespace MATHUSLA { namespace MU {

namespace Flat { ///////////////////////////////////////////////////////////////////////////////

class Scintillator {

};

////////////////////////////////////////////////////////////////////////////////////////////////

class Detector : public G4VSensitiveDetector {
public:
  Detector();

  void Initialize(G4HCofThisEvent* eventHC);
  G4bool ProcessHits(G4Step* step, G4TouchableHistory*);
  void EndOfEvent(G4HCofThisEvent*);

  static G4VPhysicalVolume* Construct(G4LogicalVolume* world);
};

} /* namespace Flat */ /////////////////////////////////////////////////////////////////////////

} } /* namespace MATHUSLA::MU */

#endif /* MU__DETECTOR_FLAT_HH */
