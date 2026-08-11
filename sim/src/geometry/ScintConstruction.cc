#include "ScintConstruction.hh"
#include "UserSD.hh"

#include "G4Material.hh"
#include "G4MaterialTable.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SubtractionSolid.hh"
#include "G4GeometryManager.hh"
#include "G4PVReplica.hh"
#include "G4TransportationManager.hh"
#include "G4SDManager.hh"
#include "G4SystemOfUnits.hh"

#include "G4ios.hh"


ScintConstruction::ScintConstruction(DetectorConstruction* mainConstructionIn):
  UserSD("Detector"),
  mainConstruction(mainConstructionIn)
{
  ScintLength = 150.0*cm;
  ScintOuterRadius = 18.0*cm;
  ScintInnerRadius = 15.0*cm;

  ScintPos = G4ThreeVector(0.0*cm, 0.0*cm, 25.0*cm);

  // For SD
  nModule = 1;
  nX = 1;
  nY = 1;
}


void ScintConstruction::ConstructIt()
{
  // Cylinder detector
  G4Tubs* solidTube = new G4Tubs("Scint", ScintInnerRadius, ScintOuterRadius, 0.5*ScintLength, 0., 360. * deg);

  logicScint = new G4LogicalVolume(solidTube,  
      mainConstruction->GetGeneralScintillator(), "Scint_LV", 0, 0, 0);

  physiScint = new G4PVPlacement(0,       // no rotation
      ScintPos,                           // its position
      "Scint_PV",                         // its name
      logicScint,                         // its logical volume
      mainConstruction->GetphysiWorld(),  // its mother  volume
      false,                              // no boolean operations
      0);                                 // copy number

  logicScint->SetSensitiveDetector(this);

  // Cylinder cap
  G4double ScintCapThickness = 2.0*cm;
  G4Tubs* solidCap = new G4Tubs("ScintCap", 0, ScintOuterRadius, 0.5*ScintCapThickness, 0., 360. * deg);

  logicScint = new G4LogicalVolume(solidCap,
      mainConstruction->GetGeneralScintillator(), "ScintCap_LV", 0, 0, 0);

  G4ThreeVector CapPos = ScintPos+G4ThreeVector(0,0,0.5*ScintLength+0.5*ScintCapThickness);

  // physiScint = new G4PVPlacement(0,       // no rotation*/
  //     CapPos,                             // its position*/
  //     "Scint_PV",                         // its name*/
  //     logicScint,                         // its logical volume*/
  //     mainConstruction->GetphysiWorld(),  // its mother  volume*/
  //     false,                              // no boolean operations*/
  //     1);                                 // copy number*/
      
  //Set SD detector for Scint
  G4SDManager* SDman = G4SDManager::GetSDMpointer();
  SDman->AddNewDetector( this );

  logicScint->SetSensitiveDetector(this);
}


void ScintConstruction::AccumulateHit(G4Step *step) {

  G4ParticleDefinition* theParticleDefinition = step->GetTrack()->GetDefinition();

  G4int pdgID=theParticleDefinition->GetPDGEncoding();

  G4Track *track = step->GetTrack();
  G4double edep = track->GetTotalEnergy() / MeV;
  G4double time = track->GetGlobalTime() / ns;
  G4ThreeVector hitMom = track->GetMomentum() / MeV;

  G4StepPoint *SPoint = step->GetPreStepPoint();
  G4TouchableHistory* theTouchable = (G4TouchableHistory*) (SPoint->GetTouchable());
  G4ThreeVector hitPos = SPoint->GetPosition();

  // Create new UserHit
  // Save information of hit in detector
  UserHit* hit = new UserHit();
  hit->SetModule(theTouchable->GetVolume()->GetCopyNo());
  hit->SetPDGID(pdgID);
  hit->SetEnergy(edep);
  hit->SetTime(time);
  hit->SetMomentum(hitMom);
  hit->SetPosition(hitPos);

  // Add hit to HC
  (static_cast<UserHitsCollection*>(fHitsCollection))->insert(hit);

}


G4int ScintConstruction::GetSDIndex(G4StepPoint* SPoint)
{
  G4TouchableHistory* theTouchable = (G4TouchableHistory*) (SPoint->GetTouchable());

  return 1;
}


G4double ScintConstruction::GetCalibratedEnergy(G4double EDepStep)
{
  return EDepStep;
}


ScintConstruction::~ScintConstruction()
{;}
