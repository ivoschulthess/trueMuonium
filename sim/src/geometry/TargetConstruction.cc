#include "TargetConstruction.hh"
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


TargetConstruction::TargetConstruction(DetectorConstruction* mainConstructionIn):
  UserSD("Target"),
  mainConstruction(mainConstructionIn)
{
  // Dissociation cross-section
  TargetMaterial = mainConstruction->GetGeneralLead();
  G4double sigma = pow(TargetMaterial->GetZ(),2.)*1.3e-21;                        // in mm2
  G4double dissLength = 1./((TargetMaterial->GetTotNbOfAtomsPerVolume())*sigma);  // dissociation length in mm
  G4double TargetDepth = 2*dissLength; // number of dissociation lengths
  TargetSize = G4ThreeVector(5.0*cm, 5.0*cm, TargetDepth);

  TargetPos = G4ThreeVector(0.0*cm, 0.0*cm, TargetSize.z()/2.);

  // For SD
  nModule = 1;
  nX = 1;
  nY = 1;
}


void TargetConstruction::ConstructIt()
{
  G4cout << "Constructing " << TargetMaterial->GetName() << " Target at " 
    << TargetPos << " with size " << TargetSize << G4endl;
  G4Box* solidBox = new G4Box("Target",0.5*TargetSize.x(), 0.5*TargetSize.y(), 0.5*TargetSize.z());

  logicTarget = new G4LogicalVolume(solidBox,  
      TargetMaterial, "Target_LV", 0, 0, 0);

  physiTarget = new G4PVPlacement(0,     // no rotation
      TargetPos,                         // its position
      "Target_PV",                       // its name
      logicTarget,                       // its logical volume
      mainConstruction->GetphysiWorld(), // its mother  volume
      false,                             // no boolean operations
      0);                                // copy number

  //Set SD detector for Target
  G4SDManager* SDman = G4SDManager::GetSDMpointer();
  SDman->AddNewDetector( this );
  /*logicTarget->SetSensitiveDetector(this);*/
}


void TargetConstruction::AccumulateHit(G4Step *step) {

  G4ParticleDefinition* theParticleDefinition = step->GetTrack()->GetDefinition();

  G4int ihit = GetSDIndex(step->GetPreStepPoint());

  UserHit* hit = (*(static_cast<UserHitsCollection*>(fHitsCollection)))[ihit];
  hit->AddEnergy(step->GetTotalEnergyDeposit());
}


G4int TargetConstruction::GetSDIndex(G4StepPoint* SPoint)
{
  return 1;
}


G4double TargetConstruction::GetCalibratedEnergy(G4double EDepStep)
{
  return EDepStep;
}


TargetConstruction::~TargetConstruction()
{;}
