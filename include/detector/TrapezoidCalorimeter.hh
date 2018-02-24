#ifndef DETECTOR_TRAPEZOIDCALORIMETER_HH
#define DETECTOR_TRAPEZOIDCALORIMETER_HH
#pragma once

#include "Geant4/G4VSensitiveDetector.hh"
#include "Geant4/G4Tubs.hh"

#include <initializer_list>

#include "detector/Construction.hh"
#include "tracking/TrapezoidHit.hh"

namespace MATHUSLA { namespace MU {

class Scintillator {
 public:
  Scintillator();
  Scintillator(const G4String& name,
               const G4double height,
               const G4double minwidth,
               const G4double maxwidth,
               const G4double depth,
               const G4double thickness,
               const G4double spacing);

  inline const G4String&    GetName()            const { return fName;      }
  inline G4double           GetHeight()          const { return fHeight;    }
  inline G4double           GetMinWidth()        const { return fMinWidth;  }
  inline G4double           GetMaxWidth()        const { return fMaxWidth;  }
  inline G4double           GetDepth()           const { return fDepth;     }
  inline G4double           GetThickness()       const { return fThickness; }
  inline G4double           GetSpacing()         const { return fSpacing;   }
  inline G4VSolid*          GetSolid()           const { return fSolid;     }
  inline G4LogicalVolume*   GetVolume()          const { return fVolume;    }
  inline G4VPhysicalVolume* GetSensitiveVolume() const { return fSVolume;   }
  inline G4VPhysicalVolume* GetCasingVolume()    const { return fCasing;    }  // may remove
  inline G4VPhysicalVolume* GetPMTVolume()       const { return fPMT;       }  // may remove

  inline bool operator==(const Scintillator& rhs) const {
    return (fName      == rhs.fName)
        && (fHeight    == rhs.fHeight)
        && (fMinWidth  == rhs.fMinWidth)
        && (fMaxWidth  == rhs.fMaxWidth)
        && (fDepth     == rhs.fDepth)
        && (fThickness == rhs.fThickness)
        && (fSpacing   == rhs.fSpacing);
  }

  inline bool operator!=(const Scintillator& rhs) const {
    return !(*this == rhs);
  }

  constexpr static G4double PMTRadius =  2.1*cm;
  constexpr static G4double PMTLength = 19.3*cm;

 private:
  static G4Tubs* PMTCylinder(const G4String& name);

  G4String           fName;
  G4double           fHeight;
  G4double           fMinWidth;
  G4double           fMaxWidth;
  G4double           fDepth;
  G4double           fThickness;
  G4double           fSpacing;
  G4VSolid*          fSolid;
  G4LogicalVolume*   fVolume;
  G4VPhysicalVolume* fSVolume;
  G4VPhysicalVolume* fCasing;   // may remove
  G4VPhysicalVolume* fPMT;      // may remove
};

using ScintillatorList = std::vector<Scintillator>;

class Envelope {
 public:
  enum class LayerType { TopFirst, BottomFirst };
  enum class Alignment : signed char { Left = -1, Center = 0, Right = 1 };

  Envelope(const G4String& name,
           const G4double layer_spacing,
           const LayerType layer_type,   // getter ?
           const Alignment alignment,    // getter ?
           std::initializer_list<Scintillator> scintillators);

  inline const G4String&  GetName()             const { return fName;          }
  inline G4double         GetLayerSpacing()     const { return fLayerSpacing;  }
  inline G4double         GetHeight()           const { return fHeight;        }
  inline G4double         GetTopWidth()         const { return fTopWidth;      }
  inline G4double         GetBottomWidth()      const { return fBottomWidth;   }
  inline ScintillatorList GetScintillatorList() const { return fScintillators; }
  inline G4LogicalVolume* GetVolume()           const { return fVolume;        }

  const Scintillator GetScintillator(const G4String& name) const;

 private:
  G4String         fName;
  G4double         fLayerSpacing;
  G4double         fHeight;
  G4double         fTopWidth;
  G4double         fBottomWidth;
  ScintillatorList fScintillators;
  G4LogicalVolume* fVolume;
};

using EnvelopeList = std::vector<Envelope>;

class TrapezoidCalorimeter : public G4VSensitiveDetector {
 public:
  TrapezoidCalorimeter();

  struct Material {
    static G4Material* Aluminum;
    static G4Material* Carbon;
    static G4Material* Scintillator;
   private:
    Material();
  };

  static void               DefineMaterials();
  static G4VPhysicalVolume* Construct(G4LogicalVolume*);
         void               Initialize(G4HCofThisEvent*);
         G4bool             ProcessHits(G4Step*, G4TouchableHistory*);
         void               EndOfEvent(G4HCofThisEvent*);

 private:
  static EnvelopeList fEnvelopes;

  TrapezoidHC* fHitsCollection;
};

} } /* namespace MATHUSLA::MU */

#endif /* DETECTOR_TRAPEZOIDCALORIMETER_HH */
