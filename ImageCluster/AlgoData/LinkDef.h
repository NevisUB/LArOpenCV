
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
#pragma link C++ class larocv::data::Refine2DVertexData+;
#pragma link C++ class larocv::data::Refine2DVertexPlaneData+;

#pragma link C++ class larocv::data::LinearVtxFilterData+;
#pragma link C++ class larocv::data::LinearVtxFilterPlaneData+;

#pragma link C++ class larocv::data::ContourDefect+;
#pragma link C++ class larocv::data::AtomicContour+;
#pragma link C++ class larocv::data::ClusterCompound+;
#pragma link C++ class larocv::data::DefectClusterPlaneData+;
#pragma link C++ class larocv::data::DefectClusterData+;


#pragma link C++ class larocv::data::ParticleCluster+;
#pragma link C++ class larocv::data::ParticleClusterArray+;
#pragma link C++ class larocv::data::VertexClusterArray+;

//ADD_NEW_CLASS ... do not change this line
#endif



