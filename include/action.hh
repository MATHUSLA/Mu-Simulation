/*
 * include/action.hh
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

#ifndef MU__ACTION_HH
#define MU__ACTION_HH
#pragma once

#include <Geant4/G4VUserActionInitialization.hh>
#include <Geant4/G4UserEventAction.hh>
#include <Geant4/G4UserRunAction.hh>
#include <Geant4/G4VUserPrimaryGeneratorAction.hh>
#include <Geant4/G4Event.hh>
#include <Geant4/G4Run.hh>

#include "physics/Generator.hh"
#include "ui.hh"

namespace MATHUSLA { namespace MU {

//__Geant4 Action Initializer___________________________________________________________________
class ActionInitialization : public G4VUserActionInitialization {
public:
  ActionInitialization(const std::string& generator="",
                       const std::string& data_dir="");
  void BuildForMaster() const;
  void Build() const;
};
//----------------------------------------------------------------------------------------------

//__Event Action Manager________________________________________________________________________
class EventAction : public G4UserEventAction {
public:
  EventAction(const size_t print_modulo);
  void BeginOfEventAction(const G4Event* event);
  static size_t EventID();
};
//----------------------------------------------------------------------------------------------

//__Run Action Manager__________________________________________________________________________
class RunAction : public G4UserRunAction {
public:
  RunAction(const std::string& data_dir="");
  void BeginOfRunAction(const G4Run* run);
  void EndOfRunAction(const G4Run*);
  static size_t RunID();
  static size_t EventCount();
};
//----------------------------------------------------------------------------------------------

//__Generator Action Manager____________________________________________________________________
class GeneratorAction : public G4VUserPrimaryGeneratorAction, public G4UImessenger {
public:
  GeneratorAction(const std::string& generator="");
  void GeneratePrimaries(G4Event* event);
  void SetNewValue(G4UIcommand* command, G4String value);
  static const Physics::Generator* GetGenerator();
  static void SetGenerator(const std::string& generator);

private:
  Command::NoArg*     _list;
  Command::NoArg*     _current;
  Command::StringArg* _select;
};
//----------------------------------------------------------------------------------------------

} } /* namespace MATHUSLA::MU */

#endif /* MU__ACTION_HH */
