#ifndef __LARLITE_LARIMAGEHIT_CXX__
#define __LARLITE_LARIMAGEHIT_CXX__

#include "LArImageHit.h"
#include "LArUtil/Geometry.h"
#include "DataFormat/rawdigit.h"
//#include "Utils/NDArrayConverter.h"
#include "DataFormat/hit.h"
#include "DataFormat/cluster.h"
#include "DataFormat/event_ass.h"
#include "Core/larbys.h"
#include "DataFormat/pfpart.h"

namespace larlite {

  void LArImageHit::_Configure_(const ::fcllite::PSet &pset)
  {
    _charge_to_gray_scale = pset.get<double>("Q2Gray");
    //    _charge_threshold = pset.get<double>("QMin");
    
  }

  void LArImageHit::_Report_() const
  {
    std::cout << "  # of clusters  ......... ";
    std::cout << Form("%-6zu ",_num_clusters);

 
     std::cout << " ... Average (per-event) ";
     double nclusters = _num_clusters;
     std::cout << Form("%g ",nclusters / ((double)(_num_stored)));
     std::cout << std::endl;

    std::cout << "  # of clustered hits ... ";
    //for(auto const& num : _num_clustered_hits_v) std::cout << Form("%-6zu ",num);
    std::cout << Form("%-6zu ",_num_clustered_hits);

    std::cout << " ... Fractions: ";
    double clustered   = _num_clustered_hits;
    double unclustered = _num_unclustered_hits;
    std::cout << Form("%g%% ",clustered/(clustered+unclustered));
    std::cout << std::endl;
  }
  
  void LArImageHit::store_clusters(storage_manager* storage)
  {
    ++_num_stored;

    auto geom = ::larutil::Geometry::GetME();
    if(_num_clusters == 0){
      _num_clusters = 0;
      _num_clustered_hits = 0;
      _num_unclustered_hits = 0;
    }

    auto ev_hit = storage->get_data<event_hit>(producer());
    
    if(!ev_hit) throw DataFormatException("Could not locate hit data product!");

    auto const& alg_mgr = algo_manager();

    _num_clusters = alg_mgr.NumClusters();

    //std::cout<<"# clusters: "<<_num_clusters<<std::endl;

    AssSet_t cluster_ass;
    cluster_ass.resize(_num_clusters);

    for(auto& ass_unit : cluster_ass) ass_unit.reserve(100);

    for(size_t hindex=0; hindex<ev_hit->size(); ++hindex) {

      auto const& h = (*ev_hit)[hindex];

      auto const& wid = h.WireID();

      auto const cid = alg_mgr.ClusterID(wid.Wire,h.PeakTime(),wid.Plane);

      if(cid == ::larcv::kINVALID_CLUSTER_ID) {
	_num_unclustered_hits += 1;
	continue;
      }

      _num_clustered_hits += 1;
      
      cluster_ass[cid].push_back(hindex);
    }

    auto ev_cluster = storage->get_data<event_cluster>("ImageClusterHit");
    auto ev_ass     = storage->get_data<event_ass>    ("ImageClusterHit");
    if(ev_cluster) {
      ev_cluster->reserve(_num_clusters);

      for(size_t cid=0; cid<_num_clusters; ++cid) {

	auto const& image_cluster = alg_mgr.Cluster(cid);
	
	if(cid != image_cluster.ClusterID())
	  throw DataFormatException("ClusterID ordering seems inconsistent!");

	::larlite::cluster c;
	c.set_view(geom->PlaneToView(image_cluster.PlaneID()));
	c.set_planeID(geo::PlaneID(0,0,image_cluster.PlaneID()));
	c.set_id(image_cluster.ClusterID());
	ev_cluster->push_back(c);
      }

      if(ev_ass)
	ev_ass->set_association(ev_cluster->id(),ev_hit->id(),cluster_ass);
    }

    auto ev_pfpart  = storage->get_data<event_pfpart> ("ImageClusterHit");
    if(ev_pfpart) {
      auto const match_info = alg_mgr.BookKeeper().GetResult();
      AssSet_t pfpart_ass;
      pfpart_ass.reserve(match_info.size());

      for(size_t pfp_index=0; pfp_index<match_info.size(); ++pfp_index) {

	pfpart p;
	ev_pfpart->push_back(p);
	AssUnit_t ass;
	for(auto const& cid : match_info[pfp_index]) ass.push_back(cid);

	pfpart_ass.push_back(ass);
      }

      if(ev_ass && ev_cluster) ev_ass->set_association(ev_pfpart->id(),ev_cluster->id(),pfpart_ass);
    }
    
  }

