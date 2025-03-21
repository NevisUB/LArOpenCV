#include "LArOpenCV/ImageCluster/Base/ImageClusterManager.h"
#include "LArOpenCV/ImageCluster/AlgoData/ParticleCluster.h"
#include "LArOpenCV/ImageCluster/AlgoModule/SuperClusterMaker.h"



int main()  {

  larocv::ImageClusterManager alg_mgr;
  
  //wtf...
  larocv::SuperClusterMaker mkr;
  //

  auto config = larocv::CreatePSetFromFile("../image_cluster_manager.cfg","ImageClusterManager");
    
  std::cout <<  "Loaded config" << std::endl;
  std::cout <<  config.dump() ;
  std::cout << std::endl;

  alg_mgr.Configure(config);


  //
  // Give images to the algorithm
  //

  //
  // Read in the ADC images
  //
  cv::Mat adc0 = cv::imread("../img/adc.001.jpeg",cv::IMREAD_GRAYSCALE);
  cv::Mat adc1 = cv::imread("../img/adc.002.jpeg",cv::IMREAD_GRAYSCALE);
  cv::Mat adc2 = cv::imread("../img/adc.003.jpeg",cv::IMREAD_GRAYSCALE);

  adc0 = 255 - adc0;
  adc1 = 255 - adc1;
  adc2 = 255 - adc2;
    
  std::vector<cv::Mat> adc_v = {adc0,adc1,adc2};
  //
  // Read in the Track images
  //

  cv::Mat trk0 = cv::imread("../img/track.001.jpeg",cv::IMREAD_GRAYSCALE);
  cv::Mat trk1 = cv::imread("../img/track.002.jpeg",cv::IMREAD_GRAYSCALE);
  cv::Mat trk2 = cv::imread("../img/track.003.jpeg",cv::IMREAD_GRAYSCALE);
    
  trk0 = 255 - trk0;
  trk1 = 255 - trk1;
  trk2 = 255 - trk2;

  std::vector<cv::Mat> trk_v = {trk0,trk1,trk2};

  //
  // Read in the shower images
  //
  cv::Mat shr0 = cv::imread("../img/shower.001.jpeg",cv::IMREAD_GRAYSCALE);
  cv::Mat shr1 = cv::imread("../img/shower.002.jpeg",cv::IMREAD_GRAYSCALE);
  cv::Mat shr2 = cv::imread("../img/shower.003.jpeg",cv::IMREAD_GRAYSCALE);

  shr0 = 255 - shr0;
  shr1 = 255 - shr1;
  shr2 = 255 - shr2;

  std::vector<cv::Mat> shr_v = {shr0,shr1,shr2};

  // Insert ADC, Track, & Shower images
  std::cout <<  "Inserting shower images..." << std::endl;
  for(size_t plane=0; plane<3; ++plane) {
    const auto& mat = adc_v[plane];
    larocv::ImageMeta meta(mat.rows,mat.cols,
			   mat.rows,mat.cols,
			   0,0,
			   plane);
	
    alg_mgr.Add(adc_v[plane],meta,larocv::ImageSetID_t::kImageSetWire); 
    alg_mgr.Add(trk_v[plane],meta,larocv::ImageSetID_t::kImageSetTrack);
    alg_mgr.Add(shr_v[plane],meta,larocv::ImageSetID_t::kImageSetShower);
  }     
  
  std::cout <<  "... done" << std::endl;
  std::cout << std::endl;
  
  //
  // Call reconstruct 
  //
  std::cout <<  "Processing!" << std::endl;
  alg_mgr.Process();
  std::cout <<  "...done" << std::endl;
  std::cout << std::endl;
  
  //
  // Get the algorithm data
  //
  std::string algo_name = "adc_superclustermaker";
  larocv::AlgorithmID_t algo_id = alg_mgr.GetClusterAlgID(algo_name);
  std::cout <<  "Got" << algo_name << "@ id=" << algo_id << std::endl;
  std::cout << std::endl;
    
  const larocv::data::AlgoDataManager& data_man = alg_mgr.DataManager();

  std::vector<larocv::data::ParticleClusterArray*> data_v(3);
  for(size_t plane = 0; plane<3; ++plane) 
    data_v[plane] = (larocv::data::ParticleClusterArray*)data_man.Data(algo_id,plane);


  cv::imwrite("input_adc_plane_0.png",adc_v.front());

  cv::Mat img_clone = adc_v.front().clone();

  for(auto pcluster : data_v.front()->as_vector()) {
    std::vector<std::vector<cv::Point> > ctor_v(1,pcluster._ctor);
    cv::drawContours(img_clone,ctor_v,-1,cv::Scalar(255));
  }

    
  cv::imwrite("output_adc_plane_0.png",img_clone);

  return 0;
}
