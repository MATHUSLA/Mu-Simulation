#ifndef PHYSICS_BASICGENERATOR_HH
#define PHYSICS_BASICGENERATOR_HH
#pragma once

#include "Geant4/globals.hh"
#include "Geant4/G4VPrimaryGenerator.hh"
#include "Geant4/G4VUserPrimaryGeneratorAction.hh"
#include "Geant4/G4ParticleGun.hh"
#include "Geant4/G4Event.hh"

namespace MATHUSLA { namespace MU {

class BasicGeneratorAction : public G4VUserPrimaryGeneratorAction {
public:
  BasicGeneratorAction();
  void GeneratePrimaries(G4Event*);

private:
  G4ParticleGun* _particle_gun;
};

} } /* namespace MATHUSLA::MU */

#endif /* PHYSICS_BASICGENERATOR_HH */
