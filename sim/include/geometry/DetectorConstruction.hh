#pragma once

#include "globals.hh"
#include "G4VUserDetectorConstruction.hh"

class G4Box;
class G4Tubs;
class G4LogicalVolume;
class G4VPhysicalVolume;
class G4Material;

using namespace std;


class DetectorConstruction : public G4VUserDetectorConstruction
{
  public:

    DetectorConstruction();
    ~DetectorConstruction();

  public:

    G4VPhysicalVolume* Construct();

    G4Material* GetGeneralAir() {return GeneralAir;}
    G4Material* GetGeneralBeryllium() {return GeneralBeryllium;}
    G4Material* GetGeneralAluminium() {return GeneralAluminium;}
    G4Material* GetGeneralIron() {return GeneralIron;}
    G4Material* GetGeneralLead() {return GeneralLead;}
    G4Material* GetGeneralTungsten() {return GeneralTungsten;}
    G4Material* GetGeneralVacuum() {return GeneralVacuum;}
    G4Material* GetGeneralScintillator() {return GeneralScintillator;}

    inline G4VPhysicalVolume* GetphysiWorld() {return physiWorld;}
    inline G4LogicalVolume* GetlogicWorld() {return logicWorld;}

    // Kill event flag
    void ResetKillEvent() {KillEvent = false;}
    void SetKillEvent() {KillEvent = true;}
    G4bool GetKillEvent() const {return KillEvent;}

  private:

    void DefineMaterials();
    void ConstructGeometry();
    void SaveGDML();

  private:

    G4Material* GeneralAir;
    G4Material* GeneralBeryllium;
    G4Material* GeneralAluminium;
    G4Material* GeneralIron;
    G4Material* GeneralLead;
    G4Material* GeneralTungsten;
    G4Material* GeneralVacuum;
    G4Material* GeneralScintillator;

    G4Box*             solidWorld;
    G4LogicalVolume*   logicWorld;
    G4VPhysicalVolume* physiWorld;

    G4int MuonPairProd;
    G4bool KillEvent;
};
