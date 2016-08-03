#ifndef __LARLITE_LARIMAGEWIRE_CXX__
#define __LARLITE_LARIMAGEWIRE_CXX__

#include "LArImageWire.h"
#include "ImageCluster/ImageClusterTypes.h"

#include "LArUtil/Geometry.h"
#include "DataFormat/wire.h"
#include "DataFormat/hit.h"
#include "DataFormat/pfpart.h"
#include "DataFormat/cluster.h"
#include "DataFormat/PiZeroROI.h"

namespace larlite {

  void LArImageWire::_Configure_(const ::fcllite::PSet &pset)
  {
    _charge_to_gray_scale = pset.get<double>("Q2Gray");
    _pool_time_tick       = pset.get<int>   ("PoolTimeTick");
    _use_roi              = pset.get<bool>  ("UseROI");
  }

  void LArImageWire::store_clusters(storage_manager* storage) 
  {
    //copy paste from LArImageHit -- should this be part of parent class then?
    
    ++_num_stored;
    
    auto geom = ::larutil::Geometry::GetME();
    if(_num_clusters == 0){
      _num_clusters = 0;
      _num_clustered_hits = 0;
      _num_unclustered_hits = 0;
    }

    auto ev_hit = storage->get_data<event_hit>("gaushit"); // hardcode
    
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

      if(cid == ::larocv::kINVALID_CLUSTER_ID) {
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
      std::cout << "num_clusters: " << _num_clusters << "\n";
      
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

  void LArImageWire::extract_image(storage_manager* storage)
  {
    auto const& geom = ::larutil::Geometry::GetME();
    const size_t nplanes = geom->Nplanes();

    auto ev_wire = storage->get_data<event_wire>(producer());

    if(!ev_wire) throw DataFormatException("Could not locate Wire data product!");

    std::vector< std::pair<size_t,size_t> > wire_range_v(nplanes,std::pair<size_t,size_t>(1e12,0));
    std::vector< std::pair<size_t,size_t> > tick_range_v(nplanes,std::pair<size_t,size_t>(1e12,0));

    ::larlite::event_PiZeroROI* ev_roi = nullptr;
    if ( _use_roi ) {

      ev_roi = storage->get_data<event_PiZeroROI>( "mcroi" );
      if(ev_roi->size() == 0) throw DataFormatException("Could not locate ROI data product And you have UseROI: True!");

      auto wr_v = (*ev_roi)[0].GetWireROI();
      auto tr_v = (*ev_roi)[0].GetTimeROI();
      
      for(uint k=0; k < nplanes; ++k)
	{ wire_range_v[k] = wr_v[k]; tick_range_v[k] = tr_v[k]; }

    } else {
    
      for(auto const& wire_data : *ev_wire) {
      
	auto const& wid = geom->ChannelToWireID(wire_data.Channel());
      
	auto& wire_range = wire_range_v[wid.Plane];
	if(wire_range.first  > wid.Wire) wire_range.first  = wid.Wire;
	if(wire_range.second < wid.Wire) wire_range.second = wid.Wire;
      
	auto& tick_range = tick_range_v[wid.Plane];
      
	auto const& roi_v = wire_data.SignalROI();
      
	for(auto const& roi : roi_v.get_ranges()) {	
	  size_t start_tick = roi.begin_index();
	  size_t last_tick = start_tick + roi.size() - 1;
	  if(tick_range.first  > start_tick) tick_range.first  = start_tick;
	  if(tick_range.second < last_tick)  tick_range.second = last_tick;
	}
      }
    }
    
    for(size_t plane=0; plane<nplanes; ++plane) {
      auto const& wire_range = wire_range_v[plane];
      auto const& tick_range = tick_range_v[plane];
      size_t nticks = tick_range.second - tick_range.first + 2;
      size_t nwires = wire_range.second - wire_range.first + 2;


      ::larocv::ImageMeta meta((double)nwires,(double)nticks,nwires,nticks,wire_range.first,tick_range.first,plane);
      if ( _use_roi ) {
	const auto& vtx = (*ev_roi)[0].GetVertex()[plane];
	meta.setvtx(vtx.first,vtx.second);
      }
      
      if ( nwires >= 1e10 || nticks >= 1e10 )
	_img_mgr.push_back(cv::Mat(),::larocv::ImageMeta());
      else
	_img_mgr.push_back(::cv::Mat(nwires, nticks, CV_8UC1, cvScalar(0.)),meta);
    }

    for(auto const& wire_data : *ev_wire) {
      
      auto const& wid = geom->ChannelToWireID(wire_data.Channel());

      auto& mat = _img_mgr.img_at(wid.Plane);
      
      auto const& roi_v = wire_data.SignalROI();

      auto const& wire_range = wire_range_v[wid.Plane];
      auto const& tick_range = tick_range_v[wid.Plane];

      for(auto const& roi : roi_v.get_ranges()) {
	size_t start_tick = roi.begin_index();
	for(size_t adc_index=0; adc_index < roi.size(); ++adc_index) {
	  size_t x = wid.Wire - wire_range.first;
	  size_t y = start_tick + adc_index - tick_range.first;
	  double q = roi[adc_index] / _charge_to_gray_scale;
	  q += (double)(mat.at<unsigned char>(x,y));
	  if( q <  0) q = 0.;
	  if( q >255) q = 255;
	  mat.at<unsigned char>(x,y) = (unsigned char)((int)q);
	}
      }
    }

    if ( _pool_time_tick > 1 ) {

      for(size_t plane=0; plane<nplanes; ++plane) {

	auto& img  = _img_mgr.img_at(plane);
	auto& meta = _img_mgr.meta_at(plane);

	::cv::Mat pooled(img.rows, img.cols/_pool_time_tick+1, CV_8UC1, cvScalar(0.));
      
	for(int row = 0; row < img.rows; ++row) {

	  uchar* p = img.ptr(row);
	  
	  for(int col = 0; col < img.cols; ++col) {

	    int pp = *p++;
	  
	    auto& ch = pooled.at<uchar>(row,col/_pool_time_tick);

	    int res  = pp + (int) ch;
	    if (res > 255) res = 255;
	    ch = (uchar) res;
	  
	  }
	}

	//old parameters
	auto const& wire_range = wire_range_v[plane];
	auto const& tick_range = tick_range_v[plane];

	img  = pooled;
	meta = ::larocv::ImageMeta((double)pooled.rows,
				  (double)pooled.cols*_pool_time_tick,
				  pooled.rows,
				  pooled.cols,
				  wire_range.first,
				  tick_range.first,
				  plane);
	if ( _use_roi ) {
	  const auto& vtx = (*ev_roi)[0].GetVertex()[plane];
	  meta.setvtx(vtx.first,vtx.second);
	}

      }
    }
    
  }

}
#endif
