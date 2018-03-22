#ifndef UI_VIS_HH
#define UI_VIS_HH
#pragma once

#include "Geant4/G4VMarker.hh"
#include "Geant4/G4Circle.hh"
#include "Geant4/G4VisAttributes.hh"
#include "Geant4/G4VVisManager.hh"
#include "Geant4/G4ThreeVector.hh"

namespace MATHUSLA { namespace MU {

namespace VIS {

bool Draw(const G4VMarker& marker);

G4Circle Circle(const G4ThreeVector& point,
                const G4double size,
                const G4VMarker::FillStyle fill=G4VMarker::FillStyle::filled,
                const G4VisAttributes& attr=G4VisAttributes());

G4Circle Circle(const G4ThreeVector& point,
                const G4double size,
                const G4VisAttributes& attr);

G4Circle Circle(const G4ThreeVector& point,
                const G4double size,
                const G4Color& color);

}

} } /* namespace MATHUSLA::MU */

#endif /* UI_VIS_HH */
