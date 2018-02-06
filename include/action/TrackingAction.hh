#ifndef TRACKINGACTION_HH
#define TRACKINGACTION_HH
#pragma once

#include "Geant4/G4UserTrackingAction.hh"
#include "Geant4/G4TrackingManager.hh"

namespace MATHUSLA {

class TrackingAction : public G4UserTrackingAction {
 public:
  TrackingAction();
  virtual ~TrackingAction();

  virtual void SetTrackingManagerPointer(G4TrackingManager*);
  virtual void PreUserTrackingAction(const G4Track*);
  virtual void PostUserTrackingAction(const G4Track*);

 private:
  G4TrackingManager* fpTrackingManager;
};

} /* namespace MATHUSLA */

#endif /* TRACKINGACTION_HH */
