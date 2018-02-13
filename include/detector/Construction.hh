#ifndef DETECTOR_CONSTRUCTION_HH
#define DETECTOR_CONSTRUCTION_HH
#pragma once

#include "Geant4/G4VUserDetectorConstruction.hh"
#include "Geant4/G4LogicalVolume.hh"
#include "Geant4/G4VPhysicalVolume.hh"
#include "Geant4/G4Material.hh"
#include "Geant4/G4Transform3D.hh"
#include "Geant4/G4SystemOfUnits.hh"

namespace MATHUSLA { namespace MU {

class Construction : public G4VUserDetectorConstruction {
 public:
  struct Material {
    static G4Element* H;
    static G4Element* C;
    static G4Element* N;
    static G4Element* O;
    static G4Element* Al;
    static G4Element* Si;
    static G4Element* Ca;
    static G4Material* Air;
   private:
    Material();
  };

  virtual G4VPhysicalVolume* Construct();
  virtual void ConstructSDandField();

  static G4RotationMatrix* Matrix(const G4double, const G4double,
                                  const G4double, const G4double,
                                  const G4double, const G4double);

  static G4LogicalVolume* Volume(const G4String&,
                                 G4VSolid*,
                                 G4Material*);

  static G4LogicalVolume* Volume(G4VSolid*,
                                 G4Material*);

  static G4VPhysicalVolume* PlaceVolume(const G4String&,
                                        G4LogicalVolume*,
                                        G4LogicalVolume*,
                                        const G4Transform3D& t=G4Transform3D());

  static G4VPhysicalVolume* PlaceVolume(G4LogicalVolume*,
                                        G4LogicalVolume*,
                                        const G4Transform3D& t=G4Transform3D());

  static G4VPhysicalVolume* PlaceVolume(const G4String&,
                                        G4VSolid*,
                                        G4Material*,
                                        G4LogicalVolume*,
                                        const G4Transform3D& t=G4Transform3D());

  static G4VPhysicalVolume* PlaceVolume(G4VSolid*,
                                        G4Material*,
                                        G4LogicalVolume*,
                                        const G4Transform3D& t=G4Transform3D());
 private:
  static void DefineMaterials();
  static G4VPhysicalVolume* DefineVolumes();
};

} } /* namespace MATHUSLA::MU */

#endif /* DETECTOR_CONSTRUCTION_HH */
