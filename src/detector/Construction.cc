#include "detector/Construction.hh"

#include "Geant4/G4Box.hh"
#include "Geant4/G4GeometryManager.hh"
#include "Geant4/G4GeometryTolerance.hh"
#include "Geant4/G4LogicalVolumeStore.hh"
#include "Geant4/G4PhysicalVolumeStore.hh"
#include "Geant4/G4SDManager.hh"
#include "Geant4/G4Colour.hh"
#include "Geant4/G4SolidStore.hh"
#include "Geant4/G4PVPlacement.hh"

#include "detector/Earth.hh"
#include "detector/Prototype.hh"

namespace MATHUSLA { namespace MU {

G4Element*  Construction::Material::H   = nullptr;
G4Element*  Construction::Material::C   = nullptr;
G4Element*  Construction::Material::N   = nullptr;
G4Element*  Construction::Material::O   = nullptr;
G4Material* Construction::Material::Air = nullptr;

G4VPhysicalVolume* Construction::WorldVolume = nullptr;

void Construction::Material::Define() {
  Material::H = new G4Element("Hydrogen", "H", 1., 1.01*g/mole);
  Material::C = new G4Element("Carbon",   "C", 6., 12.01*g/mole);
  Material::N = new G4Element("Nitrogen", "N", 7., 14.01*g/mole);
  Material::O = new G4Element("Oxygen",   "O", 8., 16.00*g/mole);

  Material::Air = new G4Material("Air", 1.290*mg/cm3, 2);
  Material::Air->AddElement(Material::N, 0.7);
  Material::Air->AddElement(Material::O, 0.3);

  Earth::Material::Define();
  Prototype::Material::Define();

  G4cout << *G4Material::GetMaterialTable() << '\n';
}

G4VPhysicalVolume* Construction::Construct() {
  Material::Define();

  G4GeometryManager::GetInstance()->OpenGeometry();
  G4PhysicalVolumeStore::GetInstance()->Clean();
  G4LogicalVolumeStore::GetInstance()->Clean();
  G4SolidStore::GetInstance()->Clean();

  G4GeometryManager::GetInstance()->SetWorldMaximumExtent(WorldLength);

  G4cout << "Computed tolerance = "
         << G4GeometryTolerance::GetInstance()->GetSurfaceTolerance()/m
         << " m\n";

  auto worldLV = BoxVolume("World", WorldLength, WorldLength, WorldLength);

  Earth::Construct(worldLV);
  Prototype::Construct(worldLV);

  WorldVolume = PlaceVolume(worldLV, nullptr);
  return WorldVolume;
}

void Construction::ConstructSDandField() {
  auto detector = new Prototype();
  G4SDManager::GetSDMpointer()->AddNewDetector(detector);
}

const G4VisAttributes Construction::SensitiveAttributes() {
  auto attr = G4VisAttributes(G4Colour(0., 1., 0., 1.0));
  attr.SetForceSolid(true);
  return attr;
}

const G4VisAttributes Construction::CasingAttributes() {
  auto attr = G4VisAttributes(G4Colour(0., 0., 1., 0.2));
  attr.SetForceSolid(true);
  return attr;
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
                                      const G4VisAttributes& attr) {
  auto out = new G4LogicalVolume(solid, material, name);
  out->SetVisAttributes(attr);
  return out;
}

G4LogicalVolume* Construction::Volume(G4VSolid* solid,
                                      G4Material* material,
                                      const G4VisAttributes& attr) {
  return Volume(solid->GetName(), solid, material, attr);
}

G4LogicalVolume* Construction::Volume(const G4String& name,
                                      G4VSolid* solid,
                                      const G4VisAttributes& attr) {
  return Volume(name, solid, Material::Air, attr);
}

G4LogicalVolume* Construction::Volume(G4VSolid* solid,
                                      const G4VisAttributes& attr) {
  return Volume(solid, Material::Air, attr);
}

G4LogicalVolume* Construction::BoxVolume(const G4String& name,
                                         const G4double width,
                                         const G4double height,
                                         const G4double depth,
                                         G4Material* material,
                                         const G4VisAttributes& attr) {
  return Volume(
    new G4Box(name, 0.5 * width, 0.5 * height, 0.5 * depth),
    material, attr);
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


G4VPhysicalVolume* Construction::PlaceVolume(const G4String& name,
                                             G4LogicalVolume* current,
                                             const G4VisAttributes& attr,
                                             G4LogicalVolume* parent,
                                             const G4Transform3D& transform) {
  current->SetVisAttributes(attr);
  return PlaceVolume(name, current, parent, transform);
}

G4VPhysicalVolume* Construction::PlaceVolume(G4LogicalVolume* current,
                                             const G4VisAttributes& attr,
                                             G4LogicalVolume* parent,
                                             const G4Transform3D& transform) {
  current->SetVisAttributes(attr);
  return PlaceVolume(current, parent, transform);
}

G4VPhysicalVolume* Construction::PlaceVolume(const G4String& name,
                                             G4VSolid* solid,
                                             G4Material* material,
                                             const G4VisAttributes& attr,
                                             G4LogicalVolume* parent,
                                             const G4Transform3D& transform) {
  return PlaceVolume(name, Volume(solid, material, attr), parent, transform);
}

G4VPhysicalVolume* Construction::PlaceVolume(G4VSolid* solid,
                                             G4Material* material,
                                             const G4VisAttributes& attr,
                                             G4LogicalVolume* parent,
                                             const G4Transform3D& transform) {
  return PlaceVolume(Volume(solid, material, attr), parent, transform);
}

G4RotationMatrix Construction::Matrix(const G4double th1, const G4double phi1,
                                      const G4double th2, const G4double phi2,
                                      const G4double th3, const G4double phi3) {
  const G4double sinth1 = std::sin(th1);
  const G4double sinth2 = std::sin(th2);
  const G4double sinth3 = std::sin(th3);
  auto matrix = G4RotationMatrix();
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

} } /* namespace MATHUSLA::MU */
