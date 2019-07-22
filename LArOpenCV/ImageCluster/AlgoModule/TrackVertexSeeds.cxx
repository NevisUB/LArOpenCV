#ifndef __TRACKVERTEXSEEDS_CXX__
#define __TRACKVERTEXSEEDS_CXX__

#include "TrackVertexSeeds.h"
#include "LArOpenCV/ImageCluster/AlgoData/VertexSeed.h"
#include "LArOpenCV/ImageCluster/AlgoData/TrackClusterCompound.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.h"

namespace larocv {

  /// Global TrackVertexSeedsFactory to register AlgoFactory
  static TrackVertexSeedsFactory __global_TrackVertexSeedsFactory__;

  void TrackVertexSeeds::_Configure_(const Config_t &pset)
  {
    
    // Set algo configure, get the producer names for track vertex seeds.
    _ClusterHIPMIP.Configure(pset.get<Config_t>("ClusterHIPMIP"));
    _DefectBreaker.Configure(pset.get<Config_t>("DefectBreaker"));
    _PCACrossing.Configure(pset.get<Config_t>("PCACrossing"));
    _SimpleKink.Configure(pset.get<Config_t>("SimpleKink"));
		
		// Do we want to consider dead wires
		_dead_wire_adc = pset.get<int>("DeadWireADC",0);
    
    // Create a container for VertexSeed2DArray algorithm data
    for(size_t plane=0; plane<3; ++plane) {
      LAROCV_DEBUG() << "Registering VertexSeed2DArray on plane " << plane << std::endl;
      Register(new data::VertexSeed2DArray);
    }

    // Create a container for TrackClusterCompoundArray algorithm data
    for(size_t plane=0; plane<3; ++plane) {
      LAROCV_DEBUG() << "Registering TrackClusterCompoundArray on plane " << plane << std::endl;
      Register(new data::TrackClusterCompoundArray);
    }

  }

  void TrackVertexSeeds::_Process_()
  {
    // Read the track images (configured in cfg file)
    auto img_v = ImageArray();
		auto ch_img_v = ImageArray(ImageSetID_t::kImageSetChStatus);
    auto const& meta_v = MetaArray();

    for(size_t img_idx=0; img_idx < img_v.size(); ++img_idx) {
			
			int nSeedsDefect(0), nSeedsPCA(0); 

      auto& img = img_v[img_idx];
			auto& ch_img = ch_img_v[img_idx];
      auto const& meta = meta_v.at(img_idx);
      
      auto& vertex_seeds_v = AlgoData<data::VertexSeed2DArray>(meta.plane());
      auto& track_cluster_v = AlgoData<data::TrackClusterCompoundArray>(meta.plane()+3);
      
      // Cluster the HIPS and MIPS into seperate contours
      auto hip_mip_p = _ClusterHIPMIP.IsolateHIPMIP(img,meta.plane());
      
      LAROCV_DEBUG() << "Isolated " << hip_mip_p.first.size()
		     << " HIPs and " << hip_mip_p.second.size()
		     << " MIPS" << std::endl;
      
      // Merge the hips and the mips into a single array of contours
      auto ctor_arr_v = AddContourArrays(hip_mip_p.first,hip_mip_p.second);
      
      LAROCV_DEBUG() << "Combined: " << ctor_arr_v.size() << std::endl;
      
      std::vector<geo2d::Line<float> > line_v;
      line_v.reserve(ctor_arr_v.size()*3);
     

			cv::Mat img_rgb;
			cv::cvtColor(img,img_rgb,CV_GRAY2BGR);
 
      // Loop over ctors in this plane    
      for (const auto& ctor : ctor_arr_v) {
				LAROCV_DEBUG() << "Analyzing contour of size " << ctor.size() << std::endl;
	
				// Split ctor and create a cluster compound (has defects, atomics, start/end pt)
				auto cluscomp = _DefectBreaker.BreakContour(ctor);
	
				LAROCV_DEBUG() << "I split this contour into " << cluscomp.size() << " atomics" << std::endl;
				LAROCV_DEBUG() << "Found " << cluscomp.get_defects().size() << " defects for seeds" << std::endl;

	
				auto simpleKinkSeed = _SimpleKink.FindInflections(ctor,img);
				for (const auto& crowSeed : simpleKinkSeed) {
				  data::VertexSeed2D seed(crowSeed);
				  vertex_seeds_v.emplace_back(std::move(seed));
				}

				// Generate seeds from PCA
				for(auto & pca : _PCACrossing.ComputePCALines(cluscomp))
				  line_v.emplace_back(std::move(pca));
	
				//
				//0)Track Vertex Seeds from Defect Point
				//

				// Find defect point in HIP and MIP contours 
				LAROCV_DEBUG() << "Inserting " << cluscomp.get_defects().size() << " defects" << std::endl;
				for(const auto& ctor_defect : cluscomp.get_defects()) {
				  LAROCV_DEBUG() << "..." << ctor_defect._pt_defect << std::endl;
				  data::VertexSeed2D seed(ctor_defect._pt_defect);
				  seed.type=data::SeedType_t::kDefect;
				  // Store defect point as track vertex seeds
				  vertex_seeds_v.emplace_back(std::move(seed));
					cv::circle(img_rgb,cv::Point(seed.x,seed.y),int(2),cv::Scalar(0,255,0),-1);
					nSeedsDefect++;
				}
				// Store defected contours
				track_cluster_v.emplace_back(std::move(cluscomp));
      }

      //
      //1)Track Vertex Seeds from PCA
      //
     
      // Find crossing points of PCA lines and store as track vertex seeds
			int ctdead(0),ctlive(0);
			if(_dead_wire_adc){
				for(int i = 0; i < ch_img.rows * ch_img.cols; i++){
					if(ch_img.at<uchar>(i) > 1){
						ctlive ++;
					}
					else{ 
						ctdead ++;
						img.at<uchar>(i) = _dead_wire_adc;
					}
				}
			}
						
			LAROCV_DEBUG() << "Generated " << line_v.size() << " pca lines" << std::endl;
      for ( const auto& pca_seed : _PCACrossing.ComputeIntersections(line_v,img) ) {
				data::VertexSeed2D seed(pca_seed);
				seed.type=data::SeedType_t::kPCA;
				vertex_seeds_v.emplace_back(std::move(seed));
				cv::circle(img_rgb,cv::Point(seed.x,seed.y),int(2),cv::Scalar(255,0,0),-1);
				nSeedsPCA++;
      }
	
			//cv::imwrite(std::to_string(img_idx)+"_3_seeds.png",img_rgb);
			//std::cout << "We've got " << nSeedsPCA << " seeds from PCA on plane " << img_idx << "." << std::endl;
			//std::cout << "We've got " << nSeedsDefect << " seeds from defects on plane " << img_idx << "." << std::endl;

      
    }
    return;
  }
  

}
#endif
