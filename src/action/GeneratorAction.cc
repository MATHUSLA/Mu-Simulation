#include "action.hh"

#include <unordered_map>

#include "physics/PythiaGenerator.hh"
#include "physics/Units.hh"

namespace MATHUSLA { namespace MU {

namespace { ////////////////////////////////////////////////////////////////////////////////////

//__Generator Map_______________________________________________________________________________
G4ThreadLocal std::unordered_map<std::string, Generator*> _gen_map;
//----------------------------------------------------------------------------------------------

//__Current Generator___________________________________________________________________________
Generator* _gen;
//----------------------------------------------------------------------------------------------

} /* anonymous namespace */ ////////////////////////////////////////////////////////////////////

//__Generator Action Constructor________________________________________________________________
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

//__Get the Current Generator___________________________________________________________________
const Generator* GeneratorAction::GetGenerator() {
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
