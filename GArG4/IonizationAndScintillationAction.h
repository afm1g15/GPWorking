////////////////////////////////////////////////////////////////////////
/// \file  IonizationAndScintillationAction.h
/// \brief this UserAction derived class is to provide a hook
///        during G4 stepping in which to call the code that 
///        calculates the number of ionization electrons and scintillation
///        photons produced by each step
///
/// \version $Id:  $
/// \author  brebel@fnal.gov
////////////////////////////////////////////////////////////////////////

#ifndef GARG4IONIZATIONANDSCINTILLAITONACTION_H
#define GARG4IONIZATIONANDSCINTILLATIONACTION_H

#include "nutools/G4Base/UserAction.h"

#include "Geant4/globals.hh"
#include <map>

// Forward declarations.
class G4Step;

namespace gar {
  namespace garg4 {
    
    class IonizationAndScintillationAction : public g4b::UserAction
    {
    public:
      // Standard constructors and destructors;
      IonizationAndScintillationAction();
      virtual ~IonizationAndScintillationAction();
      
      // UserActions method that we'll override, to obtain access to
      // Geant4's steps
      virtual void SteppingAction    (const G4Step*);
      
    private:
      
    };
    
  } // namespace garg4
} // gar
#endif // GARG4G4BADIDEAACTION_H
