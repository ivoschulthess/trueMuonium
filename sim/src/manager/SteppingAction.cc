#include "G4ios.hh"

#include "SteppingAction.hh"
#include "EventAction.hh"
#include "DetectorConstruction.hh"

#include "G4SteppingManager.hh"
#include "G4VTouchable.hh"
#include "G4TouchableHistory.hh"

#include "G4ProcessManager.hh"
#include "G4ProcessType.hh"
#include "Randomize.hh"

#include "G4ParticleTypes.hh"
#include "G4DynamicParticle.hh"
#include "G4EventManager.hh"
#include "G4TrackVector.hh"
#include "G4SystemOfUnits.hh"

#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4Gamma.hh"

#include "G4ParticleTypes.hh"
#include "G4DynamicParticle.hh"
#include "G4EventManager.hh"
#include "G4TrackVector.hh"


SteppingAction::SteppingAction(DetectorConstruction* myDC, EventAction* myEA)
  : myDetector(myDC), eventAction(myEA)
{
  eventAction->SetSteppingAction(this);
}


void SteppingAction::UserSteppingAction(const G4Step* aStep)
{
  if(myDetector->GetKillEvent()) {
    aStep->GetTrack()->SetKineticEnergy(0.);
    aStep->GetTrack()->SetTrackStatus(fStopAndKill);
  }

  G4StepPoint* SPointPreStep = aStep->GetPreStepPoint();
  G4StepPoint* SPointPostStep = aStep->GetPostStepPoint();

  // Save parent particles info before decay
  if(aStep->GetTrack()->GetCurrentStepNumber() == 1
      && (aStep->GetTrack()->GetParticleDefinition()->GetParticleName()).find(string("tm")) != string::npos
      && aStep->GetTrack()->GetTrackID() == 1) {
    eventAction->GetEventInfoPointer()->AddParentInfo(aStep);

    if(eventAction->GetVerboseLevel() > 0) G4cout << "True muonium production with E = " << SPointPreStep->GetKineticEnergy()/MeV << " at (" << SPointPreStep->GetPosition()[0] << ", " << SPointPreStep->GetPosition()[1] << ", " << SPointPreStep->GetPosition()[2] << ")" << G4endl;
  }

  // Save daughter particles info from decay
  if(SPointPostStep->GetProcessDefinedStep() != 0) {
    if((SPointPostStep->GetProcessDefinedStep()->GetProcessName()).find(string("Decay")) != string::npos) {
      if((aStep->GetTrack()->GetParticleDefinition()->GetParticleName()).find(string("tm")) != string::npos) {

        eventAction->GetEventInfoPointer()->AddDaughtersInfo(aStep);

        if(eventAction->GetVerboseLevel() > 0) G4cout << "True muonium decay with E = " << SPointPostStep->GetKineticEnergy()/MeV << " at (" << SPointPostStep->GetPosition()[0] << ", " << SPointPostStep->GetPosition()[1] << ", " << SPointPostStep->GetPosition()[2] << ")" << G4endl;
      }
    }
  }

  // Save muon pair from Bethe-Heitler process
  if(SPointPostStep->GetProcessDefinedStep() != 0) {
    if((SPointPostStep->GetProcessDefinedStep()->GetProcessName()).find(string("GammaToMuPair")) != string::npos) {
      if(abs(aStep->GetTrack()->GetParticleDefinition()->GetPDGEncoding()) == 22) { // Gamma
        if(eventAction->GetVerboseLevel() > 0) G4cout << "Muon pair production with E = " << SPointPreStep->GetKineticEnergy()/MeV << " at (" << SPointPreStep->GetPosition()[0] << ", " << SPointPreStep->GetPosition()[1] << ", " << SPointPreStep->GetPosition()[2] << ")" << G4endl;
        eventAction->SetMuPairProd();
        eventAction->GetEventInfoPointer()->FillMuons(aStep);
      }
    }
  }

  // Stop simulation when detector is reached
  if(SPointPreStep->GetPhysicalVolume() != nullptr) {
    if(G4StrUtil::contains(SPointPreStep->GetPhysicalVolume()->GetName(), "Scint")) {
      aStep->GetTrack()->SetKineticEnergy(0.);
      aStep->GetTrack()->SetTrackStatus(fStopAndKill);
    }
  }


}


void SteppingAction::Reset()
{;}


void SteppingAction::Finalize()
{
}
