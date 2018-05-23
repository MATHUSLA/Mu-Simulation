/* src/physics/PythiaGenerator.cc
 *
 * Copyright 2018 Brandon Gomes
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "physics/PythiaGenerator.hh"

#include "Pythia8/ParticleData.h"

#include "physics/Units.hh"

namespace MATHUSLA { namespace MU {

//__Pythia Generator Constructor________________________________________________________________
PythiaGenerator::PythiaGenerator(const int id,
                                 const double pT,
                                 const double eta,
                                 const double phi,
                                 Pythia8::Pythia* pythia)
    : PythiaGenerator(id, pT, pT, -eta, eta, -phi, phi, pythia) {}
//----------------------------------------------------------------------------------------------

//__Pythia Generator Constructor________________________________________________________________
PythiaGenerator::PythiaGenerator(const int id,
                                 const double pT,
                                 const double eta,
                                 const double phi,
                                 std::vector<std::string> settings)
    : PythiaGenerator(id, pT, eta, phi) {
  SetPythia(std::move(settings));
}
//----------------------------------------------------------------------------------------------

//__Pythia Generator Constructor________________________________________________________________
PythiaGenerator::PythiaGenerator(const int id,
                                 const double pT_min,
                                 const double pT_max,
                                 const double eta_min,
                                 const double eta_max,
                                 const double phi_min,
                                 const double phi_max,
                                 Pythia8::Pythia* pythia)
    : RangeGenerator("pythia", "Pythia8 Generator.", id,
                     pT_min, pT_max, eta_min, eta_max, phi_min, phi_max) {

  SetPythia(pythia);

  _read_string = CreateCommand<Command::StringArg>("readString", "Read Pythia String.");
  _read_string->SetParameterName("string", false);
  _read_string->AvailableForStates(G4State_PreInit, G4State_Idle);

  _read_file = CreateCommand<Command::StringArg>("readFile", "Read Pythia File.");
  _read_file->SetParameterName("file", false);
  _read_file->AvailableForStates(G4State_PreInit, G4State_Idle);
}
//----------------------------------------------------------------------------------------------

//__Pythia Generator Constructor________________________________________________________________
PythiaGenerator::PythiaGenerator(const int id,
                                 const double pT_min,
                                 const double pT_max,
                                 const double eta_min,
                                 const double eta_max,
                                 const double phi_min,
                                 const double phi_max,
                                 std::vector<std::string> settings)
    : PythiaGenerator(id, pT_min, pT_max, eta_min, eta_max, phi_min, phi_max) {
  SetPythia(std::move(settings));
}
//----------------------------------------------------------------------------------------------

namespace { ////////////////////////////////////////////////////////////////////////////////////

//__Reconstruct Pythia Object from Old Object___________________________________________________
Pythia8::Pythia* _reconstruct_pythia(Pythia8::Pythia* pythia) {
  if (!pythia) {
    return new Pythia8::Pythia();
  } else {
    auto out = new Pythia8::Pythia(pythia->settings, pythia->particleData);
    // delete pythia;
    return out;
  }
}
//----------------------------------------------------------------------------------------------

//__Create Pythia from Settings_________________________________________________________________
Pythia8::Pythia* _create_pythia(std::vector<std::string>& settings) {
  auto pythia = new Pythia8::Pythia();
  for (const auto& setting : settings) {
    pythia->readString(setting);
  }
  pythia->init();
  settings.clear();
  return pythia;
}
//----------------------------------------------------------------------------------------------

} /* anonymous namespace */ ////////////////////////////////////////////////////////////////////

