#ifndef TRACKERSD_HH
#define TRACKERSD_HH
#pragma once

#include <vector>

#include "Geant4/G4VSensitiveDetector.hh"
#include "Geant4/G4ParticleTable.hh"
#include "Geant4/G4Step.hh"
#include "Geant4/G4HCofThisEvent.hh"

#include "tracking/TrackerHit.hh"

namespace MATHUSLA {

/// Tracker sensitive detector class
///
/// The hits are accounted in hits in ProcessHits() function which is called
/// by Geant4 kernel at each step. A hit is created with each step with non zero
/// energy deposit.

class TrackerSD : public G4VSensitiveDetector {
 public:
  TrackerSD(const G4String& name, const G4String& hitsCollectionName);
  virtual ~TrackerSD();

  virtual void Initialize(G4HCofThisEvent* hitCollection);
  virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* history);
  virtual void EndOfEvent(G4HCofThisEvent* hitCollection);

 private:
  TrackerHitsCollection* fHitsCollection;
};

} /* namespace MATHUSLA */

#endif /* TRACKERSD_HH */
