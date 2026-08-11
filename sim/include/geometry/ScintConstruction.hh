#pragma once

#include "DetectorConstruction.hh"
#include "G4VUserDetectorConstruction.hh"
#include "UserSD.hh"

class G4Box;
class G4Tubs;
class G4LogicalVolume;
class G4VPhysicalVolume;

class UserSD;

class ScintConstruction
: public UserSD
{
  public:

    ScintConstruction(DetectorConstruction* mainConstructionIn);
    ~ScintConstruction();

    // Override methods from UserSD
    void AccumulateHit(G4Step *theStep) override;
    G4int GetSDIndex(G4StepPoint* SPoint) override;
    G4double GetCalibratedEnergy(G4double EDepStep) override;

  public:

    virtual void ConstructIt();

  private:

    G4LogicalVolume*   logicScint;
    G4VPhysicalVolume* physiScint;

    // Values to be specified:
    G4double ScintLength;
    G4double ScintOuterRadius;
    G4double ScintInnerRadius;
    G4ThreeVector ScintPos;

    // Pointer to main construction
    DetectorConstruction* mainConstruction;
};
