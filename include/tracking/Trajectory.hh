#ifndef TRACKING_TRAJECTORY_HH
#define TRACKING_TRAJECTORY_HH
#pragma once

#include "Geant4/G4Trajectory.hh"

namespace MATHUSLA { namespace MU {

class Trajectory : public G4Trajectory {
 public:
  Trajectory();
  explicit Trajectory(const G4Track*);
  Trajectory(Trajectory&);
  virtual ~Trajectory();

  virtual void AppendStep(const G4Step*);
  virtual void ShowTrajectory(std::ostream& os = G4cout) const;
  virtual void DrawTrajectory() const;
};

} } /* namespace MATHUSLA::MU */

#endif /* TRACKING_TRAJECTORY_HH */
