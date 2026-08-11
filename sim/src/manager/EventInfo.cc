#include "EventInfo.hh"
#include "ParticleInfo.hh"
#include "UserHit.hh"

// Geant4 dependencies
#include "G4Step.hh"
#include "G4ios.hh"

// ROOT dependencies
#include <TH1.h>
#include <TFile.h>
#include <TTree.h>


EventInfo::EventInfo() {
  tree = new TTree("tree", "tree");
  /*tree->Branch("energyDep", &energyDep);*/
  /*tree->Branch("energyDepDetectorCell", &energyDepDetectorCell, "energyDepDetectorCell[12][6]/D");*/

  // general info
  tree->Branch("eventID", &eventID);
  tree->Branch("rngState", &rngState); 

  // Hits to detector
  tree->Branch("hitEnergy", &hitEnergy); tree->Branch("hitPDGID", &hitPDGID); tree->Branch("hitTime", &hitTime);
  tree->Branch("hitMomX", &hitMomX); tree->Branch("hitMomY", &hitMomY); tree->Branch("hitMomZ", &hitMomZ);
  tree->Branch("hitPosX", &hitPosX); tree->Branch("hitPosY", &hitPosY); tree->Branch("hitPosZ", &hitPosZ);
  
  // Muons
  tree->Branch("muEnergy", &muEnergy); tree->Branch("muPDGID", &muPDGID); tree->Branch("muTime", &muTime);
  tree->Branch("muMomX", &muMomX); tree->Branch("muMomY", &muMomY); tree->Branch("muMomZ", &muMomZ);
  tree->Branch("muPosX", &muPosX); tree->Branch("muPosY", &muPosY); tree->Branch("muPosZ", &muPosZ);
  
  hEnergyDep = new TH1D("hEnergyDep", "Energy Deposition; Energy [GeV]; nevts [-]", 1000, 0, 100);
  hEnergyDep->SetDirectory(nullptr);
  decayEvents.clear();
  energyDep = 0;
  hitEnergy.clear(); hitPDGID.clear(); hitTime.clear();
  hitMomX.clear(); hitMomY.clear(); hitMomZ.clear();
  hitPosX.clear(); hitPosY.clear(); hitPosZ.clear();

  muEnergy.clear(); muPDGID.clear(); muTime.clear();
  muMomX.clear(); muMomY.clear(); muMomZ.clear();
  muPosX.clear(); muPosY.clear(); muPosZ.clear();

  for (int i = 0; i < 12; i++) {
    for (int j = 0; j < 6; j++) {
      energyDepDetectorCell[i][j] = 0.;
    }
  }
}

EventInfo::~EventInfo() {
  delete hEnergyDep;
}

void EventInfo::AddParentInfo(const G4Step* aStep) {
  G4int trackID = aStep->GetTrack()->GetTrackID();
  G4String particleName = aStep->GetTrack()->GetParticleDefinition()->GetParticleName();
  //G4cout << "INFO: EventInfo: Particle (" << particleName << ", " << trackID << ")  added to map!" << G4endl;
  // Add new decay event's parent info
  decayEvents[trackID].AddParentInfo(aStep);
}

void EventInfo::AddDaughtersInfo(const G4Step* aStep) {
  G4int trackID = aStep->GetTrack()->GetTrackID();
  if(decayEvents.find(trackID) != decayEvents.end()) {
    // Add new decay event's daughter info
    decayEvents[trackID].AddDaughtersInfo(aStep);
  } else {
    G4cerr << "ERROR! EventInfo: Parent ID " << trackID << " not present in map!" << G4endl;
  }
}

void EventInfo::AddEnergy(const G4Step* aStep) {
  energyDep += aStep->GetTotalEnergyDeposit()/GeV;
}

void EventInfo::FillGeneral(int evtID, const std::vector<unsigned long>& state) 
{
  eventID = evtID;
  rngState = state;
}

void EventInfo::FillTarget(UserHitsCollection* TargetHits) {
  if (TargetHits==NULL) {
    return;
  }

  for (auto hit : *(TargetHits->GetVector())) {
    if (hit->GetEnergy() <= 0.) continue;

    energyDep+=hit->GetEnergy();
  }

}

