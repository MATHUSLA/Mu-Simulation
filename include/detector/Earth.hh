#ifndef DETECTOR_EARTH_HH
#define DETECTOR_EARTH_HH
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

constexpr G4double LayerWidthX    = 20000*cm;
constexpr G4double LayerWidthY    = 20000*cm;
constexpr G4double SandstoneDepth =  4530*cm;
constexpr G4double MarlDepth      =  1825*cm;
constexpr G4double MixDepth       =  3645*cm - 10*m;
constexpr G4double TotalDepth     = SandstoneDepth + MarlDepth + MixDepth;

G4VPhysicalVolume* Construct(G4LogicalVolume* world);

} /* namespace Earth */

} } /* namespace MATHUSLA::MU */

#endif /* DETECTOR_EARTH_HH */
