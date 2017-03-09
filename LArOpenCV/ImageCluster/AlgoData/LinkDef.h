
//
// cint script to generate libraries
// Declaire namespace & classes you defined
// #pragma statement: order matters! Google it ;)
//

#ifdef __CINT__
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ class larocv::data::Vertex2D+;
#pragma link C++ class std::vector<larocv::data::Vertex2D>+;
#pragma link C++ class larocv::data::CircleVertex+;
#pragma link C++ class std::vector<larocv::data::CircleVertex>+;
#pragma link C++ class larocv::data::Vertex3D+;
#pragma link C++ class std::vector<larocv::data::Vertex3D>+;
#pragma link C++ class larocv::data::Vertex3DArray+;

#pragma link C++ class larocv::data::AtomicContour;
#pragma link C++ class std::vector<larocv::data::AtomicContour>+;
#pragma link C++ class larocv::data::ContourDefect;
#pragma link C++ class std::vector<larocv::data::ContourDefect>+;
#pragma link C++ class larocv::data::TrackClusterCompound+;
#pragma link C++ class std::vector<larocv::data::TrackClusterCompound>+;
#pragma link C++ class std::vector<std::vector<larocv::data::TrackClusterCompound> >+;
#pragma link C++ class std::vector<std::vector<std::vector<larocv::data::TrackClusterCompound> > >+;

#pragma link C++ class larocv::data::ParticleCluster+;
#pragma link C++ class std::vector<larocv::data::ParticleCluster>+;
#pragma link C++ class std::vector<std::vector<larocv::data::ParticleCluster> >+;
#pragma link C++ class std::vector<std::vector<std::vector<larocv::data::ParticleCluster> > >+;

#pragma link C++ class larocv::data::VertexSeed2D;
#pragma link C++ class std::vector<larocv::data::VertexSeed2D>+;
#pragma link C++ class larocv::data::VertexSeed3D;
#pragma link C++ class std::vector<larocv::data::VertexSeed3D>+;

#pragma link C++ class larocv::data::LinearTrack2D+;
#pragma link C++ class std::vector<larocv::data::LinearTrack2D>+;
#pragma link C++ class larocv::data::LinearTrack3D+;
#pragma link C++ class std::vector<larocv::data::LinearTrack3D>+;

#pragma link C++ class larocv::data::PointPCA+;
#pragma link C++ class std::vector<larocv::data::PointPCA>+;
#pragma link C++ class larocv::data::CircleVertex+;
#pragma link C++ class std::vector<larocv::data::CircleVertex>+;
#pragma link C++ enum class larocv::data::ParticleType_t+;
#pragma link C++ enum class larocv::data::VertexType_t+;

#pragma link C++ class larocv::data::ParticleClusterArray+;
#pragma link C++ class larocv::data::TrackClusterCompoundArray+;
#pragma link C++ class std::vector<larocv::data::ParticleClusterArray>+;
#pragma link C++ class std::vector<larocv::data::TrackClusterCompoundArray>+;

#pragma link C++ class larocv::data::AlgoDataArrayTemplate<larocv::data::Vertex3D>+;
#pragma link C++ class larocv::data::AlgoDataArrayTemplate<larocv::data::ParticleCluster>+;
#pragma link C++ class larocv::data::AlgoDataArrayTemplate<larocv::data::TrackClusterCompound>+;

#pragma link C++ class std::vector<larocv::data::AlgoDataArrayTemplate<larocv::data::Vertex3D> >+;
#pragma link C++ class std::vector<larocv::data::AlgoDataArrayTemplate<larocv::data::ParticleCluster> >+;
#pragma link C++ class std::vector<larocv::data::AlgoDataArrayTemplate<larocv::data::TrackClusterCompound> >+;

//ADD_NEW_CLASS ... do not change this line
#endif



