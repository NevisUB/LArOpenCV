
//
// cint script to generate libraries
// Declaire namespace & classes you defined
// #pragma statement: order matters! Google it ;)
//

#ifdef __CINT__
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ class larocv::data::Vertex3D+;
#pragma link C++ class larocv::data::Vertex2D+;
#pragma link C++ class larocv::data::PointPCA+;
#pragma link C++ class larocv::data::CircleVertex+;

#pragma link C++ class larocv::data::ContourDefect+;
#pragma link C++ class larocv::data::AtomicContour+;
#pragma link C++ class larocv::data::ClusterCompound+;
#pragma link C++ class larocv::data::ClusterCompoundArray+;

#pragma link C++ class larocv::data::ParticleCluster+;
#pragma link C++ class larocv::data::ParticleClusterArray+;

#pragma link C++ class larocv::data::VertexClusterArray+;

#pragma link C++ class larocv::data::LinearTrack2D+;
#pragma link C++ class larocv::data::LinearTrack+;
#pragma link C++ class larocv::data::LinearTrackArray+;

#pragma link C++ class larocv::data::ShowerCluster+;
#pragma link C++ class larocv::data::SingleShower+;
#pragma link C++ class larocv::data::SingleShowerArray+;

#pragma link C++ class larocv::data::VertexSeedsData+;
#pragma link C++ class larocv::data::VertexEstimateData+;
#pragma link C++ class larocv::data::TrackVertexEstimateData+;
#pragma link C++ class larocv::data::ShowerVertexEstimateData+;

#pragma link C++ class larocv::data::ContourArrayData+;
//ADD_NEW_CLASS ... do not change this line
#endif



