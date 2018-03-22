#ifndef ACTION_TRACKINGACTION_HH
#define ACTION_TRACKINGACTION_HH
#pragma once

#include "Geant4/G4UserTrackingAction.hh"
#include "Geant4/G4TrackingManager.hh"

namespace MATHUSLA { namespace MU {

class TrackingAction : public G4UserTrackingAction {
public:
  TrackingAction();
  void SetTrackingManagerPointer(G4TrackingManager*);
  void PreUserTrackingAction(const G4Track*);
  void PostUserTrackingAction(const G4Track*);
private:
  G4TrackingManager* _manager;
};

} } /* namespace MATHUSLA::MU */

#endif /* ACTION_TRACKINGACTION_HH */
