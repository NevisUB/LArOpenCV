#ifndef __LARLITE_LARDOWNIMAGE_CXX__
#define __LARLITE_LARDOWNIMAGE_CXX__

#include "LArDownImage.h"
#include "LArUtil/Geometry.h"
#include "DataFormat/rawdigit.h"
//#include "Utils/NDArrayConverter.h"
#include "DataFormat/hit.h"
#include "DataFormat/cluster.h"
#include "DataFormat/event_ass.h"
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>


namespace larlite {

  void LArDownImage::_Configure_(const ::fcllite::PSet &pset)
  {
    _charge_to_gray_scale = pset.get<double>("Q2Gray");
    //_nbins_x              = pset.get<int>("NBinsX");
    //_nbins_y              = pset.get<int>("NBinsY");
  }

  void LArDownImage::_Report_() const
  {
    std::cout << "THE FOLLOWING 2 LINES ARE FROM LARIMAGEHIT--NOT YET ADJUSTED TO REFLECT QUANTITIES RETURNED BY THIS ALG; DO NOT TRUST"<<std::endl ;
    std::cout << "  # of clusters per plane ......... ";
    for(auto const& num : _num_clusters_v) std::cout << Form("%-6zu ",num);
    std::cout << " ... Average (per-event) ";
    for(size_t plane=0; plane<_num_clustered_hits_v.size(); ++plane) {
      double nclusters = _num_clusters_v[plane];
      std::cout << Form("%g ",nclusters / ((double)(_num_stored)));
    }
    std::cout << std::endl;

    std::cout << "  # of clustered hits per plane ... ";
    for(auto const& num : _num_clustered_hits_v) std::cout << Form("%-6zu ",num);
    std::cout << " ... Fractions: ";
    for(size_t plane=0; plane<_num_clustered_hits_v.size(); ++plane) {
      double clustered = _num_clustered_hits_v[plane];
      double unclustered = _num_unclustered_hits_v[plane];
      std::cout << Form("%g%% ",clustered/(clustered+unclustered));
    }
    std::cout << std::endl;
  }
  
  void LArDownImage::store_clusters(storage_manager* storage)
  {
    ++_num_stored;
    
    auto geom = ::larutil::Geometry::GetME();
    if(_num_clusters_v.empty()){
      _num_clusters_v.resize(geom->Nplanes(),0);
      _num_clustered_hits_v.resize(geom->Nplanes(),0);
      _num_unclustered_hits_v.resize(geom->Nplanes(),0);
    }
    
    auto ev_hit = storage->get_data<event_hit>(producer());
    auto out_cluster = storage->get_data<event_cluster>("ImageCluster");
    auto out_ass     = storage->get_data<event_ass>("ImageCluster");

    AssSet_t my_ass;

    std::vector<int> filled_contours ;
    std::vector<int> hit_v;
    
    if(!ev_hit) throw DataFormatException("Could not locate hit data product!");

    auto const& alg_mgr_v = algo_manager_set();

    std::vector<size_t> nclusters_v(alg_mgr_v.size(),0);
    size_t nclusters_total=0;
    
    if (out_cluster){  

    for(size_t plane=0; plane<alg_mgr_v.size(); ++plane) {
      nclusters_v[plane] = alg_mgr_v[plane].Clusters().size();
      _num_clusters_v[plane] += nclusters_v[plane];
      nclusters_total += nclusters_v[plane];
      
      auto const & contours = alg_mgr_v[plane].Clusters(); //GetContours(ev_hit);
      auto const & algID    = alg_mgr_v[plane].GetAlgID("sbc");
      auto const & meta     = alg_mgr_v[plane].MetaData(algID); 

      filled_contours.reserve(contours.size() + filled_contours.size()) ;      
      hit_v.reserve(contours.size() + filled_contours.size());

      geo::PlaneID pID(0,0,plane);

      // Create cluster + association for each contour that contains at least 1 hit
      const size_t ass_set_index_offset = my_ass.size();
      my_ass.reserve(contours.size() + ass_set_index_offset );

      std::map<size_t,larlite::hit> used_hits ;

      int temp_contour_index = 0;

      for(int contour_index=0; contour_index<contours.size(); contour_index++){
          auto const& c = contours[contour_index];
          int n_hits = 0;

        for(size_t hit_index=0; hit_index<ev_hit->size(); hit_index++) {

          // Check that hit has not been used before proceeding
          if(used_hits.find(hit_index) != used_hits.end()) continue;

          auto const& h = (*ev_hit)[hit_index];

          // Continue for hit that's not on this plane
          if( h.WireID().Plane != plane) continue;

          int wire = ( h.WireID().Wire - meta.origin().x )/  meta.pixel_width() ; 
          int time = ( h.PeakTime() - meta.origin().y )/ meta.pixel_height() ;  

          ::cv::Point2d point (time,wire);

          if ( ::cv::pointPolygonTest(c,point,false) < 0 ) continue;

          // At this point we've found at least 1 hit inside this contour!
          // Create a cluster to store association info for this contour
          if( used_hits.size() == 0 || (temp_contour_index != contour_index ) ){

            filled_contours.emplace_back(contour_index) ;


            ::larlite::cluster cl;
            //cl.set_original_producer("ImageCluster");
            cl.set_view(geom->PlaneToView(plane)); //geo::View_t(plane));
            cl.set_planeID(pID);

            out_cluster->push_back(cl);

            cl.set_id(out_cluster->size());

            AssUnit_t one_ass;
            one_ass.reserve(ev_hit->size());
            my_ass.emplace_back(one_ass);
            temp_contour_index = contour_index ;

            }

          used_hits[hit_index] = h ;

          auto& this_ass = my_ass[ out_cluster->size() - 1] ;
          this_ass.push_back(hit_index);
          n_hits++;

        }
        if(filled_contours.size() > hit_v.size())
          hit_v.emplace_back(n_hits);
      }
    }// plane loop
   }// if out_cluster

    // Store association
    if(out_ass)
      out_ass->set_association( out_cluster->id(), ev_hit->id(), my_ass );

  }

