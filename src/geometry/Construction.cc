/* src/geometry/Construction.cc
 *
 * Copyright 2018 Brandon Gomes
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "geometry/Construction.hh"

#include "Geant4/G4SubtractionSolid.hh"
#include "Geant4/G4GeometryManager.hh"
#include "Geant4/G4GeometryTolerance.hh"
#include "Geant4/G4LogicalVolumeStore.hh"
#include "Geant4/G4PhysicalVolumeStore.hh"
#include "Geant4/G4SDManager.hh"
#include "Geant4/G4Colour.hh"
#include "Geant4/G4SolidStore.hh"
#include "Geant4/G4PVPlacement.hh"
#include "Geant4/G4NistManager.hh"
#include "Geant4/G4GDMLParser.hh"

#include "geometry/Earth.hh"
#include "geometry/Prototype.hh"
#include "geometry/Flat.hh"

#include "util/io.hh"

namespace MATHUSLA { namespace MU {

namespace { ////////////////////////////////////////////////////////////////////////////////////

//__NIST Material Manager_______________________________________________________________________
const auto _nist = G4NistManager::Instance();
//----------------------------------------------------------------------------------------------

//__Detector Name for Builder___________________________________________________________________
std::string _detector;
//----------------------------------------------------------------------------------------------

//__Detector List_______________________________________________________________________________
const std::string& _detectors = "Prototype Flat";
//----------------------------------------------------------------------------------------------

} /* anonymous namespace */ ////////////////////////////////////////////////////////////////////

