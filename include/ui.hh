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
#include "Geant4/G4UIcmdWithABool.hh"
#include "Geant4/G4UIcmdWithAnInteger.hh"
#include "Geant4/G4UIcmdWithADouble.hh"
#include "Geant4/G4UIcmdWithAString.hh"
#include "Geant4/G4UIcmdWith3Vector.hh"
#include "Geant4/G4UIcmdWithADoubleAndUnit.hh"
#include "Geant4/G4UIcmdWith3VectorAndUnit.hh"
#include "Geant4/G4UImessenger.hh"

namespace MATHUSLA { namespace MU {

namespace Vis { ////////////////////////////////////////////////////////////////////////////////

inline bool Draw(const G4VMarker& marker) {
  auto pVVisManager = G4VVisManager::GetConcreteInstance();
  if (pVVisManager) {
    pVVisManager->Draw(marker);
    return true;
  }
  return false;
}
//----------------------------------------------------------------------------------------------

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

inline G4Circle Circle(const G4ThreeVector& point,
                       const double size,
                       const G4VisAttributes& attr) {
  return Circle(point, size, G4VMarker::FillStyle::filled, attr);
}
//----------------------------------------------------------------------------------------------

inline G4Circle Circle(const G4ThreeVector& point,
                       const double size,
                       const G4Color& color) {
  return Circle(point, size, G4VisAttributes(color));
}
//----------------------------------------------------------------------------------------------

} /* namespace Vis */ //////////////////////////////////////////////////////////////////////////

namespace Command { ////////////////////////////////////////////////////////////////////////////

using NoArg              = G4UIcmdWithoutParameter;
using BoolArg            = G4UIcmdWithABool;
using IntegerArg         = G4UIcmdWithAnInteger;
using DoubleArg          = G4UIcmdWithADouble;
using StringArg          = G4UIcmdWithAString;
using ThreeVectorArg     = G4UIcmdWith3Vector;
using DoubleUnitArg      = G4UIcmdWithADoubleAndUnit;
using ThreeVectorUnitArg = G4UIcmdWith3VectorAndUnit;
//----------------------------------------------------------------------------------------------

} /* namespace Command */ //////////////////////////////////////////////////////////////////////

} } /* namespace MATHUSLA::MU */

#endif /* MU__UI_HH */