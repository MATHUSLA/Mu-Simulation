#ifndef DETECTOR_EARTH_HH
#define DETECTOR_EARTH_HH
#pragma once

#include "Geant4/G4Material.hh"
#include "Geant4/G4VPhysicalVolume.hh"
#include "Geant4/G4LogicalVolume.hh"

namespace MATHUSLA { namespace MU {

namespace Earth {

struct Material {
  static G4Material* CaCO3;
  static G4Material* Kaolinite;
  static G4Material* SiO2;
  static G4Material* Marl;
  static G4Material* Mix;
  static void Define();
 private:
  Material();
};

G4VPhysicalVolume* Construct(G4LogicalVolume*);

} /* namespace Earth */

} } /* namespace MATHUSLA::MU */

#endif /* DETECTOR_EARTH_HH */
