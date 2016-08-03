// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME LArOpenCV_ImageClusterBase

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
#include "BaseUtil.h"
#include "Cluster2D.h"
#include "ClusterAlgoBase.h"
#include "ClusterAlgoFactory.h"
#include "ImageClusterBase.h"
#include "ImageClusterManager.h"
#include "ImageClusterTypes.h"
#include "ImageClusterViewer.h"
#include "MatchAlgoBase.h"
#include "MatchAlgoFactory.h"
#include "MatchBookKeeper.h"
#include "ReClusterAlgoBase.h"
#include "ReClusterAlgoFactory.h"

// Header files passed via #pragma extra_include

namespace ROOT {
   static TClass *larocvcLcLImageClusterBase_Dictionary();
   static void larocvcLcLImageClusterBase_TClassManip(TClass*);
   static void delete_larocvcLcLImageClusterBase(void *p);
   static void deleteArray_larocvcLcLImageClusterBase(void *p);
   static void destruct_larocvcLcLImageClusterBase(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::larocv::ImageClusterBase*)
   {
      ::larocv::ImageClusterBase *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::larocv::ImageClusterBase));
      static ::ROOT::TGenericClassInfo 
         instance("larocv::ImageClusterBase", "ImageClusterBase.h", 47,
                  typeid(::larocv::ImageClusterBase), DefineBehavior(ptr, ptr),
                  &larocvcLcLImageClusterBase_Dictionary, isa_proxy, 4,
                  sizeof(::larocv::ImageClusterBase) );
      instance.SetDelete(&delete_larocvcLcLImageClusterBase);
      instance.SetDeleteArray(&deleteArray_larocvcLcLImageClusterBase);
      instance.SetDestructor(&destruct_larocvcLcLImageClusterBase);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::larocv::ImageClusterBase*)
   {
      return GenerateInitInstanceLocal((::larocv::ImageClusterBase*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::larocv::ImageClusterBase*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *larocvcLcLImageClusterBase_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::larocv::ImageClusterBase*)0x0)->GetClass();
      larocvcLcLImageClusterBase_TClassManip(theClass);
   return theClass;
   }

   static void larocvcLcLImageClusterBase_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *larocvcLcLClusterAlgoBase_Dictionary();
   static void larocvcLcLClusterAlgoBase_TClassManip(TClass*);
   static void delete_larocvcLcLClusterAlgoBase(void *p);
   static void deleteArray_larocvcLcLClusterAlgoBase(void *p);
   static void destruct_larocvcLcLClusterAlgoBase(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::larocv::ClusterAlgoBase*)
   {
      ::larocv::ClusterAlgoBase *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::larocv::ClusterAlgoBase));
      static ::ROOT::TGenericClassInfo 
         instance("larocv::ClusterAlgoBase", "ClusterAlgoBase.h", 40,
                  typeid(::larocv::ClusterAlgoBase), DefineBehavior(ptr, ptr),
                  &larocvcLcLClusterAlgoBase_Dictionary, isa_proxy, 4,
                  sizeof(::larocv::ClusterAlgoBase) );
      instance.SetDelete(&delete_larocvcLcLClusterAlgoBase);
      instance.SetDeleteArray(&deleteArray_larocvcLcLClusterAlgoBase);
      instance.SetDestructor(&destruct_larocvcLcLClusterAlgoBase);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::larocv::ClusterAlgoBase*)
   {
      return GenerateInitInstanceLocal((::larocv::ClusterAlgoBase*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::larocv::ClusterAlgoBase*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *larocvcLcLClusterAlgoBase_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::larocv::ClusterAlgoBase*)0x0)->GetClass();
      larocvcLcLClusterAlgoBase_TClassManip(theClass);
   return theClass;
   }

   static void larocvcLcLClusterAlgoBase_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *larocvcLcLMatchAlgoBase_Dictionary();
   static void larocvcLcLMatchAlgoBase_TClassManip(TClass*);
   static void delete_larocvcLcLMatchAlgoBase(void *p);
   static void deleteArray_larocvcLcLMatchAlgoBase(void *p);
   static void destruct_larocvcLcLMatchAlgoBase(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::larocv::MatchAlgoBase*)
   {
      ::larocv::MatchAlgoBase *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::larocv::MatchAlgoBase));
      static ::ROOT::TGenericClassInfo 
         instance("larocv::MatchAlgoBase", "MatchAlgoBase.h", 40,
                  typeid(::larocv::MatchAlgoBase), DefineBehavior(ptr, ptr),
                  &larocvcLcLMatchAlgoBase_Dictionary, isa_proxy, 4,
                  sizeof(::larocv::MatchAlgoBase) );
      instance.SetDelete(&delete_larocvcLcLMatchAlgoBase);
      instance.SetDeleteArray(&deleteArray_larocvcLcLMatchAlgoBase);
      instance.SetDestructor(&destruct_larocvcLcLMatchAlgoBase);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::larocv::MatchAlgoBase*)
   {
      return GenerateInitInstanceLocal((::larocv::MatchAlgoBase*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::larocv::MatchAlgoBase*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *larocvcLcLMatchAlgoBase_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::larocv::MatchAlgoBase*)0x0)->GetClass();
      larocvcLcLMatchAlgoBase_TClassManip(theClass);
   return theClass;
   }

   static void larocvcLcLMatchAlgoBase_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *larocvcLcLReClusterAlgoBase_Dictionary();
   static void larocvcLcLReClusterAlgoBase_TClassManip(TClass*);
   static void delete_larocvcLcLReClusterAlgoBase(void *p);
   static void deleteArray_larocvcLcLReClusterAlgoBase(void *p);
   static void destruct_larocvcLcLReClusterAlgoBase(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::larocv::ReClusterAlgoBase*)
   {
      ::larocv::ReClusterAlgoBase *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::larocv::ReClusterAlgoBase));
      static ::ROOT::TGenericClassInfo 
         instance("larocv::ReClusterAlgoBase", "ReClusterAlgoBase.h", 25,
                  typeid(::larocv::ReClusterAlgoBase), DefineBehavior(ptr, ptr),
                  &larocvcLcLReClusterAlgoBase_Dictionary, isa_proxy, 4,
                  sizeof(::larocv::ReClusterAlgoBase) );
      instance.SetDelete(&delete_larocvcLcLReClusterAlgoBase);
      instance.SetDeleteArray(&deleteArray_larocvcLcLReClusterAlgoBase);
      instance.SetDestructor(&destruct_larocvcLcLReClusterAlgoBase);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::larocv::ReClusterAlgoBase*)
   {
      return GenerateInitInstanceLocal((::larocv::ReClusterAlgoBase*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::larocv::ReClusterAlgoBase*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *larocvcLcLReClusterAlgoBase_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::larocv::ReClusterAlgoBase*)0x0)->GetClass();
      larocvcLcLReClusterAlgoBase_TClassManip(theClass);
   return theClass;
   }

   static void larocvcLcLReClusterAlgoBase_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *larocvcLcLClusterAlgoFactory_Dictionary();
   static void larocvcLcLClusterAlgoFactory_TClassManip(TClass*);
   static void *new_larocvcLcLClusterAlgoFactory(void *p = 0);
   static void *newArray_larocvcLcLClusterAlgoFactory(Long_t size, void *p);
   static void delete_larocvcLcLClusterAlgoFactory(void *p);
   static void deleteArray_larocvcLcLClusterAlgoFactory(void *p);
   static void destruct_larocvcLcLClusterAlgoFactory(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::larocv::ClusterAlgoFactory*)
   {
      ::larocv::ClusterAlgoFactory *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::larocv::ClusterAlgoFactory));
      static ::ROOT::TGenericClassInfo 
         instance("larocv::ClusterAlgoFactory", "ClusterAlgoFactory.h", 47,
                  typeid(::larocv::ClusterAlgoFactory), DefineBehavior(ptr, ptr),
                  &larocvcLcLClusterAlgoFactory_Dictionary, isa_proxy, 4,
                  sizeof(::larocv::ClusterAlgoFactory) );
      instance.SetNew(&new_larocvcLcLClusterAlgoFactory);
      instance.SetNewArray(&newArray_larocvcLcLClusterAlgoFactory);
      instance.SetDelete(&delete_larocvcLcLClusterAlgoFactory);
      instance.SetDeleteArray(&deleteArray_larocvcLcLClusterAlgoFactory);
      instance.SetDestructor(&destruct_larocvcLcLClusterAlgoFactory);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::larocv::ClusterAlgoFactory*)
   {
      return GenerateInitInstanceLocal((::larocv::ClusterAlgoFactory*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::larocv::ClusterAlgoFactory*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *larocvcLcLClusterAlgoFactory_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::larocv::ClusterAlgoFactory*)0x0)->GetClass();
      larocvcLcLClusterAlgoFactory_TClassManip(theClass);
   return theClass;
   }

   static void larocvcLcLClusterAlgoFactory_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *larocvcLcLMatchAlgoFactory_Dictionary();
   static void larocvcLcLMatchAlgoFactory_TClassManip(TClass*);
   static void *new_larocvcLcLMatchAlgoFactory(void *p = 0);
   static void *newArray_larocvcLcLMatchAlgoFactory(Long_t size, void *p);
   static void delete_larocvcLcLMatchAlgoFactory(void *p);
   static void deleteArray_larocvcLcLMatchAlgoFactory(void *p);
   static void destruct_larocvcLcLMatchAlgoFactory(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::larocv::MatchAlgoFactory*)
   {
      ::larocv::MatchAlgoFactory *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::larocv::MatchAlgoFactory));
      static ::ROOT::TGenericClassInfo 
         instance("larocv::MatchAlgoFactory", "MatchAlgoFactory.h", 47,
                  typeid(::larocv::MatchAlgoFactory), DefineBehavior(ptr, ptr),
                  &larocvcLcLMatchAlgoFactory_Dictionary, isa_proxy, 4,
                  sizeof(::larocv::MatchAlgoFactory) );
      instance.SetNew(&new_larocvcLcLMatchAlgoFactory);
      instance.SetNewArray(&newArray_larocvcLcLMatchAlgoFactory);
      instance.SetDelete(&delete_larocvcLcLMatchAlgoFactory);
      instance.SetDeleteArray(&deleteArray_larocvcLcLMatchAlgoFactory);
      instance.SetDestructor(&destruct_larocvcLcLMatchAlgoFactory);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::larocv::MatchAlgoFactory*)
   {
      return GenerateInitInstanceLocal((::larocv::MatchAlgoFactory*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::larocv::MatchAlgoFactory*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *larocvcLcLMatchAlgoFactory_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::larocv::MatchAlgoFactory*)0x0)->GetClass();
      larocvcLcLMatchAlgoFactory_TClassManip(theClass);
   return theClass;
   }

   static void larocvcLcLMatchAlgoFactory_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *larocvcLcLReClusterAlgoFactory_Dictionary();
   static void larocvcLcLReClusterAlgoFactory_TClassManip(TClass*);
   static void *new_larocvcLcLReClusterAlgoFactory(void *p = 0);
   static void *newArray_larocvcLcLReClusterAlgoFactory(Long_t size, void *p);
   static void delete_larocvcLcLReClusterAlgoFactory(void *p);
   static void deleteArray_larocvcLcLReClusterAlgoFactory(void *p);
   static void destruct_larocvcLcLReClusterAlgoFactory(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::larocv::ReClusterAlgoFactory*)
   {
      ::larocv::ReClusterAlgoFactory *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::larocv::ReClusterAlgoFactory));
      static ::ROOT::TGenericClassInfo 
         instance("larocv::ReClusterAlgoFactory", "ReClusterAlgoFactory.h", 47,
                  typeid(::larocv::ReClusterAlgoFactory), DefineBehavior(ptr, ptr),
                  &larocvcLcLReClusterAlgoFactory_Dictionary, isa_proxy, 4,
                  sizeof(::larocv::ReClusterAlgoFactory) );
      instance.SetNew(&new_larocvcLcLReClusterAlgoFactory);
      instance.SetNewArray(&newArray_larocvcLcLReClusterAlgoFactory);
      instance.SetDelete(&delete_larocvcLcLReClusterAlgoFactory);
      instance.SetDeleteArray(&deleteArray_larocvcLcLReClusterAlgoFactory);
      instance.SetDestructor(&destruct_larocvcLcLReClusterAlgoFactory);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::larocv::ReClusterAlgoFactory*)
   {
      return GenerateInitInstanceLocal((::larocv::ReClusterAlgoFactory*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::larocv::ReClusterAlgoFactory*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *larocvcLcLReClusterAlgoFactory_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::larocv::ReClusterAlgoFactory*)0x0)->GetClass();
      larocvcLcLReClusterAlgoFactory_TClassManip(theClass);
   return theClass;
   }

   static void larocvcLcLReClusterAlgoFactory_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *larocvcLcLImageClusterManager_Dictionary();
   static void larocvcLcLImageClusterManager_TClassManip(TClass*);
   static void *new_larocvcLcLImageClusterManager(void *p = 0);
   static void *newArray_larocvcLcLImageClusterManager(Long_t size, void *p);
   static void delete_larocvcLcLImageClusterManager(void *p);
   static void deleteArray_larocvcLcLImageClusterManager(void *p);
   static void destruct_larocvcLcLImageClusterManager(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::larocv::ImageClusterManager*)
   {
      ::larocv::ImageClusterManager *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::larocv::ImageClusterManager));
      static ::ROOT::TGenericClassInfo 
         instance("larocv::ImageClusterManager", "ImageClusterManager.h", 39,
                  typeid(::larocv::ImageClusterManager), DefineBehavior(ptr, ptr),
                  &larocvcLcLImageClusterManager_Dictionary, isa_proxy, 4,
                  sizeof(::larocv::ImageClusterManager) );
      instance.SetNew(&new_larocvcLcLImageClusterManager);
      instance.SetNewArray(&newArray_larocvcLcLImageClusterManager);
      instance.SetDelete(&delete_larocvcLcLImageClusterManager);
      instance.SetDeleteArray(&deleteArray_larocvcLcLImageClusterManager);
      instance.SetDestructor(&destruct_larocvcLcLImageClusterManager);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::larocv::ImageClusterManager*)
   {
      return GenerateInitInstanceLocal((::larocv::ImageClusterManager*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::larocv::ImageClusterManager*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *larocvcLcLImageClusterManager_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::larocv::ImageClusterManager*)0x0)->GetClass();
      larocvcLcLImageClusterManager_TClassManip(theClass);
   return theClass;
   }

   static void larocvcLcLImageClusterManager_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *larocvcLcLImageClusterViewer_Dictionary();
   static void larocvcLcLImageClusterViewer_TClassManip(TClass*);
   static void *new_larocvcLcLImageClusterViewer(void *p = 0);
   static void *newArray_larocvcLcLImageClusterViewer(Long_t size, void *p);
   static void delete_larocvcLcLImageClusterViewer(void *p);
   static void deleteArray_larocvcLcLImageClusterViewer(void *p);
   static void destruct_larocvcLcLImageClusterViewer(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::larocv::ImageClusterViewer*)
   {
      ::larocv::ImageClusterViewer *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::larocv::ImageClusterViewer));
      static ::ROOT::TGenericClassInfo 
         instance("larocv::ImageClusterViewer", "ImageClusterViewer.h", 26,
                  typeid(::larocv::ImageClusterViewer), DefineBehavior(ptr, ptr),
                  &larocvcLcLImageClusterViewer_Dictionary, isa_proxy, 4,
                  sizeof(::larocv::ImageClusterViewer) );
      instance.SetNew(&new_larocvcLcLImageClusterViewer);
      instance.SetNewArray(&newArray_larocvcLcLImageClusterViewer);
      instance.SetDelete(&delete_larocvcLcLImageClusterViewer);
      instance.SetDeleteArray(&deleteArray_larocvcLcLImageClusterViewer);
      instance.SetDestructor(&destruct_larocvcLcLImageClusterViewer);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::larocv::ImageClusterViewer*)
   {
      return GenerateInitInstanceLocal((::larocv::ImageClusterViewer*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::larocv::ImageClusterViewer*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *larocvcLcLImageClusterViewer_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::larocv::ImageClusterViewer*)0x0)->GetClass();
      larocvcLcLImageClusterViewer_TClassManip(theClass);
   return theClass;
   }

   static void larocvcLcLImageClusterViewer_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *larocvcLcLCluster2D_Dictionary();
   static void larocvcLcLCluster2D_TClassManip(TClass*);
   static void *new_larocvcLcLCluster2D(void *p = 0);
   static void *newArray_larocvcLcLCluster2D(Long_t size, void *p);
   static void delete_larocvcLcLCluster2D(void *p);
   static void deleteArray_larocvcLcLCluster2D(void *p);
   static void destruct_larocvcLcLCluster2D(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::larocv::Cluster2D*)
   {
      ::larocv::Cluster2D *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::larocv::Cluster2D));
      static ::ROOT::TGenericClassInfo 
         instance("larocv::Cluster2D", "Cluster2D.h", 37,
                  typeid(::larocv::Cluster2D), DefineBehavior(ptr, ptr),
                  &larocvcLcLCluster2D_Dictionary, isa_proxy, 4,
                  sizeof(::larocv::Cluster2D) );
      instance.SetNew(&new_larocvcLcLCluster2D);
      instance.SetNewArray(&newArray_larocvcLcLCluster2D);
      instance.SetDelete(&delete_larocvcLcLCluster2D);
      instance.SetDeleteArray(&deleteArray_larocvcLcLCluster2D);
      instance.SetDestructor(&destruct_larocvcLcLCluster2D);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::larocv::Cluster2D*)
   {
      return GenerateInitInstanceLocal((::larocv::Cluster2D*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::larocv::Cluster2D*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *larocvcLcLCluster2D_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::larocv::Cluster2D*)0x0)->GetClass();
      larocvcLcLCluster2D_TClassManip(theClass);
   return theClass;
   }

   static void larocvcLcLCluster2D_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrapper around operator delete
   static void delete_larocvcLcLImageClusterBase(void *p) {
      delete ((::larocv::ImageClusterBase*)p);
   }
   static void deleteArray_larocvcLcLImageClusterBase(void *p) {
      delete [] ((::larocv::ImageClusterBase*)p);
   }
   static void destruct_larocvcLcLImageClusterBase(void *p) {
      typedef ::larocv::ImageClusterBase current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::larocv::ImageClusterBase

namespace ROOT {
   // Wrapper around operator delete
   static void delete_larocvcLcLClusterAlgoBase(void *p) {
      delete ((::larocv::ClusterAlgoBase*)p);
   }
   static void deleteArray_larocvcLcLClusterAlgoBase(void *p) {
      delete [] ((::larocv::ClusterAlgoBase*)p);
   }
   static void destruct_larocvcLcLClusterAlgoBase(void *p) {
      typedef ::larocv::ClusterAlgoBase current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::larocv::ClusterAlgoBase

namespace ROOT {
   // Wrapper around operator delete
   static void delete_larocvcLcLMatchAlgoBase(void *p) {
      delete ((::larocv::MatchAlgoBase*)p);
   }
   static void deleteArray_larocvcLcLMatchAlgoBase(void *p) {
      delete [] ((::larocv::MatchAlgoBase*)p);
   }
   static void destruct_larocvcLcLMatchAlgoBase(void *p) {
      typedef ::larocv::MatchAlgoBase current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::larocv::MatchAlgoBase

namespace ROOT {
   // Wrapper around operator delete
   static void delete_larocvcLcLReClusterAlgoBase(void *p) {
      delete ((::larocv::ReClusterAlgoBase*)p);
   }
   static void deleteArray_larocvcLcLReClusterAlgoBase(void *p) {
      delete [] ((::larocv::ReClusterAlgoBase*)p);
   }
   static void destruct_larocvcLcLReClusterAlgoBase(void *p) {
      typedef ::larocv::ReClusterAlgoBase current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::larocv::ReClusterAlgoBase

namespace ROOT {
   // Wrappers around operator new
   static void *new_larocvcLcLClusterAlgoFactory(void *p) {
      return  p ? ::new((::ROOT::TOperatorNewHelper*)p) ::larocv::ClusterAlgoFactory : new ::larocv::ClusterAlgoFactory;
   }
   static void *newArray_larocvcLcLClusterAlgoFactory(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::TOperatorNewHelper*)p) ::larocv::ClusterAlgoFactory[nElements] : new ::larocv::ClusterAlgoFactory[nElements];
   }
   // Wrapper around operator delete
   static void delete_larocvcLcLClusterAlgoFactory(void *p) {
      delete ((::larocv::ClusterAlgoFactory*)p);
   }
   static void deleteArray_larocvcLcLClusterAlgoFactory(void *p) {
      delete [] ((::larocv::ClusterAlgoFactory*)p);
   }
   static void destruct_larocvcLcLClusterAlgoFactory(void *p) {
      typedef ::larocv::ClusterAlgoFactory current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::larocv::ClusterAlgoFactory

namespace ROOT {
   // Wrappers around operator new
   static void *new_larocvcLcLMatchAlgoFactory(void *p) {
      return  p ? ::new((::ROOT::TOperatorNewHelper*)p) ::larocv::MatchAlgoFactory : new ::larocv::MatchAlgoFactory;
   }
   static void *newArray_larocvcLcLMatchAlgoFactory(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::TOperatorNewHelper*)p) ::larocv::MatchAlgoFactory[nElements] : new ::larocv::MatchAlgoFactory[nElements];
   }
   // Wrapper around operator delete
   static void delete_larocvcLcLMatchAlgoFactory(void *p) {
      delete ((::larocv::MatchAlgoFactory*)p);
   }
   static void deleteArray_larocvcLcLMatchAlgoFactory(void *p) {
      delete [] ((::larocv::MatchAlgoFactory*)p);
   }
   static void destruct_larocvcLcLMatchAlgoFactory(void *p) {
      typedef ::larocv::MatchAlgoFactory current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::larocv::MatchAlgoFactory

namespace ROOT {
   // Wrappers around operator new
   static void *new_larocvcLcLReClusterAlgoFactory(void *p) {
      return  p ? ::new((::ROOT::TOperatorNewHelper*)p) ::larocv::ReClusterAlgoFactory : new ::larocv::ReClusterAlgoFactory;
   }
   static void *newArray_larocvcLcLReClusterAlgoFactory(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::TOperatorNewHelper*)p) ::larocv::ReClusterAlgoFactory[nElements] : new ::larocv::ReClusterAlgoFactory[nElements];
   }
   // Wrapper around operator delete
   static void delete_larocvcLcLReClusterAlgoFactory(void *p) {
      delete ((::larocv::ReClusterAlgoFactory*)p);
   }
   static void deleteArray_larocvcLcLReClusterAlgoFactory(void *p) {
      delete [] ((::larocv::ReClusterAlgoFactory*)p);
   }
   static void destruct_larocvcLcLReClusterAlgoFactory(void *p) {
      typedef ::larocv::ReClusterAlgoFactory current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::larocv::ReClusterAlgoFactory

namespace ROOT {
   // Wrappers around operator new
   static void *new_larocvcLcLImageClusterManager(void *p) {
      return  p ? ::new((::ROOT::TOperatorNewHelper*)p) ::larocv::ImageClusterManager : new ::larocv::ImageClusterManager;
   }
   static void *newArray_larocvcLcLImageClusterManager(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::TOperatorNewHelper*)p) ::larocv::ImageClusterManager[nElements] : new ::larocv::ImageClusterManager[nElements];
   }
   // Wrapper around operator delete
   static void delete_larocvcLcLImageClusterManager(void *p) {
      delete ((::larocv::ImageClusterManager*)p);
   }
   static void deleteArray_larocvcLcLImageClusterManager(void *p) {
      delete [] ((::larocv::ImageClusterManager*)p);
   }
   static void destruct_larocvcLcLImageClusterManager(void *p) {
      typedef ::larocv::ImageClusterManager current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::larocv::ImageClusterManager

namespace ROOT {
   // Wrappers around operator new
   static void *new_larocvcLcLImageClusterViewer(void *p) {
      return  p ? ::new((::ROOT::TOperatorNewHelper*)p) ::larocv::ImageClusterViewer : new ::larocv::ImageClusterViewer;
   }
   static void *newArray_larocvcLcLImageClusterViewer(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::TOperatorNewHelper*)p) ::larocv::ImageClusterViewer[nElements] : new ::larocv::ImageClusterViewer[nElements];
   }
   // Wrapper around operator delete
   static void delete_larocvcLcLImageClusterViewer(void *p) {
      delete ((::larocv::ImageClusterViewer*)p);
   }
   static void deleteArray_larocvcLcLImageClusterViewer(void *p) {
      delete [] ((::larocv::ImageClusterViewer*)p);
   }
   static void destruct_larocvcLcLImageClusterViewer(void *p) {
      typedef ::larocv::ImageClusterViewer current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::larocv::ImageClusterViewer

namespace ROOT {
   // Wrappers around operator new
   static void *new_larocvcLcLCluster2D(void *p) {
      return  p ? ::new((::ROOT::TOperatorNewHelper*)p) ::larocv::Cluster2D : new ::larocv::Cluster2D;
   }
   static void *newArray_larocvcLcLCluster2D(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::TOperatorNewHelper*)p) ::larocv::Cluster2D[nElements] : new ::larocv::Cluster2D[nElements];
   }
   // Wrapper around operator delete
   static void delete_larocvcLcLCluster2D(void *p) {
      delete ((::larocv::Cluster2D*)p);
   }
   static void deleteArray_larocvcLcLCluster2D(void *p) {
      delete [] ((::larocv::Cluster2D*)p);
   }
   static void destruct_larocvcLcLCluster2D(void *p) {
      typedef ::larocv::Cluster2D current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::larocv::Cluster2D

namespace ROOT {
   static TClass *vectorlEvectorlEvectorlElarocvcLcLCluster2DgRsPgRsPgR_Dictionary();
   static void vectorlEvectorlEvectorlElarocvcLcLCluster2DgRsPgRsPgR_TClassManip(TClass*);
   static void *new_vectorlEvectorlEvectorlElarocvcLcLCluster2DgRsPgRsPgR(void *p = 0);
   static void *newArray_vectorlEvectorlEvectorlElarocvcLcLCluster2DgRsPgRsPgR(Long_t size, void *p);
   static void delete_vectorlEvectorlEvectorlElarocvcLcLCluster2DgRsPgRsPgR(void *p);
   static void deleteArray_vectorlEvectorlEvectorlElarocvcLcLCluster2DgRsPgRsPgR(void *p);
   static void destruct_vectorlEvectorlEvectorlElarocvcLcLCluster2DgRsPgRsPgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<vector<vector<larocv::Cluster2D> > >*)
   {
      vector<vector<vector<larocv::Cluster2D> > > *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<vector<vector<larocv::Cluster2D> > >));
      static ::ROOT::TGenericClassInfo 
         instance("vector<vector<vector<larocv::Cluster2D> > >", -2, "vector", 457,
                  typeid(vector<vector<vector<larocv::Cluster2D> > >), DefineBehavior(ptr, ptr),
                  &vectorlEvectorlEvectorlElarocvcLcLCluster2DgRsPgRsPgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<vector<vector<larocv::Cluster2D> > >) );
      instance.SetNew(&new_vectorlEvectorlEvectorlElarocvcLcLCluster2DgRsPgRsPgR);
      instance.SetNewArray(&newArray_vectorlEvectorlEvectorlElarocvcLcLCluster2DgRsPgRsPgR);
      instance.SetDelete(&delete_vectorlEvectorlEvectorlElarocvcLcLCluster2DgRsPgRsPgR);
      instance.SetDeleteArray(&deleteArray_vectorlEvectorlEvectorlElarocvcLcLCluster2DgRsPgRsPgR);
      instance.SetDestructor(&destruct_vectorlEvectorlEvectorlElarocvcLcLCluster2DgRsPgRsPgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<vector<vector<larocv::Cluster2D> > > >()));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const vector<vector<vector<larocv::Cluster2D> > >*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEvectorlEvectorlElarocvcLcLCluster2DgRsPgRsPgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<vector<vector<larocv::Cluster2D> > >*)0x0)->GetClass();
      vectorlEvectorlEvectorlElarocvcLcLCluster2DgRsPgRsPgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEvectorlEvectorlElarocvcLcLCluster2DgRsPgRsPgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEvectorlEvectorlElarocvcLcLCluster2DgRsPgRsPgR(void *p) {
      return  p ? ::new((::ROOT::TOperatorNewHelper*)p) vector<vector<vector<larocv::Cluster2D> > > : new vector<vector<vector<larocv::Cluster2D> > >;
   }
   static void *newArray_vectorlEvectorlEvectorlElarocvcLcLCluster2DgRsPgRsPgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::TOperatorNewHelper*)p) vector<vector<vector<larocv::Cluster2D> > >[nElements] : new vector<vector<vector<larocv::Cluster2D> > >[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEvectorlEvectorlElarocvcLcLCluster2DgRsPgRsPgR(void *p) {
      delete ((vector<vector<vector<larocv::Cluster2D> > >*)p);
   }
   static void deleteArray_vectorlEvectorlEvectorlElarocvcLcLCluster2DgRsPgRsPgR(void *p) {
      delete [] ((vector<vector<vector<larocv::Cluster2D> > >*)p);
   }
   static void destruct_vectorlEvectorlEvectorlElarocvcLcLCluster2DgRsPgRsPgR(void *p) {
      typedef vector<vector<vector<larocv::Cluster2D> > > current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<vector<vector<larocv::Cluster2D> > >

namespace ROOT {
   static TClass *vectorlEvectorlElarocvcLcLCluster2DgRsPgR_Dictionary();
   static void vectorlEvectorlElarocvcLcLCluster2DgRsPgR_TClassManip(TClass*);
   static void *new_vectorlEvectorlElarocvcLcLCluster2DgRsPgR(void *p = 0);
   static void *newArray_vectorlEvectorlElarocvcLcLCluster2DgRsPgR(Long_t size, void *p);
   static void delete_vectorlEvectorlElarocvcLcLCluster2DgRsPgR(void *p);
   static void deleteArray_vectorlEvectorlElarocvcLcLCluster2DgRsPgR(void *p);
   static void destruct_vectorlEvectorlElarocvcLcLCluster2DgRsPgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<vector<larocv::Cluster2D> >*)
   {
      vector<vector<larocv::Cluster2D> > *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<vector<larocv::Cluster2D> >));
      static ::ROOT::TGenericClassInfo 
         instance("vector<vector<larocv::Cluster2D> >", -2, "vector", 457,
                  typeid(vector<vector<larocv::Cluster2D> >), DefineBehavior(ptr, ptr),
                  &vectorlEvectorlElarocvcLcLCluster2DgRsPgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<vector<larocv::Cluster2D> >) );
      instance.SetNew(&new_vectorlEvectorlElarocvcLcLCluster2DgRsPgR);
      instance.SetNewArray(&newArray_vectorlEvectorlElarocvcLcLCluster2DgRsPgR);
      instance.SetDelete(&delete_vectorlEvectorlElarocvcLcLCluster2DgRsPgR);
      instance.SetDeleteArray(&deleteArray_vectorlEvectorlElarocvcLcLCluster2DgRsPgR);
      instance.SetDestructor(&destruct_vectorlEvectorlElarocvcLcLCluster2DgRsPgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<vector<larocv::Cluster2D> > >()));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const vector<vector<larocv::Cluster2D> >*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEvectorlElarocvcLcLCluster2DgRsPgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<vector<larocv::Cluster2D> >*)0x0)->GetClass();
      vectorlEvectorlElarocvcLcLCluster2DgRsPgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEvectorlElarocvcLcLCluster2DgRsPgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEvectorlElarocvcLcLCluster2DgRsPgR(void *p) {
      return  p ? ::new((::ROOT::TOperatorNewHelper*)p) vector<vector<larocv::Cluster2D> > : new vector<vector<larocv::Cluster2D> >;
   }
   static void *newArray_vectorlEvectorlElarocvcLcLCluster2DgRsPgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::TOperatorNewHelper*)p) vector<vector<larocv::Cluster2D> >[nElements] : new vector<vector<larocv::Cluster2D> >[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEvectorlElarocvcLcLCluster2DgRsPgR(void *p) {
      delete ((vector<vector<larocv::Cluster2D> >*)p);
   }
   static void deleteArray_vectorlEvectorlElarocvcLcLCluster2DgRsPgR(void *p) {
      delete [] ((vector<vector<larocv::Cluster2D> >*)p);
   }
   static void destruct_vectorlEvectorlElarocvcLcLCluster2DgRsPgR(void *p) {
      typedef vector<vector<larocv::Cluster2D> > current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<vector<larocv::Cluster2D> >

namespace ROOT {
   static TClass *vectorlEvectorlElarcaffecLcLImageMetagRsPgR_Dictionary();
   static void vectorlEvectorlElarcaffecLcLImageMetagRsPgR_TClassManip(TClass*);
   static void *new_vectorlEvectorlElarcaffecLcLImageMetagRsPgR(void *p = 0);
   static void *newArray_vectorlEvectorlElarcaffecLcLImageMetagRsPgR(Long_t size, void *p);
   static void delete_vectorlEvectorlElarcaffecLcLImageMetagRsPgR(void *p);
   static void deleteArray_vectorlEvectorlElarcaffecLcLImageMetagRsPgR(void *p);
   static void destruct_vectorlEvectorlElarcaffecLcLImageMetagRsPgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<vector<larcaffe::ImageMeta> >*)
   {
      vector<vector<larcaffe::ImageMeta> > *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<vector<larcaffe::ImageMeta> >));
      static ::ROOT::TGenericClassInfo 
         instance("vector<vector<larcaffe::ImageMeta> >", -2, "vector", 457,
                  typeid(vector<vector<larcaffe::ImageMeta> >), DefineBehavior(ptr, ptr),
                  &vectorlEvectorlElarcaffecLcLImageMetagRsPgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<vector<larcaffe::ImageMeta> >) );
      instance.SetNew(&new_vectorlEvectorlElarcaffecLcLImageMetagRsPgR);
      instance.SetNewArray(&newArray_vectorlEvectorlElarcaffecLcLImageMetagRsPgR);
      instance.SetDelete(&delete_vectorlEvectorlElarcaffecLcLImageMetagRsPgR);
      instance.SetDeleteArray(&deleteArray_vectorlEvectorlElarcaffecLcLImageMetagRsPgR);
      instance.SetDestructor(&destruct_vectorlEvectorlElarcaffecLcLImageMetagRsPgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<vector<larcaffe::ImageMeta> > >()));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const vector<vector<larcaffe::ImageMeta> >*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEvectorlElarcaffecLcLImageMetagRsPgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<vector<larcaffe::ImageMeta> >*)0x0)->GetClass();
      vectorlEvectorlElarcaffecLcLImageMetagRsPgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEvectorlElarcaffecLcLImageMetagRsPgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEvectorlElarcaffecLcLImageMetagRsPgR(void *p) {
      return  p ? ::new((::ROOT::TOperatorNewHelper*)p) vector<vector<larcaffe::ImageMeta> > : new vector<vector<larcaffe::ImageMeta> >;
   }
   static void *newArray_vectorlEvectorlElarcaffecLcLImageMetagRsPgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::TOperatorNewHelper*)p) vector<vector<larcaffe::ImageMeta> >[nElements] : new vector<vector<larcaffe::ImageMeta> >[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEvectorlElarcaffecLcLImageMetagRsPgR(void *p) {
      delete ((vector<vector<larcaffe::ImageMeta> >*)p);
   }
   static void deleteArray_vectorlEvectorlElarcaffecLcLImageMetagRsPgR(void *p) {
      delete [] ((vector<vector<larcaffe::ImageMeta> >*)p);
   }
   static void destruct_vectorlEvectorlElarcaffecLcLImageMetagRsPgR(void *p) {
      typedef vector<vector<larcaffe::ImageMeta> > current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<vector<larcaffe::ImageMeta> >

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
                  &vectorlEvectorlEcvcLcLPoint_lEintgRsPgRsPgR_Dictionary, isa_proxy, 4,
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
   static TClass *vectorlEvectorlEcvcLcLPoint_lEfloatgRsPgRsPgR_Dictionary();
   static void vectorlEvectorlEcvcLcLPoint_lEfloatgRsPgRsPgR_TClassManip(TClass*);
   static void *new_vectorlEvectorlEcvcLcLPoint_lEfloatgRsPgRsPgR(void *p = 0);
   static void *newArray_vectorlEvectorlEcvcLcLPoint_lEfloatgRsPgRsPgR(Long_t size, void *p);
   static void delete_vectorlEvectorlEcvcLcLPoint_lEfloatgRsPgRsPgR(void *p);
   static void deleteArray_vectorlEvectorlEcvcLcLPoint_lEfloatgRsPgRsPgR(void *p);
   static void destruct_vectorlEvectorlEcvcLcLPoint_lEfloatgRsPgRsPgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<vector<cv::Point_<float> > >*)
   {
      vector<vector<cv::Point_<float> > > *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<vector<cv::Point_<float> > >));
      static ::ROOT::TGenericClassInfo 
         instance("vector<vector<cv::Point_<float> > >", -2, "vector", 457,
                  typeid(vector<vector<cv::Point_<float> > >), DefineBehavior(ptr, ptr),
                  &vectorlEvectorlEcvcLcLPoint_lEfloatgRsPgRsPgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<vector<cv::Point_<float> > >) );
      instance.SetNew(&new_vectorlEvectorlEcvcLcLPoint_lEfloatgRsPgRsPgR);
      instance.SetNewArray(&newArray_vectorlEvectorlEcvcLcLPoint_lEfloatgRsPgRsPgR);
      instance.SetDelete(&delete_vectorlEvectorlEcvcLcLPoint_lEfloatgRsPgRsPgR);
      instance.SetDeleteArray(&deleteArray_vectorlEvectorlEcvcLcLPoint_lEfloatgRsPgRsPgR);
      instance.SetDestructor(&destruct_vectorlEvectorlEcvcLcLPoint_lEfloatgRsPgRsPgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<vector<cv::Point_<float> > > >()));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const vector<vector<cv::Point_<float> > >*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEvectorlEcvcLcLPoint_lEfloatgRsPgRsPgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<vector<cv::Point_<float> > >*)0x0)->GetClass();
      vectorlEvectorlEcvcLcLPoint_lEfloatgRsPgRsPgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEvectorlEcvcLcLPoint_lEfloatgRsPgRsPgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEvectorlEcvcLcLPoint_lEfloatgRsPgRsPgR(void *p) {
      return  p ? ::new((::ROOT::TOperatorNewHelper*)p) vector<vector<cv::Point_<float> > > : new vector<vector<cv::Point_<float> > >;
   }
   static void *newArray_vectorlEvectorlEcvcLcLPoint_lEfloatgRsPgRsPgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::TOperatorNewHelper*)p) vector<vector<cv::Point_<float> > >[nElements] : new vector<vector<cv::Point_<float> > >[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEvectorlEcvcLcLPoint_lEfloatgRsPgRsPgR(void *p) {
      delete ((vector<vector<cv::Point_<float> > >*)p);
   }
   static void deleteArray_vectorlEvectorlEcvcLcLPoint_lEfloatgRsPgRsPgR(void *p) {
      delete [] ((vector<vector<cv::Point_<float> > >*)p);
   }
   static void destruct_vectorlEvectorlEcvcLcLPoint_lEfloatgRsPgRsPgR(void *p) {
      typedef vector<vector<cv::Point_<float> > > current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<vector<cv::Point_<float> > >

namespace ROOT {
   static TClass *vectorlEunsignedsPlonggR_Dictionary();
   static void vectorlEunsignedsPlonggR_TClassManip(TClass*);
   static void *new_vectorlEunsignedsPlonggR(void *p = 0);
   static void *newArray_vectorlEunsignedsPlonggR(Long_t size, void *p);
   static void delete_vectorlEunsignedsPlonggR(void *p);
   static void deleteArray_vectorlEunsignedsPlonggR(void *p);
   static void destruct_vectorlEunsignedsPlonggR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<unsigned long>*)
   {
      vector<unsigned long> *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<unsigned long>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<unsigned long>", -2, "vector", 457,
                  typeid(vector<unsigned long>), DefineBehavior(ptr, ptr),
                  &vectorlEunsignedsPlonggR_Dictionary, isa_proxy, 0,
                  sizeof(vector<unsigned long>) );
      instance.SetNew(&new_vectorlEunsignedsPlonggR);
      instance.SetNewArray(&newArray_vectorlEunsignedsPlonggR);
      instance.SetDelete(&delete_vectorlEunsignedsPlonggR);
      instance.SetDeleteArray(&deleteArray_vectorlEunsignedsPlonggR);
      instance.SetDestructor(&destruct_vectorlEunsignedsPlonggR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<unsigned long> >()));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const vector<unsigned long>*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEunsignedsPlonggR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<unsigned long>*)0x0)->GetClass();
      vectorlEunsignedsPlonggR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEunsignedsPlonggR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEunsignedsPlonggR(void *p) {
      return  p ? ::new((::ROOT::TOperatorNewHelper*)p) vector<unsigned long> : new vector<unsigned long>;
   }
   static void *newArray_vectorlEunsignedsPlonggR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::TOperatorNewHelper*)p) vector<unsigned long>[nElements] : new vector<unsigned long>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEunsignedsPlonggR(void *p) {
      delete ((vector<unsigned long>*)p);
   }
   static void deleteArray_vectorlEunsignedsPlonggR(void *p) {
      delete [] ((vector<unsigned long>*)p);
   }
   static void destruct_vectorlEunsignedsPlonggR(void *p) {
      typedef vector<unsigned long> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<unsigned long>

namespace ROOT {
   static TClass *vectorlElarocvcLcLClusterAlgoBasemUgR_Dictionary();
   static void vectorlElarocvcLcLClusterAlgoBasemUgR_TClassManip(TClass*);
   static void *new_vectorlElarocvcLcLClusterAlgoBasemUgR(void *p = 0);
   static void *newArray_vectorlElarocvcLcLClusterAlgoBasemUgR(Long_t size, void *p);
   static void delete_vectorlElarocvcLcLClusterAlgoBasemUgR(void *p);
   static void deleteArray_vectorlElarocvcLcLClusterAlgoBasemUgR(void *p);
   static void destruct_vectorlElarocvcLcLClusterAlgoBasemUgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<larocv::ClusterAlgoBase*>*)
   {
      vector<larocv::ClusterAlgoBase*> *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<larocv::ClusterAlgoBase*>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<larocv::ClusterAlgoBase*>", -2, "vector", 457,
                  typeid(vector<larocv::ClusterAlgoBase*>), DefineBehavior(ptr, ptr),
                  &vectorlElarocvcLcLClusterAlgoBasemUgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<larocv::ClusterAlgoBase*>) );
      instance.SetNew(&new_vectorlElarocvcLcLClusterAlgoBasemUgR);
      instance.SetNewArray(&newArray_vectorlElarocvcLcLClusterAlgoBasemUgR);
      instance.SetDelete(&delete_vectorlElarocvcLcLClusterAlgoBasemUgR);
      instance.SetDeleteArray(&deleteArray_vectorlElarocvcLcLClusterAlgoBasemUgR);
      instance.SetDestructor(&destruct_vectorlElarocvcLcLClusterAlgoBasemUgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<larocv::ClusterAlgoBase*> >()));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const vector<larocv::ClusterAlgoBase*>*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlElarocvcLcLClusterAlgoBasemUgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<larocv::ClusterAlgoBase*>*)0x0)->GetClass();
      vectorlElarocvcLcLClusterAlgoBasemUgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlElarocvcLcLClusterAlgoBasemUgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlElarocvcLcLClusterAlgoBasemUgR(void *p) {
      return  p ? ::new((::ROOT::TOperatorNewHelper*)p) vector<larocv::ClusterAlgoBase*> : new vector<larocv::ClusterAlgoBase*>;
   }
   static void *newArray_vectorlElarocvcLcLClusterAlgoBasemUgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::TOperatorNewHelper*)p) vector<larocv::ClusterAlgoBase*>[nElements] : new vector<larocv::ClusterAlgoBase*>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlElarocvcLcLClusterAlgoBasemUgR(void *p) {
      delete ((vector<larocv::ClusterAlgoBase*>*)p);
   }
   static void deleteArray_vectorlElarocvcLcLClusterAlgoBasemUgR(void *p) {
      delete [] ((vector<larocv::ClusterAlgoBase*>*)p);
   }
   static void destruct_vectorlElarocvcLcLClusterAlgoBasemUgR(void *p) {
      typedef vector<larocv::ClusterAlgoBase*> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<larocv::ClusterAlgoBase*>

namespace ROOT {
   static TClass *vectorlElarocvcLcLCluster2DgR_Dictionary();
   static void vectorlElarocvcLcLCluster2DgR_TClassManip(TClass*);
   static void *new_vectorlElarocvcLcLCluster2DgR(void *p = 0);
   static void *newArray_vectorlElarocvcLcLCluster2DgR(Long_t size, void *p);
   static void delete_vectorlElarocvcLcLCluster2DgR(void *p);
   static void deleteArray_vectorlElarocvcLcLCluster2DgR(void *p);
   static void destruct_vectorlElarocvcLcLCluster2DgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<larocv::Cluster2D>*)
   {
      vector<larocv::Cluster2D> *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<larocv::Cluster2D>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<larocv::Cluster2D>", -2, "vector", 457,
                  typeid(vector<larocv::Cluster2D>), DefineBehavior(ptr, ptr),
                  &vectorlElarocvcLcLCluster2DgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<larocv::Cluster2D>) );
      instance.SetNew(&new_vectorlElarocvcLcLCluster2DgR);
      instance.SetNewArray(&newArray_vectorlElarocvcLcLCluster2DgR);
      instance.SetDelete(&delete_vectorlElarocvcLcLCluster2DgR);
      instance.SetDeleteArray(&deleteArray_vectorlElarocvcLcLCluster2DgR);
      instance.SetDestructor(&destruct_vectorlElarocvcLcLCluster2DgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<larocv::Cluster2D> >()));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const vector<larocv::Cluster2D>*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlElarocvcLcLCluster2DgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<larocv::Cluster2D>*)0x0)->GetClass();
      vectorlElarocvcLcLCluster2DgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlElarocvcLcLCluster2DgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlElarocvcLcLCluster2DgR(void *p) {
      return  p ? ::new((::ROOT::TOperatorNewHelper*)p) vector<larocv::Cluster2D> : new vector<larocv::Cluster2D>;
   }
   static void *newArray_vectorlElarocvcLcLCluster2DgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::TOperatorNewHelper*)p) vector<larocv::Cluster2D>[nElements] : new vector<larocv::Cluster2D>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlElarocvcLcLCluster2DgR(void *p) {
      delete ((vector<larocv::Cluster2D>*)p);
   }
   static void deleteArray_vectorlElarocvcLcLCluster2DgR(void *p) {
      delete [] ((vector<larocv::Cluster2D>*)p);
   }
   static void destruct_vectorlElarocvcLcLCluster2DgR(void *p) {
      typedef vector<larocv::Cluster2D> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<larocv::Cluster2D>

namespace ROOT {
   static TClass *vectorlElarcaffecLcLImageMetagR_Dictionary();
   static void vectorlElarcaffecLcLImageMetagR_TClassManip(TClass*);
   static void *new_vectorlElarcaffecLcLImageMetagR(void *p = 0);
   static void *newArray_vectorlElarcaffecLcLImageMetagR(Long_t size, void *p);
   static void delete_vectorlElarcaffecLcLImageMetagR(void *p);
   static void deleteArray_vectorlElarcaffecLcLImageMetagR(void *p);
   static void destruct_vectorlElarcaffecLcLImageMetagR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<larcaffe::ImageMeta>*)
   {
      vector<larcaffe::ImageMeta> *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<larcaffe::ImageMeta>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<larcaffe::ImageMeta>", -2, "vector", 457,
                  typeid(vector<larcaffe::ImageMeta>), DefineBehavior(ptr, ptr),
                  &vectorlElarcaffecLcLImageMetagR_Dictionary, isa_proxy, 0,
                  sizeof(vector<larcaffe::ImageMeta>) );
      instance.SetNew(&new_vectorlElarcaffecLcLImageMetagR);
      instance.SetNewArray(&newArray_vectorlElarcaffecLcLImageMetagR);
      instance.SetDelete(&delete_vectorlElarcaffecLcLImageMetagR);
      instance.SetDeleteArray(&deleteArray_vectorlElarcaffecLcLImageMetagR);
      instance.SetDestructor(&destruct_vectorlElarcaffecLcLImageMetagR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<larcaffe::ImageMeta> >()));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const vector<larcaffe::ImageMeta>*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlElarcaffecLcLImageMetagR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<larcaffe::ImageMeta>*)0x0)->GetClass();
      vectorlElarcaffecLcLImageMetagR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlElarcaffecLcLImageMetagR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlElarcaffecLcLImageMetagR(void *p) {
      return  p ? ::new((::ROOT::TOperatorNewHelper*)p) vector<larcaffe::ImageMeta> : new vector<larcaffe::ImageMeta>;
   }
   static void *newArray_vectorlElarcaffecLcLImageMetagR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::TOperatorNewHelper*)p) vector<larcaffe::ImageMeta>[nElements] : new vector<larcaffe::ImageMeta>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlElarcaffecLcLImageMetagR(void *p) {
      delete ((vector<larcaffe::ImageMeta>*)p);
   }
   static void deleteArray_vectorlElarcaffecLcLImageMetagR(void *p) {
      delete [] ((vector<larcaffe::ImageMeta>*)p);
   }
   static void destruct_vectorlElarcaffecLcLImageMetagR(void *p) {
      typedef vector<larcaffe::ImageMeta> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<larcaffe::ImageMeta>

namespace ROOT {
   static TClass *vectorlEdoublegR_Dictionary();
   static void vectorlEdoublegR_TClassManip(TClass*);
   static void *new_vectorlEdoublegR(void *p = 0);
   static void *newArray_vectorlEdoublegR(Long_t size, void *p);
   static void delete_vectorlEdoublegR(void *p);
   static void deleteArray_vectorlEdoublegR(void *p);
   static void destruct_vectorlEdoublegR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<double>*)
   {
      vector<double> *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<double>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<double>", -2, "vector", 457,
                  typeid(vector<double>), DefineBehavior(ptr, ptr),
                  &vectorlEdoublegR_Dictionary, isa_proxy, 0,
                  sizeof(vector<double>) );
      instance.SetNew(&new_vectorlEdoublegR);
      instance.SetNewArray(&newArray_vectorlEdoublegR);
      instance.SetDelete(&delete_vectorlEdoublegR);
      instance.SetDeleteArray(&deleteArray_vectorlEdoublegR);
      instance.SetDestructor(&destruct_vectorlEdoublegR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<double> >()));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const vector<double>*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEdoublegR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<double>*)0x0)->GetClass();
      vectorlEdoublegR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEdoublegR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEdoublegR(void *p) {
      return  p ? ::new((::ROOT::TOperatorNewHelper*)p) vector<double> : new vector<double>;
   }
   static void *newArray_vectorlEdoublegR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::TOperatorNewHelper*)p) vector<double>[nElements] : new vector<double>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEdoublegR(void *p) {
      delete ((vector<double>*)p);
   }
   static void deleteArray_vectorlEdoublegR(void *p) {
      delete [] ((vector<double>*)p);
   }
   static void destruct_vectorlEdoublegR(void *p) {
      typedef vector<double> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<double>

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
                  &vectorlEcvcLcLPoint_lEintgRsPgR_Dictionary, isa_proxy, 4,
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
   static TClass *vectorlEcvcLcLPoint_lEfloatgRsPgR_Dictionary();
   static void vectorlEcvcLcLPoint_lEfloatgRsPgR_TClassManip(TClass*);
   static void *new_vectorlEcvcLcLPoint_lEfloatgRsPgR(void *p = 0);
   static void *newArray_vectorlEcvcLcLPoint_lEfloatgRsPgR(Long_t size, void *p);
   static void delete_vectorlEcvcLcLPoint_lEfloatgRsPgR(void *p);
   static void deleteArray_vectorlEcvcLcLPoint_lEfloatgRsPgR(void *p);
   static void destruct_vectorlEcvcLcLPoint_lEfloatgRsPgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<cv::Point_<float> >*)
   {
      vector<cv::Point_<float> > *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<cv::Point_<float> >));
      static ::ROOT::TGenericClassInfo 
         instance("vector<cv::Point_<float> >", -2, "vector", 457,
                  typeid(vector<cv::Point_<float> >), DefineBehavior(ptr, ptr),
                  &vectorlEcvcLcLPoint_lEfloatgRsPgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<cv::Point_<float> >) );
      instance.SetNew(&new_vectorlEcvcLcLPoint_lEfloatgRsPgR);
      instance.SetNewArray(&newArray_vectorlEcvcLcLPoint_lEfloatgRsPgR);
      instance.SetDelete(&delete_vectorlEcvcLcLPoint_lEfloatgRsPgR);
      instance.SetDeleteArray(&deleteArray_vectorlEcvcLcLPoint_lEfloatgRsPgR);
      instance.SetDestructor(&destruct_vectorlEcvcLcLPoint_lEfloatgRsPgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<cv::Point_<float> > >()));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const vector<cv::Point_<float> >*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEcvcLcLPoint_lEfloatgRsPgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<cv::Point_<float> >*)0x0)->GetClass();
      vectorlEcvcLcLPoint_lEfloatgRsPgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEcvcLcLPoint_lEfloatgRsPgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEcvcLcLPoint_lEfloatgRsPgR(void *p) {
      return  p ? ::new((::ROOT::TOperatorNewHelper*)p) vector<cv::Point_<float> > : new vector<cv::Point_<float> >;
   }
   static void *newArray_vectorlEcvcLcLPoint_lEfloatgRsPgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::TOperatorNewHelper*)p) vector<cv::Point_<float> >[nElements] : new vector<cv::Point_<float> >[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEcvcLcLPoint_lEfloatgRsPgR(void *p) {
      delete ((vector<cv::Point_<float> >*)p);
   }
   static void deleteArray_vectorlEcvcLcLPoint_lEfloatgRsPgR(void *p) {
      delete [] ((vector<cv::Point_<float> >*)p);
   }
   static void destruct_vectorlEcvcLcLPoint_lEfloatgRsPgR(void *p) {
      typedef vector<cv::Point_<float> > current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<cv::Point_<float> >

namespace ROOT {
   static TClass *vectorlEcvcLcLMatgR_Dictionary();
   static void vectorlEcvcLcLMatgR_TClassManip(TClass*);
   static void *new_vectorlEcvcLcLMatgR(void *p = 0);
   static void *newArray_vectorlEcvcLcLMatgR(Long_t size, void *p);
   static void delete_vectorlEcvcLcLMatgR(void *p);
   static void deleteArray_vectorlEcvcLcLMatgR(void *p);
   static void destruct_vectorlEcvcLcLMatgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<cv::Mat>*)
   {
      vector<cv::Mat> *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<cv::Mat>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<cv::Mat>", -2, "vector", 457,
                  typeid(vector<cv::Mat>), DefineBehavior(ptr, ptr),
                  &vectorlEcvcLcLMatgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<cv::Mat>) );
      instance.SetNew(&new_vectorlEcvcLcLMatgR);
      instance.SetNewArray(&newArray_vectorlEcvcLcLMatgR);
      instance.SetDelete(&delete_vectorlEcvcLcLMatgR);
      instance.SetDeleteArray(&deleteArray_vectorlEcvcLcLMatgR);
      instance.SetDestructor(&destruct_vectorlEcvcLcLMatgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<cv::Mat> >()));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const vector<cv::Mat>*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEcvcLcLMatgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<cv::Mat>*)0x0)->GetClass();
      vectorlEcvcLcLMatgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEcvcLcLMatgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEcvcLcLMatgR(void *p) {
      return  p ? ::new((::ROOT::TOperatorNewHelper*)p) vector<cv::Mat> : new vector<cv::Mat>;
   }
   static void *newArray_vectorlEcvcLcLMatgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::TOperatorNewHelper*)p) vector<cv::Mat>[nElements] : new vector<cv::Mat>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEcvcLcLMatgR(void *p) {
      delete ((vector<cv::Mat>*)p);
   }
   static void deleteArray_vectorlEcvcLcLMatgR(void *p) {
      delete [] ((vector<cv::Mat>*)p);
   }
   static void destruct_vectorlEcvcLcLMatgR(void *p) {
      typedef vector<cv::Mat> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<cv::Mat>

namespace ROOT {
   static TClass *maplEstringcOunsignedsPlonggR_Dictionary();
   static void maplEstringcOunsignedsPlonggR_TClassManip(TClass*);
   static void *new_maplEstringcOunsignedsPlonggR(void *p = 0);
   static void *newArray_maplEstringcOunsignedsPlonggR(Long_t size, void *p);
   static void delete_maplEstringcOunsignedsPlonggR(void *p);
   static void deleteArray_maplEstringcOunsignedsPlonggR(void *p);
   static void destruct_maplEstringcOunsignedsPlonggR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const map<string,unsigned long>*)
   {
      map<string,unsigned long> *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(map<string,unsigned long>));
      static ::ROOT::TGenericClassInfo 
         instance("map<string,unsigned long>", -2, "map", 830,
                  typeid(map<string,unsigned long>), DefineBehavior(ptr, ptr),
                  &maplEstringcOunsignedsPlonggR_Dictionary, isa_proxy, 0,
                  sizeof(map<string,unsigned long>) );
      instance.SetNew(&new_maplEstringcOunsignedsPlonggR);
      instance.SetNewArray(&newArray_maplEstringcOunsignedsPlonggR);
      instance.SetDelete(&delete_maplEstringcOunsignedsPlonggR);
      instance.SetDeleteArray(&deleteArray_maplEstringcOunsignedsPlonggR);
      instance.SetDestructor(&destruct_maplEstringcOunsignedsPlonggR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::MapInsert< map<string,unsigned long> >()));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const map<string,unsigned long>*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *maplEstringcOunsignedsPlonggR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const map<string,unsigned long>*)0x0)->GetClass();
      maplEstringcOunsignedsPlonggR_TClassManip(theClass);
   return theClass;
   }

   static void maplEstringcOunsignedsPlonggR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_maplEstringcOunsignedsPlonggR(void *p) {
      return  p ? ::new((::ROOT::TOperatorNewHelper*)p) map<string,unsigned long> : new map<string,unsigned long>;
   }
   static void *newArray_maplEstringcOunsignedsPlonggR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::TOperatorNewHelper*)p) map<string,unsigned long>[nElements] : new map<string,unsigned long>[nElements];
   }
   // Wrapper around operator delete
   static void delete_maplEstringcOunsignedsPlonggR(void *p) {
      delete ((map<string,unsigned long>*)p);
   }
   static void deleteArray_maplEstringcOunsignedsPlonggR(void *p) {
      delete [] ((map<string,unsigned long>*)p);
   }
   static void destruct_maplEstringcOunsignedsPlonggR(void *p) {
      typedef map<string,unsigned long> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class map<string,unsigned long>

namespace ROOT {
   static TClass *maplEstringcOlarocvcLcLReClusterAlgoFactoryBasemUgR_Dictionary();
   static void maplEstringcOlarocvcLcLReClusterAlgoFactoryBasemUgR_TClassManip(TClass*);
   static void *new_maplEstringcOlarocvcLcLReClusterAlgoFactoryBasemUgR(void *p = 0);
   static void *newArray_maplEstringcOlarocvcLcLReClusterAlgoFactoryBasemUgR(Long_t size, void *p);
   static void delete_maplEstringcOlarocvcLcLReClusterAlgoFactoryBasemUgR(void *p);
   static void deleteArray_maplEstringcOlarocvcLcLReClusterAlgoFactoryBasemUgR(void *p);
   static void destruct_maplEstringcOlarocvcLcLReClusterAlgoFactoryBasemUgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const map<string,larocv::ReClusterAlgoFactoryBase*>*)
   {
      map<string,larocv::ReClusterAlgoFactoryBase*> *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(map<string,larocv::ReClusterAlgoFactoryBase*>));
      static ::ROOT::TGenericClassInfo 
         instance("map<string,larocv::ReClusterAlgoFactoryBase*>", -2, "map", 830,
                  typeid(map<string,larocv::ReClusterAlgoFactoryBase*>), DefineBehavior(ptr, ptr),
                  &maplEstringcOlarocvcLcLReClusterAlgoFactoryBasemUgR_Dictionary, isa_proxy, 0,
                  sizeof(map<string,larocv::ReClusterAlgoFactoryBase*>) );
      instance.SetNew(&new_maplEstringcOlarocvcLcLReClusterAlgoFactoryBasemUgR);
      instance.SetNewArray(&newArray_maplEstringcOlarocvcLcLReClusterAlgoFactoryBasemUgR);
      instance.SetDelete(&delete_maplEstringcOlarocvcLcLReClusterAlgoFactoryBasemUgR);
      instance.SetDeleteArray(&deleteArray_maplEstringcOlarocvcLcLReClusterAlgoFactoryBasemUgR);
      instance.SetDestructor(&destruct_maplEstringcOlarocvcLcLReClusterAlgoFactoryBasemUgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::MapInsert< map<string,larocv::ReClusterAlgoFactoryBase*> >()));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const map<string,larocv::ReClusterAlgoFactoryBase*>*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *maplEstringcOlarocvcLcLReClusterAlgoFactoryBasemUgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const map<string,larocv::ReClusterAlgoFactoryBase*>*)0x0)->GetClass();
      maplEstringcOlarocvcLcLReClusterAlgoFactoryBasemUgR_TClassManip(theClass);
   return theClass;
   }

   static void maplEstringcOlarocvcLcLReClusterAlgoFactoryBasemUgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_maplEstringcOlarocvcLcLReClusterAlgoFactoryBasemUgR(void *p) {
      return  p ? ::new((::ROOT::TOperatorNewHelper*)p) map<string,larocv::ReClusterAlgoFactoryBase*> : new map<string,larocv::ReClusterAlgoFactoryBase*>;
   }
   static void *newArray_maplEstringcOlarocvcLcLReClusterAlgoFactoryBasemUgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::TOperatorNewHelper*)p) map<string,larocv::ReClusterAlgoFactoryBase*>[nElements] : new map<string,larocv::ReClusterAlgoFactoryBase*>[nElements];
   }
   // Wrapper around operator delete
   static void delete_maplEstringcOlarocvcLcLReClusterAlgoFactoryBasemUgR(void *p) {
      delete ((map<string,larocv::ReClusterAlgoFactoryBase*>*)p);
   }
   static void deleteArray_maplEstringcOlarocvcLcLReClusterAlgoFactoryBasemUgR(void *p) {
      delete [] ((map<string,larocv::ReClusterAlgoFactoryBase*>*)p);
   }
   static void destruct_maplEstringcOlarocvcLcLReClusterAlgoFactoryBasemUgR(void *p) {
      typedef map<string,larocv::ReClusterAlgoFactoryBase*> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class map<string,larocv::ReClusterAlgoFactoryBase*>

namespace ROOT {
   static TClass *maplEstringcOlarocvcLcLMatchAlgoFactoryBasemUgR_Dictionary();
   static void maplEstringcOlarocvcLcLMatchAlgoFactoryBasemUgR_TClassManip(TClass*);
   static void *new_maplEstringcOlarocvcLcLMatchAlgoFactoryBasemUgR(void *p = 0);
   static void *newArray_maplEstringcOlarocvcLcLMatchAlgoFactoryBasemUgR(Long_t size, void *p);
   static void delete_maplEstringcOlarocvcLcLMatchAlgoFactoryBasemUgR(void *p);
   static void deleteArray_maplEstringcOlarocvcLcLMatchAlgoFactoryBasemUgR(void *p);
   static void destruct_maplEstringcOlarocvcLcLMatchAlgoFactoryBasemUgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const map<string,larocv::MatchAlgoFactoryBase*>*)
   {
      map<string,larocv::MatchAlgoFactoryBase*> *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(map<string,larocv::MatchAlgoFactoryBase*>));
      static ::ROOT::TGenericClassInfo 
         instance("map<string,larocv::MatchAlgoFactoryBase*>", -2, "map", 830,
                  typeid(map<string,larocv::MatchAlgoFactoryBase*>), DefineBehavior(ptr, ptr),
                  &maplEstringcOlarocvcLcLMatchAlgoFactoryBasemUgR_Dictionary, isa_proxy, 0,
                  sizeof(map<string,larocv::MatchAlgoFactoryBase*>) );
      instance.SetNew(&new_maplEstringcOlarocvcLcLMatchAlgoFactoryBasemUgR);
      instance.SetNewArray(&newArray_maplEstringcOlarocvcLcLMatchAlgoFactoryBasemUgR);
      instance.SetDelete(&delete_maplEstringcOlarocvcLcLMatchAlgoFactoryBasemUgR);
      instance.SetDeleteArray(&deleteArray_maplEstringcOlarocvcLcLMatchAlgoFactoryBasemUgR);
      instance.SetDestructor(&destruct_maplEstringcOlarocvcLcLMatchAlgoFactoryBasemUgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::MapInsert< map<string,larocv::MatchAlgoFactoryBase*> >()));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const map<string,larocv::MatchAlgoFactoryBase*>*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *maplEstringcOlarocvcLcLMatchAlgoFactoryBasemUgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const map<string,larocv::MatchAlgoFactoryBase*>*)0x0)->GetClass();
      maplEstringcOlarocvcLcLMatchAlgoFactoryBasemUgR_TClassManip(theClass);
   return theClass;
   }

   static void maplEstringcOlarocvcLcLMatchAlgoFactoryBasemUgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_maplEstringcOlarocvcLcLMatchAlgoFactoryBasemUgR(void *p) {
      return  p ? ::new((::ROOT::TOperatorNewHelper*)p) map<string,larocv::MatchAlgoFactoryBase*> : new map<string,larocv::MatchAlgoFactoryBase*>;
   }
   static void *newArray_maplEstringcOlarocvcLcLMatchAlgoFactoryBasemUgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::TOperatorNewHelper*)p) map<string,larocv::MatchAlgoFactoryBase*>[nElements] : new map<string,larocv::MatchAlgoFactoryBase*>[nElements];
   }
   // Wrapper around operator delete
   static void delete_maplEstringcOlarocvcLcLMatchAlgoFactoryBasemUgR(void *p) {
      delete ((map<string,larocv::MatchAlgoFactoryBase*>*)p);
   }
   static void deleteArray_maplEstringcOlarocvcLcLMatchAlgoFactoryBasemUgR(void *p) {
      delete [] ((map<string,larocv::MatchAlgoFactoryBase*>*)p);
   }
   static void destruct_maplEstringcOlarocvcLcLMatchAlgoFactoryBasemUgR(void *p) {
      typedef map<string,larocv::MatchAlgoFactoryBase*> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class map<string,larocv::MatchAlgoFactoryBase*>

namespace ROOT {
   static TClass *maplEstringcOlarocvcLcLClusterAlgoFactoryBasemUgR_Dictionary();
   static void maplEstringcOlarocvcLcLClusterAlgoFactoryBasemUgR_TClassManip(TClass*);
   static void *new_maplEstringcOlarocvcLcLClusterAlgoFactoryBasemUgR(void *p = 0);
   static void *newArray_maplEstringcOlarocvcLcLClusterAlgoFactoryBasemUgR(Long_t size, void *p);
   static void delete_maplEstringcOlarocvcLcLClusterAlgoFactoryBasemUgR(void *p);
   static void deleteArray_maplEstringcOlarocvcLcLClusterAlgoFactoryBasemUgR(void *p);
   static void destruct_maplEstringcOlarocvcLcLClusterAlgoFactoryBasemUgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const map<string,larocv::ClusterAlgoFactoryBase*>*)
   {
      map<string,larocv::ClusterAlgoFactoryBase*> *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(map<string,larocv::ClusterAlgoFactoryBase*>));
      static ::ROOT::TGenericClassInfo 
         instance("map<string,larocv::ClusterAlgoFactoryBase*>", -2, "map", 830,
                  typeid(map<string,larocv::ClusterAlgoFactoryBase*>), DefineBehavior(ptr, ptr),
                  &maplEstringcOlarocvcLcLClusterAlgoFactoryBasemUgR_Dictionary, isa_proxy, 0,
                  sizeof(map<string,larocv::ClusterAlgoFactoryBase*>) );
      instance.SetNew(&new_maplEstringcOlarocvcLcLClusterAlgoFactoryBasemUgR);
      instance.SetNewArray(&newArray_maplEstringcOlarocvcLcLClusterAlgoFactoryBasemUgR);
      instance.SetDelete(&delete_maplEstringcOlarocvcLcLClusterAlgoFactoryBasemUgR);
      instance.SetDeleteArray(&deleteArray_maplEstringcOlarocvcLcLClusterAlgoFactoryBasemUgR);
      instance.SetDestructor(&destruct_maplEstringcOlarocvcLcLClusterAlgoFactoryBasemUgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::MapInsert< map<string,larocv::ClusterAlgoFactoryBase*> >()));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const map<string,larocv::ClusterAlgoFactoryBase*>*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *maplEstringcOlarocvcLcLClusterAlgoFactoryBasemUgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const map<string,larocv::ClusterAlgoFactoryBase*>*)0x0)->GetClass();
      maplEstringcOlarocvcLcLClusterAlgoFactoryBasemUgR_TClassManip(theClass);
   return theClass;
   }

   static void maplEstringcOlarocvcLcLClusterAlgoFactoryBasemUgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_maplEstringcOlarocvcLcLClusterAlgoFactoryBasemUgR(void *p) {
      return  p ? ::new((::ROOT::TOperatorNewHelper*)p) map<string,larocv::ClusterAlgoFactoryBase*> : new map<string,larocv::ClusterAlgoFactoryBase*>;
   }
   static void *newArray_maplEstringcOlarocvcLcLClusterAlgoFactoryBasemUgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::TOperatorNewHelper*)p) map<string,larocv::ClusterAlgoFactoryBase*>[nElements] : new map<string,larocv::ClusterAlgoFactoryBase*>[nElements];
   }
   // Wrapper around operator delete
   static void delete_maplEstringcOlarocvcLcLClusterAlgoFactoryBasemUgR(void *p) {
      delete ((map<string,larocv::ClusterAlgoFactoryBase*>*)p);
   }
   static void deleteArray_maplEstringcOlarocvcLcLClusterAlgoFactoryBasemUgR(void *p) {
      delete [] ((map<string,larocv::ClusterAlgoFactoryBase*>*)p);
   }
   static void destruct_maplEstringcOlarocvcLcLClusterAlgoFactoryBasemUgR(void *p) {
      typedef map<string,larocv::ClusterAlgoFactoryBase*> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class map<string,larocv::ClusterAlgoFactoryBase*>

namespace {
  void TriggerDictionaryInitialization_libLArOpenCV_ImageClusterBase_Impl() {
    static const char* headers[] = {
"BaseUtil.h",
"Cluster2D.h",
"ClusterAlgoBase.h",
"ClusterAlgoFactory.h",
"ImageClusterBase.h",
"ImageClusterManager.h",
"ImageClusterTypes.h",
"ImageClusterViewer.h",
"MatchAlgoBase.h",
"MatchAlgoFactory.h",
"MatchBookKeeper.h",
"ReClusterAlgoBase.h",
"ReClusterAlgoFactory.h",
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
"/usr/local/ROOT//include/root",
"/Users/vgenty/sw/laropencv/ImageCluster/Base/",
0
    };
    static const char* fwdDeclCode = 
R"DICTFWDDCLS(
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_Autoloading_Map;
namespace cv{template <typename _Tp> class __attribute__((annotate("$clingAutoload$BaseUtil.h")))  Point_;
}
namespace std{inline namespace __1{template <class _Tp> class __attribute__((annotate("$clingAutoload$string")))  allocator;
}}
namespace larocv{class __attribute__((annotate("$clingAutoload$ClusterAlgoBase.h")))  ImageClusterBase;}
namespace larocv{class __attribute__((annotate("$clingAutoload$ClusterAlgoBase.h")))  ClusterAlgoBase;}
namespace larocv{class __attribute__((annotate("$clingAutoload$ImageClusterManager.h")))  MatchAlgoBase;}
namespace larocv{class __attribute__((annotate("$clingAutoload$ImageClusterManager.h")))  ReClusterAlgoBase;}
namespace larocv{class __attribute__((annotate("$clingAutoload$ClusterAlgoFactory.h")))  ClusterAlgoFactory;}
namespace larocv{class __attribute__((annotate("$clingAutoload$MatchAlgoFactory.h")))  MatchAlgoFactory;}
namespace larocv{class __attribute__((annotate("$clingAutoload$ReClusterAlgoFactory.h")))  ReClusterAlgoFactory;}
namespace larocv{class __attribute__((annotate("$clingAutoload$ImageClusterManager.h")))  ImageClusterManager;}
namespace larocv{class __attribute__((annotate("$clingAutoload$ImageClusterManager.h")))  ImageClusterViewer;}
namespace larocv{class __attribute__((annotate("$clingAutoload$Cluster2D.h")))  Cluster2D;}
namespace cv{typedef Point_<int> Point2i __attribute__((annotate("$clingAutoload$BaseUtil.h"))) ;}
namespace cv{typedef Point2i Point __attribute__((annotate("$clingAutoload$BaseUtil.h"))) ;}
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(

#ifndef G__VECTOR_HAS_CLASS_ITERATOR
  #define G__VECTOR_HAS_CLASS_ITERATOR 1
#endif

#define _BACKWARD_BACKWARD_WARNING_H
#include "BaseUtil.h"
#include "Cluster2D.h"
#include "ClusterAlgoBase.h"
#include "ClusterAlgoFactory.h"
#include "ImageClusterBase.h"
#include "ImageClusterManager.h"
#include "ImageClusterTypes.h"
#include "ImageClusterViewer.h"
#include "MatchAlgoBase.h"
#include "MatchAlgoFactory.h"
#include "MatchBookKeeper.h"
#include "ReClusterAlgoBase.h"
#include "ReClusterAlgoFactory.h"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[]={
"larocv::Cluster2D", payloadCode, "@",
"larocv::ClusterAlgoBase", payloadCode, "@",
"larocv::ClusterAlgoFactory", payloadCode, "@",
"larocv::ContourArray_t", payloadCode, "@",
"larocv::Contour_t", payloadCode, "@",
"larocv::ImageClusterBase", payloadCode, "@",
"larocv::ImageClusterManager", payloadCode, "@",
"larocv::ImageClusterViewer", payloadCode, "@",
"larocv::MatchAlgoBase", payloadCode, "@",
"larocv::MatchAlgoFactory", payloadCode, "@",
"larocv::ReClusterAlgoBase", payloadCode, "@",
"larocv::ReClusterAlgoFactory", payloadCode, "@",
nullptr};

    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("libLArOpenCV_ImageClusterBase",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_libLArOpenCV_ImageClusterBase_Impl, {}, classesHeaders);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_libLArOpenCV_ImageClusterBase_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_libLArOpenCV_ImageClusterBase() {
  TriggerDictionaryInitialization_libLArOpenCV_ImageClusterBase_Impl();
}
