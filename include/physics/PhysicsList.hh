#ifndef PHYSICS_PHYSICSLIST_H
#define PHYSICS_PHYSICSLIST_H
#pragma once

#include "Geant4/G4VUserPhysicsList.hh"

namespace MATHUSLA { namespace MU {

class PhysicsList : G4VUserPhysicsList {
 public:
  PhysicsList();
  void ConstructParticle();
  void ConstructProcess();
  void SetCuts();
};

} } /* namespace MATHUSLA::MU */

#endif /* PHYSICS_PHYSICSLIST_H */
