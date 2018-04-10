#include "action/GeneratorAction.hh"

#include "physics/PythiaGenerator.hh"
#include "physics/Units.hh"

namespace MATHUSLA { namespace MU {

G4String GeneratorAction::_generators = "";
Generator* GeneratorAction::_gen = nullptr;
std::unordered_map<std::string, Generator*> GeneratorAction::_gen_map = {};

GeneratorAction::GeneratorAction(const G4String& generator)
    : G4VUserPrimaryGeneratorAction(),
      G4UImessenger(Generator::MessengerDirectory, "Particle Generators.") {

  _gen_map.insert({"basic",
    new Generator("basic", "Default Generator.",
      13, 60*GeVperC, 0, 0*deg
    )});

  _gen_map.insert({"range",
    new RangeGenerator("range", "Default Range Generator.",
      13, 60*GeVperC, 0.1, 5*deg
    )});

  _gen_map.insert({"pythia",
    new PythiaGenerator(
      -13, 60*GeVperC, 0.5, 10*deg, {
      "Print:quiet = on",
      "Next:numberCount = 10000",
      "Stat:showErrors = off",
      "Beams:eCM = 13000.",
      "WeakSingleBoson:ffbar2W = on",
      "24:onMode = off",
      "24:onIfAny = 13"
    })});

  for (const auto& element : _gen_map) {
    _generators += element.first + " ";
  }

  SetGenerator(generator);

  _select = CreateCommand<G4CMD_String>("select", "Select Generator.");
  _select->SetParameterName("generator", false);
  _select->SetDefaultValue("basic");
  _select->SetCandidates(_generators);
  _select->AvailableForStates(G4State_PreInit, G4State_Idle);

  _list = CreateCommand<G4CMD_NoArg>("list", "List Avaliable Generators.");
  _list->AvailableForStates(G4State_PreInit, G4State_Idle);

  _current = CreateCommand<G4CMD_NoArg>("current", "Current Generator.");
  _current->AvailableForStates(G4State_PreInit, G4State_Idle);
}

void GeneratorAction::GeneratePrimaries(G4Event* event) {
  _gen->GeneratePrimaryVertex(event);
}

void GeneratorAction::SetNewValue(G4UIcommand* command, G4String value) {
  if (command == _select) {
    SetGenerator(value);
  } else if (command == _list) {
    G4cout << "Generators: \n";
    for (const auto& element : _gen_map) {
      const auto& gen = element.second;
      G4cout << element.first << ": \n  "
             << "pT "  << G4BestUnit(gen->pT(), "Momentum") << "  "
             << "eta " << gen->eta()                        << "  "
             << "phi " << G4BestUnit(gen->phi(), "Angle")   << "\n\n";
    }
  } else if (command == _current) {
    G4cout << "Current Generator: \n  " << _gen->GetName() << "\n\n";
  }
}

void GeneratorAction::SetGenerator(const G4String& generator) {
  const auto search = _gen_map.find(generator);
  if (search != _gen_map.end()) {
    _gen = search->second;
  } else {
    _gen = _gen_map["basic"];
  }
}

} } /* namespace MATHUSLA::MU */
