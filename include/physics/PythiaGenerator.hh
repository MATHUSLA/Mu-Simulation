#ifndef PHYSICS_PYTHIA8GENERATOR_HH
#define PHYSICS_PYTHIA8GENERATOR_HH
#pragma once

#include <initializer_list>
#include <string>
#include <vector>

#include "Pythia8/Pythia.h"
#include "Pythia8/Event.h"

#include "physics/Generator.hh"
#include "ui/Command.hh"

namespace MATHUSLA { namespace MU {

class PythiaGenerator : public RangeGenerator {
public:
  PythiaGenerator(const G4int id,
                  const G4double pT,
                  const G4double eta,
                  const G4double phi,
                  Pythia8::Pythia* pythia=nullptr);

  PythiaGenerator(const G4int id,
                  const G4double pT,
                  const G4double eta,
                  const G4double phi,
                  std::initializer_list<std::string> settings);

  PythiaGenerator(const G4int id,
                  const G4double pT_min,
                  const G4double pT_max,
                  const G4double eta_min,
                  const G4double eta_max,
                  const G4double phi_min,
                  const G4double phi_max,
                  Pythia8::Pythia* pythia=nullptr);

  PythiaGenerator(const G4int id,
                  const G4double pT_min,
                  const G4double pT_max,
                  const G4double eta_min,
                  const G4double eta_max,
                  const G4double phi_min,
                  const G4double phi_max,
                  std::initializer_list<std::string> settings);

  void GeneratePrimaryVertex(G4Event* event);
  void SetNewValue(G4UIcommand* command, G4String value);
  void SetPythia(Pythia8::Pythia* pythia);
  void SetPythia(std::initializer_list<std::string> settings);
  Pythia8::Particle* FindParticle(Pythia8::Event& event) const;

private:
  Pythia8::Pythia* _pythia;
  std::vector<std::string> _settings;
  G4CMD_String* _read_string;
  G4CMD_String* _read_file;
};

} } /* namespace MATHUSLA::MU */

#endif /* PHYSICS_PYTHIA8GENERATOR_HH */
