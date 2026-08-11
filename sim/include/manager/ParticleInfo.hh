#pragma once

// Geant4 dependencies
#include <G4ThreeVector.hh>

// STL dependencies
#include <iostream>
#include <fstream>


struct ParticleInfo {
  ParticleInfo() : ID(0), Ene(0), Time(std::nan("")), Mom(std::nan(""),std::nan(""),std::nan("")), Pos(std::nan(""),std::nan(""),std::nan("")) {;}
  ParticleInfo(G4double ID_in, G4double Ene_in, G4double Time_in, G4ThreeVector Mom_in, G4ThreeVector Pos_in)
    : ID(ID_in), Ene(Ene_in), Time(Time_in), Mom(Mom_in), Pos(Pos_in) {;}

  G4double ID;       // PDG Encoding
  G4double Ene;      // Saved in MeV
  G4double Time;     // Saved in ns
  G4ThreeVector Mom; // Saved in MeV/c
  G4ThreeVector Pos; // Origin of track, saved in cm

  void Reset() {
    // Reset all values
    ID = 0;
    Ene = 0;
    Time = std::nan("");
    Mom.set(std::nan(""),std::nan(""),std::nan(""));
    Pos.set(std::nan(""),std::nan(""),std::nan(""));
  }

};

std::ofstream& operator<<(std::ofstream& stream, const ParticleInfo& particle);
