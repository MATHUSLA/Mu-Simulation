#ifndef ACTION_PRIMARYGENERATORACTION_HH
#define ACTION_PRIMARYGENERATORACTION_HH
#pragma once

#include "Geant4/globals.hh"
#include "Geant4/G4VUserPrimaryGeneratorAction.hh"
#include "Geant4/G4ParticleGun.hh"
#include "Geant4/G4Event.hh"

namespace MATHUSLA { namespace MU {

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction {
 public:
  PrimaryGeneratorAction();
  void GeneratePrimaries(G4Event*);
  void SetRandomFlag(G4bool);
  G4ParticleGun* GetParticleGun() const { return fParticleGun; }

 private:
  G4ParticleGun* fParticleGun;
};

} } /* namespace MATHUSLA::MU */

#endif /* ACTION_PRIMARYGENERATORACTION_HH */
