#ifndef __VERTEXTRACKCLUSTER_CXX__
#define __VERTEXTRACKCLUSTER_CXX__

#include "VertexTrackCluster.h"
#include "Refine2DVertex.h"
#include "Geo2D/Core/Geo2D.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoData/ContourArrayData.h"

namespace larocv {

  /// Global larocv::VertexTrackClusterFactory to register AlgoFactory
  static VertexTrackClusterFactory __global_VERTEXTRACKClusterFactory__;

  void VertexTrackCluster::_Configure_(const Config_t &pset)
  {
    _algo.set_verbosity(this->logger().level());
    _algo.Configure(pset.get<Config_t>("VertexParticleCluster"));
    _algo.PrintConfig();

    _pi_threshold = _algo._pi_threshold;

    auto const vtx_algo_name = pset.get<std::string>("Refine2DVertexAlgo");
    _refine2d_algo_id = this->ID(vtx_algo_name);
    
    auto const scluster_algo_name = pset.get<std::string>("TrackSuperClusterAlgo");
    _track_super_cluster_algo_id = this->ID(scluster_algo_name);

    _contour_pad = pset.get<float>("ContourPad",0.);

    Register(new data::VertexClusterArray);
  }

  larocv::Cluster2DArray_t VertexTrackCluster::_Process_(const larocv::Cluster2DArray_t& clusters,
							 const ::cv::Mat& img,
							 larocv::ImageMeta& meta,
							 larocv::ROI& roi)
  {
    // Algorithm data
    auto& data = AlgoData<data::VertexClusterArray>(0);
    
    // if given from Refine2DVertex, fill
    if(_refine2d_algo_id!=kINVALID_ID && data._vtx_cluster_v.empty()) {

      auto const& ref_data = AlgoData<data::Refine2DVertexData>(_refine2d_algo_id,0);

      data._vtx_cluster_v.resize(ref_data.get_vertex().size());
      
      for(size_t vtx_id = 0; vtx_id < ref_data.get_vertex().size(); ++vtx_id) {

	auto const& vtx3d = ref_data.get_vertex()[vtx_id];
	auto const& circle_vtx_v = ref_data.get_circle_vertex(vtx3d.id());

	auto& vtx_cluster = data._vtx_cluster_v[vtx_id];
	vtx_cluster.set_vertex(vtx3d, circle_vtx_v);
      }
    }

    // Run clustering for this plane & store
    auto const plane = meta.plane();
    auto const& super_cluster_v = AlgoData<data::ContourArrayData>(_track_super_cluster_algo_id,plane);
    
    for(size_t vtx_id = 0; vtx_id < data._vtx_cluster_v.size(); ++vtx_id) {
      
      auto& vtx_cluster = data._vtx_cluster_v[vtx_id];
      auto const& circle_vtx = vtx_cluster.get_circle_vertex(plane);
      LAROCV_DEBUG() << "Vertex ID " << vtx_id << " plane " << plane
		     << " CircleVertex @ " << circle_vtx.center << " w/ R = " << circle_vtx.radius << std::endl;

      // Find corresponding super cluster
      auto const super_cluster_id = FindContainingContour(super_cluster_v, circle_vtx.center);
      if(super_cluster_id == kINVALID_SIZE) {
	LAROCV_WARNING() << "Skipping Vertex ID " << vtx_id << " on plane " << plane << " as no super-cluster found..." << std::endl;
	continue;
      }
      
      // Create track contours from the vertex point
      auto contour_v = _algo.CreateParticleCluster(img,circle_vtx,super_cluster_v[super_cluster_id]);

      //
      // Analyze fraction of pixels clustered
      // 0) find a contour that contains the subject 2D vertex, and find allcontained non-zero pixels inside
      // 1) per track cluster count # of pixels from 0) that is contained inside

      geo2d::VectorArray<int> parent_points;
      auto const& parent_ctor = super_cluster_v[super_cluster_id];
      geo2d::VectorArray<int> points;
      findNonZero(img, points);
      parent_points.reserve(points.size());
      for(auto const& pt : points) {
	auto dist = ::cv::pointPolygonTest(parent_ctor,pt,true);
	if(dist<-1.0*_contour_pad) continue;
	parent_points.push_back(pt);
      }

      vtx_cluster.set_num_pixel(plane,parent_points.size());

      LAROCV_INFO() << "# non-zero pixels " << parent_points.size()
		    << " ... # track clusters " << contour_v.size()
		    << std::endl;
      
      for(size_t cidx=0; cidx<contour_v.size(); ++cidx) {
	auto& contour = contour_v[cidx];
	LAROCV_DEBUG() << "On contour: " << cidx << "... size: " << contour.size() << std::endl;
	data::ParticleCluster cluster;
	cluster._ctor = std::move(contour);
	// Find # pixels in this cluster from the parent
	size_t num_pixel = 0;
	float qsum = 0;
	for(auto const& pt : parent_points) {
	  // LAROCV_DEBUG() << "pt: " << pt << std::endl;
	  auto dist = ::cv::pointPolygonTest(cluster._ctor,pt,true);
	  // LAROCV_DEBUG() << "... @ dist: " << dist << std::endl;
	  if(dist<-1.0*_contour_pad) continue;
	  num_pixel += 1;
	  qsum      += (float)img.at<uchar>(pt.y,pt.x);
	}
	LAROCV_DEBUG() << "Inserting num_pixel: " << num_pixel << "... qsum: " << qsum << std::endl;
	cluster._num_pixel = num_pixel;
	cluster._qsum = qsum;
	vtx_cluster.emplace_back(plane,std::move(cluster));
      }
    }

    // If only 1 cluster, return that cluster
    // NOT IMPLEMENTED YET

    return clusters;
  }
}

#endif
