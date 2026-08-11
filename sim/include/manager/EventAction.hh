#pragma once

#include "G4UserEventAction.hh"
#include "EventInfo.hh"
#include "EventActionMessenger.hh"

class DetectorConstruction;
class SteppingAction;

class G4Event;


class EventAction : public G4UserEventAction
{
  public:
    EventAction(DetectorConstruction* myDC);
    ~EventAction();

  public:
    void BeginOfEventAction(const G4Event*);
    void EndOfEventAction(const G4Event*);
    void SetSteppingAction(SteppingAction* action) {theSteppingAction = action;}
    EventInfo* GetEventInfoPointer() {return myEventInfo;}
    void CountNSaved() {NSaved++;}

    void SetMuPairProd(G4bool _hasMuPair=true) {hasMuPair=_hasMuPair;}
    G4bool GetMuPairProd() const {return hasMuPair;}

    void SetROOTFileName(std::string _filename) {fROOTFileName = _filename;}
    std::string GetROOTFileName() const {return fROOTFileName;}

    void SetRNGFileName(std::string _filename) {fRNGFileName = _filename;}
    std::string GetRNGFileName() const {return fRNGFileName;}

    void SetVerboseLevel(G4int _level) {fVerboseLevel = _level;}
    G4int GetVerboseLevel() const {return fVerboseLevel;}

    void SetTriggerLevel(G4int _level) {fTriggerLevel = _level;}
    G4int GetTriggerLevel() const {return fTriggerLevel;}

    void SetRNGState(const std::vector<unsigned long>& _state) { fRNGState = _state; }
    const std::vector<unsigned long>& GetRNGState() const { return fRNGState; }

    void SetRNGEventIdx(const G4int _idx) {fRNGEventIdx = _idx;}
    const G4int GetRNGEventIdx() const {return fRNGEventIdx;}

  private:
    DetectorConstruction* myDetector;
    SteppingAction* theSteppingAction;
    EventInfo* myEventInfo;
    EventActionMessenger* EAMessenger;
    std::vector<unsigned long> fRNGState;
    G4int fRNGEventIdx;

    G4int fVerboseLevel;
    G4int fTriggerLevel;
    std::string fRNGFileName = "";
    std::string fROOTFileName = "output.root";
    G4int NSaved;
    G4bool hasMuPair;
};
