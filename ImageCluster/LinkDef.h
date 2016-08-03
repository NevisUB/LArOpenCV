//
// cint script to generate libraries
// Declaire namespace & classes you defined
// #pragma statement: order matters! Google it ;)
//

#ifdef __CINT__
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ class larocv::Contour_t+;
#pragma link C++ class larocv::ContourArray_t+;
#pragma link C++ class larocv::ImageClusterBase+;
#pragma link C++ class larocv::ClusterAlgoBase+;
#pragma link C++ class larocv::MatchAlgoBase+;
#pragma link C++ class larocv::ReClusterAlgoBase+;

#pragma link C++ class larocv::ClusterAlgoFactory+;
#pragma link C++ class larocv::MatchAlgoFactory+;

#pragma link C++ class larocv::ImageClusterManager+;
#pragma link C++ class larocv::ImageClusterViewer+;

#pragma link C++ class larocv::Cluster2D+;
#pragma link C++ class larocv::SBCluster+;
#pragma link C++ class larocv::SatelliteMerge+;
#pragma link C++ class larocv::TrackShower+;

#pragma link C++ class larocv::ClusterParams+;
#pragma link C++ class larocv::PCABox+;
#pragma link C++ class larocv::PCAPath+;
#pragma link C++ class larocv::PCASegmentation+;

#pragma link C++ class larocv::PCAOverall+;
#pragma link C++ class larocv::PiZeroFilter+;
#pragma link C++ class larocv::PiZeroFilterV+;

#pragma link C++ class larocv::MatchTimeOverlap+;
#pragma link C++ class larocv::EmptyMatcher+;
#pragma link C++ class larocv::BoundRectStart+;
#pragma link C++ class larocv::CheckStartDistance+;
#pragma link C++ class larocv::SimpleCuts+;

#pragma link C++ class larocv::PID+;

#pragma link C++ class larocv::StartEndMerge+;

#pragma link C++ class larocv::Vertex2D+;
#pragma link C++ class larocv::ROIStart+;
//#pragma link C++ class larocv::BigClusFilter+;
#pragma link C++ class larocv::InConeCluster+;

#pragma link C++ class larocv::AttachedClusters+;		
#pragma link C++ class larocv::DeadWireCombine+;		
#pragma link C++ class larocv::RecoROIFilter+;		
#pragma link C++ class larocv::ROIParameters+;		
#pragma link C++ class larocv::RecoParameters+;
#pragma link C++ class larocv::NearestConeCluster+;
#pragma link C++ class larocv::SBManCluster+;
#pragma link C++ class larocv::ROIAssistedStart+;

#pragma link C++ class larocv::TriangleClusterExt+;
//ADD_NEW_CLASS ... do not change this line
#endif



