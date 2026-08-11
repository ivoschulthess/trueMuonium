#pragma once
/*
 * UserSD.hh
 *
 *  Created on: Jun 17, 2025
 *      Author: bbantoob
 */


#include <G4VSensitiveDetector.hh>

#include "UserHit.hh"

#include <map>
#include <string>

class G4VHitsCollection;

class UserSD: public G4VSensitiveDetector {

  private:
    // Purely virtual functions to be overriden by detector-specific definitions
    // --- Defined in Detector Classes! ---
    virtual void     AccumulateHit(G4Step *theStep)=0;
    virtual G4int    GetSDIndex(G4StepPoint* SPoint)=0;
    virtual G4double GetCalibratedEnergy(G4double EDepStep)=0;


  protected:
    G4String detName;
    G4String hitsName;
    G4VHitsCollection* fHitsCollection;

    G4int HCID;

    //Variables to create the hits
    G4int nModule,nX,nY;

  public:
    UserSD(G4String _detName);
    UserSD(G4String _detName, G4String _hitsName);
    UserSD(G4String _detName, int _nModule, int _nX, int _nY);
    UserSD(G4String _detName, int _nModule, int _nX, int _nY, G4String _hitsName);
    virtual ~UserSD()=0; //purely virtual

    //methods from G4
    virtual void Initialize(G4HCofThisEvent*);
    virtual void EndOfEvent(G4HCofThisEvent*);
    virtual G4bool ProcessHits(G4Step *aStep, G4TouchableHistory *ROhist);

    static G4String collection_name_by_det_name(const G4String & detName) {return G4String("User") + detName + G4String("Hits");}
};