void EventInfo::FillMuons(const G4Step* aStep) {
  auto secondaries = aStep->GetSecondaryInCurrentStep();
  if (secondaries->empty()) return;
  for(const auto& daughter : *secondaries) {
    muEnergy.push_back(daughter->GetTotalEnergy()/MeV); muPDGID.push_back(daughter->GetParticleDefinition()->GetPDGEncoding()); muTime.push_back(daughter->GetGlobalTime()/ns);
    muMomX.push_back(daughter->GetMomentum().x()/MeV); muMomY.push_back(daughter->GetMomentum().y()/MeV); muMomZ.push_back(daughter->GetMomentum().z()/MeV);
    muPosX.push_back(daughter->GetPosition().x()); muPosY.push_back(daughter->GetPosition().y()); muPosZ.push_back(daughter->GetPosition().z());
  }
}


void EventInfo::FillDetector(UserHitsCollection* DetectorHits) {
  if (DetectorHits==NULL) {
    return;
  }

  for (auto hit : *(DetectorHits->GetVector())) {
    if (hit->GetEnergy() <= 0.) continue;

    /*energyDepDetectorCell[hit->GetX()][hit->GetY()] = hit->GetEnergy();*/
    energyDep+=hit->GetEnergy();
    hitEnergy.push_back(hit->GetEnergy()); hitPDGID.push_back(hit->GetPDGID()); hitTime.push_back(hit->GetTime());
    hitMomX.push_back(hit->GetMomentum().x()); hitMomY.push_back(hit->GetMomentum().y()); hitMomZ.push_back(hit->GetMomentum().z());
    hitPosX.push_back(hit->GetPosition().x()); hitPosY.push_back(hit->GetPosition().y()); hitPosZ.push_back(hit->GetPosition().z());
  }
}

void EventInfo::FillHistos() {
  hEnergyDep->Fill(energyDep);
  tree->Fill();
}

void EventInfo::DumpROOT(std::string outName) {
  TFile* file = new TFile(outName.c_str(), "RECREATE");
  file->cd();
  /*file->mkdir("Histos");*/
  /*file->cd("Histos");*/
  /*hEnergyDep->Write();*/
  file->cd();
  tree->Write();
  file->Write();
  file->Close();
  delete file;
}

G4bool EventInfo::DumpInfo(std::ofstream& outFile) {

  // Check file status
  if(outFile.is_open()) {
    outFile << "DECAYINFO " << decayEvents.size() << std::endl;
    for(auto& decayEvt : decayEvents) {
      decayEvt.second.DumpInfo(outFile);
    }

    outFile << "HITS " << hitEnergy.size() << std::endl;
    for(size_t ihit(0); ihit<hitEnergy.size(); ihit++) {
      outFile << hitPDGID[ihit] << " " << hitEnergy[ihit] << " " << hitTime[ihit] << " " << hitMomX[ihit] << " " << hitMomY[ihit] << " " << hitMomZ[ihit] << " " << hitPosX[ihit] << " " << hitPosY[ihit] << " " << hitPosZ[ihit] << std::endl;
    }
  } else {
    G4cerr << "ERROR! EventInfo: File not open!" << G4endl;
  }

  return outFile.good();

}

void EventInfo::Reset() {
  for (int i = 0; i < 12; i++) {
    for (int j = 0; j < 6; j++) {
      energyDepDetectorCell[i][j] = 0.;
    }
  }
  eventID = -1;
  rngState.clear();
  
  energyDep = 0;
  decayEvents.clear();
  hitEnergy.clear(); hitPDGID.clear(); hitTime.clear();
  hitMomX.clear(); hitMomY.clear(); hitMomZ.clear();
  hitPosX.clear(); hitPosY.clear(); hitPosZ.clear();
  muEnergy.clear(); muPDGID.clear(); muTime.clear();
  muMomX.clear(); muMomY.clear(); muMomZ.clear();
  muPosX.clear(); muPosY.clear(); muPosZ.clear();
}
