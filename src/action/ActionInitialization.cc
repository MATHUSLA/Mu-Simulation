#include "action/ActionInitialization.hh"

#include "action/RunAction.hh"
#include "action/EventAction.hh"
#include "action/SteppingAction.hh"
#include "action/TrackingAction.hh"
#include "action/StackingAction.hh"

#include "physics/BasicGenerator.hh"
#include "physics/Pythia8Generator.hh"

namespace MATHUSLA { namespace MU {

ActionInitialization::ActionInitialization(const G4String& generator)
    : G4VUserActionInitialization(), _generator(generator) {}

void ActionInitialization::BuildForMaster() const {
  SetUserAction(new RunAction);
}

void ActionInitialization::Build() const {
  auto run_action = new RunAction;
  SetUserAction(run_action);
  SetUserAction(new EventAction(run_action));
  SetUserAction(new SteppingAction);
  SetUserAction(new TrackingAction);
  SetUserAction(new StackingAction);

  G4VUserPrimaryGeneratorAction* generator_action;
  if (_generator == "pythia" || _generator == "pythia8") {
    generator_action = new Pythia8GeneratorAction;
  } else {
    generator_action = new BasicGeneratorAction;
  }

  SetUserAction(generator_action);
}

} } /* namespace MATHUSLA::MU */
