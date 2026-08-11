#include "DetectorConstruction.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "EventAction.hh"
#include "SteppingAction.hh"
#include "StackingAction.hh"

#include "QGSP_BERT.hh"
#include "FTFP_BERT.hh"
#include "G4ExoticPhysics.hh"

#include "G4UImanager.hh"
#include "G4UIExecutive.hh"

#include "G4RunManager.hh"
#include "G4PhysListFactory.hh"
#include "G4ios.hh"

#include "G4VisExecutive.hh"

#include <iostream>


int main(int argc,char** argv) {
  // Run manager
  G4RunManager * runManager = new G4RunManager;

  // UserInitialization classes
  DetectorConstruction* mkexp = new DetectorConstruction;
  runManager->SetUserInitialization(mkexp);

  G4PhysListFactory factory;
  G4VModularPhysicsList * phys = factory.GetReferencePhysList("FTFP_BERT");
  // ^^^ most of the standard physics lists are available by this interface

  // ___ Here the "extension" part starts ___
  G4ExoticPhysics* myPhysics = new G4ExoticPhysics();
  phys->RegisterPhysics(myPhysics);
  // ^^^ Here the "extension" part ends ^^^
  runManager->SetUserInitialization(phys);  // init phys


  // Visualization, if you choose to have it!
  G4VisManager* visManager = new G4VisExecutive;

  visManager->Initialize();

  // UserAction classes
  runManager->SetUserAction(new PrimaryGeneratorAction(mkexp));

  // Define UserEventAction to store and write event information to output
  EventAction* myEA = new EventAction(mkexp);
  runManager->SetUserAction(myEA);
  // Define UserSteppingAction to save TM information
  runManager->SetUserAction(new SteppingAction(mkexp, myEA));
  // Define UserStackingAction to skip non-TM events (non-active at the moment)
  runManager->SetUserAction(new StackingAction(mkexp));
  // Define UserRunAction to bias muon pair production
  runManager->SetUserAction(new RunAction());


  // User interactions
  G4UImanager * UI = G4UImanager::GetUIpointer();  

  if(argc==1) // Define (G)UI terminal/executive for interactive mode  
  { 
    G4UIExecutive* session = new G4UIExecutive(argc, argv);
    session->SessionStart();
    delete session;
  }
  else // Batch mode
  { 
    G4String command = "/control/execute ";
    G4String fileName = argv[1];
    UI->ApplyCommand(command+fileName);
  }

  delete visManager;
  delete runManager;

  return 0;
}
