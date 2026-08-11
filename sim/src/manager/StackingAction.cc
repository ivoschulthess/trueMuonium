#include "G4ios.hh"

#include "StackingAction.hh"
#include "EventAction.hh"
#include "DetectorConstruction.hh"
#include "G4ParticleTypes.hh"
#include "G4DynamicParticle.hh"
#include "G4EventManager.hh"
#include "G4TrackVector.hh"
#include "G4SystemOfUnits.hh"

#include "G4KaonZero.hh"
#include "G4AntiKaonZero.hh"
#include "G4KaonZeroLong.hh"
#include "G4KaonZeroShort.hh"

#include "G4StackManager.hh"

StackingAction::StackingAction(DetectorConstruction *myDC) :
  myDetector(myDC), stage(0)
{;}

StackingAction::~StackingAction()
{;}

G4ClassificationOfNewTrack StackingAction::ClassifyNewTrack(const G4Track *aTrack) {

  G4ClassificationOfNewTrack classification;

  /*First stage*/
  if (stage == 0) {
    /*Store all high-energy particles in the urgent stack, wait for the others*/
    /*After all fUrgent tracks are processed, NewStage() is called*/
    G4double ene = aTrack->GetTotalEnergy();
    /*If energy is below ~210*MeV, muon pair production is kinematically forbidden */
    G4double minEnergy = 210*MeV;
    if (ene > minEnergy) {
      classification = fUrgent;
    } else {
      classification = fWaiting;
    }
  }
  /*Second stage - if we are here, all tracks are urgent*/
  else {
    classification = G4UserStackingAction::ClassifyNewTrack(aTrack);
  }
  return classification;
}

void StackingAction::NewStage() {
  stage = 1;

  //If there is a DM production, re-classify all tracks according to stage=1. Otherwise, kill event
  /*if (myDetector->SimulateMuPairProdOnly()) {*/
  /*  if (myDetector->GetMuPairProd()) {*/
  /*    stackManager->ReClassify();*/
  /*  } else {*/
  /*    myDetector->SetKillEvent();*/
  /*    stackManager->clear();*/
  /*  }*/
  /*}*/
}

void StackingAction::PrepareNewEvent() {
  stage = 0;
  myDetector->ResetKillEvent();
}
