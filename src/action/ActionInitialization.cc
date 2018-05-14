#include "action.hh"

namespace MATHUSLA { namespace MU {

namespace { ////////////////////////////////////////////////////////////////////////////////////
//__Generator Name______________________________________________________________________________
std::string _generator;
//----------------------------------------------------------------------------------------------
} /* anonymous namespace */ ////////////////////////////////////////////////////////////////////

//__Action Initialization Constructor___________________________________________________________
ActionInitialization::ActionInitialization(const std::string& generator)
    : G4VUserActionInitialization() {
  _generator = generator;
}
//----------------------------------------------------------------------------------------------

//__Build for Thread Master_____________________________________________________________________
void ActionInitialization::BuildForMaster() const {
  SetUserAction(new RunAction);
}
//----------------------------------------------------------------------------------------------

//__Build for Threads___________________________________________________________________________
void ActionInitialization::Build() const {
  SetUserAction(new RunAction);
  SetUserAction(new EventAction(100));
  SetUserAction(new GeneratorAction(_generator));
}
//----------------------------------------------------------------------------------------------

} } /* namespace MATHUSLA::MU */
