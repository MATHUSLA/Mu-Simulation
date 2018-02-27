#include "action/PrimaryGeneratorAction.hh"

#include "Geant4/G4LogicalVolumeStore.hh"
#include "Geant4/G4LogicalVolume.hh"
#include "Geant4/G4Box.hh"
#include "Geant4/G4ParticleTable.hh"
#include "Geant4/G4ParticleDefinition.hh"
#include "Geant4/G4SystemOfUnits.hh"
#include "Geant4/Randomize.hh"

namespace MATHUSLA { namespace MU {

PrimaryGeneratorAction::PrimaryGeneratorAction()
    : G4VUserPrimaryGeneratorAction() {
  fParticleGun = new G4ParticleGun(1);

  auto particleDefinition
    = G4ParticleTable::GetParticleTable()->FindParticle("mu-");

  fParticleGun->SetParticleDefinition(particleDefinition);
  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0., 0., -1.));
  fParticleGun->SetParticleEnergy(60.0*GeV);
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event) {
  G4double worldZHalfLength = 6000*cm;
  auto worldLV = G4LogicalVolumeStore::GetInstance()->GetVolume("World");
  G4Box* worldBox = nullptr;
  if (worldLV) worldBox = dynamic_cast<G4Box*>(worldLV->GetSolid());

  if (worldBox) {
    worldZHalfLength = worldBox->GetZHalfLength();
  } else {
    G4cerr << "World volume of box not found.\n"
           << "Perhaps you have changed geometry.\n"
           << "The gun will be place in the center.\n";
  }

  fParticleGun->SetParticlePosition(G4ThreeVector(0., 0., worldZHalfLength));
  fParticleGun->GeneratePrimaryVertex(event);
}

} } /* namespace MATHUSLA::MU */
