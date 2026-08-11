#include "PrimaryGeneratorAction.hh"

#include "DetectorConstruction.hh"

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4GeneralParticleSource.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "Randomize.hh"
#include "G4SystemOfUnits.hh"


PrimaryGeneratorAction::PrimaryGeneratorAction(DetectorConstruction* myDC) :
  myDetector(myDC)
{
  generateTM = false;
  atomicState = 1;
  particleSource = new G4GeneralParticleSource();
  PGAMessenger = new PrimaryGeneratorActionMessenger(this);
}

PrimaryGeneratorAction::~PrimaryGeneratorAction() {
  delete particleSource;
  delete PGAMessenger;
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  if (!GetGenerateTM()) particleSource->GeneratePrimaryVertex(anEvent);

  else {
    if (GetSpinAllignTM()) { // ortho-states
      if (GetAtomicState() == 0) { // all available states
        G4double state = sqrt(pow(CLHEP::RandGauss::shoot(0,1),2));
        if (state <= 1.6) {
          particleSource->SetParticleDefinition(G4ParticleTable::GetParticleTable()
              ->FindParticle("otm-1s"));
        }
        else {
          particleSource->SetParticleDefinition(G4ParticleTable::GetParticleTable()
              ->FindParticle("otm-2s"));
        }
      }
      else if (GetAtomicState() == 1) { // 1s state
        particleSource->SetParticleDefinition(G4ParticleTable::GetParticleTable()
            ->FindParticle("otm-1s"));
      }
      else if (GetAtomicState() == 2) { // 2s state
        particleSource->SetParticleDefinition(G4ParticleTable::GetParticleTable()
            ->FindParticle("otm-2s"));
      }
    }
    else {
      if (GetAtomicState() == 0) { // all available states
        G4double state = sqrt(pow(CLHEP::RandGauss::shoot(0,1),2));
        if (state <= 1.6) {
          particleSource->SetParticleDefinition(G4ParticleTable::GetParticleTable()
              ->FindParticle("ptm-1s"));
        }
        else {
          particleSource->SetParticleDefinition(G4ParticleTable::GetParticleTable()
              ->FindParticle("ptm-2s"));
        }
      }
      else if (GetAtomicState() == 1) { // 1s state
        particleSource->SetParticleDefinition(G4ParticleTable::GetParticleTable()
            ->FindParticle("ptm-1s"));
      }
      else if (GetAtomicState() == 2) { // 2s state
        particleSource->SetParticleDefinition(G4ParticleTable::GetParticleTable()
            ->FindParticle("ptm-2s"));
      }
    }
    particleSource->GeneratePrimaryVertex(anEvent);

  }

}
