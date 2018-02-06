#ifndef TRAJECTORYPOINT_HH
#define TRAJECTORYPOINT_HH
#pragma once

#include "Geant4/G4TrajectoryPoint.hh"
#include "Geant4/G4Step.hh"
#include "Geant4/G4ThreeVector.hh"

namespace MATHUSLA {

class TrajectoryPoint : public G4TrajectoryPoint {
 public:
  TrajectoryPoint();
  explicit TrajectoryPoint(G4ThreeVector);
  explicit TrajectoryPoint(G4Step*);
  TrajectoryPoint(const TrajectoryPoint&);
  virtual ~TrajectoryPoint();

 private:
};

} /* namespace MATHUSLA */

#endif /* TRAJECTORYPOINT_HH */