  void LArDownImage::extract_image(storage_manager* storage)
  {
    
    auto const& geom = ::larutil::Geometry::GetME();
    const size_t nplanes = geom->Nplanes();

    auto ev_hit = storage->get_data<event_hit>(producer());

    if(ev_hit==nullptr) throw DataFormatException("Could not locate hit data product!");
    
    std::vector< std::pair<size_t,size_t> > wire_range_v(nplanes,std::pair<size_t,size_t>(1e12,0));
    std::vector< std::pair<size_t,size_t> > tick_range_v(nplanes,std::pair<size_t,size_t>(1e12,0));
    
    for(auto const& h : *ev_hit) {

      if(h.Integral() < _charge_threshold) continue;
      
      auto const& wid = h.WireID();

      auto& wire_range = wire_range_v[wid.Plane];
      if(wire_range.first  > wid.Wire) wire_range.first  = wid.Wire;
      if(wire_range.second < wid.Wire) wire_range.second = wid.Wire;
      
      auto& tick_range = tick_range_v[wid.Plane];
      size_t peak_time = (size_t)(h.PeakTime());
      if(tick_range.first  > peak_time    ) tick_range.first  = ( bool(peak_time) ? peak_time - 1 : 0 );
      if(tick_range.second < (peak_time+1)) tick_range.second = peak_time+1;
    }
    
    for(size_t plane=0; plane<nplanes; ++plane) {
      auto const& wire_range = wire_range_v[plane];
      auto const& tick_range = tick_range_v[plane];
      size_t nticks = tick_range.second - tick_range.first + 2;
      size_t nwires = wire_range.second - wire_range.first + 2;
      ::larcv::ImageMeta meta((double)nwires,(double)nticks,nwires,nticks,wire_range.first,tick_range.first);
      //::larcv::ImageMeta meta((double)nwires,(double)nticks,_nbins_x,_nbins_y,wire_range.first,tick_range.first);
      if ( nwires >= 1e10 || nticks >= 1e10 )
        _img_mgr.push_back(::cv::Mat(),::larcv::ImageMeta());
      else
        _img_mgr.push_back(::cv::Mat(nwires, nticks, CV_8UC1, cvScalar(0.)),meta);
      //_img_mgr.push_back(::cv::Mat(_nbins_x,_nbins_y, CV_8UC1, cvScalar(0.)),meta);

    }
    
    for(auto const& h : *ev_hit) {
      
      auto const& wid = h.WireID();
      
      if(wid.Plane >= wire_range_v.size()) continue;
      
      auto& mat = _img_mgr.img_at(wid.Plane);
      
      auto const& wire_range = wire_range_v[wid.Plane];
      auto const& tick_range = tick_range_v[wid.Plane];
      
      size_t nticks = tick_range.second - tick_range.first + 2;
      size_t nwires = wire_range.second - wire_range.first + 2;
      
      size_t y = (size_t)(h.PeakTime()+0.5) - tick_range.first;
      size_t x = wid.Wire - wire_range.first;
      
      if(y>=nticks || x>=nwires) { std::cout << "\t==> Skipping hit\n"; continue; } // skip this hit

      //y /= ( float(nticks) / _nbins_y ); 
      //x /= ( float(nwires) / _nbins_x );

      double charge = h.Integral() / _charge_to_gray_scale;
      charge += (double)(mat.at<unsigned char>(x,y));

      if(charge>=255.) charge=255.;
      if(charge<0.) charge=0.;
      
      mat.at<unsigned char>(x,y) = (unsigned char)((int)charge);
    }
    
  }

}
#endif
