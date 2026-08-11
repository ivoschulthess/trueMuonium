/*
 * UserSD.cc
 *
 *  Created on: Jun 17, 2025
 *      Author: bbantoob
 */

#include "UserSD.hh"
#include "UserHit.hh"
#include "G4SystemOfUnits.hh"
#include "G4SDManager.hh"
#include <map>
#include <string>


UserSD::UserSD(G4String _detName):
  G4VSensitiveDetector(_detName),
  detName(_detName), hitsName(collection_name_by_det_name(_detName)), fHitsCollection(nullptr)
{
  collectionName.insert(hitsName);
  HCID = -1;
}

UserSD::UserSD(G4String _detName, G4String _hitsName):
  G4VSensitiveDetector(_detName),
  detName(_detName),hitsName(_hitsName),fHitsCollection(nullptr)
{
  collectionName.insert(hitsName);
  HCID = -1;
}

UserSD::UserSD(G4String _detName, G4int _nModule, G4int _nX, G4int _nY) :
  UserSD(_detName) {
  nModule = _nModule;
  nX = _nX;
  nY = _nY;
}

UserSD::UserSD(G4String _detName, G4int _nModule, G4int _nX, G4int _nY, G4String _hitsName) :
  UserSD(_detName, _hitsName) {
  nModule = _nModule;
  nX = _nX;
  nY = _nY;
}


//purely virtual destructors must be always defined :)
UserSD::~UserSD()
{}

void UserSD::Initialize(G4HCofThisEvent *HCE) {
  fHitsCollection = new UserHitsCollection(GetName(), collectionName[0]);

  if (HCID < 0) {
    HCID = G4SDManager::GetSDMpointer()->GetCollectionID(fHitsCollection);
  }

  HCE->AddHitsCollection(HCID, fHitsCollection);
  /*//Since this SD accumulates the energy deposited in each cell for the whole event, I need to create them here already.*/
  /*for (int im = 0; im < nModule; im++) {*/
  /*  for (int ix = 0; ix < nX; ix++) {*/
  /*    for (int iy = 0; iy < nY; iy++) {*/
  /*      UserHit *hit = new UserHit(im, ix, iy, 0);*/
  /*      (static_cast<UserHitsCollection*>(fHitsCollection))->insert(hit);*/
  /*    }*/
  /*  }*/
  /*}*/
}


G4bool UserSD::ProcessHits(G4Step *aStep, G4TouchableHistory *ROhist) {

  G4Track *track = aStep->GetTrack();
  G4StepPoint *SPoint = aStep->GetPreStepPoint();
  G4TouchableHistory *theTouchable = (G4TouchableHistory*) (SPoint->GetTouchable());

  G4double EdepStep = aStep->GetTotalEnergyDeposit();
  /*G4double StepLength = aStep->GetStepLength();*/
  /*G4double Charge = track->GetDefinition()->GetPDGCharge();*/
  /*G4double Response = EdepStep;*/
  /*G4double birk1 = track->GetMaterial()->GetIonisation()->GetBirksConstant();*/
  /**/
  /*//Standard birks law*/
  /*if (birk1 * EdepStep * StepLength * Charge != 0.) {*/
  /*  Response = EdepStep / (1. + birk1 * EdepStep / StepLength);*/
  /*}*/
  /**/
  /*EdepStep = Response;*/

  /*if (EdepStep>0.)*/
  /*G4cout << " Origin of " << track->GetTrackID() << " " << track->GetOriginTouchable()->GetVolume()->GetName() << G4endl;*/

  if (aStep->IsFirstStepInVolume()
      && aStep->GetTrack()->GetKineticEnergy() > 1.*MeV
      && !(G4StrUtil::contains(track->GetOriginTouchable()->GetVolume()->GetName(), "Scint_PV")))
    this->AccumulateHit(aStep);

  return true;
}


void UserSD::EndOfEvent(G4HCofThisEvent*)
{
  // Perform here the final calibration?
}
