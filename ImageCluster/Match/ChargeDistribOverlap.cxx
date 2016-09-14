#ifndef __CHARGEDISTRIBOVERLAP_CXX__
#define __CHARGEDISTRIBOVERLAP_CXX__

#include "ChargeDistribOverlap.h"



namespace larocv{

  void ChargeDistribOverlap::_Configure_(const ::fcllite::PSet &pset)
  {
    _verbose = pset.get<bool>("Verbose");
    _debug   = pset.get<bool>("Debug");
  }
  
  double ChargeDistribOverlap::_Process_(const larocv::Cluster2DPtrArray_t& clusters,
					 const std::vector<double>& vtx)
  {

    //So first, make sure clusters vector has more than 1 element.
    //If not, return -1. Algorithm does not make sense
    if ( clusters.size() == 1 )
      return -1;

    if (_verbose) { std::cout << "Number of clusters taken into account: " << clusters.size() << std::endl; }
    
    //MicroBooNE Nomenclature:
    //Planes: U == 0; V == 1; Y == 2.

    //Get hits for all 3 clusters
    std::vector<std::vector<::larutil::PxHit> > Hits;//(clusters.size(), std::vector<larutil::PxHit>());

    for (size_t c=0; c < clusters.size(); c++) {
      auto& cluster = clusters[c];
      std::vector<::larutil::PxHit> hit_vector;
      hit_vector.reserve(cluster->_insideHits.size());

      for(const auto& h : cluster->_insideHits) {
	::larutil::PxHit pxh;
	pxh.w = _pixel_x(cluster,h.x);
	pxh.t = _pixel_y(cluster,h.y);
	pxh.charge = 1.0;
      }
      
      Hits.emplace_back( std::move(hit_vector) );
    }
    // return parameter is the product of 
    // "convolutions" of each pair of clusters
    // return conv(a,b)*conv(b,c)*conv(c,a)
    // in the case of 3 planes with clusters
    // a, b and c.
    // The functions being convolved are the
    // charge-distribution in time of each clusters
    // The "convolution" is the common area of the
    // two distributions: the larger the charge-
    // -overlap the better the match
    // No normalization is performed.

    float totOverlap = 1;
    for (size_t c1=0; c1 < (Hits.size()-1); c1++){
      for (size_t c2=c1+1; c2 < Hits.size(); c2++){
        if (_verbose) { std::cout << "Considering Clusters: " << c1 << ", " << c2 << std::endl; }
        totOverlap *= TProfConvol(Hits.at(c1), Hits.at(c2) );
      }
    }

    if (_verbose) { std::cout << "Tot Overlap is: " << totOverlap << std::endl << std::endl; }
    
    return totOverlap;
  }


  // Function to calculate the "convolution"
  float ChargeDistribOverlap::TProfConvol(std::vector<::larutil::PxHit> hA ,std::vector<::larutil::PxHit> hB)
  {
    int NumTimeSamples = ::larutil::DetectorProperties::GetME()->NumberTimeSamples()*::larutil::GeometryHelper::GetME()->TimeToCm();
    
    double Tmin = NumTimeSamples;
    double Tmax = 0;

    // Make two vectors to hold the Time-Profile of the Q-distribution
    // of the two clusters
    std::vector<float> QprofA(100,0.);
    std::vector<float> QprofB(100,0.);
    float QB = 0;
    float QA = 0;
    
    //First find min and max times
    for(auto const& hitA : hA) {
      if(hitA.t > Tmax) Tmax = hitA.t;
      if(hitA.t < Tmin) Tmin = hitA.t;
    }
    for(auto const& hitB : hB) {
      if(hitB.t > Tmax) Tmax = hitB.t;
      if(hitB.t < Tmin) Tmin = hitB.t;
    }

    //Now Fill Q-profile vectors
    for(auto const& hitA : hA){
      QprofA.at( int(99*(hitA.t-Tmin)/(Tmax-Tmin)) ) += hitA.charge;
      QA += hitA.charge;
    }
    for(auto const& hitB : hB){
      QprofB.at( int(99*(hitB.t-Tmin)/(Tmax-Tmin)) ) += hitB.charge;
      QB += hitB.charge;
    }

    /*//Normalize distributions
    for (size_t b=0; b < QprofA.size(); b++)
      QprofA.at(b) /= QA;
    for (size_t b=0; b < QprofB.size(); b++)
      QprofB.at(b) /= QB;
    */

    //print out distribution if verbose
    if (_debug){
      std::cout << "Q distribution for Cluster A:" << std::endl;
      for (size_t b=0; b < QprofA.size(); b++)
        if ( QprofA.at(b) != 0 ) { std::cout << b << "\t" << QprofA.at(b) << std::endl; }
      std::cout << "Q distribution for Cluster B:" << std::endl;
      for (size_t b=0; b < QprofB.size(); b++)
        if ( QprofB.at(b) != 0 ) { std::cout << b << "\t" << QprofB.at(b) << std::endl; }
    }

    // Now find convolution between the two distributions
    // Scan two vectors and always take smallest quantity
    // Add this smallest quantity as vector is scanned to
    // obtain the common Q-distribution for the clusters
    float conv= 0;
    for (size_t b=0; b < QprofA.size(); b++){
      if ( QprofA.at(b) < QprofB.at(b) ) { conv += QprofA.at(b); }
      else { conv += QprofB.at(b); }
    }
    if (_verbose) { std::cout << "Convolution is: " << conv << std::endl; }
    
    return conv;
  }

  double ChargeDistribOverlap::_pixel_x(const Cluster2D* cluster,size_t pix) {
    auto const& px_w   = cluster->PixelWidth();
    auto const& origin = cluster->Origin();
      
    return (pix+0.5)*px_w + origin.x;
  }
  
  double ChargeDistribOverlap::_pixel_y(const Cluster2D* cluster,size_t pix) {
    auto const& px_h   = cluster->PixelHeight();
    auto const& origin = cluster->Origin();
      
    return (pix+0.5)*px_h + origin.y;
  }
}
#endif
