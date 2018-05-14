#include "action.hh"

namespace MATHUSLA { namespace MU {

ActionInitialization::ActionInitialization(const std::string& generator)
    : G4VUserActionInitialization(), _generator(generator) {}
//----------------------------------------------------------------------------------------------

void ActionInitialization::BuildForMaster() const {
  SetUserAction(new RunAction);
}
//----------------------------------------------------------------------------------------------

void ActionInitialization::Build() const {
  SetUserAction(new RunAction);
  SetUserAction(new EventAction(100));
  SetUserAction(new GeneratorAction(_generator));
}
//----------------------------------------------------------------------------------------------

} } /* namespace MATHUSLA::MU */
