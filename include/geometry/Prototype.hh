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

  constexpr static double Centroids[12][3] = {{ 616.85*mm,   87.79*mm, -2486.38*mm},
                                              {-618.50*mm, -108.63*mm, -2383.38*mm},
                                              { -85.73*mm,  616.81*mm, -2049.38*mm},
                                              {  98.09*mm, -615.71*mm, -2148.38*mm},
                                              { 616.12*mm,  -94.51*mm,  -651.13*mm},
                                              {-614.31*mm,  107.15*mm,  -751.13*mm},
                                              {  94.86*mm,  620.81*mm,  -408.13*mm},
                                              { -93.55*mm, -614.15*mm,  -311.13*mm},
                                              { 615.11*mm, -118.18*mm,   983.12*mm},
                                              {-610.33*mm,  122.71*mm,  1081.12*mm},
                                              { 120.28*mm,  616.35*mm,  1423.12*mm},
                                              {-147.19*mm, -601.52*mm,  1323.12*mm}};

  constexpr static double Angles[12] = { 0.14719*rad,
                                         0.14753*rad,
                                        -1.42123*rad,
                                        -1.42069*rad,
                                         2.98398*rad,
                                         2.98463*rad,
                                         1.41717*rad,
                                         1.41976*rad,
                                         2.94831*rad,
                                         2.95236*rad,
                                         1.37958*rad,
                                         1.38013*rad};

  constexpr static auto Width       = 1252*mm;
  constexpr static auto Height      = 2802*mm;
  constexpr static auto Depth       =   44*mm;
  constexpr static auto Thickness   =    1*mm;

  constexpr static auto PadWidth     = 618*mm;
  constexpr static auto PadHeight    = 556*mm;
  constexpr static auto PadDepth     =  42*mm;
  constexpr static auto PadThickness =   0*mm;

  //constexpr static auto PadStartX    = 318*mm;
  //constexpr static auto PadStartY    = 312*mm;
  constexpr static auto PadSpacingX = PadWidth + (Width - 2.0 * PadWidth) / 2.0;
  //constexpr static auto PadSpacingY = 557*mm;
  constexpr static auto PadSpacingY = PadHeight + (Height - 5.0 * PadHeight) / 5.0;

  constexpr static auto StripWidth      =  618*mm;
  constexpr static auto StripHeight     = 67.5*mm;
  constexpr static auto StripDepth      =    6*mm;

  constexpr static auto StripSpacingY = StripHeight + (PadHeight - 8.0 * StripHeight) / 8.0;

  //constexpr static auto StripPadEdgeGap =    1*mm;
  //constexpr static auto StripYGap       =    2*mm;

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
