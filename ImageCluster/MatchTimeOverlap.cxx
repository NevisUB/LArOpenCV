#ifndef __MATCHTIMEOVERLAP_CXX__
#define __MATCHTIMEOVERLAP_CXX__

#include "MatchTimeOverlap.h"

namespace larcv{

  void MatchTimeOverlap::_Configure_(const ::fcllite::PSet &pset)
  {
    _time_ratio_cut  = pset.get<float> ("RatioCut");
    _start_time_cut  = pset.get<float> ("StartTimeCut");
    _require_3planes = pset.get<bool>  ("RequireThreePlanes");

  }
  
  double MatchTimeOverlap::_Process_(const larcv::Cluster2DPtrArray_t& clusters)
  {

    if(_require_3planes && clusters.size() == 2) return -1;

    double time_difference     = 0;
    double max_time_difference = 0;
    double max_charge          = 0;

    double max_hits_1 = 0;
    double max_hits_2 = 0;
    
    for(auto const& c : clusters){

      time_difference  = std::abs(c->_startPt.x - c->_endPt.x); 
      
      if(max_time_difference < time_difference)
        max_time_difference = time_difference;
      
      auto charge = c->_sumCharge;
      
      if(max_charge < charge)
        max_charge = charge ;

      if(c->_numHits > max_hits_1) {
        max_hits_2 = max_hits_1;
        max_hits_1 = c->_numHits;
      }else if(c->_numHits > max_hits_2) 
        max_hits_2 = c->_numHits;
    }

    // Preserve location in time space. Cut clusters that have similar time differences,
    // but hit wires at very different times. 
    double start_t      = 0;
    double end_t        = 0;
    double prev_start_t = 0;
    double prev_end_t   = 0;
    double ratio        = 0;
    double charge_ratio = max_hits_2 / max_hits_1;        

    for(size_t c_index=0; c_index<clusters.size(); ++c_index) {
      auto const& c = clusters[c_index];
      
      // Make sure start point always smaller in t
      // Can remove this condition later when start pt/
      // direction information is stronger
      if(c->_startPt.x > c->_endPt.x) {
        start_t = c->_endPt.x   + c->Origin().y ;
        end_t   = c->_startPt.x + c->Origin().y ;
      }
      else{
        start_t = c->_startPt.x + c->Origin().y;
        end_t   = c->_endPt.x   + c->Origin().y;
      }         
      
      if(prev_start_t == 0) prev_start_t = start_t;
      if(prev_end_t   == 0) prev_end_t   = end_t;
      
      time_difference  = end_t - start_t;
      
      ratio += time_difference/max_time_difference;

      // If current cluster's start time is not within some range of the previous cluster's start time,
      // modify ratio to discourage matching
      
      if(    (start_t > (prev_start_t - _start_time_cut) && start_t < (prev_start_t + _start_time_cut)) 
          || (end_t   > (prev_end_t   - _start_time_cut) && end_t   < (prev_end_t   + _start_time_cut)) )
        ratio *= 1;
      else        
        ratio *= 0.001;
      
      prev_start_t = start_t ;
      prev_end_t   = end_t ;        
      
      //if( c_index==(clusters.size()-1) && ratio > _time_ratio_cut ){
      // std::cout<<"\n\n\n\n";
      // std::cout<<"PLANE:------------------: " << c->PlaneID() << std::endl;
      // std::cout<<"Start point:------------: " << start_t<<std::endl << std::endl;
      // std::cout<<"End Point:--------------: " << end_t  <<std::endl;
      // std::cout<<"Time diff:--------------: " << time_difference<<std::endl;
      // std::cout<<"Max time diff:----------: " << max_time_difference<<std::endl;
      // std::cout<<"Ratio for each cluster:-: " << ratio<<std::endl;
      // std::cout<<"Charge Ratio:-----------: " << charge_ratio<<std::endl;
      //} 
      
    }

    ratio /= clusters.size();
    ratio *= charge_ratio;
    
    // if(_verbose && ratio> _time_ratio_cut)
    //   std::cout<<"**************************FOUND A MATCH . ratio is: "<<ratio<<"\n\n\n"<<std::endl;
    
    return (ratio > _time_ratio_cut ? ratio : -1 );  // negative means they did not match

  }

}
#endif
