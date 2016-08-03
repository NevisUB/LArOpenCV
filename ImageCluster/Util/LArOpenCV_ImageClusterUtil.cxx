// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME LArOpenCV_ImageClusterUtil

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
#include "PCABox.h"
#include "PCAPath.h"
#include "PCAUtilities.h"
#include "PID.h"
#include "StatUtilities.h"
#include "Vertex2D.h"

// Header files passed via #pragma extra_include

namespace ROOT {
   static TClass *larocvcLcLPCABox_Dictionary();
   static void larocvcLcLPCABox_TClassManip(TClass*);
   static void *new_larocvcLcLPCABox(void *p = 0);
   static void *newArray_larocvcLcLPCABox(Long_t size, void *p);
   static void delete_larocvcLcLPCABox(void *p);
   static void deleteArray_larocvcLcLPCABox(void *p);
   static void destruct_larocvcLcLPCABox(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::larocv::PCABox*)
   {
      ::larocv::PCABox *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::larocv::PCABox));
      static ::ROOT::TGenericClassInfo 
         instance("larocv::PCABox", "PCABox.h", 19,
                  typeid(::larocv::PCABox), DefineBehavior(ptr, ptr),
                  &larocvcLcLPCABox_Dictionary, isa_proxy, 4,
                  sizeof(::larocv::PCABox) );
      instance.SetNew(&new_larocvcLcLPCABox);
      instance.SetNewArray(&newArray_larocvcLcLPCABox);
      instance.SetDelete(&delete_larocvcLcLPCABox);
      instance.SetDeleteArray(&deleteArray_larocvcLcLPCABox);
      instance.SetDestructor(&destruct_larocvcLcLPCABox);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::larocv::PCABox*)
   {
      return GenerateInitInstanceLocal((::larocv::PCABox*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::larocv::PCABox*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *larocvcLcLPCABox_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::larocv::PCABox*)0x0)->GetClass();
      larocvcLcLPCABox_TClassManip(theClass);
   return theClass;
   }

   static void larocvcLcLPCABox_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *larocvcLcLPCAPath_Dictionary();
   static void larocvcLcLPCAPath_TClassManip(TClass*);
   static void *new_larocvcLcLPCAPath(void *p = 0);
   static void *newArray_larocvcLcLPCAPath(Long_t size, void *p);
   static void delete_larocvcLcLPCAPath(void *p);
   static void deleteArray_larocvcLcLPCAPath(void *p);
   static void destruct_larocvcLcLPCAPath(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::larocv::PCAPath*)
   {
      ::larocv::PCAPath *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::larocv::PCAPath));
      static ::ROOT::TGenericClassInfo 
         instance("larocv::PCAPath", "PCAPath.h", 9,
                  typeid(::larocv::PCAPath), DefineBehavior(ptr, ptr),
                  &larocvcLcLPCAPath_Dictionary, isa_proxy, 4,
                  sizeof(::larocv::PCAPath) );
      instance.SetNew(&new_larocvcLcLPCAPath);
      instance.SetNewArray(&newArray_larocvcLcLPCAPath);
      instance.SetDelete(&delete_larocvcLcLPCAPath);
      instance.SetDeleteArray(&deleteArray_larocvcLcLPCAPath);
      instance.SetDestructor(&destruct_larocvcLcLPCAPath);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::larocv::PCAPath*)
   {
      return GenerateInitInstanceLocal((::larocv::PCAPath*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::larocv::PCAPath*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *larocvcLcLPCAPath_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::larocv::PCAPath*)0x0)->GetClass();
      larocvcLcLPCAPath_TClassManip(theClass);
   return theClass;
   }

   static void larocvcLcLPCAPath_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *larocvcLcLVertex2D_Dictionary();
   static void larocvcLcLVertex2D_TClassManip(TClass*);
   static void *new_larocvcLcLVertex2D(void *p = 0);
   static void *newArray_larocvcLcLVertex2D(Long_t size, void *p);
   static void delete_larocvcLcLVertex2D(void *p);
   static void deleteArray_larocvcLcLVertex2D(void *p);
   static void destruct_larocvcLcLVertex2D(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::larocv::Vertex2D*)
   {
      ::larocv::Vertex2D *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::larocv::Vertex2D));
      static ::ROOT::TGenericClassInfo 
         instance("larocv::Vertex2D", "Vertex2D.h", 10,
                  typeid(::larocv::Vertex2D), DefineBehavior(ptr, ptr),
                  &larocvcLcLVertex2D_Dictionary, isa_proxy, 4,
                  sizeof(::larocv::Vertex2D) );
      instance.SetNew(&new_larocvcLcLVertex2D);
      instance.SetNewArray(&newArray_larocvcLcLVertex2D);
      instance.SetDelete(&delete_larocvcLcLVertex2D);
      instance.SetDeleteArray(&deleteArray_larocvcLcLVertex2D);
      instance.SetDestructor(&destruct_larocvcLcLVertex2D);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::larocv::Vertex2D*)
   {
      return GenerateInitInstanceLocal((::larocv::Vertex2D*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::larocv::Vertex2D*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *larocvcLcLVertex2D_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::larocv::Vertex2D*)0x0)->GetClass();
      larocvcLcLVertex2D_TClassManip(theClass);
   return theClass;
   }

   static void larocvcLcLVertex2D_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *larocvcLcLPID_Dictionary();
   static void larocvcLcLPID_TClassManip(TClass*);
   static void *new_larocvcLcLPID(void *p = 0);
   static void *newArray_larocvcLcLPID(Long_t size, void *p);
   static void delete_larocvcLcLPID(void *p);
   static void deleteArray_larocvcLcLPID(void *p);
   static void destruct_larocvcLcLPID(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::larocv::PID*)
   {
      ::larocv::PID *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::larocv::PID));
      static ::ROOT::TGenericClassInfo 
         instance("larocv::PID", "PID.h", 25,
                  typeid(::larocv::PID), DefineBehavior(ptr, ptr),
                  &larocvcLcLPID_Dictionary, isa_proxy, 4,
                  sizeof(::larocv::PID) );
      instance.SetNew(&new_larocvcLcLPID);
      instance.SetNewArray(&newArray_larocvcLcLPID);
      instance.SetDelete(&delete_larocvcLcLPID);
      instance.SetDeleteArray(&deleteArray_larocvcLcLPID);
      instance.SetDestructor(&destruct_larocvcLcLPID);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::larocv::PID*)
   {
      return GenerateInitInstanceLocal((::larocv::PID*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::larocv::PID*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *larocvcLcLPID_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::larocv::PID*)0x0)->GetClass();
      larocvcLcLPID_TClassManip(theClass);
   return theClass;
   }

   static void larocvcLcLPID_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_larocvcLcLPCABox(void *p) {
      return  p ? ::new((::ROOT::TOperatorNewHelper*)p) ::larocv::PCABox : new ::larocv::PCABox;
   }
   static void *newArray_larocvcLcLPCABox(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::TOperatorNewHelper*)p) ::larocv::PCABox[nElements] : new ::larocv::PCABox[nElements];
   }
   // Wrapper around operator delete
   static void delete_larocvcLcLPCABox(void *p) {
      delete ((::larocv::PCABox*)p);
   }
   static void deleteArray_larocvcLcLPCABox(void *p) {
      delete [] ((::larocv::PCABox*)p);
   }
   static void destruct_larocvcLcLPCABox(void *p) {
      typedef ::larocv::PCABox current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::larocv::PCABox

namespace ROOT {
   // Wrappers around operator new
   static void *new_larocvcLcLPCAPath(void *p) {
      return  p ? ::new((::ROOT::TOperatorNewHelper*)p) ::larocv::PCAPath : new ::larocv::PCAPath;
   }
   static void *newArray_larocvcLcLPCAPath(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::TOperatorNewHelper*)p) ::larocv::PCAPath[nElements] : new ::larocv::PCAPath[nElements];
   }
   // Wrapper around operator delete
   static void delete_larocvcLcLPCAPath(void *p) {
      delete ((::larocv::PCAPath*)p);
   }
   static void deleteArray_larocvcLcLPCAPath(void *p) {
      delete [] ((::larocv::PCAPath*)p);
   }
   static void destruct_larocvcLcLPCAPath(void *p) {
      typedef ::larocv::PCAPath current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::larocv::PCAPath

namespace ROOT {
   // Wrappers around operator new
   static void *new_larocvcLcLVertex2D(void *p) {
      return  p ? ::new((::ROOT::TOperatorNewHelper*)p) ::larocv::Vertex2D : new ::larocv::Vertex2D;
   }
   static void *newArray_larocvcLcLVertex2D(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::TOperatorNewHelper*)p) ::larocv::Vertex2D[nElements] : new ::larocv::Vertex2D[nElements];
   }
   // Wrapper around operator delete
   static void delete_larocvcLcLVertex2D(void *p) {
      delete ((::larocv::Vertex2D*)p);
   }
   static void deleteArray_larocvcLcLVertex2D(void *p) {
      delete [] ((::larocv::Vertex2D*)p);
   }
   static void destruct_larocvcLcLVertex2D(void *p) {
      typedef ::larocv::Vertex2D current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::larocv::Vertex2D

namespace ROOT {
   // Wrappers around operator new
   static void *new_larocvcLcLPID(void *p) {
      return  p ? ::new((::ROOT::TOperatorNewHelper*)p) ::larocv::PID : new ::larocv::PID;
   }
   static void *newArray_larocvcLcLPID(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::TOperatorNewHelper*)p) ::larocv::PID[nElements] : new ::larocv::PID[nElements];
   }
   // Wrapper around operator delete
   static void delete_larocvcLcLPID(void *p) {
      delete ((::larocv::PID*)p);
   }
   static void deleteArray_larocvcLcLPID(void *p) {
      delete [] ((::larocv::PID*)p);
   }
   static void destruct_larocvcLcLPID(void *p) {
      typedef ::larocv::PID current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::larocv::PID

namespace ROOT {
   static TClass *vectorlElarocvcLcLPCABoxmUgR_Dictionary();
   static void vectorlElarocvcLcLPCABoxmUgR_TClassManip(TClass*);
   static void *new_vectorlElarocvcLcLPCABoxmUgR(void *p = 0);
   static void *newArray_vectorlElarocvcLcLPCABoxmUgR(Long_t size, void *p);
   static void delete_vectorlElarocvcLcLPCABoxmUgR(void *p);
   static void deleteArray_vectorlElarocvcLcLPCABoxmUgR(void *p);
   static void destruct_vectorlElarocvcLcLPCABoxmUgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<larocv::PCABox*>*)
   {
      vector<larocv::PCABox*> *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<larocv::PCABox*>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<larocv::PCABox*>", -2, "vector", 457,
                  typeid(vector<larocv::PCABox*>), DefineBehavior(ptr, ptr),
                  &vectorlElarocvcLcLPCABoxmUgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<larocv::PCABox*>) );
      instance.SetNew(&new_vectorlElarocvcLcLPCABoxmUgR);
      instance.SetNewArray(&newArray_vectorlElarocvcLcLPCABoxmUgR);
      instance.SetDelete(&delete_vectorlElarocvcLcLPCABoxmUgR);
      instance.SetDeleteArray(&deleteArray_vectorlElarocvcLcLPCABoxmUgR);
      instance.SetDestructor(&destruct_vectorlElarocvcLcLPCABoxmUgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<larocv::PCABox*> >()));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const vector<larocv::PCABox*>*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlElarocvcLcLPCABoxmUgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<larocv::PCABox*>*)0x0)->GetClass();
      vectorlElarocvcLcLPCABoxmUgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlElarocvcLcLPCABoxmUgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlElarocvcLcLPCABoxmUgR(void *p) {
      return  p ? ::new((::ROOT::TOperatorNewHelper*)p) vector<larocv::PCABox*> : new vector<larocv::PCABox*>;
   }
   static void *newArray_vectorlElarocvcLcLPCABoxmUgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::TOperatorNewHelper*)p) vector<larocv::PCABox*>[nElements] : new vector<larocv::PCABox*>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlElarocvcLcLPCABoxmUgR(void *p) {
      delete ((vector<larocv::PCABox*>*)p);
   }
   static void deleteArray_vectorlElarocvcLcLPCABoxmUgR(void *p) {
      delete [] ((vector<larocv::PCABox*>*)p);
   }
   static void destruct_vectorlElarocvcLcLPCABoxmUgR(void *p) {
      typedef vector<larocv::PCABox*> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<larocv::PCABox*>

namespace ROOT {
   static TClass *vectorlEintgR_Dictionary();
   static void vectorlEintgR_TClassManip(TClass*);
   static void *new_vectorlEintgR(void *p = 0);
   static void *newArray_vectorlEintgR(Long_t size, void *p);
   static void delete_vectorlEintgR(void *p);
   static void deleteArray_vectorlEintgR(void *p);
   static void destruct_vectorlEintgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<int>*)
   {
      vector<int> *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<int>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<int>", -2, "vector", 457,
                  typeid(vector<int>), DefineBehavior(ptr, ptr),
                  &vectorlEintgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<int>) );
      instance.SetNew(&new_vectorlEintgR);
      instance.SetNewArray(&newArray_vectorlEintgR);
      instance.SetDelete(&delete_vectorlEintgR);
      instance.SetDeleteArray(&deleteArray_vectorlEintgR);
      instance.SetDestructor(&destruct_vectorlEintgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<int> >()));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const vector<int>*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEintgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<int>*)0x0)->GetClass();
      vectorlEintgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEintgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEintgR(void *p) {
      return  p ? ::new((::ROOT::TOperatorNewHelper*)p) vector<int> : new vector<int>;
   }
   static void *newArray_vectorlEintgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::TOperatorNewHelper*)p) vector<int>[nElements] : new vector<int>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEintgR(void *p) {
      delete ((vector<int>*)p);
   }
   static void deleteArray_vectorlEintgR(void *p) {
      delete [] ((vector<int>*)p);
   }
   static void destruct_vectorlEintgR(void *p) {
      typedef vector<int> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<int>

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
   static TClass *vectorlEcvcLcLRect_lEintgRsPgR_Dictionary();
   static void vectorlEcvcLcLRect_lEintgRsPgR_TClassManip(TClass*);
   static void *new_vectorlEcvcLcLRect_lEintgRsPgR(void *p = 0);
   static void *newArray_vectorlEcvcLcLRect_lEintgRsPgR(Long_t size, void *p);
   static void delete_vectorlEcvcLcLRect_lEintgRsPgR(void *p);
   static void deleteArray_vectorlEcvcLcLRect_lEintgRsPgR(void *p);
   static void destruct_vectorlEcvcLcLRect_lEintgRsPgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<cv::Rect_<int> >*)
   {
      vector<cv::Rect_<int> > *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<cv::Rect_<int> >));
      static ::ROOT::TGenericClassInfo 
         instance("vector<cv::Rect_<int> >", -2, "vector", 457,
                  typeid(vector<cv::Rect_<int> >), DefineBehavior(ptr, ptr),
                  &vectorlEcvcLcLRect_lEintgRsPgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<cv::Rect_<int> >) );
      instance.SetNew(&new_vectorlEcvcLcLRect_lEintgRsPgR);
      instance.SetNewArray(&newArray_vectorlEcvcLcLRect_lEintgRsPgR);
      instance.SetDelete(&delete_vectorlEcvcLcLRect_lEintgRsPgR);
      instance.SetDeleteArray(&deleteArray_vectorlEcvcLcLRect_lEintgRsPgR);
      instance.SetDestructor(&destruct_vectorlEcvcLcLRect_lEintgRsPgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<cv::Rect_<int> > >()));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const vector<cv::Rect_<int> >*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEcvcLcLRect_lEintgRsPgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<cv::Rect_<int> >*)0x0)->GetClass();
      vectorlEcvcLcLRect_lEintgRsPgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEcvcLcLRect_lEintgRsPgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEcvcLcLRect_lEintgRsPgR(void *p) {
      return  p ? ::new((::ROOT::TOperatorNewHelper*)p) vector<cv::Rect_<int> > : new vector<cv::Rect_<int> >;
   }
   static void *newArray_vectorlEcvcLcLRect_lEintgRsPgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::TOperatorNewHelper*)p) vector<cv::Rect_<int> >[nElements] : new vector<cv::Rect_<int> >[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEcvcLcLRect_lEintgRsPgR(void *p) {
      delete ((vector<cv::Rect_<int> >*)p);
   }
   static void deleteArray_vectorlEcvcLcLRect_lEintgRsPgR(void *p) {
      delete [] ((vector<cv::Rect_<int> >*)p);
   }
   static void destruct_vectorlEcvcLcLRect_lEintgRsPgR(void *p) {
      typedef vector<cv::Rect_<int> > current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<cv::Rect_<int> >

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

namespace {
  void TriggerDictionaryInitialization_libLArOpenCV_ImageClusterUtil_Impl() {
    static const char* headers[] = {
"PCABox.h",
"PCAPath.h",
"PCAUtilities.h",
"PID.h",
"StatUtilities.h",
"Vertex2D.h",
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
"/usr/local/ROOT//include/root",
"/Users/vgenty/sw/laropencv/ImageCluster/Util/",
0
    };
    static const char* fwdDeclCode = 
R"DICTFWDDCLS(
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_Autoloading_Map;
namespace larocv{class __attribute__((annotate("$clingAutoload$PCABox.h")))  PCABox;}
namespace larocv{class __attribute__((annotate("$clingAutoload$PCAPath.h")))  PCAPath;}
namespace larocv{class __attribute__((annotate("$clingAutoload$Vertex2D.h")))  Vertex2D;}
namespace larocv{class __attribute__((annotate("$clingAutoload$PID.h")))  PID;}
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(

#ifndef G__VECTOR_HAS_CLASS_ITERATOR
  #define G__VECTOR_HAS_CLASS_ITERATOR 1
#endif

#define _BACKWARD_BACKWARD_WARNING_H
#include "PCABox.h"
#include "PCAPath.h"
#include "PCAUtilities.h"
#include "PID.h"
#include "StatUtilities.h"
#include "Vertex2D.h"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[]={
"larocv::PCABox", payloadCode, "@",
"larocv::PCAPath", payloadCode, "@",
"larocv::PID", payloadCode, "@",
"larocv::Vertex2D", payloadCode, "@",
nullptr};

    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("libLArOpenCV_ImageClusterUtil",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_libLArOpenCV_ImageClusterUtil_Impl, {}, classesHeaders);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_libLArOpenCV_ImageClusterUtil_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_libLArOpenCV_ImageClusterUtil() {
  TriggerDictionaryInitialization_libLArOpenCV_ImageClusterUtil_Impl();
}
