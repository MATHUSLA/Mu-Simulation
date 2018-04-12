#ifndef MU__UI_VIS_HH
#define MU__UI_VIS_HH
#pragma once

#include "Geant4/G4VMarker.hh"
#include "Geant4/G4Circle.hh"
#include "Geant4/G4VisAttributes.hh"
#include "Geant4/G4VVisManager.hh"
#include "Geant4/G4ThreeVector.hh"

namespace MATHUSLA { namespace MU {

namespace VIS {

inline bool Draw(const G4VMarker& marker) {
  auto pVVisManager = G4VVisManager::GetConcreteInstance();
  if (pVVisManager) {
    pVVisManager->Draw(marker);
    return true;
  }
  return false;
}

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

inline G4Circle Circle(const G4ThreeVector& point,
                       const double size,
                       const G4VisAttributes& attr) {
  return Circle(point, size, G4VMarker::FillStyle::filled, attr);
}

inline G4Circle Circle(const G4ThreeVector& point,
                       const double size,
                       const G4Color& color) {
  return Circle(point, size, G4VisAttributes(color));
}

} /* namespace VIS */

} } /* namespace MATHUSLA::MU */

#endif /* MU__UI_VIS_HH */