//__Generate Initial Particles__________________________________________________________________
void PythiaGenerator::GeneratePrimaryVertex(G4Event* event) {
  if (!_settings.empty()) {
    // delete _pythia;
    _pythia = _create_pythia(_settings);
  } else if (!_pythia) {
    std::cout << "\n[ERROR] No Pythia Configuration Specified.\n";
  }

  Pythia8::Particle* particle = nullptr;

  uint_fast64_t counter = 0;
  while (true) {
    ++counter;
    if (!_pythia->next()) continue;
    particle = FindParticle(_pythia->process);
    if (particle) break;
  }

  std::cout << "\n\nPythiaGenerator:\n"
            << "  Attempt " << counter << " | Filtered Particle: "
            << particle->id()  << " "
            << particle->pT()  << " "
            << particle->eta() << " "
            << particle->phi() << "\n\n\n";

  auto vertex = DefaultVertex();

  vertex->SetPrimary(CreateParticle(
    particle->id(),
    G4ThreeVector(particle->pz(), particle->py(), -particle->px())*GeVperC));

  event->AddPrimaryVertex(vertex);
}
//----------------------------------------------------------------------------------------------

//__Messenger Set Value_________________________________________________________________________
void PythiaGenerator::SetNewValue(G4UIcommand* command, G4String value) {
  if (command == _read_string) {
    _settings.push_back(value);
  } else if (command == _read_file) {
    _settings.clear();
    // delete _pythia;
    _pythia = new Pythia8::Pythia();
    _pythia->readFile(value);
    _pythia->init();
  }

  else if (command == _ui_id) {
    _id = _ui_id->GetNewIntValue(value);

  } else if (command == _ui_pT) {
    _pT = _ui_pT->GetNewDoubleValue(value);
    _pT_min = _pT;
    _pT_max = _pT;
  } else if (command == _ui_pT_min) {
    _pT_min = _ui_pT_min->GetNewDoubleValue(value);
  } else if (command == _ui_pT_max) {
    _pT_max = _ui_pT_max->GetNewDoubleValue(value);

  } else if (command == _ui_eta) {
    _eta = _ui_eta->GetNewDoubleValue(value);
    _eta_min = _eta;
    _eta_max = _eta;
  } else if (command == _ui_eta_min) {
    _eta_min = _ui_eta_min->GetNewDoubleValue(value);
  } else if (command == _ui_eta_max) {
    _eta_max = _ui_eta_max->GetNewDoubleValue(value);

  } else if (command == _ui_phi) {
    _phi = _ui_phi->GetNewDoubleValue(value);
    _phi_min = _phi;
    _phi_max = _phi;
  } else if (command == _ui_phi_min) {
    _phi_min = _ui_phi_min->GetNewDoubleValue(value);
  } else if (command == _ui_phi_max) {
    _phi_max = _ui_phi_max->GetNewDoubleValue(value);
  }
}
//----------------------------------------------------------------------------------------------

//__Set Pythia Object from Copy_________________________________________________________________
void PythiaGenerator::SetPythia(Pythia8::Pythia* pythia) {
  if (!pythia) return;
  _settings.clear();
  // delete _pythia;
  _pythia = _reconstruct_pythia(pythia);
  _pythia->init();
}
//----------------------------------------------------------------------------------------------

//__Set Pythia Object from Settings_____________________________________________________________
void PythiaGenerator::SetPythia(std::vector<std::string> settings) {
  _settings = std::move(settings);
  // delete _pythia;
  _pythia = _create_pythia(_settings);
}
//----------------------------------------------------------------------------------------------

//__Find Particle in Event______________________________________________________________________
Pythia8::Particle* PythiaGenerator::FindParticle(Pythia8::Event& event) const {
  for (int i = 0; i < event.size(); ++i) {
    const auto& particle = event[i];
    if (particle.id() != _id) continue;
    const auto pT  = particle.pT()*GeVperC;
    const auto eta = particle.eta();
    const auto phi = particle.phi()*rad;
    if ((_pT_min <= pT)
        && (_eta_min <= eta && eta <= _eta_max)
        && (_phi_min <= phi && phi <= _phi_max)) {
        return &event[i];
    }
  }
  return nullptr;
}
//----------------------------------------------------------------------------------------------

} } /* namespace MATHUSLA::MU */
