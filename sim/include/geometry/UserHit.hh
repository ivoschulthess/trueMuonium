#pragma once
/*
 * UserHit.hh
 *
 *  Created on: Jun 17, 2025
 *      Author: bbantoob
 */

#include <G4VHit.hh>
#include <G4THitsCollection.hh>
#include <G4ThreeVector.hh>
#include <ParticleInfo.hh>


class UserHit: public G4VHit {

  private:
    //the identifiers of the cell - note that not necessarly all SD detectors creating UserHits will set all of these
    int X,Y,Z;
    int fModule;

    // Energy
    ParticleInfo fHit;

  public:
    UserHit();
    UserHit(int _module,int X,int Y, int Z);
    virtual ~UserHit();

    // Getters and setters
    void SetPDGID(G4int _id){fHit.ID=_id;};
    G4int GetPDGID() const {return fHit.ID;};

    void SetEnergy(G4double _energy){fHit.Ene=_energy;};
    void AddEnergy(G4double _energy){fHit.Ene+=_energy;};
    G4double GetEnergy() const {return fHit.Ene;};

    void SetTime(G4double _time){fHit.Time=_time;};
    G4double GetTime() const {return fHit.Time;};

    void SetMomentum(G4ThreeVector _momentum){fHit.Mom=_momentum;};
    G4ThreeVector GetMomentum() const {return fHit.Mom;};

    void SetPosition(G4ThreeVector _position){fHit.Pos=_position;};
    G4ThreeVector GetPosition() const {return fHit.Pos;};

    void SetModule(int _module) {fModule=_module;};
    int GetModule() const {return fModule;};

    int GetX(){return X;}
    int GetY(){return Y;}
    int GetZ(){return Z;}

    void SetX(int x) { X = x; }
    void SetY(int y) { Y = y; }
    void SetZ(int z) { Z = z; }
};

typedef G4THitsCollection<UserHit> UserHitsCollection;
