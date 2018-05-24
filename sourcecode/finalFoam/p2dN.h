#ifndef _p2dN_
#define _p2dN_

#include <maya/MPxNode.h>
#include "distort.h"

class p2dN : public MPxNode
{
	public:
						p2dN();
		virtual         ~p2dN();

		virtual MStatus compute( const MPlug&, MDataBlock& );
		virtual void    postConstructor();

		static  void *  creator();
		static  MStatus initialize();

		static  MTypeId id;

	public:

		// Input attributes

		static MObject  aUVCoord;

		static MObject  turFrequency;
		static MObject  turPower;
		static MObject  turRoughness;
		static MObject  turSeed;

		// Output attributes
		static MObject  aOutUV;
};

#endif
