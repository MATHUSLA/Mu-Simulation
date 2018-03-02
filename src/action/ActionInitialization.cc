#include "action/ActionInitialization.hh"

#include "action/RunAction.hh"
#include "action/EventAction.hh"
#include "action/SteppingAction.hh"
#include "action/TrackingAction.hh"
#include "action/StackingAction.hh"

#include "physics/BasicGenerator.hh"
#include "physics/Pythia8Generator.hh"

namespace MATHUSLA { namespace MU {

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
  SetUserAction(new BasicGeneratorAction);
}

} } /* namespace MATHUSLA::MU */
