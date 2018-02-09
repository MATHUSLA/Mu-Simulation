#ifndef PHYSICSLIST_H
#define PHYSICSLIST_H
#pragma once

#include "Geant4/G4VUserPhysicsList.hh"

namespace MATHUSLA { namespace MU {

class PhysicsList : G4VUserPhysicsList {
 public:
  PhysicsList();
  ~PhysicsList();
  void ConstructParticle();
  void ConstructProcess();
  void SetCuts();
};

} } /* namespace MATHUSLA::MU */

#endif /* PHYSICSLIST_H */
