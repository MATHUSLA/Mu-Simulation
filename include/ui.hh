/* include/ui.hh
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

#ifndef MU__UI_HH
#define MU__UI_HH
#pragma once

#include "Geant4/G4VMarker.hh"
#include "Geant4/G4Circle.hh"
#include "Geant4/G4VisAttributes.hh"
#include "Geant4/G4VVisManager.hh"
#include "Geant4/G4ThreeVector.hh"
#include "Geant4/G4UIcommand.hh"
#include "Geant4/G4UIdirectory.hh"
#include "Geant4/G4UIcmdWithoutParameter.hh"
#include "Geant4/G4UIcmdWithAString.hh"
#include "Geant4/G4UIcmdWithABool.hh"
#include "Geant4/G4UIcmdWithAnInteger.hh"
#include "Geant4/G4UIcmdWithADouble.hh"
#include "Geant4/G4UIcmdWith3Vector.hh"
#include "Geant4/G4UIcmdWithADoubleAndUnit.hh"
#include "Geant4/G4UIcmdWith3VectorAndUnit.hh"
#include "Geant4/G4UImessenger.hh"
#include "Geant4/G4UImanager.hh"

namespace MATHUSLA { namespace MU {

namespace Command { ////////////////////////////////////////////////////////////////////////////

//__Geant4 UI Messenger Command Option Shortcut Definitions_____________________________________
using NoArg              = G4UIcmdWithoutParameter;
using StringArg          = G4UIcmdWithAString;
using BoolArg            = G4UIcmdWithABool;
using IntegerArg         = G4UIcmdWithAnInteger;
using DoubleArg          = G4UIcmdWithADouble;
using ThreeVectorArg     = G4UIcmdWith3Vector;
using DoubleUnitArg      = G4UIcmdWithADoubleAndUnit;
using ThreeVectorUnitArg = G4UIcmdWith3VectorAndUnit;
//----------------------------------------------------------------------------------------------

namespace detail { /////////////////////////////////////////////////////////////////////////////
//__Execute Geant4 Command Helper_______________________________________________________________
inline void Execute(G4UImanager* manager,
                    const std::string& command) {
  manager->ApplyCommand(command.c_str());
}
template<class ...String>
inline void Execute(G4UImanager* manager,
                    const std::string& command,
                    const String&... commands) {
  Execute(manager, command);
  Execute(manager, commands...);
}
//----------------------------------------------------------------------------------------------
} /* namespace detail */ ///////////////////////////////////////////////////////////////////////

//__Execute Geant4 Command______________________________________________________________________
template<class ...String>
inline void Execute(const std::string& command,
                    const String&... commands) {
  auto manager = G4UImanager::GetUIpointer();
  detail::Execute(manager, command, commands...);
}
//----------------------------------------------------------------------------------------------

} /* namespace Command */ //////////////////////////////////////////////////////////////////////

namespace Vis { ////////////////////////////////////////////////////////////////////////////////

//__Draw G4Marker_______________________________________________________________________________
inline bool Draw(const G4VMarker& marker) {
  auto pVVisManager = G4VVisManager::GetConcreteInstance();
  if (pVVisManager) {
    pVVisManager->Draw(marker);
    return true;
  }
  return false;
}
//----------------------------------------------------------------------------------------------

//__Circle Constructor__________________________________________________________________________
inline G4Circle Circle(const G4ThreeVector& point,
                       const double size,
                       const G4VMarker::FillStyle fill=G4VMarker::FillStyle::filled,
                       const G4VisAttributes& attr=G4VisAttributes()) {
  auto out = G4Circle(point);
  out.SetScreenSize(size);
  out.SetFillStyle(fill);
  out.SetVisAttributes(attr);
  return out;
}
//----------------------------------------------------------------------------------------------

//__Circle Constructor__________________________________________________________________________
inline G4Circle Circle(const G4ThreeVector& point,
                       const double size,
                       const G4VisAttributes& attr) {
  return Circle(point, size, G4VMarker::FillStyle::filled, attr);
}
//----------------------------------------------------------------------------------------------

//__Circle Constructor__________________________________________________________________________
inline G4Circle Circle(const G4ThreeVector& point,
                       const double size,
                       const G4Color& color) {
  return Circle(point, size, G4VisAttributes(color));
}
//----------------------------------------------------------------------------------------------

} /* namespace Vis */ //////////////////////////////////////////////////////////////////////////

} } /* namespace MATHUSLA::MU */

#endif /* MU__UI_HH */