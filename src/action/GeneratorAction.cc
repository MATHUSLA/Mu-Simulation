#include "action.hh"

#include <unordered_map>

#include "physics/PythiaGenerator.hh"
#include "physics/Units.hh"

namespace MATHUSLA { namespace MU {

namespace { ////////////////////////////////////////////////////////////////////////////////////
G4ThreadLocal std::string _generators;
Generator* _gen;
G4ThreadLocal std::unordered_map<std::string, Generator*> _gen_map;
//----------------------------------------------------------------------------------------------
} /* anonymous namespace */ ////////////////////////////////////////////////////////////////////

GeneratorAction::GeneratorAction(const std::string& generator)
    : G4VUserPrimaryGeneratorAction(),
      G4UImessenger(Generator::MessengerDirectory, "Particle Generators.") {

  _gen_map["basic"] = new Generator(
      "basic", "Default Generator.",
      13, 60*GeVperC, 0, 0*deg);

  _gen_map["range"] = new RangeGenerator(
      "range", "Default Range Generator.",
      13, 60*GeVperC, 0.1, 5*deg);

  _gen_map["pythia"] = new PythiaGenerator(
      -13, 60*GeVperC, 0.5, 10*deg, {
          "Print:quiet = on",
          "Next:numberCount = 10000",
          "Stat:showErrors = off",
          "Beams:eCM = 13000.",
          "WeakSingleBoson:ffbar2W = on",
          "24:onMode = off",
          "24:onIfAny = 13"
      });

  _generators.clear();
  for (const auto& element : _gen_map) {
    _generators += element.first + " ";
  }

  SetGenerator(generator);

  _select = CreateCommand<Command::StringArg>("select", "Select Generator.");
  _select->SetParameterName("generator", false);
  _select->SetDefaultValue("basic");
  _select->SetCandidates(_generators.c_str());
  _select->AvailableForStates(G4State_PreInit, G4State_Idle);

  _list = CreateCommand<Command::NoArg>("list", "List Avaliable Generators.");
  _list->AvailableForStates(G4State_PreInit, G4State_Idle);

  _current = CreateCommand<Command::NoArg>("current", "Current Generator.");
  _current->AvailableForStates(G4State_PreInit, G4State_Idle);
}
//----------------------------------------------------------------------------------------------

void GeneratorAction::GeneratePrimaries(G4Event* event) {
  _gen->GeneratePrimaryVertex(event);
}
//----------------------------------------------------------------------------------------------

void GeneratorAction::SetNewValue(G4UIcommand* command, G4String value) {
  if (command == _select) {
    SetGenerator(value);
  } else if (command == _list) {
    std::cout << "Generators: \n";
    for (const auto& element : _gen_map) {
      const auto& gen = element.second;
      std::cout << element.first << ": \n  "
                << "pT "  << G4BestUnit(gen->pT(), "Momentum") << "  "
                << "eta " << gen->eta()                        << "  "
                << "phi " << G4BestUnit(gen->phi(), "Angle")   << "\n\n";
    }
  } else if (command == _current) {
    std::cout << "Current Generator: \n  " << _gen->GetName() << "\n\n";
  }
}
//----------------------------------------------------------------------------------------------

void GeneratorAction::SetGenerator(const std::string& generator) {
  const auto& search = _gen_map.find(generator);
  _gen = (search != _gen_map.end()) ? search->second : _gen_map["basic"];
}
//----------------------------------------------------------------------------------------------

const Generator* GeneratorAction::GetGenerator() {
  return _gen;
}
//----------------------------------------------------------------------------------------------

} } /* namespace MATHUSLA::MU */
