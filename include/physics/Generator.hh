#ifndef PHYSICS_GENERATOR_HH
#define PHYSICS_GENERATOR_HH
#pragma once

#include "Geant4/G4Event.hh"
#include "Geant4/G4PrimaryVertex.hh"
#include "Geant4/G4PrimaryParticle.hh"
#include "Geant4/G4VUserPrimaryGeneratorAction.hh"

namespace MATHUSLA { namespace MU {

class Generator : public G4VUserPrimaryGeneratorAction {
public:
  Generator() = default;
  Generator(G4int id, G4double pT, G4double eta, G4double phi);
  void GeneratePrimaries(G4Event* event);

protected:
  static G4PrimaryVertex* DefaultVertex();
  static G4PrimaryParticle* CreateParticle(G4int id, G4double pT, G4double eta, G4double phi);

  G4int    _id;
  G4double _pT;
  G4double _eta;
  G4double _phi;
};

class RangeGenerator : public Generator {
public:
  RangeGenerator(G4int id, G4double pT, G4double eta, G4double phi);
  RangeGenerator(G4int id, G4double pT_min, G4double pT_max, G4double eta_min, G4double eta_max, G4double phi_min, G4double phi_max);
  void GeneratePrimaries(G4Event* event);

protected:
  G4int    _id;
  G4double _pT_min;
  G4double _pT_max;
  G4double _eta_min;
  G4double _eta_max;
  G4double _phi_min;
  G4double _phi_max;
};

} } /* namespace MATHUSLA::MU */

#endif /* PHYSICS_GENERATOR_HH */
