#ifndef PHYSICS_PYTHIA8GENERATOR_HH
#define PHYSICS_PYTHIA8GENERATOR_HH
#pragma once

#include "Pythia8/Pythia.h"
#include "Pythia8Plugins/HepMC2.h"

#include "HepMC/GenEvent.h"

#include "Geant4/G4VPrimaryGenerator.hh"
#include "Geant4/G4VUserPrimaryGeneratorAction.hh"
#include "Geant4/G4VSolid.hh"

namespace MATHUSLA { namespace MU {

class Pythia8Generator : public G4VPrimaryGenerator {
public:
  Pythia8Generator() = default;
  Pythia8Generator(Pythia8::Pythia* pythia);

  void setPythia(Pythia8::Pythia* pythia) { _pythia = pythia; }
  void GeneratePrimaryVertex(G4Event* event);

private:
  HepMC::GenEvent*      _hepmcevt;
  G4VSolid*             _world;
  Pythia8::Pythia*      _pythia;
  HepMC::Pythia8ToHepMC _hepmc_converter;
};

class Pythia8GeneratorAction : public G4VUserPrimaryGeneratorAction {
public:
  Pythia8GeneratorAction();

  void GeneratePrimaries(G4Event* event);
private:
  Pythia8Generator _p8generator;
};

} } /* namespace MATHUSLA::MU */

#endif /* PHYSICS_PYTHIA8GENERATOR_HH */
