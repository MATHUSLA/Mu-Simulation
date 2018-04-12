#ifndef MU__ACTION_GENERATORACTION_HH
#define MU__ACTION_GENERATORACTION_HH
#pragma once

#include <unordered_map>

#include "Geant4/G4VUserPrimaryGeneratorAction.hh"

#include "physics/Generator.hh"
#include "ui/Command.hh"

namespace MATHUSLA { namespace MU {

class GeneratorAction : public G4VUserPrimaryGeneratorAction, public G4UImessenger {
public:
  GeneratorAction(const G4String& generator="");

  void GeneratePrimaries(G4Event* event);
  void SetNewValue(G4UIcommand* command, G4String value);
  void SetGenerator(const G4String& generator);

  inline static const Generator* GetGenerator() { return _gen; }

private:
  static G4String _generators;
  static Generator* _gen;
  static std::unordered_map<std::string, Generator*> _gen_map;

  G4CMD_String* _select;
  G4CMD_NoArg*  _list;
  G4CMD_NoArg*  _current;
};

} } /* namespace MATHUSLA::MU */

#endif /* MU__ACTION_GENERATORACTION_HH */
