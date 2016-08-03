// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME LArOpenCV_ImageClusterDirection

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
#include "PCAOverall.h"
#include "PCASegmentation.h"

// Header files passed via #pragma extra_include

namespace ROOT {
   static TClass *larocvcLcLPCASegmentation_Dictionary();
   static void larocvcLcLPCASegmentation_TClassManip(TClass*);
   static void *new_larocvcLcLPCASegmentation(void *p = 0);
   static void *newArray_larocvcLcLPCASegmentation(Long_t size, void *p);
   static void delete_larocvcLcLPCASegmentation(void *p);
   static void deleteArray_larocvcLcLPCASegmentation(void *p);
   static void destruct_larocvcLcLPCASegmentation(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::larocv::PCASegmentation*)
   {
      ::larocv::PCASegmentation *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::larocv::PCASegmentation));
      static ::ROOT::TGenericClassInfo 
         instance("larocv::PCASegmentation", "PCASegmentation.h", 18,
                  typeid(::larocv::PCASegmentation), DefineBehavior(ptr, ptr),
                  &larocvcLcLPCASegmentation_Dictionary, isa_proxy, 4,
                  sizeof(::larocv::PCASegmentation) );
      instance.SetNew(&new_larocvcLcLPCASegmentation);
      instance.SetNewArray(&newArray_larocvcLcLPCASegmentation);
      instance.SetDelete(&delete_larocvcLcLPCASegmentation);
      instance.SetDeleteArray(&deleteArray_larocvcLcLPCASegmentation);
      instance.SetDestructor(&destruct_larocvcLcLPCASegmentation);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::larocv::PCASegmentation*)
   {
      return GenerateInitInstanceLocal((::larocv::PCASegmentation*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::larocv::PCASegmentation*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *larocvcLcLPCASegmentation_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::larocv::PCASegmentation*)0x0)->GetClass();
      larocvcLcLPCASegmentation_TClassManip(theClass);
   return theClass;
   }

   static void larocvcLcLPCASegmentation_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *larocvcLcLPCAOverall_Dictionary();
   static void larocvcLcLPCAOverall_TClassManip(TClass*);
   static void *new_larocvcLcLPCAOverall(void *p = 0);
   static void *newArray_larocvcLcLPCAOverall(Long_t size, void *p);
   static void delete_larocvcLcLPCAOverall(void *p);
   static void deleteArray_larocvcLcLPCAOverall(void *p);
   static void destruct_larocvcLcLPCAOverall(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::larocv::PCAOverall*)
   {
      ::larocv::PCAOverall *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::larocv::PCAOverall));
      static ::ROOT::TGenericClassInfo 
         instance("larocv::PCAOverall", "PCAOverall.h", 25,
                  typeid(::larocv::PCAOverall), DefineBehavior(ptr, ptr),
                  &larocvcLcLPCAOverall_Dictionary, isa_proxy, 4,
                  sizeof(::larocv::PCAOverall) );
      instance.SetNew(&new_larocvcLcLPCAOverall);
      instance.SetNewArray(&newArray_larocvcLcLPCAOverall);
      instance.SetDelete(&delete_larocvcLcLPCAOverall);
      instance.SetDeleteArray(&deleteArray_larocvcLcLPCAOverall);
      instance.SetDestructor(&destruct_larocvcLcLPCAOverall);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::larocv::PCAOverall*)
   {
      return GenerateInitInstanceLocal((::larocv::PCAOverall*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::larocv::PCAOverall*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *larocvcLcLPCAOverall_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::larocv::PCAOverall*)0x0)->GetClass();
      larocvcLcLPCAOverall_TClassManip(theClass);
   return theClass;
   }

   static void larocvcLcLPCAOverall_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_larocvcLcLPCASegmentation(void *p) {
      return  p ? ::new((::ROOT::TOperatorNewHelper*)p) ::larocv::PCASegmentation : new ::larocv::PCASegmentation;
   }
   static void *newArray_larocvcLcLPCASegmentation(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::TOperatorNewHelper*)p) ::larocv::PCASegmentation[nElements] : new ::larocv::PCASegmentation[nElements];
   }
   // Wrapper around operator delete
   static void delete_larocvcLcLPCASegmentation(void *p) {
      delete ((::larocv::PCASegmentation*)p);
   }
   static void deleteArray_larocvcLcLPCASegmentation(void *p) {
      delete [] ((::larocv::PCASegmentation*)p);
   }
   static void destruct_larocvcLcLPCASegmentation(void *p) {
      typedef ::larocv::PCASegmentation current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::larocv::PCASegmentation

namespace ROOT {
   // Wrappers around operator new
   static void *new_larocvcLcLPCAOverall(void *p) {
      return  p ? ::new((::ROOT::TOperatorNewHelper*)p) ::larocv::PCAOverall : new ::larocv::PCAOverall;
   }
   static void *newArray_larocvcLcLPCAOverall(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::TOperatorNewHelper*)p) ::larocv::PCAOverall[nElements] : new ::larocv::PCAOverall[nElements];
   }
   // Wrapper around operator delete
   static void delete_larocvcLcLPCAOverall(void *p) {
      delete ((::larocv::PCAOverall*)p);
   }
   static void deleteArray_larocvcLcLPCAOverall(void *p) {
      delete [] ((::larocv::PCAOverall*)p);
   }
   static void destruct_larocvcLcLPCAOverall(void *p) {
      typedef ::larocv::PCAOverall current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::larocv::PCAOverall

namespace ROOT {
   static TClass *vectorlElarocvcLcLPCAPathgR_Dictionary();
   static void vectorlElarocvcLcLPCAPathgR_TClassManip(TClass*);
   static void *new_vectorlElarocvcLcLPCAPathgR(void *p = 0);
   static void *newArray_vectorlElarocvcLcLPCAPathgR(Long_t size, void *p);
   static void delete_vectorlElarocvcLcLPCAPathgR(void *p);
   static void deleteArray_vectorlElarocvcLcLPCAPathgR(void *p);
   static void destruct_vectorlElarocvcLcLPCAPathgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<larocv::PCAPath>*)
   {
      vector<larocv::PCAPath> *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<larocv::PCAPath>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<larocv::PCAPath>", -2, "vector", 457,
                  typeid(vector<larocv::PCAPath>), DefineBehavior(ptr, ptr),
                  &vectorlElarocvcLcLPCAPathgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<larocv::PCAPath>) );
      instance.SetNew(&new_vectorlElarocvcLcLPCAPathgR);
      instance.SetNewArray(&newArray_vectorlElarocvcLcLPCAPathgR);
      instance.SetDelete(&delete_vectorlElarocvcLcLPCAPathgR);
      instance.SetDeleteArray(&deleteArray_vectorlElarocvcLcLPCAPathgR);
      instance.SetDestructor(&destruct_vectorlElarocvcLcLPCAPathgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<larocv::PCAPath> >()));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const vector<larocv::PCAPath>*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlElarocvcLcLPCAPathgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<larocv::PCAPath>*)0x0)->GetClass();
      vectorlElarocvcLcLPCAPathgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlElarocvcLcLPCAPathgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlElarocvcLcLPCAPathgR(void *p) {
      return  p ? ::new((::ROOT::TOperatorNewHelper*)p) vector<larocv::PCAPath> : new vector<larocv::PCAPath>;
   }
   static void *newArray_vectorlElarocvcLcLPCAPathgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::TOperatorNewHelper*)p) vector<larocv::PCAPath>[nElements] : new vector<larocv::PCAPath>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlElarocvcLcLPCAPathgR(void *p) {
      delete ((vector<larocv::PCAPath>*)p);
   }
   static void deleteArray_vectorlElarocvcLcLPCAPathgR(void *p) {
      delete [] ((vector<larocv::PCAPath>*)p);
   }
   static void destruct_vectorlElarocvcLcLPCAPathgR(void *p) {
      typedef vector<larocv::PCAPath> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<larocv::PCAPath>

namespace {
  void TriggerDictionaryInitialization_libLArOpenCV_ImageClusterDirection_Impl() {
    static const char* headers[] = {
"PCAOverall.h",
"PCASegmentation.h",
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
"/Users/vgenty/sw/laropencv/ImageCluster/Direction/",
0
    };
    static const char* fwdDeclCode = 
R"DICTFWDDCLS(
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_Autoloading_Map;
namespace larocv{class __attribute__((annotate("$clingAutoload$PCASegmentation.h")))  PCASegmentation;}
namespace larocv{class __attribute__((annotate("$clingAutoload$PCAOverall.h")))  PCAOverall;}
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(

#ifndef G__VECTOR_HAS_CLASS_ITERATOR
  #define G__VECTOR_HAS_CLASS_ITERATOR 1
#endif

#define _BACKWARD_BACKWARD_WARNING_H
#include "PCAOverall.h"
#include "PCASegmentation.h"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[]={
"larocv::PCAOverall", payloadCode, "@",
"larocv::PCASegmentation", payloadCode, "@",
nullptr};

    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("libLArOpenCV_ImageClusterDirection",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_libLArOpenCV_ImageClusterDirection_Impl, {}, classesHeaders);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_libLArOpenCV_ImageClusterDirection_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_libLArOpenCV_ImageClusterDirection() {
  TriggerDictionaryInitialization_libLArOpenCV_ImageClusterDirection_Impl();
}
