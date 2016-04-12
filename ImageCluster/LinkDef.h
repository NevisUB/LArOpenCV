//
// cint script to generate libraries
// Declaire namespace & classes you defined
// #pragma statement: order matters! Google it ;)
//

#ifdef __CINT__
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ class larcv::Contour_t+;
#pragma link C++ class larcv::ContourArray_t+;
#pragma link C++ class larcv::ImageClusterBase+;
#pragma link C++ class larcv::ClusterAlgoBase+;
#pragma link C++ class larcv::MatchAlgoBase+;
#pragma link C++ class larcv::ReClusterAlgoBase+;

#pragma link C++ class larcv::ClusterAlgoFactory+;
#pragma link C++ class larcv::MatchAlgoFactory+;

#pragma link C++ class larcv::ImageClusterManager+;
#pragma link C++ class larcv::ImageClusterViewer+;

#pragma link C++ class larcv::Cluster2D+;
#pragma link C++ class larcv::SBCluster+;
#pragma link C++ class larcv::SatelliteMerge+;
#pragma link C++ class larcv::TrackShower+;

#pragma link C++ class larcv::ClusterParams+;
#pragma link C++ class larcv::PCABox+;
#pragma link C++ class larcv::PCAPath+;
#pragma link C++ class larcv::PCASegmentation+;

#pragma link C++ class larcv::PCAOverall+;
#pragma link C++ class larcv::PiZeroFilter+;
#pragma link C++ class larcv::PiZeroFilterV+;

#pragma link C++ class larcv::MatchTimeOverlap+;
#pragma link C++ class larcv::EmptyMatcher+;
#pragma link C++ class larcv::BoundRectStart+;
#pragma link C++ class larcv::CheckStartDistance+;
#pragma link C++ class larcv::SimpleCuts+;

#pragma link C++ class larcv::PID+;

#pragma link C++ class larcv::StartEndMerge+;

#pragma link C++ class larcv::Vertex2D+;
#pragma link C++ class larcv::ROIStart+;

#pragma link C++ class larcv::AttachedClusters+;
#pragma link C++ class larcv::DeadWireCombine+;
//ADD_NEW_CLASS ... do not change this line
#endif












