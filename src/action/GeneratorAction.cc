/* src/action/GeneratorAction.cc
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

#include "action.hh"

#include <unordered_map>

#include <Geant4/tls.hh>

#include "physics/CORSIKAReaderGenerator.hh"
#include "physics/PythiaGenerator.hh"
#include "physics/HepMCGenerator.hh"
#include "physics/Units.hh"

namespace MATHUSLA { namespace MU {

namespace { ////////////////////////////////////////////////////////////////////////////////////

//__Generator Map_______________________________________________________________________________
G4ThreadLocal std::unordered_map<std::string, Physics::Generator*> _gen_map;
//----------------------------------------------------------------------------------------------

//__Current Generator___________________________________________________________________________
Physics::Generator* _gen;
//----------------------------------------------------------------------------------------------

} /* anonymous namespace */ ////////////////////////////////////////////////////////////////////

//__Generator Action Constructor________________________________________________________________
GeneratorAction::GeneratorAction(const std::string& generator)
    : G4VUserPrimaryGeneratorAction(),
      G4UImessenger(Physics::Generator::MessengerDirectory, "Particle Generators.") {

  _gen_map["basic"] = new Physics::Generator(
      "basic", "Default Generator.", {});

  _gen_map["range"] = new Physics::RangeGenerator(
      "range", "Default Range Generator.", {});

  _gen_map["pythia"] = new Physics::PythiaGenerator(
      {},
      {
          "Print:quiet = on",
          "Next:numberCount = 10000",
          "Stat:showErrors = off",
          "Beams:eCM = 13000.",
          "WeakSingleBoson:ffbar2W = on",
          "24:onMode = off",
          "24:onIfAny = 13"
      });

  _gen_map["hepmc"] = new Physics::HepMCGenerator({});

  _gen_map["corsica_reader"] = new Physics::CORSIKAReaderGenerator();

  std::string generators;
  for (const auto& element : _gen_map) {
    generators.append(element.first);
    generators.push_back(' ');
  }

  SetGenerator(generator);

  _select = CreateCommand<Command::StringArg>("select", "Select Generator.");
  _select->SetParameterName("generator", false);
  _select->SetDefaultValue("basic");
  _select->SetCandidates(generators.c_str());
  _select->AvailableForStates(G4State_PreInit, G4State_Idle);

  _list = CreateCommand<Command::NoArg>("list", "List Avaliable Generators.");
  _list->AvailableForStates(G4State_PreInit, G4State_Idle);

  _current = CreateCommand<Command::NoArg>("current", "Current Generator.");
  _current->AvailableForStates(G4State_PreInit, G4State_Idle);
}
//----------------------------------------------------------------------------------------------

//__Create Initial Vertex_______________________________________________________________________
void GeneratorAction::GeneratePrimaries(G4Event* event) {
  _gen->GeneratePrimaryVertex(event);
}
//----------------------------------------------------------------------------------------------

//__Generator Action Messenger Set Value________________________________________________________
void GeneratorAction::SetNewValue(G4UIcommand* command,
                                  G4String value) {
  if (command == _select) {
    SetGenerator(value);
  } else if (command == _list) {
    std::cout << "Generators: \n";
    for (const auto& element : _gen_map)
      std::cout << element.second << "\n";
  } else if (command == _current) {
    std::cout << "Current Generator: \n  " << _gen->name() << "\n\n";
  }
}
//----------------------------------------------------------------------------------------------

//__Get the Current Generator___________________________________________________________________
const Physics::Generator* GeneratorAction::GetGenerator() {
  return _gen;
}
//----------------------------------------------------------------------------------------------

//__Set the Current Generator___________________________________________________________________
void GeneratorAction::SetGenerator(const std::string& generator) {
  const auto& search = _gen_map.find(generator);
  _gen = (search != _gen_map.end()) ? search->second : _gen_map["basic"];
}
//----------------------------------------------------------------------------------------------

} } /* namespace MATHUSLA::MU */
