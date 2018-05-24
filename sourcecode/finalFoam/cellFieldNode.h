#ifndef _cellFieldNode
#define _cellFieldNode

#include <maya/MIOStream.h>
#include <maya/MVector.h>
#include <maya/MObject.h>
#include <maya/MPlug.h>
#include <maya/MDataBlock.h>
#include <maya/MPxFieldNode.h>

#include "cell.h"

#if defined(OSMac_MachO_)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

 
class cellField : public MPxFieldNode
{
public:
						cellField();
	virtual				~cellField(); 

	virtual MStatus		compute( const MPlug& plug, MDataBlock& data );

	static  void*		creator();
	static  MStatus		initialize();
	void                getForceArray(MDataBlock& block,
									  const MVectorArray &points,
									  const MVectorArray &velocities,
									  const MDoubleArray &masses,
									  MVectorArray &outputForce);
public:

	static MObject  constraintY;

	static MObject  phase;

	static MObject  frequency;
	static MObject  jitter;
	static MObject  seed;

	static MObject  aNearValue;
	static MObject  aFarValue;
	static MObject  aPower;

	static	MTypeId		id;
};

#endif
