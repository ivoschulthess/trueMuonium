#pragma once

// Geant4 dependencies
#include <G4ThreeVector.hh>
#include <G4Step.hh>
#include "G4SystemOfUnits.hh"
#include "G4ios.hh"

// ROOT dependencies
#include <TH1.h>
#include <TTree.h>

// STL dependencies
#include <iostream>
#include <fstream>
#include <vector>
#include <map>

#include "UserHit.hh"
#include "ParticleInfo.hh"


struct DecayEventInfo {
  ParticleInfo parent;
  std::vector<ParticleInfo> daughters;

  void AddParentInfo(const G4Step* aStep) {
    parent.ID   = aStep->GetTrack()->GetParticleDefinition()->GetPDGEncoding();
    parent.Ene  = aStep->GetTrack()->GetTotalEnergy()/MeV;
    parent.Time = aStep->GetPreStepPoint()->GetGlobalTime()/ns;
    parent.Mom  = aStep->GetTrack()->GetMomentum()/MeV;
    parent.Pos  = aStep->GetPreStepPoint()->GetPosition();
  }

  void AddDaughtersInfo(const G4Step* aStep) {
    for(const auto& daughter : *aStep->GetSecondaryInCurrentStep()) {
      daughters.emplace_back(
          daughter->GetParticleDefinition()->GetPDGEncoding(),
          daughter->GetTotalEnergy()/MeV,
          aStep->GetPreStepPoint()->GetGlobalTime()/ns,
          daughter->GetMomentum()/MeV,
          aStep->GetPreStepPoint()->GetPosition()
          );
    }
  }

  void DumpInfo(std::ofstream& outFile) {
    if(outFile.is_open()) {
      // Dump Parent info
      outFile << parent;

      outFile << daughters.size() << std::endl;

      // Dump Daughters info
      for (const auto &daughter : daughters) {
        outFile << daughter;
      }
    }
  }

  void Reset() {
    // Reset all values
    daughters.clear();
    parent.Reset();
  }
};

class EventInfo
{
  public:
    EventInfo();
    ~EventInfo();

    void AddParentInfo(const G4Step* aStep);
    void AddDaughtersInfo(const G4Step* aStep);
    void AddEnergy(const G4Step* aStep);
    void FillGeneral(int evtID, const std::vector<unsigned long>& rngState);
    void FillHistos();
    void FillTarget(UserHitsCollection* TargetHits);
    void FillDetector(UserHitsCollection* DetectorHits);
    void FillMuons(const G4Step* aStep);
    void DumpROOT(std::string outName);
    G4bool DumpInfo(std::ofstream& outFile);
    G4bool hasHit() const { return (!hitEnergy.empty());}
    void Reset();

  private:
    // general info (per event)
    std::vector<unsigned long> rngState;
    int eventID;

    std::map<G4int, DecayEventInfo> decayEvents;
    TH1D* hEnergyDep=0;
    TTree* tree=0;
    double energyDep;
    
    // hit info
    std::vector<double> hitEnergy;
    std::vector<int> hitPDGID;
    std::vector<double> hitTime;
    std::vector<double> hitPosX, hitPosY, hitPosZ;
    std::vector<double> hitMomX, hitMomY, hitMomZ;

    // muon info
    std::vector<double> muEnergy;
    std::vector<int> muPDGID;
    std::vector<double> muTime;
    std::vector<double> muPosX, muPosY, muPosZ;
    std::vector<double> muMomX, muMomY, muMomZ;
    double energyDepDetectorCell[12][6];
};
