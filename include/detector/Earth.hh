#ifndef MU__DETECTOR_EARTH_HH
#define MU__DETECTOR_EARTH_HH
#pragma once

#include "Geant4/G4Material.hh"
#include "Geant4/G4VPhysicalVolume.hh"
#include "Geant4/G4LogicalVolume.hh"
#include "Geant4/G4SystemOfUnits.hh"

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

constexpr double LayerWidthX    = 20000*cm;
constexpr double LayerWidthY    = 20000*cm;
constexpr double SandstoneDepth =  4530*cm;
constexpr double MarlDepth      =  1825*cm;
constexpr double MixDepth       =  3645*cm - 10*m;
constexpr double TotalDepth     = SandstoneDepth + MarlDepth + MixDepth;

G4VPhysicalVolume* Construct(G4LogicalVolume* world);

} /* namespace Earth */

} } /* namespace MATHUSLA::MU */

#endif /* MU__DETECTOR_EARTH_HH */
