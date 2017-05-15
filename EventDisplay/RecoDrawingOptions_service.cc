////////////////////////////////////////////////////////////////////////
/// \file RecoDrawingOption_plugin.cc
///
/// \version $Id: RecoDrawingOptions_plugin.cc,v 1.1 2010/11/11 18:11:22 p-novaart Exp $
/// \author  brebel@fnal.gov

// Framework includes

/// GArSoft includes
#include "EventDisplay/RecoDrawingOptions.h"

#include <iostream>

namespace gar {
namespace evd {

//......................................................................
RecoDrawingOptions::RecoDrawingOptions(fhicl::ParameterSet const& pset, 
                                       art::ActivityRegistry& /* reg */)
{
    this->reconfigure(pset);
}

//......................................................................
RecoDrawingOptions::~RecoDrawingOptions() 
{
}

//......................................................................
void RecoDrawingOptions::reconfigure(fhicl::ParameterSet const& pset)
{
    fDrawHits                  = pset.get< int                      >("DrawHits"                 );
    fDrawTracks      	         = pset.get< int                      >("DrawTracks"     	         );
    fDrawTrackTrajectoryPoints = pset.get< int                      >("DrawTrackTrajectoryPoints");
    fDrawShowers     	         = pset.get< int                      >("DrawShowers"    	         );
    fDrawVertices    	         = pset.get< int                      >("DrawVertices"   	     	   );
    fDrawEvents      	         = pset.get< int                      >("DrawEvents"     	     	   );
    fHitLabels                 = pset.get< std::vector<std::string> >("HitModuleLabels"          );
    fTrackLabels      	       = pset.get< std::vector<std::string> >("TrackModuleLabels"     	 );
    fShowerLabels     	       = pset.get< std::vector<std::string> >("ShowerModuleLabels"    	 );
    fVertexLabels     	       = pset.get< std::vector<std::string> >("VertexModuleLabels"    	 );
    fEventLabels      	       = pset.get< std::vector<std::string> >("EventModuleLabels"     	 );
  }
  
}

namespace evd {

  DEFINE_ART_SERVICE(RecoDrawingOptions)

} // namespace evd
}
////////////////////////////////////////////////////////////////////////
