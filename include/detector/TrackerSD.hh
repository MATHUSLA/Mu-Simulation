#ifndef TRACKERSD_HH
#define TRACKERSD_HH
#pragma once

#include <vector>

#include "Geant4/G4VSensitiveDetector.hh"
#include "Geant4/G4ParticleTable.hh"
#include "Geant4/G4Step.hh"
#include "Geant4/G4HCofThisEvent.hh"

#include "tracking/TrackerHit.hh"

namespace MATHUSLA { namespace MU {

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

} } /* namespace MATHUSLA::MU */

#endif /* TRACKERSD_HH */
