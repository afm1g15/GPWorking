////////////////////////////////////////////////////////////////////////
/// \file IonizationAndScintillation.cxx
///
/// \author  brebel@fnal.gov
////////////////////////////////////////////////////////////////////////

// gar includes
#include "GArG4/G4SimulationParameters.h"
#include "ReadoutSimulation/IonizationAndScintillation.h"
#include "ReadoutSimulation/ISCalculationNEST.h"
#include "ReadoutSimulation/ISCalculationSeparate.h"

// ROOT includes

// Framework includes
#include "cetlib/exception.h"
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art/Framework/Services/Optional/TFileService.h"
#include "art/Framework/Services/Optional/TFileDirectory.h"

// C/C++ standard libraries

namespace gar {
  namespace rosim {
    
    static IonizationAndScintillation* gInstance = nullptr;
    
    //......................................................................
    IonizationAndScintillation* IonizationAndScintillation::CreateInstance(CLHEP::HepRandomEngine& engine)
    {
      if(!gInstance) gInstance = new IonizationAndScintillation(engine);
      return gInstance;
    }
    
    //......................................................................
    IonizationAndScintillation* IonizationAndScintillation::Instance()
    {
      // the instance must have been created already by CreateInstance()
      if(!gInstance)
        throw cet::exception("IonizationAndScintillation")
        << "instance pointer is null, that is bad";
      
      return gInstance;
    }
    
    //......................................................................
    // Constructor.
    IonizationAndScintillation::IonizationAndScintillation(CLHEP::HepRandomEngine& engine)
    : fISCalc(nullptr)
    , fStep(nullptr)
    , fElectronsPerStep(0)
    , fStepSize(0)
    , fPhotonsPerStep(0)
    , fEnergyPerStep(0)
    , fElectronsVsPhotons(0)
    , fEngine(engine)
    {
      
      fISCalculator = gar::garg4::G4SimulationParameters::Instance()->IonAndScintCalculator();
      
      if(fISCalculator.compare("NEST") == 0)
        fISCalc = new garg4::ISCalculationNEST(fEngine);
      else if(fISCalculator.compare("Separate") == 0)
        fISCalc = new garg4::ISCalculationSeparate(fEngine);
      else
        LOG_WARNING("IonizationAndScintillation")
        << "No ISCalculation set, this can't be good.";
      
      // Reset the values for the electrons, photons, and energy to 0
      // in the calculator
      fISCalc->Reset();
      
      //set the current track and step number values to bogus so that it will run the first reset:
      fStepNumber = -1;
      fTrkID      = -1;
      
      // initialize the calculator
      fISCalc->Initialize();
      
      // make the histograms
      ::art::ServiceHandle< ::art::TFileService> tfs;
      
      fElectronsPerStep   = tfs->make<TH1F>("electronsPerStep", ";Electrons;Steps",
                                            500, 0., 5000.);
      fPhotonsPerStep   	= tfs->make<TH1F>("photonsPerStep", ";Photons;Steps",
                                            500, 0., 5000.);
      fEnergyPerStep    	= tfs->make<TH1F>("energyPerStep", ";Energy (MeV);Steps",
                                            100, 0., 0.5);
      fStepSize         	= tfs->make<TH1F>("stepSize", ";Step Size (CLHEP::cm);Steps",
                                            500, 0., 0.2);
      fElectronsPerLength = tfs->make<TH1F>("electronsPerLength", ";Electrons #times 10^{3}/CLHEP::cm;Steps",
                                            1000, 0., 1000.);
      fPhotonsPerLength   = tfs->make<TH1F>("photonsPerLength", ";Photons #times 10^{3}/CLHEP::cm;Steps",
                                            1000, 0., 1000.);
      fElectronsPerEDep   = tfs->make<TH1F>("electronsPerEDep", ";Electrons #times 10^{3}/MeV;Steps",
                                            1000, 0., 1000.);
      fPhotonsPerEDep     = tfs->make<TH1F>("photonsPerEDep", ";Photons #times 10^{3}/MeV;Steps",
                                            1000, 0., 1000.);
      
      fElectronsVsPhotons = tfs->make<TH2F>("electronsVsPhotons", ";Photons;Electrons",
                                            500, 0., 5000., 500, 0., 5000.);
      
      return;
    }
    
    //......................................................................
    IonizationAndScintillation::~IonizationAndScintillation()
    {
      if(fISCalc) delete fISCalc;
    }
    
    
    //......................................................................
    void IonizationAndScintillation::Reset(sdp::EnergyDeposit const& dep)
    {
      
      if(fStepNumber == step->GetTrack()->GetCurrentStepNumber() &&
         fTrkID      == step->GetTrack()->GetTrackID())
        return;
      
      fStepNumber = step->GetTrack()->GetCurrentStepNumber();
      fTrkID      = step->GetTrack()->GetTrackID();
      
      fEnergyDeposit = dep;
      
      // reset the calculator
      fISCalc->Reset();
      
      // double check that the energy deposit is non-zero
      // then do the calculation if it is
      if( dep.Energy() > 0 ){
        
        fISCalc->CalculateIonizationAndScintillation(fEnergyDeposit);
        
        LOG_DEBUG("IonizationAndScintillation")
        << "\nEnergy: "    << fISCalc->EnergyDeposit()
        << "\nElectrons: " << fISCalc->NumberIonizationElectrons()
        << "\nPhotons: "   << fISCalc->NumberScintillationPhotons();
        
        // Fill the histograms
        fEnergyPerStep     ->Fill(fISCalc->EnergyDeposit());
        fElectronsPerStep  ->Fill(fISCalc->NumberIonizationElectrons());
        fPhotonsPerStep    ->Fill(fISCalc->NumberScintillationPhotons());
        fElectronsVsPhotons->Fill(fISCalc->NumberScintillationPhotons(), 
                                  fISCalc->NumberIonizationElectrons());
        fElectronsPerEDep  ->Fill(fISCalc->NumberIonizationElectrons()  * 1.e-3 / fISCalc->EnergyDeposit()   );
        fPhotonsPerEDep    ->Fill(fISCalc->NumberScintillationPhotons() * 1.e-3 / fISCalc->EnergyDeposit()   );
        
      } // end if the energy deposition is non-zero
      
      return;
    }
    
  } // namespace
} // gar
