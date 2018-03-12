#include "physics/BasicGenerator.hh"

#include "Geant4/G4ParticleTable.hh"
#include "Geant4/G4SystemOfUnits.hh"

namespace MATHUSLA { namespace MU {

BasicGeneratorAction::BasicGeneratorAction() : G4VUserPrimaryGeneratorAction() {
  _particle_gun = new G4ParticleGun(
    G4ParticleTable::GetParticleTable()->FindParticle("mu-"), 1);
  _particle_gun->SetParticlePosition(G4ThreeVector(0, 0, 100*m));
  _particle_gun->SetParticleMomentumDirection(G4ThreeVector(0, 0, -1));
  _particle_gun->SetParticleEnergy(65*GeV);
}

void BasicGeneratorAction::GeneratePrimaries(G4Event* event) {
  _particle_gun->GeneratePrimaryVertex(event);
}

} } /* namespace MATHUSLA::MU */
