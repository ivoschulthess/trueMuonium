#pragma once

#include "DetectorConstruction.hh"
#include "G4VUserDetectorConstruction.hh"
#include "UserSD.hh"

class G4Box;
class G4Tubs;
class G4LogicalVolume;
class G4VPhysicalVolume;

class UserSD;

class TargetConstruction
: public UserSD
{
  public:

    TargetConstruction(DetectorConstruction* mainConstructionIn);
    ~TargetConstruction();

    // Override methods from UserSD
    void AccumulateHit(G4Step *theStep) override;
    G4int GetSDIndex(G4StepPoint* SPoint) override;
    G4double GetCalibratedEnergy(G4double EDepStep) override;

  public:

    virtual void ConstructIt();

  private:

    G4LogicalVolume*   logicTarget;
    G4VPhysicalVolume* physiTarget;

    // Values to be specified:
    G4ThreeVector TargetSize;
    G4ThreeVector TargetPos;
    G4Material* TargetMaterial;

    // Pointer to main construction
    DetectorConstruction* mainConstruction;
};
