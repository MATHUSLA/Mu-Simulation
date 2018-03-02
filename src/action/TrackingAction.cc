#include "action/TrackingAction.hh"

namespace MATHUSLA { namespace MU {

TrackingAction::TrackingAction() : G4UserTrackingAction() {}

void TrackingAction::SetTrackingManagerPointer(G4TrackingManager* manager) {
  _manager = manager;
}

void TrackingAction::PreUserTrackingAction(const G4Track*) {}

void TrackingAction::PostUserTrackingAction(const G4Track*) {}

} } /* namespace MATHUSLA::MU */
