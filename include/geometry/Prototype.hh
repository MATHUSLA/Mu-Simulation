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
  Scintillator(const std::string& name,
               const double height,
               const double minwidth,
               const double maxwidth);

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

  constexpr static auto Depth     =  2.0*cm;
  constexpr static auto Thickness =  0.1*cm;
  constexpr static auto Spacing   =  0.1*cm;
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

class Envelope {
public:
  enum class LayerType { TopFirst, BottomFirst };
  enum class Alignment : signed char { Left = -1, Center = 0, Right = 1 };
  enum class Rotation { NoFlip, Flip };

  Envelope(const std::string& name,
           const LayerType layer_type,
           const Alignment alignment,
           const Rotation rotation,
           std::initializer_list<Scintillator*> scintillators);

  const std::string& GetName()             const { return _name;          }
  double             GetHeight()           const { return _height;        }
  double             GetTopWidth()         const { return _top_width;     }
  double             GetBottomWidth()      const { return _bottom_width;  }
  G4LogicalVolume*   GetVolume()           const { return _volume;        }
  G4VPhysicalVolume* GetPlacement()        const { return _placement;     }
  ScintillatorList   GetScintillatorList() const { return _scintillators; }

  G4VPhysicalVolume* PlaceIn(G4LogicalVolume* parent,
                             const G4Transform3D& transform=G4Transform3D());

  constexpr static auto LayerSpacing = 5*cm;

private:
  std::string        _name;
  double             _height;
  double             _top_width;
  double             _bottom_width;
  ScintillatorList   _scintillators;
  G4LogicalVolume*   _volume;
  G4VPhysicalVolume* _placement;
};

////////////////////////////////////////////////////////////////////////////////////////////////

class RPC {
public:
  struct Pad {
    Pad(int id);
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

  RPC(int id);

  int                GetID()        const { return _id;        }
  const std::string& GetName()      const { return _name;      }
  G4LogicalVolume*   GetVolume()    const { return _volume;    }
  G4VPhysicalVolume* GetPlacement() const { return _placement; }
  std::vector<Pad*>  GetPadList()   const { return _pads;      }

  void Register(G4VSensitiveDetector* detector);

  G4VPhysicalVolume* PlaceIn(G4LogicalVolume* parent,
                             const G4Transform3D& transform=G4Transform3D());

  constexpr static auto Width     = 1257*mm;
  constexpr static auto Height    = 2854*mm;
  constexpr static auto Depth     =   60*mm;   // what is true value?
  constexpr static auto Thickness =   10*mm;   // what is true value?
  constexpr static auto Angle     =   12*deg;  // what is true value?

  constexpr static auto PadWidth     = 618*mm;
  constexpr static auto PadHeight    = 556*mm;
  constexpr static auto PadDepth     =  55*mm;  // what is true value?
  constexpr static auto PadThickness =  10*mm;  // what is true value?
  constexpr static auto PadStartX    = 318*mm;
  constexpr static auto PadStartY    = 312*mm;
  constexpr static auto PadSpacingX  = 620*mm;
  constexpr static auto PadSpacingY  = 557*mm;

  constexpr static auto StripWidth     =  618*mm;
  constexpr static auto StripHeight    = 67.5*mm;
  constexpr static auto StripDepth     =   50*mm;  // what is true value?
  constexpr static auto StripThickness =    2*mm;  // what is true value?
  constexpr static auto StripTopGap    =    1*mm;
  constexpr static auto StripYGap      =    2*mm;

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
