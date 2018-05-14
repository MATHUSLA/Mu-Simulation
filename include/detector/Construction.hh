#ifndef MU__DETECTOR_CONSTRUCTION_HH
#define MU__DETECTOR_CONSTRUCTION_HH
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

namespace Construction { ///////////////////////////////////////////////////////////////////////

constexpr auto WorldLength = 240*m;

const G4VPhysicalVolume* WorldVolume();

namespace Material { ///////////////////////////////////////////////////////////////////////////
extern G4Element* H;
extern G4Element* C;
extern G4Element* N;
extern G4Element* O;
extern G4Element* F;
extern G4Element* S;
extern G4Element* Ar;
extern G4Material* Air;
extern G4Material* Aluminum;
} /* namespace Material */ /////////////////////////////////////////////////////////////////////

class Builder : public G4VUserDetectorConstruction {
public:
  Builder(const std::string& detector);
  G4VPhysicalVolume* Construct();
  void ConstructSDandField();
};

const G4VisAttributes SensitiveAttributes();
const G4VisAttributes CasingAttributes();

G4Trap* Trap(const std::string& name,
             const double height,
             const double minwidth,
             const double maxwidth,
             const double depth);

G4LogicalVolume* Volume(const std::string& name,
                        G4VSolid* solid,
                        G4Material* material=Material::Air,
                        const G4VisAttributes& attr=G4VisAttributes());

G4LogicalVolume* Volume(G4VSolid* solid,
                        G4Material* material=Material::Air,
                        const G4VisAttributes& attr=G4VisAttributes());

G4LogicalVolume* Volume(const std::string& name,
                        G4VSolid* solid,
                        const G4VisAttributes& attr);

G4LogicalVolume* Volume(G4VSolid* solid,
                        const G4VisAttributes& attr);

G4LogicalVolume* BoxVolume(const std::string& name,
                           const double width,
                           const double height,
                           const double depth,
                           G4Material* material=Material::Air,
                           const G4VisAttributes& attr=G4VisAttributes());

G4LogicalVolume* OpenBoxVolume(const std::string& name,
                               const double width,
                               const double height,
                               const double depth,
                               const double thickness,
                               G4Material* material=Material::Air,
                               const G4VisAttributes& attr=G4VisAttributes());

G4VPhysicalVolume* PlaceVolume(const std::string& name,
                               G4LogicalVolume* current,
                               G4LogicalVolume* parent,
                               const G4Transform3D& transform=G4Transform3D());

G4VPhysicalVolume* PlaceVolume(G4LogicalVolume* current,
                               G4LogicalVolume* parent,
                               const G4Transform3D& transform=G4Transform3D());

G4VPhysicalVolume* PlaceVolume(const std::string& name,
                               G4VSolid* solid,
                               G4Material* material,
                               G4LogicalVolume* parent,
                               const G4Transform3D& transform=G4Transform3D());

G4VPhysicalVolume* PlaceVolume(G4VSolid* solid,
                               G4Material* material,
                               G4LogicalVolume* parent,
                               const G4Transform3D& transform=G4Transform3D());

G4VPhysicalVolume* PlaceVolume(const std::string& name,
                               G4LogicalVolume* current,
                               const G4VisAttributes& attr,
                               G4LogicalVolume* parent,
                               const G4Transform3D& transform=G4Transform3D());

G4VPhysicalVolume* PlaceVolume(G4LogicalVolume* current,
                               const G4VisAttributes& attr,
                               G4LogicalVolume* parent,
                               const G4Transform3D& transform=G4Transform3D());

G4VPhysicalVolume* PlaceVolume(const std::string& name,
                               G4VSolid* solid,
                               G4Material* material,
                               const G4VisAttributes& attr,
                               G4LogicalVolume* parent,
                               const G4Transform3D& transform=G4Transform3D());

G4VPhysicalVolume* PlaceVolume(G4VSolid* solid,
                               G4Material* material,
                               const G4VisAttributes& attr,
                               G4LogicalVolume* parent,
                               const G4Transform3D& transform=G4Transform3D());

G4RotationMatrix Matrix(const double th1,
                        const double phi1,
                        const double th2,
                        const double phi2,
                        const double th3,
                        const double phi3);

G4Transform3D Transform(const G4ThreeVector& translate,
                        const G4ThreeVector& axis,
                        const double angle);

G4Transform3D Transform(const double x,
                        const double y,
                        const double z,
                        const double axisx,
                        const double axisy,
                        const double axisz,
                        const double angle);

G4Transform3D Rotate(const double axisx,
                     const double axisy,
                     const double axisz,
                     const double angle);

void Export(const G4LogicalVolume* volume,
            const std::string& file,
            const std::string& schema="");

void Export(const G4VPhysicalVolume* volume,
            const std::string& file,
            const std::string& schema="");

} /* namespace Construction */ /////////////////////////////////////////////////////////////////

} } /* namespace MATHUSLA::MU */

#endif /* MU__DETECTOR_CONSTRUCTION_HH */
