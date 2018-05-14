#ifndef MU__DETECTOR_EARTH_HH
#define MU__DETECTOR_EARTH_HH
#pragma once

#include "Geant4/G4Material.hh"
#include "Geant4/G4VPhysicalVolume.hh"
#include "Geant4/G4LogicalVolume.hh"
#include "Geant4/G4SystemOfUnits.hh"

namespace MATHUSLA { namespace MU {

namespace Earth { //////////////////////////////////////////////////////////////////////////////

//__Earth Materials_____________________________________________________________________________
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
//----------------------------------------------------------------------------------------------

//__Earth Layer Sizes___________________________________________________________________________
constexpr auto LayerWidthX    = 20000*cm;
constexpr auto LayerWidthY    = 20000*cm;
constexpr auto SandstoneDepth =  4530*cm;
constexpr auto MarlDepth      =  1825*cm;
constexpr auto MixDepth       =  3645*cm - 10*m;
constexpr auto TotalDepth     = SandstoneDepth + MarlDepth + MixDepth;
//----------------------------------------------------------------------------------------------

//__Construct Earth_____________________________________________________________________________
G4VPhysicalVolume* Construct(G4LogicalVolume* world);
//----------------------------------------------------------------------------------------------

} /* namespace Earth */ ////////////////////////////////////////////////////////////////////////

} } /* namespace MATHUSLA::MU */

#endif /* MU__DETECTOR_EARTH_HH */
