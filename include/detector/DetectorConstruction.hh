#ifndef DETECTORCONSTRUCTION_HH
#define DETECTORCONSTRUCTION_HH
#pragma once

#include "Geant4/globals.hh"
#include "Geant4/tls.hh"
#include "Geant4/G4VUserDetectorConstruction.hh"
#include "Geant4/G4LogicalVolume.hh"
#include "Geant4/G4VPhysicalVolume.hh"
#include "Geant4/G4Material.hh"
#include "Geant4/G4UserLimits.hh"
#include "Geant4/G4GlobalMagFieldMessenger.hh"

#include "DetectorMessenger.hh"

namespace MATHUSLA {

class DetectorMessenger;

class DetectorConstruction : public G4VUserDetectorConstruction {
 public:
  DetectorConstruction();
  virtual ~DetectorConstruction();

  virtual G4VPhysicalVolume* Construct();
  virtual void ConstructSDandField();

  void SetTargetMaterial(G4String);
  void SetChamberMaterial(G4String);
  void SetMaxStep(G4double);
  void SetCheckOverlaps(G4bool);

 private:
  void DefineMaterials();
  G4VPhysicalVolume* DefineVolumes();

  G4int fNbOfChambers;

  G4LogicalVolume* fLogicTarget;
  G4LogicalVolume** fLogicChamber;

  G4Material* fTargetMaterial;
  G4Material* fChamberMaterial;
  G4Material* SiO2;
  G4Material* Marl;
  G4Material* Kaolinite;
  G4Material* CaCO3;
  G4Material* Mix;

  G4UserLimits* fStepLimit;

  DetectorMessenger* fMessenger;

  G4bool fCheckOverlaps;

  static G4ThreadLocal G4GlobalMagFieldMessenger*  fMagFieldMessenger;
};

} /* namespace MATHUSLA */

#endif /* DETECTORCONSTRUCTION_HH */
