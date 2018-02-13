#ifndef DETECTOR_TRACKERSD_HH
#define DETECTOR_TRACKERSD_HH
#pragma once

#include <vector>

#include "Geant4/G4VSensitiveDetector.hh"
#include "Geant4/G4ParticleTable.hh"
#include "Geant4/G4Step.hh"
#include "Geant4/G4HCofThisEvent.hh"
#include "Geant4/G4VPhysicalVolume.hh"
#include "Geant4/G4LogicalVolume.hh"

#include "tracking/TrackerHit.hh"

namespace MATHUSLA { namespace MU {

class TrackerSD : public G4VSensitiveDetector {
 public:
  TrackerSD(const G4String&, const G4String&);

  // TODO: integrate with Trapezoid Calorimeter
  G4VPhysicalVolume* Construct(G4LogicalVolume*);

  void Initialize(G4HCofThisEvent*);
  G4bool ProcessHits(G4Step*, G4TouchableHistory*);
  void EndOfEvent(G4HCofThisEvent*);

 private:
  TrackerHitsCollection* fHitsCollection;
};

} } /* namespace MATHUSLA::MU */

#endif /* DETECTOR_TRACKERSD_HH */
