#ifndef DETECTOR_CONSTRUCTION_HH
#define DETECTOR_CONSTRUCTION_HH
#pragma once

#include "Geant4/G4VUserDetectorConstruction.hh"
#include "Geant4/G4LogicalVolume.hh"
#include "Geant4/G4VPhysicalVolume.hh"
#include "Geant4/G4Transform3D.hh"
#include "Geant4/G4Trap.hh"
#include "Geant4/G4Material.hh"
#include "Geant4/G4SystemOfUnits.hh"

namespace MATHUSLA { namespace MU {

class Construction : public G4VUserDetectorConstruction {
 public:
  struct Material {
    static G4Element* H;
    static G4Element* C;
    static G4Element* N;
    static G4Element* O;
    static G4Material* Air;
   private:
    Material();
  };

  G4VPhysicalVolume* Construct();
  void ConstructSDandField();

  static G4Trap* Trap(const G4String& name,
                      const G4double height,
                      const G4double minwidth,
                      const G4double maxwidth,
                      const G4double depth);

  static G4LogicalVolume* Volume(const G4String& name,
                                 G4VSolid* solid,
                                 G4Material* material,
                                 const G4VisAttributes* attr=0);

  static G4LogicalVolume* Volume(const G4String& name,
                                 G4VSolid* solid,
                                 G4Material* material,
                                 const G4VisAttributes& attr);

  static G4LogicalVolume* Volume(G4VSolid* solid,
                                 G4Material* material,
                                 const G4VisAttributes* attr=0);

  static G4LogicalVolume* Volume(G4VSolid* solid,
                                 G4Material* material,
                                 const G4VisAttributes& attr);

  static G4VPhysicalVolume* PlaceVolume(const G4String& name,
                                        G4LogicalVolume* current,
                                        G4LogicalVolume* parent,
                                        const G4Transform3D& transform
                                            =G4Transform3D());

  static G4VPhysicalVolume* PlaceVolume(G4LogicalVolume* current,
                                        G4LogicalVolume* parent,
                                        const G4Transform3D& transform
                                            =G4Transform3D());

  static G4VPhysicalVolume* PlaceVolume(const G4String& name,
                                        G4VSolid* solid,
                                        G4Material* material,
                                        G4LogicalVolume* parent,
                                        const G4Transform3D& transform
                                            =G4Transform3D());

  static G4VPhysicalVolume* PlaceVolume(G4VSolid* solid,
                                        G4Material* material,
                                        G4LogicalVolume* parent,
                                        const G4Transform3D& transform
                                            =G4Transform3D());

  static G4RotationMatrix Matrix(const G4double th1,
                                 const G4double phi1,
                                 const G4double th2,
                                 const G4double phi2,
                                 const G4double th3,
                                 const G4double phi3);

  static G4Transform3D Transform(const G4ThreeVector& translate,
                                 const G4ThreeVector& axis,
                                 const G4double angle);

  static G4Transform3D Transform(const G4double x,
                                 const G4double y,
                                 const G4double z,
                                 const G4double axisx,
                                 const G4double axisy,
                                 const G4double axisz,
                                 const G4double angle);

  static G4Transform3D Rotate(const G4double axisx,
                              const G4double axisy,
                              const G4double axisz,
                              const G4double angle);

  static G4Transform3D SpecialTransformB(const G4double x,
                                         const G4double y,
                                         const G4double z);

 private:
  void DefineMaterials();
  G4VPhysicalVolume* DefineVolumes();
};

} } /* namespace MATHUSLA::MU */

#endif /* DETECTOR_CONSTRUCTION_HH */
