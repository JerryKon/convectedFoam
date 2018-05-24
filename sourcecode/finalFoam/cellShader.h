#ifndef _cellShader_
#define _cellShader_

#include <maya/MPxNode.h>

#include "cell.h"

class Cell3D : public MPxNode
{
	public:
						Cell3D();
		virtual         ~Cell3D();

		virtual MStatus compute( const MPlug&, MDataBlock& );
		virtual void    postConstructor();

		static  void *  creator();
		static  MStatus initialize();


		static  MTypeId id;

	public:

		// Input attributes

	    static MObject  aPointWorld;
		static MObject  aPlaceMat;

		static MObject  aNearValue;
		static MObject  aFarValue;
		static MObject  aPower;

		static MObject  frequency;
		static MObject  jitter;
		static MObject  seed;

		// Output attributes
		static MObject  aOutColor;
		static MObject  aOutAlpha;
};

#endif
