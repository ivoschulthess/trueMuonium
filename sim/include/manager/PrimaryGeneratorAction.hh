#pragma once

#include "G4VUserPrimaryGeneratorAction.hh"
#include "PrimaryGeneratorActionMessenger.hh"

#include "G4ios.hh"

class DetectorConstruction;
class G4GeneralParticleSource;
class G4VPrimaryGenerator;
class G4Event;


class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:
    PrimaryGeneratorAction(DetectorConstruction* myDC);    
    ~PrimaryGeneratorAction();

    // Command Getters and Setters
    G4bool GetGenerateTM() const {return generateTM;}
    G4int GetAtomicState() const {return atomicState;}
    G4bool GetSpinAllignTM() const {return spinAllignTM;}
    void SetGenerateTM(G4bool value) {generateTM = value;}
    void SetAtomicState(G4int value) {atomicState = value;}
    void SetSpinAllignTM(G4bool value) {spinAllignTM = value;}


  public:
    void GeneratePrimaries(G4Event* anEvent);

  private:
    G4bool generateTM;
    G4int atomicState;
    G4bool spinAllignTM;
    G4GeneralParticleSource* particleSource;
    PrimaryGeneratorActionMessenger* PGAMessenger;
    DetectorConstruction* myDetector;
};
