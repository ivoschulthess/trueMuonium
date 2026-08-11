#include "EventAction.hh"
#include "DetectorConstruction.hh"
#include "SteppingAction.hh"
#include "UserSD.hh"

#include "TFile.h"
#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4TrajectoryContainer.hh"
#include "G4Trajectory.hh"
#include "G4VVisManager.hh"
#include "G4UImanager.hh"
#include "G4SystemOfUnits.hh"
#include "G4ios.hh"
#include "G4SDManager.hh"
#include "Randomize.hh"

#include "EventInfo.hh"


EventAction::EventAction(DetectorConstruction* myDC)
  : myDetector(myDC), NSaved(0), hasMuPair(false)
{
  myEventInfo = new EventInfo();
  EAMessenger = new EventActionMessenger(this);
  myDetector->ResetKillEvent();
}


EventAction::~EventAction()
{
  // Printout
  G4cout << "Total number of saved events = " << NSaved << G4endl;

  // Save ROOT histos in file
  myEventInfo->DumpROOT(fROOTFileName);
  delete EAMessenger;
}


void EventAction::BeginOfEventAction(const G4Event* event)
{
  // set the state of the RNG engine from file if requested
  if (!fRNGFileName.empty()) {

    if (fVerboseLevel>0)
      G4cout << "> Restoring the state of the RNG engine from file " << fRNGFileName << G4endl;

    // open the file
    TFile* file = TFile::Open(fRNGFileName.c_str(), "READ");
    if (!file || file->IsZombie()) {
      G4cerr << "ERROR! EventAction: Could not open file " << fRNGFileName << " to restore the state of the RNG engine!" << G4endl;
      return;
    }

    // get the tree
    TTree* tree = (TTree*)file->Get("tree");
    if (!tree) {
        G4cerr << "TTree 'tree' not found!" << G4endl;
        file->Close();
        delete file;
        return;
    }

    // set branch address and get entry
    std::vector<unsigned long>* rngState = nullptr;
    tree->SetBranchAddress("rngState", &rngState);

    // get the first entry
    if (tree->GetEntries() > fRNGEventIdx) {
        
      if (fVerboseLevel>0)
        G4cout << "> Getting entry " << fRNGEventIdx << " from the tree" << G4endl;

      tree->GetEntry(fRNGEventIdx);

      if (!rngState) {
          G4cerr << "RNG state pointer is null!" << G4endl;
          file->Close();
          delete file; 
          return;
      }
      
      // set the state of the RNG engine
      // *get* the state from a vector
      G4Random::getTheEngine()->get(*rngState);

      if (fVerboseLevel>1) {
        G4cout << ">> RNG state from file: ";
        for (auto s : *rngState) {
            G4cout << s << " ";
        }
        G4cout << G4endl;
      }

    } else {
        G4cerr << "Not enough entries in the tree to access index " << fRNGEventIdx << G4endl;
    }

    file->Close();
    delete file; 
  }

  // get the status of the RNG engine
  // *put* the state into a vector
  std::vector<unsigned long> state = G4Random::getTheEngine()->put();
  SetRNGState(state);

  if (fVerboseLevel>1) {
    G4cout << ">> RNG state at the beginning of event: ";
    for (auto s : state) {
        G4cout << s << " ";
    }
    G4cout << G4endl;
  }

  theSteppingAction->Reset();
  myEventInfo->Reset();
  SetMuPairProd(false);
}


void EventAction::EndOfEventAction(const G4Event* evt)
{
  theSteppingAction->Finalize();

  G4HCofThisEvent* HCE = evt->GetHCofThisEvent();

  // get general info and save it to EventInfo  
  myEventInfo->FillGeneral(evt->GetEventID(), fRNGState);

  UserHitsCollection* TargetHits = 0;
  static G4int indexTarget = G4SDManager::GetSDMpointer()->GetCollectionID(UserSD::collection_name_by_det_name("Target"));
  if(indexTarget >= 0) TargetHits = (UserHitsCollection*) HCE->GetHC(indexTarget);
  if(TargetHits) {
    myEventInfo->FillTarget(TargetHits);
  }

  UserHitsCollection* DetectorHits = 0;
  static G4int indexDetector = G4SDManager::GetSDMpointer()->GetCollectionID(UserSD::collection_name_by_det_name("Detector"));
  if(indexDetector >= 0) DetectorHits = (UserHitsCollection*) HCE->GetHC(indexDetector);
  if(DetectorHits) {
    myEventInfo->FillDetector(DetectorHits);
  }


  if((fTriggerLevel == 1 && hasMuPair)
      || (fTriggerLevel == 2 && (hasMuPair || myEventInfo->hasHit()))
      || fTriggerLevel > 2) {
    CountNSaved();

    // Write event to file
    /*ofstream outFile("output.txt", std::ofstream::app);*/
    /*outFile << "EVENT" << std::endl;*/
    /*outFile << evt->GetEventID() << " " << NSaved << std::endl;*/
    /*myEventInfo->DumpInfo(outFile);*/
    /*outFile << "ENDEVENT" << std::endl;*/
    /*outFile.close();*/

    // Fill histos
    myEventInfo->FillHistos();
  }

  if(fVerboseLevel>0) G4cout << "> End of event " << evt->GetEventID() << G4endl;
}
