#include "detector/DetectorConstruction.hh"

#include "Geant4/G4Material.hh"
#include "Geant4/G4NistManager.hh"
#include "Geant4/G4SDManager.hh"
#include "Geant4/G4Box.hh"
#include "Geant4/G4Tubs.hh"
#include "Geant4/G4LogicalVolume.hh"
#include "Geant4/G4PVPlacement.hh"
#include "Geant4/G4GlobalMagFieldMessenger.hh"
#include "Geant4/G4AutoDelete.hh"
#include "Geant4/G4GeometryTolerance.hh"
#include "Geant4/G4GeometryManager.hh"
#include "Geant4/G4UserLimits.hh"
#include "Geant4/G4VisAttributes.hh"
#include "Geant4/G4Colour.hh"
#include "Geant4/G4SystemOfUnits.hh"
#include "Geant4/G4SDParticleFilter.hh"

#include "detector/TrackerSD.hh"

namespace MATHUSLA {

G4ThreadLocal G4GlobalMagFieldMessenger* DetectorConstruction::fMagFieldMessenger = 0;

DetectorConstruction::DetectorConstruction()
    : G4VUserDetectorConstruction(),
      fNbOfChambers(0),
      fLogicTarget(NULL),
      fLogicChamber(NULL),
      fTargetMaterial(NULL),
      fChamberMaterial(NULL),
      fStepLimit(NULL),
      fCheckOverlaps(true) {

  fMessenger = new DetectorMessenger(this);

  fNbOfChambers = 5;
  fLogicChamber = new G4LogicalVolume*[fNbOfChambers];
}

DetectorConstruction::~DetectorConstruction() {
  delete[] fLogicChamber;
  delete   fStepLimit;
  delete   fMessenger;
}

G4VPhysicalVolume* DetectorConstruction::Construct() {
  DefineMaterials();
  return DefineVolumes();
}

void DetectorConstruction::DefineMaterials() {
  // Material definition

  G4NistManager* nistManager = G4NistManager::Instance();

  // Air defined using NIST Manager
  nistManager->FindOrBuildMaterial("G4_AIR");

  // Lead defined using NIST Manager
  fTargetMaterial  = nistManager->FindOrBuildMaterial("G4_Pb");

  // Xenon gas defined using NIST Manager
  fChamberMaterial = nistManager->FindOrBuildMaterial("G4_Xe");

  // Print materials
  G4cout << *(G4Material::GetMaterialTable()) << "\n";
}

G4VPhysicalVolume* DetectorConstruction::DefineVolumes() {
  G4Material* air = G4Material::GetMaterial("G4_AIR");

  // Define Elements

  G4Element* H  = new G4Element("Hydrogen", "H",  1.,     1.01*g/mole);
  G4Element* C  = new G4Element("Carbon",   "C",  6.,    12.01*g/mole);
  G4Element* O  = new G4Element("Oxygen",   "O",  8.,    16.00*g/mole);
  G4Element* Al = new G4Element("Aluminum", "Al", 13.,  26.981*g/mole);
  G4Element* Si = new G4Element("Silicon",  "Si", 14., 28.0855*g/mole);
  G4Element* Ca = new G4Element("Calcium",  "Ca", 20.,  40.078*g/mole);

  CaCO3 = new G4Material("CaCO3", 2.71*g/cm3, 3);
  CaCO3->AddElement(C,  1);
  CaCO3->AddElement(Ca, 1);
  CaCO3->AddElement(O,  3);

  Kaolinite = new G4Material("Clay", 2.65*g/cm3, 4);
  Kaolinite->AddElement(Al, 2);
  Kaolinite->AddElement(Si, 2);
  Kaolinite->AddElement(O,  9);
  Kaolinite->AddElement(H,  4);

  Marl = new G4Material("Marl", 2.46*g/cm3, 2);
  Marl->AddMaterial(Kaolinite, 35*perCent);
  Marl->AddMaterial(CaCO3,     65*perCent);

  SiO2 = new G4Material("Quartz", 2.445*g/cm3, 2);
  SiO2->AddElement(Si, 46.743*perCent);
  SiO2->AddElement(O,  53.257*perCent);

  Mix = new G4Material("Mix", 2.54*g/cm3, 2);
  Mix->AddMaterial(Marl, 50*perCent);
  Mix->AddMaterial(SiO2, 50*perCent);

  // Sizes of the principal geometrical components (solids)

  G4double chamberSpacing = 80*cm;  // from chamber center to center!

  G4double chamberWidth = 20.0*cm;  // width of the chambers
  G4double targetLength =  5.0*cm;  // full length of Target

  G4double trackerLength = (fNbOfChambers+1)*chamberSpacing;

  G4double worldLength = 1.2 * (2*targetLength + trackerLength);

  G4double targetRadius  = 0.5*targetLength;   // Radius of Target
  targetLength = 0.5*targetLength;             // Half length of the Target
  G4double trackerSize   = 0.5*trackerLength;  // Half length of the Tracker

  // Definitions of Solids, Logical Volumes, Physical Volumes

  // World

  G4GeometryManager::GetInstance()->SetWorldMaximumExtent(worldLength);

  G4cout << "Computed tolerance = "
         << G4GeometryTolerance::GetInstance()->GetSurfaceTolerance()/mm
         << " mm\n";

  G4Box* worldS = new G4Box("world", 12000.0*cm, 12000.0*cm, 12000.0*cm);
  G4LogicalVolume* worldLV = new G4LogicalVolume(worldS, air, "World");
  G4VPhysicalVolume* worldPV
    = new G4PVPlacement(0,                // no rotation
                        G4ThreeVector(),  // at (0,0,0)
                        worldLV,          // its logical volume
                        "World",          // its name
                        0,                // its mother  volume
                        false,            // no boolean operations
                        0,                // copy number
                        fCheckOverlaps);  // checking overlaps

  // Earth Material

  G4Box* solidSandstone = new G4Box("Sandstone", 10000.0*cm, 10000.0*cm, 2265.0*cm);
  G4LogicalVolume* logSandstone = new G4LogicalVolume(solidSandstone, SiO2, "Sandstone");
  new G4PVPlacement(0,
                    G4ThreeVector(0, 0, 2265*cm-10000*cm),
                    logSandstone,
                    "Sandstone",
                    worldLV,
                    false,
                    0);

  G4Box* solidMarl = new G4Box("Marl", 10000.0*cm, 10000.0*cm, 912.5*cm);
  G4LogicalVolume* logMarl = new G4LogicalVolume(solidMarl, Marl, "Marl");
  new G4PVPlacement(0,
                    G4ThreeVector(0, 0, 5442.5*cm-10000*cm),
                    logMarl,
                    "Marl",
                    worldLV,
                    false,
                    0);

  G4Box* solidMix = new G4Box("Mix", 10000.0*cm, 10000.0*cm, 1822.5*cm);
  G4LogicalVolume* logMix = new G4LogicalVolume(solidMix, Mix, "Mix");
  new G4PVPlacement(0,
                    G4ThreeVector(0, 0, 8177.5*cm-10000*cm),
                    logMix,
                    "Mix",
                    worldLV,
                    false,
                    0);

  // Target

  G4ThreeVector positionTarget = G4ThreeVector(0, 0, -(targetLength+trackerSize));

  G4Tubs* targetS = new G4Tubs("target", 0., targetRadius, targetLength, 0.*deg, 360.*deg);

  fLogicTarget = new G4LogicalVolume(targetS, fTargetMaterial, "Target", 0, 0, 0);

  new G4PVPlacement(0,                // no rotation
                    positionTarget,   // at (x,y,z)
                    fLogicTarget,     // its logical volume
                    "Target",         // its name
                    worldLV,          // its mother volume
                    false,            // no boolean operations
                    0,                // copy number
                    fCheckOverlaps);  // checking overlaps

  G4cout << "Target is " << 2*targetLength/cm << " cm of "
         << fTargetMaterial->GetName() << "\n";

  // Tracker

  G4ThreeVector positionTracker = G4ThreeVector(0, 0, 10250.0*cm-10000*cm);

  G4Tubs* trackerS = new G4Tubs("tracker", 0, trackerSize, trackerSize, 0.*deg, 360.*deg);
  G4LogicalVolume* trackerLV = new G4LogicalVolume(trackerS, air, "Tracker", 0, 0, 0);

  new G4PVPlacement(0,                // no rotation
                    positionTracker,  // at (x,y,z)
                    trackerLV,        // its logical volume
                    "Tracker",        // its name
                    worldLV,          // its mother  volume
                    false,            // no boolean operations
                    0,                // copy number
                    fCheckOverlaps);  // checking overlaps

  // Visualization attributes

  G4VisAttributes* boxVisAtt     = new G4VisAttributes(G4Colour(1.0, 1.0, 1.0));
  G4VisAttributes* chamberVisAtt = new G4VisAttributes(G4Colour(1.0, 1.0, 0.0));

  worldLV->SetVisAttributes(boxVisAtt);
  fLogicTarget->SetVisAttributes(boxVisAtt);
  trackerLV->SetVisAttributes(boxVisAtt);

  // Tracker segments

  G4cout << "There are " << fNbOfChambers << " chambers in the tracker region.\n"
         << "The chambers are " << chamberWidth/cm << " cm of "
         << fChamberMaterial->GetName() << "\n"
         << "The distance between chamber is " << chamberSpacing/cm << " cm\n";

  G4double firstPosition = -trackerSize + chamberSpacing;
  G4double firstLength   = trackerLength/10;
  G4double lastLength    = trackerLength;

  G4double halfWidth = 0.5 * chamberWidth;
  G4double rmaxFirst = 0.5 * firstLength;

  G4double rmaxIncr = 0.0;
  if (fNbOfChambers > 0) {
    rmaxIncr =  0.5 * (lastLength-firstLength)/(fNbOfChambers-1);
    if (chamberSpacing  < chamberWidth) {
       G4Exception("DetectorConstruction::DefineVolumes()",
                   "InvalidSetup", FatalException,
                   "Width>Spacing");
    }
  }

  for (G4int copyNo = 0; copyNo < fNbOfChambers; copyNo++) {
      G4double Zposition = firstPosition + copyNo * chamberSpacing;
      G4double rmax = rmaxFirst + copyNo * rmaxIncr;

      G4Tubs* chamberS = new G4Tubs("Chamber_solid",
                                    0,
                                    rmax,
                                    halfWidth,
                                    0.*deg,
                                    360.*deg);

      fLogicChamber[copyNo] = new G4LogicalVolume(chamberS, fChamberMaterial, "Chamber_LV", 0, 0, 0);
      fLogicChamber[copyNo]->SetVisAttributes(chamberVisAtt);

      new G4PVPlacement(0,                               // no rotation
                        G4ThreeVector(0, 0, Zposition),  // at (x,y,z)
                        fLogicChamber[copyNo],           // its logical volume
                        "Chamber_PV",                    // its name
                        trackerLV,                       // its mother  volume
                        false,                           // no boolean operations
                        copyNo,                          // copy number
                        fCheckOverlaps);                 // checking overlaps
  }

  // Example of User Limits
  //
  // Below is an example of how to set tracking constraints in a given
  // logical volume
  //
  // Sets a max step length in the tracker region, with G4StepLimiter

  G4double maxStep = 0.5 * chamberWidth;
  fStepLimit = new G4UserLimits(maxStep);
  trackerLV->SetUserLimits(fStepLimit);

  /// Set additional contraints on the track, with G4UserSpecialCuts
  ///
  /// G4double maxLength = 2*trackerLength, maxTime = 0.1*ns, minEkin = 10*MeV;
  /// trackerLV->SetUserLimits(new G4UserLimits(maxStep,
  ///                                           maxLength,
  ///                                           maxTime,
  ///                                           minEkin));

  return worldPV;
}

void DetectorConstruction::ConstructSDandField() {
  TrackerSD* detector = new TrackerSD("MATHUSLA/TrackerSD", "TrackerHC");

  detector->SetFilter(new G4SDParticleFilter("muon filter", "mu-"));
  G4SDManager::GetSDMpointer()->AddNewDetector(detector);

  // Setting detector to all logical volumes with the same name
  // of "Chamber_LV".
  SetSensitiveDetector("Chamber_LV", detector, true);

  // Create global magnetic field messenger.
  // Uniform magnetic field is then created automatically if
  // the field value is not zero.
  G4ThreeVector fieldValue = G4ThreeVector();
  fMagFieldMessenger = new G4GlobalMagFieldMessenger(fieldValue);
  fMagFieldMessenger->SetVerboseLevel(1);

  // Register the field messenger for deleting
  G4AutoDelete::Register(fMagFieldMessenger);
}

void DetectorConstruction::SetTargetMaterial(G4String materialName) {
  G4NistManager* nistManager = G4NistManager::Instance();
  G4Material* pttoMaterial = nistManager->FindOrBuildMaterial(materialName);

  if (fTargetMaterial != pttoMaterial) {
     if ( pttoMaterial ) {
        fTargetMaterial = pttoMaterial;
        if (fLogicTarget) fLogicTarget->SetMaterial(fTargetMaterial);
        G4cout << "\n----> The target is made of " << materialName << "\n";
     } else {
        G4cout << "\n-->  WARNING from SetTargetMaterial : "
               << materialName
               << " not found\n";
     }
  }
}

void DetectorConstruction::SetChamberMaterial(G4String materialName) {
  G4NistManager* nistManager = G4NistManager::Instance();
  G4Material* pttoMaterial = nistManager->FindOrBuildMaterial(materialName);

  if (fChamberMaterial != pttoMaterial) {
     if (pttoMaterial) {
        fChamberMaterial = pttoMaterial;
        for (G4int copyNo = 0; copyNo < fNbOfChambers; copyNo++) {
            if (fLogicChamber[copyNo])
                fLogicChamber[copyNo]->SetMaterial(fChamberMaterial);
        }
        G4cout << "\n----> The chambers are made of " << materialName << "\n";
     } else {
        G4cout << "\n-->  WARNING from SetChamberMaterial : "
               << materialName
               << " not found\n";
     }
  }
}

void DetectorConstruction::SetMaxStep(G4double maxStep) {
  if (fStepLimit && (maxStep > 0.)) fStepLimit->SetMaxAllowedStep(maxStep);
}

void DetectorConstruction::SetCheckOverlaps(G4bool checkOverlaps) {
  fCheckOverlaps = checkOverlaps;
}

} /* namespace MATHUSLA */
