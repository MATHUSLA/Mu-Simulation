#ifndef DETECTOR_PROTOTYPE_HH
#define DETECTOR_PROTOTYPE_HH
#pragma once

#include "Geant4/G4VSensitiveDetector.hh"
#include "Geant4/G4MultiSensitiveDetector.hh"

#include <initializer_list>
#include <vector>

#include "detector/Construction.hh"
#include "tracking/PrototypeHit.hh"

namespace MATHUSLA { namespace MU {

class Scintillator : public G4VSensitiveDetector {
public:
  Scintillator(const G4String& name,
               const G4double height,
               const G4double minwidth,
               const G4double maxwidth,
               const G4double depth,
               const G4double thickness,
               const G4double spacing);

  struct Material {
    static G4Material* PMT;
    static G4Material* Casing;
    static G4Material* Scintillator;
    static void Define();
  private:
    Material();
  };

  inline const G4String&    GetName()            const { return _name;      }
  inline G4double           GetHeight()          const { return _height;    }
  inline G4double           GetMinWidth()        const { return _minwidth;  }
  inline G4double           GetMaxWidth()        const { return _maxwidth;  }
  inline G4double           GetDepth()           const { return _depth;     }
  inline G4double           GetThickness()       const { return _thickness; }
  inline G4double           GetSpacing()         const { return _spacing;   }
  inline G4VSolid*          GetSolid()           const { return _solid;     }
  inline G4LogicalVolume*   GetVolume()          const { return _volume;    }
  inline G4VPhysicalVolume* GetSensitiveVolume() const { return _sensitive; }
  inline G4VPhysicalVolume* GetCasingVolume()    const { return _casing;    }
  inline G4VPhysicalVolume* GetPMTVolume()       const { return _pmt;       }

  struct Point { G4double up, right; };

  void   Initialize(G4HCofThisEvent* eventHC);
  G4bool ProcessHits(G4Step* step, G4TouchableHistory*);
  void   EndOfEvent(G4HCofThisEvent*);

  static Scintillator* Clone(const Scintillator* other);

  constexpr static G4double PMTRadius =  2.1*cm;
  constexpr static G4double PMTLength = 19.3*cm;

private:
  PrototypeHC*       _hit_collection;
  G4String           _name;
  G4double           _height;
  G4double           _minwidth;
  G4double           _maxwidth;
  G4double           _depth;
  G4double           _thickness;
  G4double           _spacing;
  G4VSolid*          _solid;
  G4LogicalVolume*   _volume;
  G4VPhysicalVolume* _sensitive;
  G4VPhysicalVolume* _casing;
  G4VPhysicalVolume* _pmt;
};

using ScintillatorList = std::vector<Scintillator*>;

class Envelope {
public:
  enum class LayerType { TopFirst, BottomFirst };
  enum class Alignment : signed char { Left = -1, Center = 0, Right = 1 };

  Envelope(const G4String& name,
           const G4double layer_spacing,
           const LayerType layer_type,
           const Alignment alignment,
           std::initializer_list<Scintillator*> scintillators);

  inline const G4String&  GetName()             const { return _name;          }
  inline G4double         GetLayerSpacing()     const { return _layer_spacing; }
  inline G4double         GetHeight()           const { return _height;        }
  inline G4double         GetTopWidth()         const { return _top_width;     }
  inline G4double         GetBottomWidth()      const { return _bottom_width;  }
  inline G4LogicalVolume* GetVolume()           const { return _volume;        }
  inline ScintillatorList GetScintillatorList() const { return _scintillators; }

private:
  G4String         _name;
  G4double         _layer_spacing;
  G4double         _height;
  G4double         _top_width;
  G4double         _bottom_width;
  ScintillatorList _scintillators;
  G4LogicalVolume* _volume;
};

using EnvelopeList = std::vector<Envelope*>;

class RPC : public G4VSensitiveDetector {
public:
  struct Pad {
    Pad(int id);
    G4VPhysicalVolume* volume;
    std::vector<G4VPhysicalVolume*> strips;
    int id;
  };

  struct Material {
    static G4Material* Casing;
    static G4Material* Pad;
    static G4Material* Strip;
    static void Define();
  private:
    Material();
  };

  RPC(int id);

  inline int              GetID()        const { return _id;           }
  inline G4LogicalVolume* GetVolume()    const { return _volume;       }
  inline Pad              GetPad(int id) const { return _pads[id - 1]; }
  inline std::vector<Pad> GetPadList()   const { return _pads;         }

  void   Initialize(G4HCofThisEvent* eventHC);
  G4bool ProcessHits(G4Step* step, G4TouchableHistory*);
  void   EndOfEvent(G4HCofThisEvent*);

  constexpr static G4double Width  = 1257*mm;
  constexpr static G4double Height = 2854*mm;
  constexpr static G4double Depth  =   60*mm;  // what is true value?

  constexpr static G4double PadWidth    = 618*mm;
  constexpr static G4double PadHeight   = 556*mm;
  constexpr static G4double PadDepth    =  55*mm;  // what is true value?
  constexpr static G4double PadStartX   = 318*mm;
  constexpr static G4double PadStartY   = 312*mm;
  constexpr static G4double PadSpacingX = 620*mm;
  constexpr static G4double PadSpacingY = 557*mm;

  constexpr static G4double StripWidth  =  618*mm;
  constexpr static G4double StripHeight = 67.5*mm;
  constexpr static G4double StripDepth  =   50*mm;  // what is true value?
  constexpr static G4double StripTopGap =    1*mm;
  constexpr static G4double StripYGap   =    2*mm;

private:
  PrototypeHC* _hit_collection;
  G4LogicalVolume* _volume;
  std::vector<Pad> _pads;
  int _id;
};

using RPCList = std::vector<RPC*>;

class Prototype : public G4VSensitiveDetector {
public:
  Prototype();

  struct Material {
    static void Define();
  private:
    Material();
  };

  void   Initialize(G4HCofThisEvent* eventHC);
  G4bool ProcessHits(G4Step* step, G4TouchableHistory*);
  void   EndOfEvent(G4HCofThisEvent*);

  static G4VPhysicalVolume* Construct(G4LogicalVolume* world);

private:
  PrototypeHC* _hit_collection;
  static EnvelopeList _envelopes;
  static RPCList _rpcs;
};

} } /* namespace MATHUSLA::MU */

#endif /* DETECTOR_PROTOTYPE_HH */
