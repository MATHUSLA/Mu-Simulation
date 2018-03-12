#ifndef PHYSICS_PYTHIA8GENERATOR_HH
#define PHYSICS_PYTHIA8GENERATOR_HH
#pragma once

#include "Pythia8/Pythia.h"
#include "Pythia8/Event.h"
#include "Pythia8Plugins/HepMC2.h"

#include "HepMC/GenEvent.h"

#include "Geant4/G4VPrimaryGenerator.hh"
#include "Geant4/G4VUserPrimaryGeneratorAction.hh"

namespace MATHUSLA { namespace MU {

struct PythiaFilter {
  G4int id;
  G4double pT;
  G4double eta;
  G4double phi_min;
  G4double phi_max;
  Pythia8::Particle* find(Pythia8::Event& event);
};

class PythiaGenerator : public G4VPrimaryGenerator {
public:
  PythiaGenerator() = default;
  PythiaGenerator(Pythia8::Pythia* pythia, const PythiaFilter& filter);

  void SetPythia(Pythia8::Pythia* pythia)    { _pythia = pythia; }
  void SetFilter(const PythiaFilter& filter) { _filter = filter; }

  void GeneratePrimaryVertex(G4Event* event);

private:
  HepMC::GenEvent*      _hepmcevt;
  Pythia8::Pythia*      _pythia;
  PythiaFilter          _filter;
  HepMC::Pythia8ToHepMC _hepmc_converter;
};

class PythiaGeneratorAction : public G4VUserPrimaryGeneratorAction {
public:
  PythiaGeneratorAction();

  void GeneratePrimaries(G4Event* event);
private:
  PythiaGenerator _p8generator;
};

} } /* namespace MATHUSLA::MU */

#endif /* PHYSICS_PYTHIA8GENERATOR_HH */
