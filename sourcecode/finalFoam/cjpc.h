#ifndef _cjpc_
#define _cjpc_


#include <maya/MPxCommand.h>

#include <maya/MArgList.h>

#include <maya/MSyntax.h>

#include <maya/MFloatVectorArray.h>

#include <maya/MFloatArray.h>

#include <maya/MFnParticleSystem.h>

class cjpc : public MPxCommand

{

   public:

      cjpc();

      virtual ~cjpc();

      static void *creator();

      static MSyntax newSyntax();

      bool isUndoable() const;

      MStatus doIt(const MArgList&);

	  void writeCache( const char* pathName,const MVectorArray &pos,const MDoubleArray &radius );

	  MStatus getRadiusPP( MDoubleArray& radiusAry );

	  MStatus getPosPP( MVectorArray& posAry ,const MObject &obj,const MFnDependencyNode &dgfn );

	private:

		int time;

		MString pathN;

		MString particleN;

		MFnParticleSystem pSys;
};


#endif