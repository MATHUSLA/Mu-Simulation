#ifndef DETECTOR_CONSTRUCTION_HH
#define DETECTOR_CONSTRUCTION_HH
#pragma once

#include "Geant4/G4VUserDetectorConstruction.hh"
#include "Geant4/G4LogicalVolume.hh"
#include "Geant4/G4VPhysicalVolume.hh"
#include "Geant4/G4VisAttributes.hh"
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
    static G4Element* F;
    static G4Element* S;
    static G4Element* Ar;
    static G4Material* Air;
    static G4Material* Aluminum;
    static void Define();
   private:
    Material();
  };

  constexpr static G4double WorldLength = 240*m;

  static G4VPhysicalVolume* WorldVolume;

  G4VPhysicalVolume* Construct();
  void ConstructSDandField();

  static const G4VisAttributes SensitiveAttributes();
  static const G4VisAttributes CasingAttributes();

  static G4Trap* Trap(const std::string& name,
                      const G4double height,
                      const G4double minwidth,
                      const G4double maxwidth,
                      const G4double depth);

  static G4LogicalVolume* Volume(const std::string& name,
                                 G4VSolid* solid,
                                 G4Material* material
                                   =Material::Air,
                                 const G4VisAttributes& attr
                                   =G4VisAttributes());

  static G4LogicalVolume* Volume(G4VSolid* solid,
                                 G4Material* material
                                   =Material::Air,
                                 const G4VisAttributes& attr
                                   =G4VisAttributes());

  static G4LogicalVolume* Volume(const std::string& name,
                                 G4VSolid* solid,
                                 const G4VisAttributes& attr);

  static G4LogicalVolume* Volume(G4VSolid* solid,
                                 const G4VisAttributes& attr);

  static G4LogicalVolume* BoxVolume(const std::string& name,
                                    const G4double width,
                                    const G4double height,
                                    const G4double depth,
                                    G4Material* material
                                      =Material::Air,
                                    const G4VisAttributes& attr
                                      =G4VisAttributes());

  static G4LogicalVolume* OpenBoxVolume(const std::string& name,
                                        const G4double width,
                                        const G4double height,
                                        const G4double depth,
                                        const G4double thickness,
                                        G4Material* material
                                          =Material::Air,
                                        const G4VisAttributes& attr
                                          =G4VisAttributes());

  static G4VPhysicalVolume* PlaceVolume(const std::string& name,
                                        G4LogicalVolume* current,
                                        G4LogicalVolume* parent,
                                        const G4Transform3D& transform
                                            =G4Transform3D());

  static G4VPhysicalVolume* PlaceVolume(G4LogicalVolume* current,
                                        G4LogicalVolume* parent,
                                        const G4Transform3D& transform
                                            =G4Transform3D());

  static G4VPhysicalVolume* PlaceVolume(const std::string& name,
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

  static G4VPhysicalVolume* PlaceVolume(const std::string& name,
                                        G4LogicalVolume* current,
                                        const G4VisAttributes& attr,
                                        G4LogicalVolume* parent,
                                        const G4Transform3D& transform
                                            =G4Transform3D());

  static G4VPhysicalVolume* PlaceVolume(G4LogicalVolume* current,
                                        const G4VisAttributes& attr,
                                        G4LogicalVolume* parent,
                                        const G4Transform3D& transform
                                            =G4Transform3D());

  static G4VPhysicalVolume* PlaceVolume(const std::string& name,
                                        G4VSolid* solid,
                                        G4Material* material,
                                        const G4VisAttributes& attr,
                                        G4LogicalVolume* parent,
                                        const G4Transform3D& transform
                                            =G4Transform3D());

  static G4VPhysicalVolume* PlaceVolume(G4VSolid* solid,
                                        G4Material* material,
                                        const G4VisAttributes& attr,
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

  static void Export(const G4LogicalVolume* volume,
                     const std::string& file,
                     const std::string& schema="");

  static void Export(const G4VPhysicalVolume* volume,
                     const std::string& file,
                     const std::string& schema="");
};

} } /* namespace MATHUSLA::MU */

#endif /* DETECTOR_CONSTRUCTION_HH */
