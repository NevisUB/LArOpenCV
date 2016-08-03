// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME LArOpenCV_ImageClusterMerge

/*******************************************************************/
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#define G__DICTIONARY
#include "RConfig.h"
#include "TClass.h"
#include "TDictAttributeMap.h"
#include "TInterpreter.h"
#include "TROOT.h"
#include "TBuffer.h"
#include "TMemberInspector.h"
#include "TInterpreter.h"
#include "TVirtualMutex.h"
#include "TError.h"

#ifndef G__ROOT
#define G__ROOT
#endif

#include "RtypesImp.h"
#include "TIsAProxy.h"
#include "TFileMergeInfo.h"
#include <algorithm>
#include "TCollectionProxyInfo.h"
/*******************************************************************/

#include "TDataMember.h"

// Since CINT ignores the std namespace, we need to do so in this file.
namespace std {} using namespace std;

// Header files passed as explicit arguments
#include "DeadWireCombine.h"
#include "InConeCluster.h"
#include "NearestConeCluster.h"
#include "SatelliteMerge.h"
#include "StartEndMerge.h"
#include "TriangleClusterExt.h"

// Header files passed via #pragma extra_include

namespace ROOT {
   static TClass *larocvcLcLStartEndMerge_Dictionary();
   static void larocvcLcLStartEndMerge_TClassManip(TClass*);
   static void *new_larocvcLcLStartEndMerge(void *p = 0);
   static void *newArray_larocvcLcLStartEndMerge(Long_t size, void *p);
   static void delete_larocvcLcLStartEndMerge(void *p);
   static void deleteArray_larocvcLcLStartEndMerge(void *p);
   static void destruct_larocvcLcLStartEndMerge(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::larocv::StartEndMerge*)
   {
      ::larocv::StartEndMerge *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::larocv::StartEndMerge));
      static ::ROOT::TGenericClassInfo 
         instance("larocv::StartEndMerge", "StartEndMerge.h", 25,
                  typeid(::larocv::StartEndMerge), DefineBehavior(ptr, ptr),
                  &larocvcLcLStartEndMerge_Dictionary, isa_proxy, 4,
                  sizeof(::larocv::StartEndMerge) );
      instance.SetNew(&new_larocvcLcLStartEndMerge);
      instance.SetNewArray(&newArray_larocvcLcLStartEndMerge);
      instance.SetDelete(&delete_larocvcLcLStartEndMerge);
      instance.SetDeleteArray(&deleteArray_larocvcLcLStartEndMerge);
      instance.SetDestructor(&destruct_larocvcLcLStartEndMerge);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::larocv::StartEndMerge*)
   {
      return GenerateInitInstanceLocal((::larocv::StartEndMerge*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::larocv::StartEndMerge*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *larocvcLcLStartEndMerge_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::larocv::StartEndMerge*)0x0)->GetClass();
      larocvcLcLStartEndMerge_TClassManip(theClass);
   return theClass;
   }

   static void larocvcLcLStartEndMerge_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *larocvcLcLInConeCluster_Dictionary();
   static void larocvcLcLInConeCluster_TClassManip(TClass*);
   static void *new_larocvcLcLInConeCluster(void *p = 0);
   static void *newArray_larocvcLcLInConeCluster(Long_t size, void *p);
   static void delete_larocvcLcLInConeCluster(void *p);
   static void deleteArray_larocvcLcLInConeCluster(void *p);
   static void destruct_larocvcLcLInConeCluster(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::larocv::InConeCluster*)
   {
      ::larocv::InConeCluster *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::larocv::InConeCluster));
      static ::ROOT::TGenericClassInfo 
         instance("larocv::InConeCluster", "InConeCluster.h", 25,
                  typeid(::larocv::InConeCluster), DefineBehavior(ptr, ptr),
                  &larocvcLcLInConeCluster_Dictionary, isa_proxy, 4,
                  sizeof(::larocv::InConeCluster) );
      instance.SetNew(&new_larocvcLcLInConeCluster);
      instance.SetNewArray(&newArray_larocvcLcLInConeCluster);
      instance.SetDelete(&delete_larocvcLcLInConeCluster);
      instance.SetDeleteArray(&deleteArray_larocvcLcLInConeCluster);
      instance.SetDestructor(&destruct_larocvcLcLInConeCluster);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::larocv::InConeCluster*)
   {
      return GenerateInitInstanceLocal((::larocv::InConeCluster*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::larocv::InConeCluster*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *larocvcLcLInConeCluster_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::larocv::InConeCluster*)0x0)->GetClass();
      larocvcLcLInConeCluster_TClassManip(theClass);
   return theClass;
   }

   static void larocvcLcLInConeCluster_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *larocvcLcLDeadWireCombine_Dictionary();
   static void larocvcLcLDeadWireCombine_TClassManip(TClass*);
   static void *new_larocvcLcLDeadWireCombine(void *p = 0);
   static void *newArray_larocvcLcLDeadWireCombine(Long_t size, void *p);
   static void delete_larocvcLcLDeadWireCombine(void *p);
   static void deleteArray_larocvcLcLDeadWireCombine(void *p);
   static void destruct_larocvcLcLDeadWireCombine(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::larocv::DeadWireCombine*)
   {
      ::larocv::DeadWireCombine *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::larocv::DeadWireCombine));
      static ::ROOT::TGenericClassInfo 
         instance("larocv::DeadWireCombine", "DeadWireCombine.h", 41,
                  typeid(::larocv::DeadWireCombine), DefineBehavior(ptr, ptr),
                  &larocvcLcLDeadWireCombine_Dictionary, isa_proxy, 4,
                  sizeof(::larocv::DeadWireCombine) );
      instance.SetNew(&new_larocvcLcLDeadWireCombine);
      instance.SetNewArray(&newArray_larocvcLcLDeadWireCombine);
      instance.SetDelete(&delete_larocvcLcLDeadWireCombine);
      instance.SetDeleteArray(&deleteArray_larocvcLcLDeadWireCombine);
      instance.SetDestructor(&destruct_larocvcLcLDeadWireCombine);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::larocv::DeadWireCombine*)
   {
      return GenerateInitInstanceLocal((::larocv::DeadWireCombine*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::larocv::DeadWireCombine*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *larocvcLcLDeadWireCombine_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::larocv::DeadWireCombine*)0x0)->GetClass();
      larocvcLcLDeadWireCombine_TClassManip(theClass);
   return theClass;
   }

   static void larocvcLcLDeadWireCombine_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *larocvcLcLNearestConeCluster_Dictionary();
   static void larocvcLcLNearestConeCluster_TClassManip(TClass*);
   static void *new_larocvcLcLNearestConeCluster(void *p = 0);
   static void *newArray_larocvcLcLNearestConeCluster(Long_t size, void *p);
   static void delete_larocvcLcLNearestConeCluster(void *p);
   static void deleteArray_larocvcLcLNearestConeCluster(void *p);
   static void destruct_larocvcLcLNearestConeCluster(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::larocv::NearestConeCluster*)
   {
      ::larocv::NearestConeCluster *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::larocv::NearestConeCluster));
      static ::ROOT::TGenericClassInfo 
         instance("larocv::NearestConeCluster", "NearestConeCluster.h", 25,
                  typeid(::larocv::NearestConeCluster), DefineBehavior(ptr, ptr),
                  &larocvcLcLNearestConeCluster_Dictionary, isa_proxy, 4,
                  sizeof(::larocv::NearestConeCluster) );
      instance.SetNew(&new_larocvcLcLNearestConeCluster);
      instance.SetNewArray(&newArray_larocvcLcLNearestConeCluster);
      instance.SetDelete(&delete_larocvcLcLNearestConeCluster);
      instance.SetDeleteArray(&deleteArray_larocvcLcLNearestConeCluster);
      instance.SetDestructor(&destruct_larocvcLcLNearestConeCluster);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::larocv::NearestConeCluster*)
   {
      return GenerateInitInstanceLocal((::larocv::NearestConeCluster*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::larocv::NearestConeCluster*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *larocvcLcLNearestConeCluster_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::larocv::NearestConeCluster*)0x0)->GetClass();
      larocvcLcLNearestConeCluster_TClassManip(theClass);
   return theClass;
   }

   static void larocvcLcLNearestConeCluster_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *larocvcLcLSatelliteMerge_Dictionary();
   static void larocvcLcLSatelliteMerge_TClassManip(TClass*);
   static void *new_larocvcLcLSatelliteMerge(void *p = 0);
   static void *newArray_larocvcLcLSatelliteMerge(Long_t size, void *p);
   static void delete_larocvcLcLSatelliteMerge(void *p);
   static void deleteArray_larocvcLcLSatelliteMerge(void *p);
   static void destruct_larocvcLcLSatelliteMerge(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::larocv::SatelliteMerge*)
   {
      ::larocv::SatelliteMerge *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::larocv::SatelliteMerge));
      static ::ROOT::TGenericClassInfo 
         instance("larocv::SatelliteMerge", "SatelliteMerge.h", 25,
                  typeid(::larocv::SatelliteMerge), DefineBehavior(ptr, ptr),
                  &larocvcLcLSatelliteMerge_Dictionary, isa_proxy, 4,
                  sizeof(::larocv::SatelliteMerge) );
      instance.SetNew(&new_larocvcLcLSatelliteMerge);
      instance.SetNewArray(&newArray_larocvcLcLSatelliteMerge);
      instance.SetDelete(&delete_larocvcLcLSatelliteMerge);
      instance.SetDeleteArray(&deleteArray_larocvcLcLSatelliteMerge);
      instance.SetDestructor(&destruct_larocvcLcLSatelliteMerge);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::larocv::SatelliteMerge*)
   {
      return GenerateInitInstanceLocal((::larocv::SatelliteMerge*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::larocv::SatelliteMerge*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *larocvcLcLSatelliteMerge_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::larocv::SatelliteMerge*)0x0)->GetClass();
      larocvcLcLSatelliteMerge_TClassManip(theClass);
   return theClass;
   }

   static void larocvcLcLSatelliteMerge_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *larocvcLcLTriangleClusterExt_Dictionary();
   static void larocvcLcLTriangleClusterExt_TClassManip(TClass*);
   static void *new_larocvcLcLTriangleClusterExt(void *p = 0);
   static void *newArray_larocvcLcLTriangleClusterExt(Long_t size, void *p);
   static void delete_larocvcLcLTriangleClusterExt(void *p);
   static void deleteArray_larocvcLcLTriangleClusterExt(void *p);
   static void destruct_larocvcLcLTriangleClusterExt(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::larocv::TriangleClusterExt*)
   {
      ::larocv::TriangleClusterExt *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::larocv::TriangleClusterExt));
      static ::ROOT::TGenericClassInfo 
         instance("larocv::TriangleClusterExt", "TriangleClusterExt.h", 25,
                  typeid(::larocv::TriangleClusterExt), DefineBehavior(ptr, ptr),
                  &larocvcLcLTriangleClusterExt_Dictionary, isa_proxy, 4,
                  sizeof(::larocv::TriangleClusterExt) );
      instance.SetNew(&new_larocvcLcLTriangleClusterExt);
      instance.SetNewArray(&newArray_larocvcLcLTriangleClusterExt);
      instance.SetDelete(&delete_larocvcLcLTriangleClusterExt);
      instance.SetDeleteArray(&deleteArray_larocvcLcLTriangleClusterExt);
      instance.SetDestructor(&destruct_larocvcLcLTriangleClusterExt);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::larocv::TriangleClusterExt*)
   {
      return GenerateInitInstanceLocal((::larocv::TriangleClusterExt*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::larocv::TriangleClusterExt*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *larocvcLcLTriangleClusterExt_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::larocv::TriangleClusterExt*)0x0)->GetClass();
      larocvcLcLTriangleClusterExt_TClassManip(theClass);
   return theClass;
   }

   static void larocvcLcLTriangleClusterExt_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_larocvcLcLStartEndMerge(void *p) {
      return  p ? ::new((::ROOT::TOperatorNewHelper*)p) ::larocv::StartEndMerge : new ::larocv::StartEndMerge;
   }
   static void *newArray_larocvcLcLStartEndMerge(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::TOperatorNewHelper*)p) ::larocv::StartEndMerge[nElements] : new ::larocv::StartEndMerge[nElements];
   }
   // Wrapper around operator delete
   static void delete_larocvcLcLStartEndMerge(void *p) {
      delete ((::larocv::StartEndMerge*)p);
   }
   static void deleteArray_larocvcLcLStartEndMerge(void *p) {
      delete [] ((::larocv::StartEndMerge*)p);
   }
   static void destruct_larocvcLcLStartEndMerge(void *p) {
      typedef ::larocv::StartEndMerge current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::larocv::StartEndMerge

namespace ROOT {
   // Wrappers around operator new
   static void *new_larocvcLcLInConeCluster(void *p) {
      return  p ? ::new((::ROOT::TOperatorNewHelper*)p) ::larocv::InConeCluster : new ::larocv::InConeCluster;
   }
   static void *newArray_larocvcLcLInConeCluster(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::TOperatorNewHelper*)p) ::larocv::InConeCluster[nElements] : new ::larocv::InConeCluster[nElements];
   }
   // Wrapper around operator delete
   static void delete_larocvcLcLInConeCluster(void *p) {
      delete ((::larocv::InConeCluster*)p);
   }
   static void deleteArray_larocvcLcLInConeCluster(void *p) {
      delete [] ((::larocv::InConeCluster*)p);
   }
   static void destruct_larocvcLcLInConeCluster(void *p) {
      typedef ::larocv::InConeCluster current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::larocv::InConeCluster

namespace ROOT {
   // Wrappers around operator new
   static void *new_larocvcLcLDeadWireCombine(void *p) {
      return  p ? ::new((::ROOT::TOperatorNewHelper*)p) ::larocv::DeadWireCombine : new ::larocv::DeadWireCombine;
   }
   static void *newArray_larocvcLcLDeadWireCombine(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::TOperatorNewHelper*)p) ::larocv::DeadWireCombine[nElements] : new ::larocv::DeadWireCombine[nElements];
   }
   // Wrapper around operator delete
   static void delete_larocvcLcLDeadWireCombine(void *p) {
      delete ((::larocv::DeadWireCombine*)p);
   }
   static void deleteArray_larocvcLcLDeadWireCombine(void *p) {
      delete [] ((::larocv::DeadWireCombine*)p);
   }
   static void destruct_larocvcLcLDeadWireCombine(void *p) {
      typedef ::larocv::DeadWireCombine current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::larocv::DeadWireCombine

namespace ROOT {
   // Wrappers around operator new
   static void *new_larocvcLcLNearestConeCluster(void *p) {
      return  p ? ::new((::ROOT::TOperatorNewHelper*)p) ::larocv::NearestConeCluster : new ::larocv::NearestConeCluster;
   }
   static void *newArray_larocvcLcLNearestConeCluster(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::TOperatorNewHelper*)p) ::larocv::NearestConeCluster[nElements] : new ::larocv::NearestConeCluster[nElements];
   }
   // Wrapper around operator delete
   static void delete_larocvcLcLNearestConeCluster(void *p) {
      delete ((::larocv::NearestConeCluster*)p);
   }
   static void deleteArray_larocvcLcLNearestConeCluster(void *p) {
      delete [] ((::larocv::NearestConeCluster*)p);
   }
   static void destruct_larocvcLcLNearestConeCluster(void *p) {
      typedef ::larocv::NearestConeCluster current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::larocv::NearestConeCluster

namespace ROOT {
   // Wrappers around operator new
   static void *new_larocvcLcLSatelliteMerge(void *p) {
      return  p ? ::new((::ROOT::TOperatorNewHelper*)p) ::larocv::SatelliteMerge : new ::larocv::SatelliteMerge;
   }
   static void *newArray_larocvcLcLSatelliteMerge(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::TOperatorNewHelper*)p) ::larocv::SatelliteMerge[nElements] : new ::larocv::SatelliteMerge[nElements];
   }
   // Wrapper around operator delete
   static void delete_larocvcLcLSatelliteMerge(void *p) {
      delete ((::larocv::SatelliteMerge*)p);
   }
   static void deleteArray_larocvcLcLSatelliteMerge(void *p) {
      delete [] ((::larocv::SatelliteMerge*)p);
   }
   static void destruct_larocvcLcLSatelliteMerge(void *p) {
      typedef ::larocv::SatelliteMerge current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::larocv::SatelliteMerge

namespace ROOT {
   // Wrappers around operator new
   static void *new_larocvcLcLTriangleClusterExt(void *p) {
      return  p ? ::new((::ROOT::TOperatorNewHelper*)p) ::larocv::TriangleClusterExt : new ::larocv::TriangleClusterExt;
   }
   static void *newArray_larocvcLcLTriangleClusterExt(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::TOperatorNewHelper*)p) ::larocv::TriangleClusterExt[nElements] : new ::larocv::TriangleClusterExt[nElements];
   }
   // Wrapper around operator delete
   static void delete_larocvcLcLTriangleClusterExt(void *p) {
      delete ((::larocv::TriangleClusterExt*)p);
   }
   static void deleteArray_larocvcLcLTriangleClusterExt(void *p) {
      delete [] ((::larocv::TriangleClusterExt*)p);
   }
   static void destruct_larocvcLcLTriangleClusterExt(void *p) {
      typedef ::larocv::TriangleClusterExt current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::larocv::TriangleClusterExt

namespace ROOT {
   static TClass *vectorlEvectorlEpairlEintcOintgRsPgRsPgR_Dictionary();
   static void vectorlEvectorlEpairlEintcOintgRsPgRsPgR_TClassManip(TClass*);
   static void *new_vectorlEvectorlEpairlEintcOintgRsPgRsPgR(void *p = 0);
   static void *newArray_vectorlEvectorlEpairlEintcOintgRsPgRsPgR(Long_t size, void *p);
   static void delete_vectorlEvectorlEpairlEintcOintgRsPgRsPgR(void *p);
   static void deleteArray_vectorlEvectorlEpairlEintcOintgRsPgRsPgR(void *p);
   static void destruct_vectorlEvectorlEpairlEintcOintgRsPgRsPgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<vector<pair<int,int> > >*)
   {
      vector<vector<pair<int,int> > > *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<vector<pair<int,int> > >));
      static ::ROOT::TGenericClassInfo 
         instance("vector<vector<pair<int,int> > >", -2, "vector", 457,
                  typeid(vector<vector<pair<int,int> > >), DefineBehavior(ptr, ptr),
                  &vectorlEvectorlEpairlEintcOintgRsPgRsPgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<vector<pair<int,int> > >) );
      instance.SetNew(&new_vectorlEvectorlEpairlEintcOintgRsPgRsPgR);
      instance.SetNewArray(&newArray_vectorlEvectorlEpairlEintcOintgRsPgRsPgR);
      instance.SetDelete(&delete_vectorlEvectorlEpairlEintcOintgRsPgRsPgR);
      instance.SetDeleteArray(&deleteArray_vectorlEvectorlEpairlEintcOintgRsPgRsPgR);
      instance.SetDestructor(&destruct_vectorlEvectorlEpairlEintcOintgRsPgRsPgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<vector<pair<int,int> > > >()));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const vector<vector<pair<int,int> > >*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEvectorlEpairlEintcOintgRsPgRsPgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<vector<pair<int,int> > >*)0x0)->GetClass();
      vectorlEvectorlEpairlEintcOintgRsPgRsPgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEvectorlEpairlEintcOintgRsPgRsPgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEvectorlEpairlEintcOintgRsPgRsPgR(void *p) {
      return  p ? ::new((::ROOT::TOperatorNewHelper*)p) vector<vector<pair<int,int> > > : new vector<vector<pair<int,int> > >;
   }
   static void *newArray_vectorlEvectorlEpairlEintcOintgRsPgRsPgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::TOperatorNewHelper*)p) vector<vector<pair<int,int> > >[nElements] : new vector<vector<pair<int,int> > >[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEvectorlEpairlEintcOintgRsPgRsPgR(void *p) {
      delete ((vector<vector<pair<int,int> > >*)p);
   }
   static void deleteArray_vectorlEvectorlEpairlEintcOintgRsPgRsPgR(void *p) {
      delete [] ((vector<vector<pair<int,int> > >*)p);
   }
   static void destruct_vectorlEvectorlEpairlEintcOintgRsPgRsPgR(void *p) {
      typedef vector<vector<pair<int,int> > > current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<vector<pair<int,int> > >

namespace ROOT {
   static TClass *vectorlEvectorlEcvcLcLPoint_lEintgRsPgRsPgR_Dictionary();
   static void vectorlEvectorlEcvcLcLPoint_lEintgRsPgRsPgR_TClassManip(TClass*);
   static void *new_vectorlEvectorlEcvcLcLPoint_lEintgRsPgRsPgR(void *p = 0);
   static void *newArray_vectorlEvectorlEcvcLcLPoint_lEintgRsPgRsPgR(Long_t size, void *p);
   static void delete_vectorlEvectorlEcvcLcLPoint_lEintgRsPgRsPgR(void *p);
   static void deleteArray_vectorlEvectorlEcvcLcLPoint_lEintgRsPgRsPgR(void *p);
   static void destruct_vectorlEvectorlEcvcLcLPoint_lEintgRsPgRsPgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<vector<cv::Point_<int> > >*)
   {
      vector<vector<cv::Point_<int> > > *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<vector<cv::Point_<int> > >));
      static ::ROOT::TGenericClassInfo 
         instance("vector<vector<cv::Point_<int> > >", -2, "vector", 457,
                  typeid(vector<vector<cv::Point_<int> > >), DefineBehavior(ptr, ptr),
                  &vectorlEvectorlEcvcLcLPoint_lEintgRsPgRsPgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<vector<cv::Point_<int> > >) );
      instance.SetNew(&new_vectorlEvectorlEcvcLcLPoint_lEintgRsPgRsPgR);
      instance.SetNewArray(&newArray_vectorlEvectorlEcvcLcLPoint_lEintgRsPgRsPgR);
      instance.SetDelete(&delete_vectorlEvectorlEcvcLcLPoint_lEintgRsPgRsPgR);
      instance.SetDeleteArray(&deleteArray_vectorlEvectorlEcvcLcLPoint_lEintgRsPgRsPgR);
      instance.SetDestructor(&destruct_vectorlEvectorlEcvcLcLPoint_lEintgRsPgRsPgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<vector<cv::Point_<int> > > >()));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const vector<vector<cv::Point_<int> > >*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEvectorlEcvcLcLPoint_lEintgRsPgRsPgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<vector<cv::Point_<int> > >*)0x0)->GetClass();
      vectorlEvectorlEcvcLcLPoint_lEintgRsPgRsPgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEvectorlEcvcLcLPoint_lEintgRsPgRsPgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEvectorlEcvcLcLPoint_lEintgRsPgRsPgR(void *p) {
      return  p ? ::new((::ROOT::TOperatorNewHelper*)p) vector<vector<cv::Point_<int> > > : new vector<vector<cv::Point_<int> > >;
   }
   static void *newArray_vectorlEvectorlEcvcLcLPoint_lEintgRsPgRsPgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::TOperatorNewHelper*)p) vector<vector<cv::Point_<int> > >[nElements] : new vector<vector<cv::Point_<int> > >[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEvectorlEcvcLcLPoint_lEintgRsPgRsPgR(void *p) {
      delete ((vector<vector<cv::Point_<int> > >*)p);
   }
   static void deleteArray_vectorlEvectorlEcvcLcLPoint_lEintgRsPgRsPgR(void *p) {
      delete [] ((vector<vector<cv::Point_<int> > >*)p);
   }
   static void destruct_vectorlEvectorlEcvcLcLPoint_lEintgRsPgRsPgR(void *p) {
      typedef vector<vector<cv::Point_<int> > > current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<vector<cv::Point_<int> > >

namespace ROOT {
   static TClass *vectorlEpairlEintcOintgRsPgR_Dictionary();
   static void vectorlEpairlEintcOintgRsPgR_TClassManip(TClass*);
   static void *new_vectorlEpairlEintcOintgRsPgR(void *p = 0);
   static void *newArray_vectorlEpairlEintcOintgRsPgR(Long_t size, void *p);
   static void delete_vectorlEpairlEintcOintgRsPgR(void *p);
   static void deleteArray_vectorlEpairlEintcOintgRsPgR(void *p);
   static void destruct_vectorlEpairlEintcOintgRsPgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<pair<int,int> >*)
   {
      vector<pair<int,int> > *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<pair<int,int> >));
      static ::ROOT::TGenericClassInfo 
         instance("vector<pair<int,int> >", -2, "vector", 457,
                  typeid(vector<pair<int,int> >), DefineBehavior(ptr, ptr),
                  &vectorlEpairlEintcOintgRsPgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<pair<int,int> >) );
      instance.SetNew(&new_vectorlEpairlEintcOintgRsPgR);
      instance.SetNewArray(&newArray_vectorlEpairlEintcOintgRsPgR);
      instance.SetDelete(&delete_vectorlEpairlEintcOintgRsPgR);
      instance.SetDeleteArray(&deleteArray_vectorlEpairlEintcOintgRsPgR);
      instance.SetDestructor(&destruct_vectorlEpairlEintcOintgRsPgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<pair<int,int> > >()));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const vector<pair<int,int> >*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEpairlEintcOintgRsPgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<pair<int,int> >*)0x0)->GetClass();
      vectorlEpairlEintcOintgRsPgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEpairlEintcOintgRsPgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEpairlEintcOintgRsPgR(void *p) {
      return  p ? ::new((::ROOT::TOperatorNewHelper*)p) vector<pair<int,int> > : new vector<pair<int,int> >;
   }
   static void *newArray_vectorlEpairlEintcOintgRsPgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::TOperatorNewHelper*)p) vector<pair<int,int> >[nElements] : new vector<pair<int,int> >[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEpairlEintcOintgRsPgR(void *p) {
      delete ((vector<pair<int,int> >*)p);
   }
   static void deleteArray_vectorlEpairlEintcOintgRsPgR(void *p) {
      delete [] ((vector<pair<int,int> >*)p);
   }
   static void destruct_vectorlEpairlEintcOintgRsPgR(void *p) {
      typedef vector<pair<int,int> > current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<pair<int,int> >

namespace ROOT {
   static TClass *vectorlEcvcLcLPoint_lEintgRsPgR_Dictionary();
   static void vectorlEcvcLcLPoint_lEintgRsPgR_TClassManip(TClass*);
   static void *new_vectorlEcvcLcLPoint_lEintgRsPgR(void *p = 0);
   static void *newArray_vectorlEcvcLcLPoint_lEintgRsPgR(Long_t size, void *p);
   static void delete_vectorlEcvcLcLPoint_lEintgRsPgR(void *p);
   static void deleteArray_vectorlEcvcLcLPoint_lEintgRsPgR(void *p);
   static void destruct_vectorlEcvcLcLPoint_lEintgRsPgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<cv::Point_<int> >*)
   {
      vector<cv::Point_<int> > *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<cv::Point_<int> >));
      static ::ROOT::TGenericClassInfo 
         instance("vector<cv::Point_<int> >", -2, "vector", 457,
                  typeid(vector<cv::Point_<int> >), DefineBehavior(ptr, ptr),
                  &vectorlEcvcLcLPoint_lEintgRsPgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<cv::Point_<int> >) );
      instance.SetNew(&new_vectorlEcvcLcLPoint_lEintgRsPgR);
      instance.SetNewArray(&newArray_vectorlEcvcLcLPoint_lEintgRsPgR);
      instance.SetDelete(&delete_vectorlEcvcLcLPoint_lEintgRsPgR);
      instance.SetDeleteArray(&deleteArray_vectorlEcvcLcLPoint_lEintgRsPgR);
      instance.SetDestructor(&destruct_vectorlEcvcLcLPoint_lEintgRsPgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<cv::Point_<int> > >()));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const vector<cv::Point_<int> >*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEcvcLcLPoint_lEintgRsPgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<cv::Point_<int> >*)0x0)->GetClass();
      vectorlEcvcLcLPoint_lEintgRsPgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEcvcLcLPoint_lEintgRsPgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEcvcLcLPoint_lEintgRsPgR(void *p) {
      return  p ? ::new((::ROOT::TOperatorNewHelper*)p) vector<cv::Point_<int> > : new vector<cv::Point_<int> >;
   }
   static void *newArray_vectorlEcvcLcLPoint_lEintgRsPgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::TOperatorNewHelper*)p) vector<cv::Point_<int> >[nElements] : new vector<cv::Point_<int> >[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEcvcLcLPoint_lEintgRsPgR(void *p) {
      delete ((vector<cv::Point_<int> >*)p);
   }
   static void deleteArray_vectorlEcvcLcLPoint_lEintgRsPgR(void *p) {
      delete [] ((vector<cv::Point_<int> >*)p);
   }
   static void destruct_vectorlEcvcLcLPoint_lEintgRsPgR(void *p) {
      typedef vector<cv::Point_<int> > current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<cv::Point_<int> >

namespace ROOT {
   static TClass *vectorlEboolgR_Dictionary();
   static void vectorlEboolgR_TClassManip(TClass*);
   static void *new_vectorlEboolgR(void *p = 0);
   static void *newArray_vectorlEboolgR(Long_t size, void *p);
   static void delete_vectorlEboolgR(void *p);
   static void deleteArray_vectorlEboolgR(void *p);
   static void destruct_vectorlEboolgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<bool>*)
   {
      vector<bool> *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<bool>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<bool>", -2, "vector", 2112,
                  typeid(vector<bool>), DefineBehavior(ptr, ptr),
                  &vectorlEboolgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<bool>) );
      instance.SetNew(&new_vectorlEboolgR);
      instance.SetNewArray(&newArray_vectorlEboolgR);
      instance.SetDelete(&delete_vectorlEboolgR);
      instance.SetDeleteArray(&deleteArray_vectorlEboolgR);
      instance.SetDestructor(&destruct_vectorlEboolgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<bool> >()));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const vector<bool>*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEboolgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<bool>*)0x0)->GetClass();
      vectorlEboolgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEboolgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEboolgR(void *p) {
      return  p ? ::new((::ROOT::TOperatorNewHelper*)p) vector<bool> : new vector<bool>;
   }
   static void *newArray_vectorlEboolgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::TOperatorNewHelper*)p) vector<bool>[nElements] : new vector<bool>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEboolgR(void *p) {
      delete ((vector<bool>*)p);
   }
   static void deleteArray_vectorlEboolgR(void *p) {
      delete [] ((vector<bool>*)p);
   }
   static void destruct_vectorlEboolgR(void *p) {
      typedef vector<bool> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<bool>

namespace {
  void TriggerDictionaryInitialization_libLArOpenCV_ImageClusterMerge_Impl() {
    static const char* headers[] = {
"DeadWireCombine.h",
"InConeCluster.h",
"NearestConeCluster.h",
"SatelliteMerge.h",
"StartEndMerge.h",
"TriangleClusterExt.h",
0
    };
    static const char* includePaths[] = {
"/Users/vgenty/sw/larlite/UserDev/BasicTool",
"/Users/vgenty/sw/larlite/UserDev/RecoTool",
"/Users/vgenty/sw/larlite/UserDev/RecoTool/FANN/include",
"/Users/vgenty/sw/larlite/core",
"/System/Library/Frameworks/Python.framework/Versions/2.7/include/python2.7",
"/System/Library/Frameworks/Python.framework/Versions/2.7/include/python2.7",
"/Library/Python/2.7/site-packages/numpy/core/include",
"/usr/local/include",
"/Users/vgenty/sw/laropencv",
"/Users/vgenty/sw/laropencv/ImageCluster/Base",
"/Users/vgenty/sw/laropencv/ImageCluster/Util",
"/usr/local/ROOT//include/root",
"/Users/vgenty/sw/laropencv/ImageCluster/Merge/",
0
    };
    static const char* fwdDeclCode = 
R"DICTFWDDCLS(
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_Autoloading_Map;
namespace larocv{class __attribute__((annotate("$clingAutoload$StartEndMerge.h")))  StartEndMerge;}
namespace larocv{class __attribute__((annotate("$clingAutoload$InConeCluster.h")))  InConeCluster;}
namespace larocv{class __attribute__((annotate("$clingAutoload$DeadWireCombine.h")))  DeadWireCombine;}
namespace larocv{class __attribute__((annotate("$clingAutoload$NearestConeCluster.h")))  NearestConeCluster;}
namespace larocv{class __attribute__((annotate("$clingAutoload$SatelliteMerge.h")))  SatelliteMerge;}
namespace larocv{class __attribute__((annotate("$clingAutoload$TriangleClusterExt.h")))  TriangleClusterExt;}
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(

#ifndef G__VECTOR_HAS_CLASS_ITERATOR
  #define G__VECTOR_HAS_CLASS_ITERATOR 1
#endif

#define _BACKWARD_BACKWARD_WARNING_H
#include "DeadWireCombine.h"
#include "InConeCluster.h"
#include "NearestConeCluster.h"
#include "SatelliteMerge.h"
#include "StartEndMerge.h"
#include "TriangleClusterExt.h"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[]={
"larocv::DeadWireCombine", payloadCode, "@",
"larocv::InConeCluster", payloadCode, "@",
"larocv::NearestConeCluster", payloadCode, "@",
"larocv::SatelliteMerge", payloadCode, "@",
"larocv::StartEndMerge", payloadCode, "@",
"larocv::TriangleClusterExt", payloadCode, "@",
nullptr};

    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("libLArOpenCV_ImageClusterMerge",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_libLArOpenCV_ImageClusterMerge_Impl, {}, classesHeaders);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_libLArOpenCV_ImageClusterMerge_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_libLArOpenCV_ImageClusterMerge() {
  TriggerDictionaryInitialization_libLArOpenCV_ImageClusterMerge_Impl();
}
