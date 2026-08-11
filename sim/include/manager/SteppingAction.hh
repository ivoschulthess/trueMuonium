#pragma once

#include "G4UserSteppingAction.hh"
#include "globals.hh"

class DetectorConstruction;
class EventAction;

class SteppingAction : public G4UserSteppingAction
{
  public:
    SteppingAction(DetectorConstruction* myDC, EventAction* myEA);
    virtual ~SteppingAction(){};

    virtual void UserSteppingAction(const G4Step*);

    virtual void Reset();

    virtual void Finalize();

    DetectorConstruction* GetDetector() {return myDetector;}
    EventAction* GetEventAction() {return eventAction;}

  private:

    DetectorConstruction* myDetector;
    EventAction* eventAction;
};
