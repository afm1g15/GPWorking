////////////////////////////////////////////////////////////////////////
/// \file  IonizationAndScintillationAction.cxx
/// \brief Use Geant4's user "hooks" to determine the number of ionization
///        electrons and scintillation photons for each step
///
/// \version $Id: $
/// \author  brebel@fnal.gov
////////////////////////////////////////////////////////////////////////

#include "GArG4/IonizationAndScintillationAction.h"
#include "GArG4/IonizationAndScintillation.h"

#include "messagefacility/MessageLogger/MessageLogger.h"

#include "Geant4/G4Step.hh"

#include <algorithm>
namespace gar{
  namespace garg4 {
    
    //----------------------------------------------------------------------------
    // Constructor.
    IonizationAndScintillationAction::IonizationAndScintillationAction()
    {
    }
    
    //----------------------------------------------------------------------------
    // Destructor.
    IonizationAndScintillationAction::~IonizationAndScintillationAction()
    {
    }
    
    //----------------------------------------------------------------------------
    // With every step, calculate the number of ionization electrons and
    // scintillation photons using the IonizationAndScintillation singleton.
    void IonizationAndScintillationAction::SteppingAction(const G4Step* step)
    {
      garg4::IonizationAndScintillation::Instance()->Reset(step);
      
      return;
    }
    
  } // namespace garg4
} // gar