#ifndef DETECTOR_PMT_HH
#define DETECTOR_PMT_HH
#pragma once

/* -------------------------------------------------------------------------- *\
|---------------------------- !!!!!! DELETE !!!!!! ----------------------------|
\* -------------------------------------------------------------------------- */

#include "Geant4/G4SystemOfUnits.hh"
#include "Geant4/G4Tubs.hh"
#include "Geant4/G4NistManager.hh"
#include "Geant4/G4LogicalVolume.hh"

#include "Construction.hh"

namespace MATHUSLA { namespace MU {

namespace PMT {

static auto Material = G4NistManager::Instance()->FindOrBuildMaterial("G4_C");

// TO REMOVE

static constexpr G4double angle         =   4.5 * deg;
static constexpr G4double thickness     =   2.0 * cm;
static constexpr G4double envthickness  =  40.0 * cm;
static constexpr G4double anglep        =  45.0 * deg;

static constexpr G4double innerRadius   =   0.0 * cm;
static constexpr G4double outerRadius   =   2.1 * cm;
static constexpr G4double startAngle    =   0.0 * deg;
static constexpr G4double spanningAngle = 360.0 * deg;
static constexpr G4double hz            =  19.3 * cm;
static const     G4double trx           =    hz * std::cos(anglep);

static constexpr G4double phiz_1 =  90.0 * deg - 0.5 * angle;
static constexpr G4double phix_1 =  90.0 * deg               + phiz_1;
static constexpr G4double phiz_2 = 270.0 * deg - 0.5 * angle;
static constexpr G4double phix_2 = 360.0 * deg - 0.5 * angle;
static constexpr G4double phiz_3 =  90.0 * deg - 0.5 * angle + anglep;
static constexpr G4double phix_3 =  90.0 * deg               + phiz_3;
static constexpr G4double phiz_4 = 270.0 * deg - 0.5 * angle + 45.0 * cm;
static constexpr G4double phix_4 = 360.0 * deg - 0.5 * angle + 45.0 * cm;
static constexpr G4double phiz_5 =  90.0 * deg - 0.5 * angle - anglep;
static constexpr G4double phix_5 =  90.0 * deg               + phiz_5;

//static auto rot_1 = Construction::Matrix(90.0 * deg, phix_1, 0, 0, 90.0 * deg, phiz_1);
//static auto rot_2 = Construction::Matrix(90.0 * deg, phix_2, 0, 0, 90.0 * deg, phiz_2);
//static auto rot_3 = Construction::Matrix(90.0 * deg, phix_3, 0, 0, 90.0 * deg, phiz_3);
//static auto rot_4 = Construction::Matrix(90.0 * deg, phix_4, 0, 0, 90.0 * deg, phiz_4);
//static auto rot_5 = Construction::Matrix(90.0 * deg, phix_5, 0, 0, 90.0 * deg, phiz_5);

// END TO REMOVE

} /* namespace PMT */

} } /* namespace MATHUSLA::MU */

#endif /* DETECTOR_PMT_HH */