#ifndef PHYSICSLIST_H
#define PHYSICSLIST_H
#pragma once

#include "Geant4/G4VUserPhysicsList.hh"

namespace MATHUSLA {

class PhysicsList : G4VUserPhysicsList {
 public:
  PhysicsList();
  ~PhysicsList();
  void ConstructParticle();
  void ConstructProcess();
  void SetCuts();
};

} /* namespace MATHUSLA */

#endif /* PHYSICSLIST_H */
