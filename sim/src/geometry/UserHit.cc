/*
 * UserHit.cc
 *
 *  Created on: Jun 17, 2025
 *      Author: bbantoob
 */

#include "UserHit.hh"
#include "G4ThreeVector.hh"

// Default constructor
UserHit::UserHit() :
  X(-1), Y(-1), Z(-1), fModule(-1), fHit()
{;}

// Constructor with module and position identifiers
UserHit::UserHit(int _module,int _X,int _Y, int _Z) :
  X(_X), Y(_Y), Z(_Z), fModule(_module), fHit()
{;}

UserHit::~UserHit()
{;}
