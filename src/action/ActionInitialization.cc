/* src/action/ActionInitialization.cc
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

#include "action.hh"

namespace MATHUSLA { namespace MU {

namespace { ////////////////////////////////////////////////////////////////////////////////////
//__Generator Name______________________________________________________________________________
std::string _generator;
std::string _data_dir;
//----------------------------------------------------------------------------------------------
} /* anonymous namespace */ ////////////////////////////////////////////////////////////////////

//__Action Initialization Constructor___________________________________________________________
ActionInitialization::ActionInitialization(const std::string& generator,
                                           const std::string& data_dir)
    : G4VUserActionInitialization() {
  _generator = generator;
  _data_dir = data_dir;
}
//----------------------------------------------------------------------------------------------

//__Build for Thread Master_____________________________________________________________________
void ActionInitialization::BuildForMaster() const {
  SetUserAction(new RunAction(_data_dir));
}
//----------------------------------------------------------------------------------------------

//__Build for Threads___________________________________________________________________________
void ActionInitialization::Build() const {
  SetUserAction(new RunAction(_data_dir));
  SetUserAction(new EventAction(100));
  SetUserAction(new GeneratorAction(_generator));
}
//----------------------------------------------------------------------------------------------

} } /* namespace MATHUSLA::MU */
