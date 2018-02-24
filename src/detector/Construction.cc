#include "detector/Construction.hh"

#include "Geant4/G4Box.hh"
#include "Geant4/G4GeometryManager.hh"
#include "Geant4/G4GeometryTolerance.hh"
#include "Geant4/G4LogicalVolume.hh"
#include "Geant4/G4LogicalVolumeStore.hh"
#include "Geant4/G4Material.hh"
#include "Geant4/G4PhysicalVolumeStore.hh"
#include "Geant4/G4SDManager.hh"
#include "Geant4/G4SolidStore.hh"
#include "Geant4/G4PVPlacement.hh"

#include "detector/Earth.hh"
#include "detector/TrapezoidCalorimeter.hh"

namespace MATHUSLA { namespace MU {

auto Construction::Material::H
  = new G4Element("Hydrogen", "H", 1., 1.01*g/mole);
auto Construction::Material::C
  = new G4Element("Carbon",   "C", 6., 12.01*g/mole);
auto Construction::Material::N
  = new G4Element("Nitrogen", "N", 7., 14.01*g/mole);
auto Construction::Material::O
  = new G4Element("Oxygen",   "O", 8., 16.00*g/mole);

G4Material* Construction::Material::Air = 0;

G4VPhysicalVolume* Construction::Construct() {
  DefineMaterials();
  return DefineVolumes();
}

void Construction::DefineMaterials() {
  Material::Air = new G4Material("Air", 1.290*mg/cm3, 2);
  Material::Air->AddElement(Construction::Material::N, 0.7);
  Material::Air->AddElement(Construction::Material::O, 0.3);

  Earth::DefineMaterials();
  TrapezoidCalorimeter::DefineMaterials();

  G4cout << *(G4Material::GetMaterialTable()) << "\n";
}

G4VPhysicalVolume* Construction::DefineVolumes() {
  G4GeometryManager::GetInstance()->OpenGeometry();
  G4PhysicalVolumeStore::GetInstance()->Clean();
  G4LogicalVolumeStore::GetInstance()->Clean();
  G4SolidStore::GetInstance()->Clean();

  G4cout << "Computed tolerance = "
         << G4GeometryTolerance::GetInstance()->GetSurfaceTolerance()/mm
         << " mm\n";

  constexpr G4double worldlen = 12000.0;

  G4GeometryManager::GetInstance()->SetWorldMaximumExtent(worldlen);

  auto worldLV = Volume(
    new G4Box("World", worldlen*cm, worldlen*cm, worldlen*cm),
    Material::Air);

  auto worldPV = PlaceVolume(worldLV, nullptr);

  Earth::Construct(worldLV);
  TrapezoidCalorimeter::Construct(worldLV);

  return worldPV;
}

void Construction::ConstructSDandField() {
  auto detector = new TrapezoidCalorimeter();
  G4SDManager::GetSDMpointer()->AddNewDetector(detector);
}

G4Trap* Construction::Trap(const G4String& name,
                           const G4double height,
                           const G4double minwidth,
                           const G4double maxwidth,
                           const G4double depth) {
  return new G4Trap(name,
    0.5 * height, 0, 0,
    0.5 * depth, 0.5 * minwidth, 0.5 * minwidth, 0,
    0.5 * depth, 0.5 * maxwidth, 0.5 * maxwidth, 0);
}

G4LogicalVolume* Construction::Volume(const G4String& name,
                                      G4VSolid* solid,
                                      G4Material* material,
                                      const G4VisAttributes* attr) {
  auto out = new G4LogicalVolume(solid, material, name);
  out->SetVisAttributes(attr);
  return out;
}

G4LogicalVolume* Construction::Volume(const G4String& name,
                                      G4VSolid* solid,
                                      G4Material* material,
                                      const G4VisAttributes& attr) {
  auto out = new G4LogicalVolume(solid, material, name);
  out->SetVisAttributes(attr);
  return out;
}

G4LogicalVolume* Construction::Volume(G4VSolid* solid,
                                      G4Material* material,
                                      const G4VisAttributes* attr) {
  return Volume(solid->GetName(), solid, material, attr);
}

G4LogicalVolume* Construction::Volume(G4VSolid* solid,
                                      G4Material* material,
                                      const G4VisAttributes& attr) {
  return Volume(solid->GetName(), solid, material, attr);
}

G4VPhysicalVolume* Construction::PlaceVolume(const G4String& name,
                                             G4LogicalVolume* current,
                                             G4LogicalVolume* parent,
                                             const G4Transform3D& transform) {
  return new G4PVPlacement(transform, current, name, parent, false, 0);
}

G4VPhysicalVolume* Construction::PlaceVolume(G4LogicalVolume* current,
                                             G4LogicalVolume* parent,
                                             const G4Transform3D& transform) {
  return PlaceVolume(current->GetName(), current, parent, transform);
}

G4VPhysicalVolume* Construction::PlaceVolume(const G4String& name,
                                             G4VSolid* solid,
                                             G4Material* material,
                                             G4LogicalVolume* parent,
                                             const G4Transform3D& transform) {
  return PlaceVolume(name, Volume(name, solid, material), parent, transform);
}

G4VPhysicalVolume* Construction::PlaceVolume(G4VSolid* solid,
                                             G4Material* material,
                                             G4LogicalVolume* parent,
                                             const G4Transform3D& transform) {
  return PlaceVolume(solid->GetName(), solid, material, parent, transform);
}

G4RotationMatrix Construction::Matrix(const G4double th1, const G4double phi1,
                                      const G4double th2, const G4double phi2,
                                      const G4double th3, const G4double phi3) {
  const G4double sinth1 = std::sin(th1);
  const G4double sinth2 = std::sin(th2);
  const G4double sinth3 = std::sin(th3);
  G4RotationMatrix matrix = G4RotationMatrix();

  matrix.rotateAxes(
    G4ThreeVector(sinth1*std::cos(phi1), sinth1*std::sin(phi1), std::cos(th1)),
    G4ThreeVector(sinth2*std::cos(phi2), sinth2*std::sin(phi2), std::cos(th2)),
    G4ThreeVector(sinth3*std::cos(phi3), sinth3*std::sin(phi3), std::cos(th3)));

  if (matrix != G4RotationMatrix()) matrix.invert();
  return matrix;
}

G4Transform3D Construction::Transform(const G4ThreeVector& translate,
                                      const G4ThreeVector& axis,
                                      const G4double angle) {
  return G4Transform3D(G4RotationMatrix(axis, angle), translate);
}

G4Transform3D Construction::Transform(const G4double x,
                                      const G4double y,
                                      const G4double z,
                                      const G4double axisx,
                                      const G4double axisy,
                                      const G4double axisz,
                                      const G4double angle) {
  return Transform(
    G4ThreeVector(x, y, z), G4ThreeVector(axisx, axisy, axisz), angle);
}

G4Transform3D Construction::Rotate(const G4double axisx,
                                   const G4double axisy,
                                   const G4double axisz,
                                   const G4double angle) {
  return Transform(0, 0, 0, axisx, axisy, axisz, angle);
}

G4Transform3D Construction::SpecialTransformB(const G4double x,
                                              const G4double y,
                                              const G4double z)  {
  return G4Transform3D(
    Matrix(90*deg, 0*deg, 0*deg, 0*deg, 90*deg, 270*deg),
    G4ThreeVector(x, y, z));
}

} } /* namespace MATHUSLA::MU */