namespace Construction { ///////////////////////////////////////////////////////////////////////

//__Construction Materials______________________________________________________________________
auto Material::H = _nist->FindOrBuildElement("H");
auto Material::C = _nist->FindOrBuildElement("C");
auto Material::N = _nist->FindOrBuildElement("N");
auto Material::O = _nist->FindOrBuildElement("O");
auto Material::F = _nist->FindOrBuildElement("F");
auto Material::S = _nist->FindOrBuildElement("S");
auto Material::Ar = _nist->FindOrBuildElement("Ar");
auto Material::Air = _nist->FindOrBuildMaterial("G4_AIR");
auto Material::Aluminum = _nist->FindOrBuildMaterial("G4_Al");
//----------------------------------------------------------------------------------------------

//__Detector Messenger Directory Path___________________________________________________________
const std::string Builder::MessengerDirectory = "/det/";
//----------------------------------------------------------------------------------------------

//__Builder Constructor_________________________________________________________________________
Builder::Builder(const std::string& detector)
    : G4VUserDetectorConstruction(), G4UImessenger(MessengerDirectory, "Particle Detectors.") {
  _detector = detector;

  _select = CreateCommand<Command::StringArg>("select", "Select Detector.");
  _select->SetParameterName("detector", false);
  _select->SetDefaultValue("Prototype");
  _select->SetCandidates(_detectors.c_str());
  _select->AvailableForStates(G4State_PreInit, G4State_Idle);

  _list = CreateCommand<Command::NoArg>("list", "List Avaliable Detector.");
  _list->AvailableForStates(G4State_PreInit, G4State_Idle);

  _current = CreateCommand<Command::NoArg>("current", "Current Detector.");
  _current->AvailableForStates(G4State_PreInit, G4State_Idle);
}
//----------------------------------------------------------------------------------------------

//__Build World and Detector Geometry___________________________________________________________
G4VPhysicalVolume* Builder::Construct() {
  constexpr static auto WorldLength = 240*m;

  G4GeometryManager::GetInstance()->OpenGeometry();
  G4PhysicalVolumeStore::GetInstance()->Clean();
  G4LogicalVolumeStore::GetInstance()->Clean();
  G4SolidStore::GetInstance()->Clean();

  G4GeometryManager::GetInstance()->SetWorldMaximumExtent(WorldLength);

  std::cout << "Computed tolerance = "
            << G4GeometryTolerance::GetInstance()->GetSurfaceTolerance() / m << " m\n";

  auto worldLV = BoxVolume("World", WorldLength, WorldLength, WorldLength);

  Export(Earth::Construct(worldLV), "earth.gdml");

  if (_detector == "Flat") {
    Export(Flat::Detector::Construct(worldLV), "flat.gdml");
  } else {
    Export(Prototype::Detector::Construct(worldLV), "prototype.gdml");
  }

  auto world = PlaceVolume(worldLV, nullptr);
  if (_detector == "Flat") {
    Export(world, "world.flat.gdml");
  } else {
    Export(world, "world.prototype.gdml");
  }

  std::cout << "Materials: "
            << *G4Material::GetMaterialTable() << '\n';

  return world;
}
//----------------------------------------------------------------------------------------------

//__Build Detector______________________________________________________________________________
void Builder::ConstructSDandField() {
  if (_detector == "Flat") {
    G4SDManager::GetSDMpointer()->AddNewDetector(new Flat::Detector);
  } else {
    G4SDManager::GetSDMpointer()->AddNewDetector(new Prototype::Detector);
  }
  G4SDManager::GetSDMpointer()->ListTree();
}
//----------------------------------------------------------------------------------------------

//__Builder Messenger Set New Value_____________________________________________________________
void Builder::SetNewValue(G4UIcommand* command, G4String value) {
  if (command == _select && value != _detector.c_str()) {
    SetDetector(value);
  } else if (command == _list) {
    std::cout << "Detectors: " << _detectors << "\n";
  } else if (command == _current) {
    std::cout << "Current Detector: " << _detector << "\n";
  }
}
//----------------------------------------------------------------------------------------------

//__Get Current Detector Name___________________________________________________________________
const std::string& Builder::GetDetector() {
  return _detector;
}
//----------------------------------------------------------------------------------------------

//__Set Current Detector________________________________________________________________________
void Builder::SetDetector(const std::string& detector) {
  _detector = detector;
  Command::Execute("/run/reinitializeGeometry",
                   "/run/geometryModified",
                   "/run/initialize",
                   "/vis/viewer/clearTransients");
}
//----------------------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////////////////////

//__Sensitive Material Attribute Definition_____________________________________________________
const G4VisAttributes SensitiveAttributes() {
  auto attr = G4VisAttributes(G4Colour(0., 1., 0., 1.0));
  attr.SetForceSolid(true);
  return attr;
}
//----------------------------------------------------------------------------------------------

//__Casing Material Attribute Definition________________________________________________________
const G4VisAttributes CasingAttributes() {
  auto attr = G4VisAttributes(G4Colour(0., 0., 1., 0.2));
  attr.SetForceSolid(true);
  return attr;
}
//----------------------------------------------------------------------------------------------

//__Border Attribute Definition_________________________________________________________________
const G4VisAttributes BorderAttributes() {
  return G4VisAttributes(false);
}
//----------------------------------------------------------------------------------------------

//__Box Builder_________________________________________________________________________________
G4Box* Box(const std::string& name,
           const double width,
           const double height,
           const double depth) {
  return new G4Box(name, 0.5 * width, 0.5 * height, 0.5 * depth);
}
//----------------------------------------------------------------------------------------------

//__Trapezoid Builder___________________________________________________________________________
G4Trap* Trap(const std::string& name,
             const double height,
             const double minwidth,
             const double maxwidth,
             const double depth) {
  return new G4Trap(name,
    0.5 * height, 0, 0,
    0.5 * depth, 0.5 * minwidth, 0.5 * minwidth, 0,
    0.5 * depth, 0.5 * maxwidth, 0.5 * maxwidth, 0);
}
//----------------------------------------------------------------------------------------------

//__Volume Builder______________________________________________________________________________
G4LogicalVolume* Volume(const std::string& name,
                        G4VSolid* solid,
                        G4Material* material,
                        const G4VisAttributes& attr) {
  auto out = new G4LogicalVolume(solid, material, name);
  out->SetVisAttributes(attr);
  return out;
}
//----------------------------------------------------------------------------------------------

//__Volume Builder______________________________________________________________________________
G4LogicalVolume* Volume(G4VSolid* solid,
                        G4Material* material,
                        const G4VisAttributes& attr) {
  return Volume(solid->GetName(), solid, material, attr);
}
//----------------------------------------------------------------------------------------------

//__Volume Builder______________________________________________________________________________
G4LogicalVolume* Volume(const std::string& name,
                        G4VSolid* solid,
                        const G4VisAttributes& attr) {
  return Volume(name, solid, Material::Air, attr);
}
//----------------------------------------------------------------------------------------------

//__Volume Builder______________________________________________________________________________
G4LogicalVolume* Volume(G4VSolid* solid,
                        const G4VisAttributes& attr) {
  return Volume(solid, Material::Air, attr);
}
//----------------------------------------------------------------------------------------------

//__Box Volume Builder__________________________________________________________________________
G4LogicalVolume* BoxVolume(const std::string& name,
                           const double width,
                           const double height,
                           const double depth,
                           G4Material* material,
                           const G4VisAttributes& attr) {
  return Volume(
    Box(name, width, height, depth),
    material, attr);
}
//----------------------------------------------------------------------------------------------

//__Open Box Volume Builder_____________________________________________________________________
G4LogicalVolume* OpenBoxVolume(const std::string& name,
                               const double width,
                               const double height,
                               const double depth,
                               const double thickness,
                               G4Material* material,
                               const G4VisAttributes& attr) {
  auto outer = Box(name,
    width,
    height,
    depth);
  auto inner = Box(name,
    width  - 2 * thickness,
    height - 2 * thickness,
    depth  - 2 * thickness);
  return Volume(new G4SubtractionSolid(name, outer, inner), material, attr);
}
//----------------------------------------------------------------------------------------------

//__Physical Volume Placer______________________________________________________________________
G4VPhysicalVolume* PlaceVolume(const std::string& name,
                               G4LogicalVolume* current,
                               G4LogicalVolume* parent,
                               const G4Transform3D& transform) {
  return new G4PVPlacement(transform, current, name, parent, false, 0);
}
//----------------------------------------------------------------------------------------------

//__Physical Volume Placer______________________________________________________________________
G4VPhysicalVolume* PlaceVolume(G4LogicalVolume* current,
                               G4LogicalVolume* parent,
                               const G4Transform3D& transform) {
  return PlaceVolume(current->GetName(), current, parent, transform);
}
//----------------------------------------------------------------------------------------------

//__Physical Volume Placer______________________________________________________________________
G4VPhysicalVolume* PlaceVolume(const std::string& name,
                               G4VSolid* solid,
                               G4Material* material,
                               G4LogicalVolume* parent,
                               const G4Transform3D& transform) {
  return PlaceVolume(name, Volume(name, solid, material), parent, transform);
}
//----------------------------------------------------------------------------------------------

//__Physical Volume Placer______________________________________________________________________
G4VPhysicalVolume* PlaceVolume(G4VSolid* solid,
                               G4Material* material,
                               G4LogicalVolume* parent,
                               const G4Transform3D& transform) {
  return PlaceVolume(solid->GetName(), solid, material, parent, transform);
}
//----------------------------------------------------------------------------------------------

//__Physical Volume Placer______________________________________________________________________
G4VPhysicalVolume* PlaceVolume(const std::string& name,
                               G4LogicalVolume* current,
                               const G4VisAttributes& attr,
                               G4LogicalVolume* parent,
                               const G4Transform3D& transform) {
  current->SetVisAttributes(attr);
  return PlaceVolume(name, current, parent, transform);
}
//----------------------------------------------------------------------------------------------

//__Physical Volume Placer______________________________________________________________________
G4VPhysicalVolume* PlaceVolume(G4LogicalVolume* current,
                               const G4VisAttributes& attr,
                               G4LogicalVolume* parent,
                               const G4Transform3D& transform) {
  current->SetVisAttributes(attr);
  return PlaceVolume(current, parent, transform);
}
//----------------------------------------------------------------------------------------------

//__Physical Volume Placer______________________________________________________________________
G4VPhysicalVolume* PlaceVolume(const std::string& name,
                               G4VSolid* solid,
                               G4Material* material,
                               const G4VisAttributes& attr,
                               G4LogicalVolume* parent,
                               const G4Transform3D& transform) {
  return PlaceVolume(name, Volume(solid, material, attr), parent, transform);
}
//----------------------------------------------------------------------------------------------

//__Physical Volume Placer______________________________________________________________________
G4VPhysicalVolume* PlaceVolume(G4VSolid* solid,
                               G4Material* material,
                               const G4VisAttributes& attr,
                               G4LogicalVolume* parent,
                               const G4Transform3D& transform) {
  return PlaceVolume(Volume(solid, material, attr), parent, transform);
}
//----------------------------------------------------------------------------------------------

//__Translation Transformation Generator_______________________________________________________
G4Transform3D Transform(const double x,
                        const double y,
                        const double z) {
  return G4Transform3D(G4RotationMatrix(), G4ThreeVector(x, y, z));
}
//----------------------------------------------------------------------------------------------

//__Rotation/Translation Transformation Generator_______________________________________________
G4Transform3D Transform(const G4ThreeVector& translate,
                        const G4ThreeVector& axis,
                        const double angle) {
  return G4Transform3D(G4RotationMatrix(axis, angle), translate);
}
//----------------------------------------------------------------------------------------------

//__Rotation/Translation Transformation Generator_______________________________________________
G4Transform3D Transform(const double x,
                        const double y,
                        const double z,
                        const double axisx,
                        const double axisy,
                        const double axisz,
                        const double angle) {
  return Transform(
    G4ThreeVector(x, y, z), G4ThreeVector(axisx, axisy, axisz), angle);
}
//----------------------------------------------------------------------------------------------

//__Rotation Transformation Generator___________________________________________________________
G4Transform3D Rotate(const double axisx,
                     const double axisy,
                     const double axisz,
                     const double angle) {
  return Transform(0, 0, 0, axisx, axisy, axisz, angle);
}
//----------------------------------------------------------------------------------------------

//__Matrix Transformation Generator_____________________________________________________________
G4RotationMatrix Matrix(const double th1,
                        const double phi1,
                        const double th2,
                        const double phi2,
                        const double th3,
                        const double phi3) {
  const double sinth1 = std::sin(th1);
  const double sinth2 = std::sin(th2);
  const double sinth3 = std::sin(th3);
  auto matrix = G4RotationMatrix();
  matrix.rotateAxes(
    G4ThreeVector(sinth1*std::cos(phi1), sinth1*std::sin(phi1), std::cos(th1)),
    G4ThreeVector(sinth2*std::cos(phi2), sinth2*std::sin(phi2), std::cos(th2)),
    G4ThreeVector(sinth3*std::cos(phi3), sinth3*std::sin(phi3), std::cos(th3)));

  if (matrix != G4RotationMatrix()) matrix.invert();
  return matrix;
}
//----------------------------------------------------------------------------------------------

//__Matrix Transformation Generator_____________________________________________________________
G4RotationMatrix Matrix(const double mxx,
                        const double mxy,
                        const double mxz,
                        const double myx,
                        const double myy,
                        const double myz,
                        const double mzx,
                        const double mzy,
                        const double mzz) {
  return G4RotationMatrix(
    G4ThreeVector(mxx, myx, mzx),
    G4ThreeVector(mxy, myy, mzy),
    G4ThreeVector(mxz, myz, mzz));
}
//----------------------------------------------------------------------------------------------

//__GDML File Export____________________________________________________________________________
void Export(const G4LogicalVolume* volume,
            const std::string& file,
            const std::string& schema) {
  util::io::create_directory("export");
  auto path = "export/" + file;
  if (util::io::path_exists(path))
    util::io::remove_file(path);

  static G4ThreadLocal G4GDMLParser _parser;
  _parser.Write(path, volume, true,
                !schema.empty() ? schema : G4GDML_DEFAULT_SCHEMALOCATION);
}
//----------------------------------------------------------------------------------------------

//__GDML File Export____________________________________________________________________________
void Export(const G4VPhysicalVolume* volume,
            const std::string& file,
            const std::string& schema) {
  util::io::create_directory("export");
  auto path = "export/" + file;
  if (util::io::path_exists(path))
    util::io::remove_file(path);

  static G4ThreadLocal G4GDMLParser _parser;
  _parser.Write(path, volume, true,
                !schema.empty() ? schema : G4GDML_DEFAULT_SCHEMALOCATION);
}
//----------------------------------------------------------------------------------------------

} /* namespace Construction */ /////////////////////////////////////////////////////////////////

} } /* namespace MATHUSLA::MU */
