#ifndef MU__PHYSICS_PYTHIAGENERATOR_HH
#define MU__PHYSICS_PYTHIAGENERATOR_HH
#pragma once

#include <string>
#include <vector>

#include "Pythia8/Pythia.h"
#include "Pythia8/Event.h"

#include "physics/Generator.hh"
#include "ui.hh"

namespace MATHUSLA { namespace MU {

class PythiaGenerator : public RangeGenerator {
public:
  PythiaGenerator(const int id,
                  const double pT,
                  const double eta,
                  const double phi,
                  Pythia8::Pythia* pythia=nullptr);

  PythiaGenerator(const int id,
                  const double pT,
                  const double eta,
                  const double phi,
                  std::vector<std::string> settings);

  PythiaGenerator(const int id,
                  const double pT_min,
                  const double pT_max,
                  const double eta_min,
                  const double eta_max,
                  const double phi_min,
                  const double phi_max,
                  Pythia8::Pythia* pythia=nullptr);

  PythiaGenerator(const int id,
                  const double pT_min,
                  const double pT_max,
                  const double eta_min,
                  const double eta_max,
                  const double phi_min,
                  const double phi_max,
                  std::vector<std::string> settings);

  void GeneratePrimaryVertex(G4Event* event);
  void SetNewValue(G4UIcommand* command, G4String value);
  void SetPythia(Pythia8::Pythia* pythia);
  void SetPythia(std::vector<std::string> settings);
  Pythia8::Particle* FindParticle(Pythia8::Event& event) const;

private:
  Pythia8::Pythia* _pythia;
  std::vector<std::string> _settings;
  Command::StringArg* _read_string;
  Command::StringArg* _read_file;
};

} } /* namespace MATHUSLA::MU */

#endif /* MU__PHYSICS_PYTHIAGENERATOR_HH */
