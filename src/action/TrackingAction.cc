#include "action/TrackingAction.hh"

#include "tracking/Trajectory.hh"

namespace MATHUSLA { namespace MU {

TrackingAction::TrackingAction() : G4UserTrackingAction() {}

TrackingAction::~TrackingAction() {}

void TrackingAction::SetTrackingManagerPointer(G4TrackingManager* manager) {
  fpTrackingManager = manager;
}

void TrackingAction::PreUserTrackingAction(const G4Track* track) {
  auto trajectory = new Trajectory(track);
  fpTrackingManager->SetTrajectory(trajectory);
}

void TrackingAction::PostUserTrackingAction(const G4Track* track) {}

} } /* namespace MATHUSLA::MU */
