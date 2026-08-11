#include "DetectorConstruction.hh"

#include "G4Material.hh"
#include "G4MaterialTable.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4GDMLParser.hh"
#include "G4VisAttributes.hh"
#include "G4ios.hh"
#include "G4NistManager.hh"

// User classes
#include "TargetConstruction.hh"
#include "ScintConstruction.hh"


DetectorConstruction::DetectorConstruction()
{
}


DetectorConstruction::~DetectorConstruction()
{
}


G4VPhysicalVolume* DetectorConstruction::Construct()
{
  DefineMaterials();
  ConstructGeometry();
  SaveGDML();
  return GetphysiWorld();
}


void DetectorConstruction::DefineMaterials() {
  G4double a, iz, z, density;
  G4String name, symbol;
  G4int nel;

  G4NistManager* man = G4NistManager::Instance();

  // Elements: ----------------

  a = 1.01*g/mole;
  G4Element* elH = new G4Element(name="Hydrogen", symbol="H", iz=1., a);

  a = 12.011*g/mole;
  G4Element* elC = new G4Element(name="Carbon", symbol="C", iz=6., a);

  a = 14.01*g/mole;
  G4Element* elN = new G4Element(name="Nitrogen", symbol="N", iz=7., a);

  a = 16.00*g/mole;
  G4Element* elO = new G4Element(name="Oxygen", symbol="O", iz=8., a);

  // Materials: ----------------

  // Air
  GeneralAir  = man->FindOrBuildMaterial("G4_AIR");

  // Beryllium
  GeneralBeryllium = man->FindOrBuildMaterial("G4_Be");

  // Aluminium
  GeneralAluminium = man->FindOrBuildMaterial("G4_Al");

  // Iron
  GeneralIron = man->FindOrBuildMaterial("G4_Fe");

  // Lead
  GeneralLead = man->FindOrBuildMaterial("G4_Pb");

  // Tungsten
  GeneralTungsten = man->FindOrBuildMaterial("G4_W");

  // Vacuum
  a = 1.*g/mole;
  density = 1.e-15*g/cm3;
  GeneralVacuum = new G4Material(name="GeneralVacuum",z=1.,a,density);

  // Scintillator -- Polystyrene [(C6H5CHCH2)n]
  // Values from: https://pdg.lbl.gov/2023/AtomicNuclearProperties/HTML/polystyrene.html
  density = 1.060*g/cm3;
  GeneralScintillator = new G4Material(name="GeneralScintillator", density, nel=2);
  GeneralScintillator->AddElement(elC, 8);
  GeneralScintillator->AddElement(elH, 8);
  GeneralScintillator->GetIonisation()->SetBirksConstant(0.126 * mm / MeV);
}

void DetectorConstruction::ConstructGeometry() {
  //------------------------------
  // World
  //------------------------------

  G4double WorldWidth = 100.*cm;
  G4double WorldHeight = 100.*cm;
  G4double WorldDepth = 500.*cm;

  solidWorld= new G4Box("World",0.5*WorldWidth,0.5*WorldHeight,0.5*WorldDepth);
  logicWorld= new G4LogicalVolume( solidWorld, GeneralVacuum, "WorldLV", 0, 0, 0);

  //  Must place the World Physical volume unrotated at (0,0,0).
  //
  physiWorld = new G4PVPlacement(0,    // no rotation
      G4ThreeVector(),                 // at (0,0,0)
      "WorldPV",                       // its name
      logicWorld,                      // its logical volume
      0,                               // its mother  volume
      false,                           // no boolean operations
      0);                              // no field specific to volume


  //--------- Definitions of Solids, Logical Volumes, Physical Volumes ---------
  // Target
  TargetConstruction* subdetTargetConstruction = new TargetConstruction(this);
  if(subdetTargetConstruction) subdetTargetConstruction->ConstructIt();
  // Detector
  ScintConstruction* subdetScintConstruction = new ScintConstruction(this);
  if(subdetScintConstruction) subdetScintConstruction->ConstructIt();

}

void DetectorConstruction::SaveGDML() {
  //--------- Visualization attributes -------------------------------
  logicWorld->SetVisAttributes (G4VisAttributes::GetInvisible());
  G4GDMLParser parser;
  const char * gdmlname = "setup.gdml";
  ifstream f(gdmlname);
  if(f.good()) { //it exists                                                      
  } else {
    parser.Write(gdmlname, physiWorld);
  }
}