  void LArImageHit::extract_image(storage_manager* storage)
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
      ::larcv::ImageMeta meta((double)nwires,(double)nticks,nwires,nticks,wire_range.first,tick_range.first,plane);
      
      if ( nwires >= 1e10 || nticks >= 1e10 )
	_img_mgr.push_back(::cv::Mat(),::larcv::ImageMeta());
      else
	_img_mgr.push_back(::cv::Mat(nwires, nticks, CV_8UC1, cvScalar(0.)),meta);
      
      //if(!nticks || !nwires)
      //_img_mgr.push_back(cv::Mat(),meta);
      //else
      //_img_mgr.push_back(::cv::Mat(nwires, nticks, CV_32FC1, cvScalar(0.)),meta);
      //_img_mgr->emplace_back(std::move(mat));
      /*
      std::cout << "Creating ... " << wire_range.first << " => " << wire_range.second << " ... " << nwires
		<< " : " << tick_range.first << " => " << tick_range.second << " ... " << nticks
		<< " @ " << plane << std::endl;
      for(size_t x=0; x<nwires; ++x)
	for(size_t y=0; y<nticks; ++y) mat.at<unsigned char>(x,y) = (unsigned char)5;
      */
      //mat.at<unsigned char>(0,0) = (unsigned char)5;
    }
    
    for(auto const& h : *ev_hit) {
      
      auto const& wid = h.WireID();
      
      if(wid.Plane >= wire_range_v.size()) continue;
      
      auto& mat = _img_mgr.img_at(wid.Plane);
      
      //continue;
      auto const& wire_range = wire_range_v[wid.Plane];
      auto const& tick_range = tick_range_v[wid.Plane];
      
      size_t nticks = tick_range.second - tick_range.first + 2;
      size_t nwires = wire_range.second - wire_range.first + 2;
      
      size_t y = (size_t)(h.PeakTime()+0.5) - tick_range.first;
      size_t x = wid.Wire - wire_range.first;
      
      //if(y>=nticks || x>=nwires) { throw std::exception(); }
      if(y>=nticks || x>=nwires) { std::cout << "\t==> Skipping hit\n"; continue; } // skip this hit
      
      //std::cout<<"Inserting " << x << " " << y << " @ " << wid.Plane << std::endl;
      double charge = h.Integral() / _charge_to_gray_scale;
      charge += (double)(mat.at<unsigned char>(x,y));

      if(charge >= 255.) charge = 255.;
      if(charge < 0.)    charge = 0.;
      
      mat.at<unsigned char>(x,y) = (unsigned char)((int)charge);
    }
    
  }


  void LArImageHit::AnalyzeImageCluster(storage_manager* storage)
  {
    // Get ImageClusterManager instance
    auto const& alg_mgr = algo_manager();
    // Original image
    auto const& img_v   = alg_mgr.InputImages();
    // Original image meta
    auto const& meta_v  = alg_mgr.InputImageMetas();

    // Loop over images
    for(size_t img_id=0; img_id < img_v.size(); ++img_id) {

      // Image & meta object
      auto const& img  = img_v[img_id];
      auto const& meta = meta_v[img_id];
      
      // Retrieve final clusters on this image
      auto const& clusters = alg_mgr.Clusters(img_id);

      //std::cout << "Image " << img_id << " @ Plane " << meta.plane() << " has " << clusters.size() << " clusters!" << std::endl;
    }

  }

}
#endif
