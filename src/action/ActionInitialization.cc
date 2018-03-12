#include "action/ActionInitialization.hh"

#include "action/RunAction.hh"
#include "action/EventAction.hh"
#include "action/SteppingAction.hh"
#include "action/TrackingAction.hh"
#include "action/StackingAction.hh"

#include "physics/BasicGenerator.hh"
#include "physics/PythiaGenerator.hh"

namespace MATHUSLA { namespace MU {

ActionInitialization::ActionInitialization(const G4String& generator)
    : G4VUserActionInitialization(), _generator(generator) {}

void ActionInitialization::BuildForMaster() const {
  SetUserAction(new RunAction);
}

void ActionInitialization::Build() const {
  SetUserAction(new RunAction);
  SetUserAction(new EventAction(100));
  SetUserAction(new SteppingAction);
  SetUserAction(new TrackingAction);
  SetUserAction(new StackingAction);

  G4VUserPrimaryGeneratorAction* generator_action;
  if (_generator == "pythia" || _generator == "pythia8") {
    generator_action = new PythiaGeneratorAction;
  } else {
    generator_action = new BasicGeneratorAction;
  }

  SetUserAction(generator_action);
}

} } /* namespace MATHUSLA::MU */
