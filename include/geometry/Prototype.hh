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

#include "Geant4/G4VSensitiveDetector.hh"

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

  struct ScintillatorInfo {
    std::string name;
    double x;
    double y;
    double z;
    double z_rotation_angle;
    double long_base;
    double short_base;
    double trapezoid_height;
  };

  constexpr static auto n_scintillators = 59u;

  const static ScintillatorInfo scintillator_infos[n_scintillators];

  constexpr static auto Thickness = 1.15*mm;
  constexpr static auto Spacing   =    0*mm;
  constexpr static auto Depth     = 15.0*mm;

  constexpr static auto PMTRadius =  2.1*cm;
  constexpr static auto PMTLength = 19.3*cm;

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
    static G4Material* Casing;
    static G4Material* Pad;
    static G4Material* Gas;
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

  struct RPCInfo {
    double x;
    double y;
    double z;
    double z_rotation_angle;
  };

  constexpr static auto n_rpcs = 12u;

  const static RPCInfo rpc_infos[n_rpcs];

  constexpr static auto Thickness   =    1*mm;
  constexpr static auto Height      = 2800*mm + 2.0 * Thickness;
  constexpr static auto Width       = 1250*mm + 2.0 * Thickness;
  constexpr static auto Depth       =   44*mm;

  constexpr static auto PadWidth     = 618*mm;
  constexpr static auto PadHeight    = 556*mm;
  constexpr static auto PadDepth     =   6*mm;
  constexpr static auto PadThickness =   0*mm;

  constexpr static auto n_pads_per_row    = 2u;
  constexpr static auto n_pads_per_column = 5u;
  constexpr static auto n_pads_per_rpc = n_pads_per_row * n_pads_per_column;

  constexpr static auto PadSpacingX = PadWidth  + (Width  - 2.0 * Thickness - n_pads_per_row    * PadWidth ) / n_pads_per_row;
  constexpr static auto PadSpacingY = PadHeight + (Height - 2.0 * Thickness - n_pads_per_column * PadHeight) / n_pads_per_column;

  constexpr static auto StripHeight     = 67.5*mm;
  constexpr static auto StripWidth      =  618*mm;
  constexpr static auto StripDepth      =    6*mm;

  constexpr static auto n_strips_per_pad = 8u;

  constexpr static auto StripSpacingY = StripHeight + (PadHeight - 2.0 * PadThickness - n_strips_per_pad * StripHeight) / n_strips_per_pad;

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
