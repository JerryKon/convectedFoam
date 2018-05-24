#ifndef _foamCache_
#define _foamCache_

#include <maya/MPxNode.h>
#include <maya/MFloatPoint.h>
#include <maya/MVectorArray.h>
#include <maya/MDoubleArray.h>

class foamCache : public MPxNode
{
	public:
						foamCache();
		virtual         ~foamCache();

		virtual MStatus compute( const MPlug&, MDataBlock& );

		void readCache( const char* pathName ,MVectorArray &pos,MDoubleArray &radius);

		static  void *  creator();
		static  MStatus initialize();

		//  Id tag for use with binary file format
		static  MTypeId id;

	public:

		// Input attributes
		static MObject  cacheName;
		static MObject  time;
		static MObject  offset;

		static MObject  outPos;
		static MObject  outRadius;
};
#endif