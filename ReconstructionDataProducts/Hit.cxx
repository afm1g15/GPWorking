//
//  Hit.cxx
//
//  Created by Brian Rebel on 10/6/16.
//
#include "messagefacility/MessageLogger/MessageLogger.h"

#include "ReconstructionDataProducts/Hit.h"

namespace gar {
  namespace rec {
   
    //--------------------------------------------------------------------------
    Hit::Hit()
    {
      return;
    }

    //--------------------------------------------------------------------------
    Hit::Hit(unsigned int chan,
             float        sig,
             float       *pos,
             float        startT,
             float        endT)
    : fChannel  (chan  )
    , fSignal   (sig   )
    , fStartTime(startT)
    , fEndTime  (endT  )
    {
      
      fPosition[0] = pos[0];
      fPosition[1] = pos[1];
      fPosition[2] = pos[2];
      
      return;
    }
    
    //--------------------------------------------------------------------------
    // Users should be careful with this method.  It will just add hits from the
    // same channel without checking if the hits are overlapping in time or close
    // to each other in time
    void Hit::operator+=(gar::rec::Hit const& h)
    {
      // only add hits on the same channel
      if(fChannel != h.Channel()){
        LOG_WARNING("Hit")
        << "attempting to add hits from different channels, that will not work";
        return;
      }
      
      // add h to this hit.  Set the new position, etc to be the weighted
      // average of the individual hits
      float totSig = fSignal + h.Signal();
      
      if(totSig == 0.){
        LOG_WARNING("Hit")
        << "attempting to add two hits with no signal between them, bail";
        return;
      }
 
      for(size_t i = 0; i < 3; ++i)
        fPosition[i] = (fPosition[i] * fSignal + h.Position()[i] * h.Signal()) / totSig;
      
      fStartTime = (fStartTime * fSignal + h.StartTime() * h.Signal()) / totSig;
      fEndTime   = (fEndTime   * fSignal + h.EndTime()   * h.Signal()) / totSig;
      
      fSignal += h.Signal();
      
      return;
    }


    //--------------------------------------------------------------------------
    std::ostream& operator<< (std::ostream& o, gar::rec::Hit const& h)
    {
      
      o << "Hit on channel "
      << h.Channel()
      << "\n\tposition = ("
      << h.Position()[0]
      << ", "
      << h.Position()[1]
      << ", "
      << h.Position()[2]
      << ")"
      << "\n\tsignal = "
      << h.Signal()
      << "\n\tstart time: "
      << h.StartTime()
      << " end time: "
      << h.EndTime();
      
      return o;
    }

  } // rec
} // gar
