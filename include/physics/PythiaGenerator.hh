#ifndef PHYSICS_PYTHIA8GENERATOR_HH
#define PHYSICS_PYTHIA8GENERATOR_HH
#pragma once

#include "Pythia8/Pythia.h"
#include "Pythia8/Event.h"

#include "physics/Generator.hh"

namespace MATHUSLA { namespace MU {

struct PythiaFilter {
  G4int    id;
  G4double pT;
  G4double eta;
  G4double phi_min;
  G4double phi_max;
  Pythia8::Particle* find(Pythia8::Event& event);
};

class PythiaGenerator : public Generator {
public:
  PythiaGenerator(Pythia8::Pythia* pythia, const PythiaFilter& filter);

  void GeneratePrimaries(G4Event* event);

private:
  Pythia8::Pythia* _pythia;
  PythiaFilter     _filter;
};

} } /* namespace MATHUSLA::MU */

#endif /* PHYSICS_PYTHIA8GENERATOR_HH */
