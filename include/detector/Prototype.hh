#ifndef DETECTOR_PROTOTYPE_HH
#define DETECTOR_PROTOTYPE_HH
#pragma once

#include <initializer_list>
#include <vector>

#include "Geant4/G4VSensitiveDetector.hh"

#include "detector/Construction.hh"
#include "tracking/PrototypeHit.hh"

namespace MATHUSLA { namespace MU {

class Scintillator {
public:
  Scintillator(const G4String& name,
               const G4double height,
               const G4double minwidth,
               const G4double maxwidth);

  struct Material {
    static G4Material* PMT;
    static G4Material* Casing;
    static G4Material* Scintillator;
    static void Define();
  private:
    Material();
  };

  G4String           name;
  G4double           height;
  G4double           minwidth;
  G4double           maxwidth;
  G4LogicalVolume*   lvolume;
  G4VPhysicalVolume* pvolume;
  G4VPhysicalVolume* sensitive;

  const G4String GetFullName() const;

  void Register(G4VSensitiveDetector* detector);

  struct PMTPoint { G4double up, right, r; };

  static PMTPoint PMTDistance(const G4ThreeVector position,
                              const Scintillator* sci,
                              const G4ThreeVector translation,
                              const G4RotationMatrix rotation);

  static Scintillator* Clone(const Scintillator* other);

  constexpr static G4double Depth     =  2.0*cm;
  constexpr static G4double Thickness =  0.1*cm;
  constexpr static G4double Spacing   =  0.1*cm;
  constexpr static G4double PMTRadius =  2.1*cm;
  constexpr static G4double PMTLength = 19.3*cm;

  constexpr static G4double MinDeposit =  0*keV;
  constexpr static G4double MaxDeposit = 10*MeV;
};

using ScintillatorList = std::vector<Scintillator*>;

class Envelope {
public:
  enum class LayerType { TopFirst, BottomFirst };
  enum class Alignment : signed char { Left = -1, Center = 0, Right = 1 };

  Envelope(const G4String& name,
           const LayerType layer_type,
           const Alignment alignment,
           std::initializer_list<Scintillator*> scintillators);

  inline const G4String&    GetName()             const { return _name;          }
  inline G4double           GetHeight()           const { return _height;        }
  inline G4double           GetTopWidth()         const { return _top_width;     }
  inline G4double           GetBottomWidth()      const { return _bottom_width;  }
  inline G4LogicalVolume*   GetVolume()           const { return _volume;        }
  inline G4VPhysicalVolume* GetPlacement()        const { return _placement;     }
  inline ScintillatorList   GetScintillatorList() const { return _scintillators; }

  G4VPhysicalVolume* Place(G4LogicalVolume* parent, const G4Transform3D& transform);

  constexpr static G4double LayerSpacing = 5*cm;

private:
  G4String           _name;
  G4double           _height;
  G4double           _top_width;
  G4double           _bottom_width;
  ScintillatorList   _scintillators;
  G4LogicalVolume*   _volume;
  G4VPhysicalVolume* _placement;
};

using EnvelopeList = std::vector<Envelope*>;

class RPC {
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
    static G4Material* Gas;
    static void Define();
  private:
    Material();
  };

  RPC(int id);

  inline int                GetID()        const { return _id;           }
  inline const G4String&    GetName()      const { return _name;         }
  inline G4LogicalVolume*   GetVolume()    const { return _volume;       }
  inline G4VPhysicalVolume* GetPlacement() const { return _placement;    }
  inline std::vector<Pad>   GetPadList()   const { return _pads;         }

  G4VPhysicalVolume* Place(G4LogicalVolume* parent, const G4Transform3D& transform);

  void Register(G4VSensitiveDetector* detector);

  constexpr static G4double Width     = 1257*mm;
  constexpr static G4double Height    = 2854*mm;
  constexpr static G4double Depth     =   60*mm;   // what is true value?
  constexpr static G4double Thickness =   10*mm;   // what is true value?
  constexpr static G4double Angle     =   12*deg;  // what is true value?

  constexpr static G4double PadWidth     = 618*mm;
  constexpr static G4double PadHeight    = 556*mm;
  constexpr static G4double PadDepth     =  55*mm;  // what is true value?
  constexpr static G4double PadThickness =  10*mm;  // what is true value?
  constexpr static G4double PadStartX    = 318*mm;
  constexpr static G4double PadStartY    = 312*mm;
  constexpr static G4double PadSpacingX  = 620*mm;
  constexpr static G4double PadSpacingY  = 557*mm;

  constexpr static G4double StripWidth     =  618*mm;
  constexpr static G4double StripHeight    = 67.5*mm;
  constexpr static G4double StripDepth     =   50*mm;  // what is true value?
  constexpr static G4double StripThickness =    2*mm;  // what is true value?
  constexpr static G4double StripTopGap    =    1*mm;
  constexpr static G4double StripYGap      =    2*mm;

  constexpr static G4double MinDeposit =  0*keV;
  constexpr static G4double MaxDeposit = 10*MeV;

private:
  G4LogicalVolume* _volume;
  std::vector<Pad> _pads;
  int _id;
  G4String _name;
  G4VPhysicalVolume* _placement;
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

  void Initialize(G4HCofThisEvent* eventHC);
  G4bool ProcessHits(G4Step* step, G4TouchableHistory*);
  void EndOfEvent(G4HCofThisEvent*);

  static bool GenerateAnalysis(const G4int event_count);
  static G4int EncodeDetector(const G4String& name);
  static const G4String DecodeDetector(G4int id);
  // static G4int EncodeTDC(const G4String name);
  // static const G4String DecodeTDC(G4int code);
  static G4VPhysicalVolume* Construct(G4LogicalVolume* world);

private:
  PrototypeHC* _hit_collection;
  static EnvelopeList _envelopes;
  static RPCList _rpcs;
};

} } /* namespace MATHUSLA::MU */

#endif /* DETECTOR_PROTOTYPE_HH */
