#include "tracking/TrajectoryPoint.hh"

namespace MATHUSLA { namespace MU {

TrajectoryPoint::TrajectoryPoint() : G4TrajectoryPoint() {}

TrajectoryPoint::TrajectoryPoint(G4ThreeVector pos) : G4TrajectoryPoint(pos) {}

TrajectoryPoint::TrajectoryPoint(G4Step* step)
    : G4TrajectoryPoint(step->GetPostStepPoint()->GetPosition()) {}

TrajectoryPoint::TrajectoryPoint(const TrajectoryPoint&) {}

TrajectoryPoint::~TrajectoryPoint() {}

} } /* namespace MATHUSLA::MU */
