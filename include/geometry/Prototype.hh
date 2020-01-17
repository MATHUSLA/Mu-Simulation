/* include/geometry/Prototype.hh
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

#ifndef MU__GEOMETRY_PROTOTYPE_HH
#define MU__GEOMETRY_PROTOTYPE_HH
#pragma once

#include <vector>

#include "G4VSensitiveDetector.hh"

#include "geometry/Construction.hh"

namespace MATHUSLA { namespace MU {

namespace Prototype { //////////////////////////////////////////////////////////////////////////

class Scintillator {
public:
  Scintillator(const std::string& input_name,
               const double input_height,
               const double input_minwidth,
               const double input_maxwidth);

  struct Material {
    static G4Material* PMT;
    static G4Material* Casing;
    static G4Material* Scintillator;
    static void Define();
  private:
    Material();
  };

  std::string        name;
  double             height;
  double             minwidth;
  double             maxwidth;
  G4LogicalVolume*   lvolume;
  G4VPhysicalVolume* pvolume;
  G4VPhysicalVolume* sensitive;

  const std::string GetFullName() const;

  void Register(G4VSensitiveDetector* detector);

  struct PMTPoint { double up, right, r; };

  static PMTPoint PMTDistance(const G4ThreeVector position,
                              const Scintillator* sci,
                              const G4ThreeVector translation,
                              const G4RotationMatrix rotation);

  static Scintillator* Clone(const Scintillator* other);

  struct Info {
    std::string name;
    double x;
    double y;
    double z;
    double z_rotation_angle;
    double long_base;
    double short_base;
    double trapezoid_height;
  };

  constexpr static auto Count = 59u;

  const static Info InfoArray[Count];

  constexpr static auto Depth = 12.7 * mm;

  constexpr static auto PlateDepth   = 1.0  * mm;
  constexpr static auto PlateSpacing = 0.15 * mm;

  constexpr static auto HorizontalEdgeSpacing   = 4.3 * mm;
  constexpr static auto HorizontalEdgeThickness = 1.8 * mm;

  constexpr static auto VerticalEdgeSpacing   = 0.0 * mm;
  constexpr static auto VerticalEdgeThickness = 1.0 * mm;

  constexpr static auto EdgeOverlap = 18.7 * mm - HorizontalEdgeThickness - HorizontalEdgeSpacing;

  constexpr static auto PMTRadius  =  2.1 * cm;
  constexpr static auto PMTLength  = 19.3 * cm;
  constexpr static auto PMTSpacing =  3.0 * cm;
  constexpr static auto PMTAngle   = 46.1 * deg;

  constexpr static auto MinDeposit =  0*keV;
  constexpr static auto MaxDeposit = 10*MeV;
};

////////////////////////////////////////////////////////////////////////////////////////////////

//__Scintillator Vector_________________________________________________________________________
using ScintillatorList = std::vector<Scintillator*>;
//----------------------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////////////////////

class RPC {
public:
  struct Pad {
    Pad(int input_id);
    G4LogicalVolume* lvolume;
    G4VPhysicalVolume* pvolume;
    std::vector<G4LogicalVolume*> lvolume_strips;
    std::vector<G4VPhysicalVolume*> pvolume_strips;
    int id;
  };

  struct Material {
    static G4Material* Gas;
    static G4Material* PET;
    static void Define();
  private:
    Material();
  };

  RPC(int input_id);

  int                GetID()        const { return _id;        }
  const std::string& GetName()      const { return _name;      }
  G4LogicalVolume*   GetVolume()    const { return _volume;    }
  G4VPhysicalVolume* GetPlacement() const { return _placement; }
  std::vector<Pad*>  GetPadList()   const { return _pads;      }

  void Register(G4VSensitiveDetector* detector);

  G4VPhysicalVolume* PlaceIn(G4LogicalVolume* parent,
                             const G4Transform3D& transform=G4Transform3D());

  struct Info {
    double x;
    double y;
    double z;
    double z_rotation_angle;
  };

  constexpr static auto Count = 12u;

  const static Info InfoArray[Count];

  constexpr static auto Height = 2800*mm;
  constexpr static auto Width  = 1248*mm;
  constexpr static auto Depth  =   44*mm;

  constexpr static auto OuterCasingThickness   = 2.35*mm;
  constexpr static auto OuterCasingHeight      = 2850*mm;
  constexpr static auto OuterCasingWidth       = 1257*mm;
  constexpr static auto OuterCasingDepth       = Depth + 2.0 * OuterCasingThickness;
  constexpr static auto OuterCasingCornerDepth = 5*mm;

  constexpr static auto CasingGap = 4*mm;

  constexpr static auto InnerCasingThickness = 1*mm;
  constexpr static auto InnerCasingHeight    = Height + 2.0 * InnerCasingThickness;
  constexpr static auto InnerCasingWidth     = 1120*mm;
  constexpr static auto InnerCasingDepth     = Depth + 2.0 * InnerCasingThickness;
  constexpr static auto InnerCasingOverlap   = 25*mm;

  constexpr static auto AluminumDepth  = 200*um;
  constexpr static auto BakeliteDepth  =   2*mm;
  constexpr static auto CopperDepth    =  17*um;
  constexpr static auto ThickFoamDepth =  15*mm;
  constexpr static auto ThinFoamDepth  =   3*mm;
  constexpr static auto ThickPETDepth  = 250*um;
  constexpr static auto MediumPETDepth = 190*um;
  constexpr static auto ThinPETDepth   =  50*um;

  constexpr static auto PadHeight    = 556.8*mm;
  constexpr static auto PadWidth     = 616.5*mm;
  constexpr static auto PadDepth     =     2*mm;

  constexpr static auto PadsPerRow    = 2u;
  constexpr static auto PadsPerColumn = 5u;
  constexpr static auto PadsPerRPC = PadsPerRow * PadsPerColumn;

  constexpr static auto PadSpacingX = PadWidth + 1*mm;
  constexpr static auto PadSpacingY = PadHeight;

  constexpr static auto StripHeight     =  69.6*mm;
  constexpr static auto StripWidth      = 616.5*mm;
  constexpr static auto StripDepth      =     2*mm;

  constexpr static auto StripsPerPad = 8u;

  constexpr static auto StripSpacingY = 69.6*mm;

  constexpr static auto MinDeposit =  0*keV;
  constexpr static auto MaxDeposit = 10*MeV;

private:
  G4LogicalVolume* _volume;
  std::vector<Pad*> _pads;
  int _id;
  std::string _name;
  G4VPhysicalVolume* _placement;
};

////////////////////////////////////////////////////////////////////////////////////////////////

class UChannel {
public:
  UChannel(const std::string &name, const double length);
  G4LogicalVolume *getLogicalVolume() { return m_logical_volume; }

  struct Info {
    std::string name;
    double x;
    double y;
    double z;
    double length;
    double z_rotation_angle;
  };

  constexpr static auto Count = 12u;

  const static Info InfoArray[Count];

  constexpr static auto Height = 5.0 * cm;
  constexpr static auto Width = 9.0 * cm;
  constexpr static auto VerticalThickness = 6.85 * mm;
  constexpr static auto HorizontalThickness = 7.75 * mm;

  constexpr static auto RPCUChannelLength = 2.8 * m;

private:
  std::string m_name;
  double m_length = 0.0;
  G4LogicalVolume *m_logical_volume = nullptr;
};

class Detector : public G4VSensitiveDetector {
public:
  Detector();

  void Initialize(G4HCofThisEvent* event);
  G4bool ProcessHits(G4Step* step, G4TouchableHistory*);
  void EndOfEvent(G4HCofThisEvent*);

  static int EncodeDetector(const std::string& name);
  static const std::string DecodeDetector(int id);

  static const bool DataPerEvent = true;
  static const std::string& DataName;
  static const Analysis::ROOT::DataKeyList DataKeys;
  static const Analysis::ROOT::DataKeyTypeList DataKeyTypes;

  static G4VPhysicalVolume* Construct(G4LogicalVolume* world);
  static G4VPhysicalVolume* ConstructEarth(G4LogicalVolume* world);

  static bool SaveAll;
};

} /* namespace Prototype */ ////////////////////////////////////////////////////////////////////

} } /* namespace MATHUSLA::MU */

#endif /* MU__GEOMETRY_PROTOTYPE_HH */
