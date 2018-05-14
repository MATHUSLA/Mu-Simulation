#ifndef MU__ACTION_HH
#define MU__ACTION_HH
#pragma once

#include "Geant4/G4VUserActionInitialization.hh"
#include "Geant4/G4UserEventAction.hh"
#include "Geant4/G4UserRunAction.hh"
#include "Geant4/G4VUserPrimaryGeneratorAction.hh"
#include "Geant4/G4Event.hh"
#include "Geant4/G4Run.hh"

#include "physics/Generator.hh"
#include "ui.hh"

namespace MATHUSLA { namespace MU {

class ActionInitialization : public G4VUserActionInitialization {
public:
  ActionInitialization(const std::string& generator="");
  void BuildForMaster() const;
  void Build() const;
private:
  std::string _generator;
};

////////////////////////////////////////////////////////////////////////////////////////////////

class EventAction : public G4UserEventAction {
public:
  EventAction(const int print_modulo);
  void BeginOfEventAction(const G4Event* event);
  void EndOfEventAction(const G4Event* event);
};

////////////////////////////////////////////////////////////////////////////////////////////////

class RunAction : public G4UserRunAction {
public:
  void BeginOfRunAction(const G4Run* run);
  void EndOfRunAction(const G4Run* run);
};

////////////////////////////////////////////////////////////////////////////////////////////////

class GeneratorAction : public G4VUserPrimaryGeneratorAction, public G4UImessenger {
public:
  GeneratorAction(const std::string& generator="");

  void GeneratePrimaries(G4Event* event);
  void SetNewValue(G4UIcommand* command, G4String value);
  void SetGenerator(const std::string& generator);

  static const Generator* GetGenerator();

private:
  Command::NoArg*     _list;
  Command::NoArg*     _current;
  Command::StringArg* _select;
  static Generator*   _gen;
};

} } /* namespace MATHUSLA::MU */

#endif /* MU__ACTION_HH */
