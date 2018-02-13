#ifndef DETECTOR_TRAPEZOIDCALORIMETER_HH
#define DETECTOR_TRAPEZOIDCALORIMETER_HH
#pragma once

#include "Geant4/G4VSensitiveDetector.hh"

namespace MATHUSLA { namespace MU {

class TrapezoidCalorimeter : public G4VSensitiveDetector {
 public:
  TrapezoidCalorimeter();

  struct Material {
    static G4Material* Scintillator;
    static G4Material* Aluminum;
   private:
    Material();
  };

  static void DefineMaterials();
  static G4VPhysicalVolume* Construct(G4LogicalVolume*);

  // TODO: merge with TrackerSD
  void Initialize(G4HCofThisEvent*);
  G4bool ProcessHits(G4Step*, G4TouchableHistory*);
  void EndOfEvent(G4HCofThisEvent*);
};

} } /* namespace MATHUSLA::MU */

#endif /* DETECTOR_TRAPEZOIDCALORIMETER_HH */
