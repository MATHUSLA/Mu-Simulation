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
  virtual ~PrimaryGeneratorAction();

  virtual void GeneratePrimaries(G4Event*);

  G4ParticleGun* GetParticleGun() { return fParticleGun; }

  void SetRandomFlag(G4bool);

 private:
  G4ParticleGun* fParticleGun;
};

} } /* namespace MATHUSLA::MU */

#endif /* ACTION_PRIMARYGENERATORACTION_HH */
