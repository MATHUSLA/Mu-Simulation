#include "tracking/Trajectory.hh"

#include "tracking/TrajectoryPoint.hh"

namespace MATHUSLA { namespace MU {

G4ThreadLocal G4Allocator<Trajectory> *TrajectoryAllocator = 0;

Trajectory::Trajectory() : G4Trajectory() {}

Trajectory::Trajectory(const G4Track* track) : G4Trajectory(track) {}

Trajectory::Trajectory(Trajectory& rhs) : G4Trajectory(rhs) {}

Trajectory::~Trajectory() {}

void Trajectory::AppendStep(const G4Step* step) {
  // auto tpoint = new TrajectoryPoint(step);
  G4Trajectory::AppendStep(step);
}

void Trajectory::ShowTrajectory(std::ostream& os) const {
  G4Trajectory::ShowTrajectory(os);
}

void Trajectory::DrawTrajectory() const {
    G4Trajectory::DrawTrajectory();
}

} } /* namespace MATHUSLA::MU */
