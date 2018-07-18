/*
 * src/physics/PythiaGenerator.cc
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

#include <Pythia8/ParticleData.h>

#include "physics/Units.hh"

namespace MATHUSLA { namespace MU {

namespace Physics { ////////////////////////////////////////////////////////////////////////////

//__Pythia Generator Construction_______________________________________________________________
PythiaGenerator::PythiaGenerator(const PropagationList& propagation,
                                 Pythia8::Pythia* pythia)
    : Generator("pythia", "Pythia8 Generator."), _propagation_list(propagation) {
  SetPythia(pythia);

  _read_string = CreateCommand<Command::StringArg>("readString", "Read Pythia String.");
  _read_string->SetParameterName("string", false);
  _read_string->AvailableForStates(G4State_PreInit, G4State_Idle);

  _read_file = CreateCommand<Command::StringArg>("readFile", "Read Pythia File.");
  _read_file->SetParameterName("file", false);
  _read_file->AvailableForStates(G4State_PreInit, G4State_Idle);
}
//----------------------------------------------------------------------------------------------

//__Pythia Generator Construction_______________________________________________________________
PythiaGenerator::PythiaGenerator(const PropagationList& propagation,
                                 std::vector<std::string> settings)
    : PythiaGenerator(propagation) {
  SetPythia(std::move(settings));
}
//----------------------------------------------------------------------------------------------

//__Pythia Generator Construction_______________________________________________________________
PythiaGenerator::PythiaGenerator(const PropagationList& propagation,
                                 const std::string& path)
    : PythiaGenerator(propagation) {
  SetPythia(path);
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
  for (const auto& setting : settings)
    pythia->readString(setting);
  pythia->init();
  settings.clear();
  return pythia;
}
//----------------------------------------------------------------------------------------------

} /* anonymous namespace */ ////////////////////////////////////////////////////////////////////

//__Generate Initial Particles__________________________________________________________________
void PythiaGenerator::GeneratePrimaryVertex(G4Event* event) {
  if (!_pythia_settings.empty()) {
    // delete _pythia;
    _pythia = _create_pythia(_pythia_settings);
  } else if (!_pythia) {
    std::cout << "\n[ERROR] No Pythia Configuration Specified.\n";
  }

  std::vector<Pythia8::Particle> particles;
  while (true) {
    if (!_pythia->next()) continue;
    particles = FindParticles(_pythia->process);
    if (!particles.empty()) break;
  }

  for (std::size_t i{}; i < particles.size(); ++i) {
    const auto particle = particles[i];
    auto vertex = Vertex(particle.tProd()  * mm / c_light,
                         particle.zProd()  * mm,
                         particle.yProd()  * mm,
                         -particle.xProd() * mm + 100*m);
    vertex->SetPrimary(CreateParticle(
      particle.id(),
      Convert(PseudoLorentzTriplet{particle.pT() * GeVperC,
                                   particle.eta(),
                                   particle.phi() * rad})));
    event->AddPrimaryVertex(vertex);
  }
}
//----------------------------------------------------------------------------------------------

//__Messenger Set Value_________________________________________________________________________
void PythiaGenerator::SetNewValue(G4UIcommand* command,
                                  G4String value) {
  if (command == _read_string) {
    _pythia_settings.push_back(value);
  } else if (command == _read_file) {
    SetPythia(value);
  }
}
//----------------------------------------------------------------------------------------------

//__Set Pythia Object from Copy_________________________________________________________________
void PythiaGenerator::SetPythia(Pythia8::Pythia* pythia) {
  if (!pythia) return;
  _pythia_settings.clear();
  // delete _pythia;
  _pythia = _reconstruct_pythia(pythia);
  _pythia->init();
}
//----------------------------------------------------------------------------------------------

//__Set Pythia Object from Settings_____________________________________________________________
void PythiaGenerator::SetPythia(std::vector<std::string> settings) {
  _pythia_settings = std::move(settings);
  // delete _pythia;
  _pythia = _create_pythia(_pythia_settings);
}
//----------------------------------------------------------------------------------------------

//__Set Pythia Object from Settings_____________________________________________________________
void PythiaGenerator::SetPythia(const std::string& path) {
  _pythia_settings.clear();
  // delete _pythia;
  _pythia = new Pythia8::Pythia();
  _pythia->readFile(path);
  _pythia->init();
}
//----------------------------------------------------------------------------------------------

//__Find Particle in Event______________________________________________________________________
std::vector<Pythia8::Particle> PythiaGenerator::FindParticles(Pythia8::Event& event) const {
  std::vector<Pythia8::Particle> out;
  for (int i = 0; i < event.size(); ++i) {
    const auto &particle = event[i];
    if (InPropagationList(_propagation_list,
                          particle.id(),
                          PseudoLorentzTriplet{particle.pT() * GeVperC,
                                               particle.eta(),
                                               particle.phi() * rad})) {
      out.push_back(particle);
    }
  }
  return out;
}
//----------------------------------------------------------------------------------------------

//__Get PythiaGenerator Specifications__________________________________________________________
const Analysis::SimSettingList PythiaGenerator::GetSpecification() const {
  return {};
}
//----------------------------------------------------------------------------------------------

} /* namespace Physics */ //////////////////////////////////////////////////////////////////////

} } /* namespace MATHUSLA::MU */
