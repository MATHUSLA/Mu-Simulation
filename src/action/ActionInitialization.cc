#include "action/ActionInitialization.hh"

#include "action/RunAction.hh"
#include "action/EventAction.hh"
#include "action/PrimaryGeneratorAction.hh"
#include "action/SteppingAction.hh"
#include "action/TrackingAction.hh"
#include "action/StackingAction.hh"

namespace MATHUSLA { namespace MU {

ActionInitialization::ActionInitialization() : G4VUserActionInitialization() {}

void ActionInitialization::BuildForMaster() const {
  //SetUserAction(new RunAction);
}

void ActionInitialization::Build() const {
  auto manager = new HistoManager();
  auto run_action = new RunAction(manager);
  SetUserAction(run_action);
  SetUserAction(new EventAction(run_action, manager));
  SetUserAction(new PrimaryGeneratorAction);
  SetUserAction(new SteppingAction);
  SetUserAction(new TrackingAction);
  SetUserAction(new StackingAction);
}

} } /* namespace MATHUSLA::MU */
