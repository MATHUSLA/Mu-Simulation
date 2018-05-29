/* include/geometry/Construction.hh
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

#ifndef MU__GEOMETRY_CONSTRUCTION_HH
#define MU__GEOMETRY_CONSTRUCTION_HH
#pragma once

#include "Geant4/G4VUserDetectorConstruction.hh"
#include "Geant4/G4LogicalVolume.hh"
#include "Geant4/G4VPhysicalVolume.hh"
#include "Geant4/G4VisAttributes.hh"
#include "Geant4/G4Transform3D.hh"
#include "Geant4/G4Box.hh"
#include "Geant4/G4Trap.hh"
#include "Geant4/G4Material.hh"
#include "Geant4/G4SystemOfUnits.hh"

#include "ui.hh"

namespace MATHUSLA { namespace MU {

namespace Construction { ///////////////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////////////////////////////////////////

class Builder : public G4VUserDetectorConstruction, public G4UImessenger {
public:
  Builder(const std::string& detector);
  G4VPhysicalVolume* Construct();
  void ConstructSDandField();

  void SetNewValue(G4UIcommand* command, G4String value);

  static const std::string MessengerDirectory;

  static void SetDetector(const std::string& detector);

  static const std::string& GetDetectorName();
  static bool IsDetectorDataPerEvent();
  static const std::string& GetDetectorDataPrefix();
  static const std::vector<std::string>& GetDetectorDataKeys();

private:
  Command::NoArg*     _list;
  Command::NoArg*     _current;
  Command::StringArg* _select;
};

////////////////////////////////////////////////////////////////////////////////////////////////

//__Sensitive Material Attribute Definition_____________________________________________________
const G4VisAttributes SensitiveAttributes();
//----------------------------------------------------------------------------------------------

//__Casing Material Attribute Definition________________________________________________________
const G4VisAttributes CasingAttributes();
//----------------------------------------------------------------------------------------------

//__Border Attribute Definition_________________________________________________________________
const G4VisAttributes BorderAttributes();
//----------------------------------------------------------------------------------------------

//__Box Builder_________________________________________________________________________________
G4Box* Box(const std::string& name,
           const double width,
           const double height,
           const double depth);
//----------------------------------------------------------------------------------------------

//__Trapezoid Builder___________________________________________________________________________
G4Trap* Trap(const std::string& name,
             const double height,
             const double minwidth,
             const double maxwidth,
             const double depth);
//----------------------------------------------------------------------------------------------

//__Volume Builder______________________________________________________________________________
G4LogicalVolume* Volume(const std::string& name,
                        G4VSolid* solid,
                        G4Material* material=Material::Air,
                        const G4VisAttributes& attr=G4VisAttributes());
//----------------------------------------------------------------------------------------------

//__Volume Builder______________________________________________________________________________
G4LogicalVolume* Volume(G4VSolid* solid,
                        G4Material* material=Material::Air,
                        const G4VisAttributes& attr=G4VisAttributes());
//----------------------------------------------------------------------------------------------

//__Volume Builder______________________________________________________________________________
G4LogicalVolume* Volume(const std::string& name,
                        G4VSolid* solid,
                        const G4VisAttributes& attr);
//----------------------------------------------------------------------------------------------

//__Volume Builder______________________________________________________________________________
G4LogicalVolume* Volume(G4VSolid* solid,
                        const G4VisAttributes& attr);
//----------------------------------------------------------------------------------------------

//__Box Volume Builder__________________________________________________________________________
G4LogicalVolume* BoxVolume(const std::string& name,
                           const double width,
                           const double height,
                           const double depth,
                           G4Material* material=Material::Air,
                           const G4VisAttributes& attr=G4VisAttributes());
//----------------------------------------------------------------------------------------------

//__Open Box Volume Builder_____________________________________________________________________
G4LogicalVolume* OpenBoxVolume(const std::string& name,
                               const double width,
                               const double height,
                               const double depth,
                               const double thickness,
                               G4Material* material=Material::Air,
                               const G4VisAttributes& attr=G4VisAttributes());
//----------------------------------------------------------------------------------------------

//__Physical Volume Placer______________________________________________________________________
G4VPhysicalVolume* PlaceVolume(const std::string& name,
                               G4LogicalVolume* current,
                               G4LogicalVolume* parent,
                               const G4Transform3D& transform=G4Transform3D());
//----------------------------------------------------------------------------------------------

//__Physical Volume Placer______________________________________________________________________
G4VPhysicalVolume* PlaceVolume(G4LogicalVolume* current,
                               G4LogicalVolume* parent,
                               const G4Transform3D& transform=G4Transform3D());
//----------------------------------------------------------------------------------------------

//__Physical Volume Placer______________________________________________________________________
G4VPhysicalVolume* PlaceVolume(const std::string& name,
                               G4VSolid* solid,
                               G4Material* material,
                               G4LogicalVolume* parent,
                               const G4Transform3D& transform=G4Transform3D());
//----------------------------------------------------------------------------------------------

//__Physical Volume Placer______________________________________________________________________
G4VPhysicalVolume* PlaceVolume(G4VSolid* solid,
                               G4Material* material,
                               G4LogicalVolume* parent,
                               const G4Transform3D& transform=G4Transform3D());
//----------------------------------------------------------------------------------------------

//__Physical Volume Placer______________________________________________________________________
G4VPhysicalVolume* PlaceVolume(const std::string& name,
                               G4LogicalVolume* current,
                               const G4VisAttributes& attr,
                               G4LogicalVolume* parent,
                               const G4Transform3D& transform=G4Transform3D());
//----------------------------------------------------------------------------------------------

//__Physical Volume Placer______________________________________________________________________
G4VPhysicalVolume* PlaceVolume(G4LogicalVolume* current,
                               const G4VisAttributes& attr,
                               G4LogicalVolume* parent,
                               const G4Transform3D& transform=G4Transform3D());
//----------------------------------------------------------------------------------------------

//__Physical Volume Placer______________________________________________________________________
G4VPhysicalVolume* PlaceVolume(const std::string& name,
                               G4VSolid* solid,
                               G4Material* material,
                               const G4VisAttributes& attr,
                               G4LogicalVolume* parent,
                               const G4Transform3D& transform=G4Transform3D());
//----------------------------------------------------------------------------------------------

//__Physical Volume Placer______________________________________________________________________
G4VPhysicalVolume* PlaceVolume(G4VSolid* solid,
                               G4Material* material,
                               const G4VisAttributes& attr,
                               G4LogicalVolume* parent,
                               const G4Transform3D& transform=G4Transform3D());
//----------------------------------------------------------------------------------------------

//__Translation Transformation Generator_______________________________________________________
G4Transform3D Transform(const double x,
                        const double y,
                        const double z);
//----------------------------------------------------------------------------------------------

//__Rotation/Translation Transformation Generator_______________________________________________
G4Transform3D Transform(const G4ThreeVector& translate,
                        const G4ThreeVector& axis,
                        const double angle);
//----------------------------------------------------------------------------------------------

//__Rotation/Translation Transformation Generator_______________________________________________
G4Transform3D Transform(const double x,
                        const double y,
                        const double z,
                        const double axisx,
                        const double axisy,
                        const double axisz,
                        const double angle);
//----------------------------------------------------------------------------------------------

//__Rotation Transformation Generator___________________________________________________________
G4Transform3D Rotate(const double axisx,
                     const double axisy,
                     const double axisz,
                     const double angle);
//----------------------------------------------------------------------------------------------

//__Matrix Transformation Generator_____________________________________________________________
G4RotationMatrix Matrix(const double th1,
                        const double phi1,
                        const double th2,
                        const double phi2,
                        const double th3,
                        const double phi3);
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
                        const double mzz);
//----------------------------------------------------------------------------------------------

//__GDML File Export____________________________________________________________________________
void Export(const G4LogicalVolume* volume,
            const std::string& file,
            const std::string& schema="");
//----------------------------------------------------------------------------------------------

//__GDML File Export____________________________________________________________________________
void Export(const G4VPhysicalVolume* volume,
            const std::string& file,
            const std::string& schema="");
//----------------------------------------------------------------------------------------------

} /* namespace Construction */ /////////////////////////////////////////////////////////////////

} } /* namespace MATHUSLA::MU */

#endif /* MU__GEOMETRY_CONSTRUCTION_HH */
