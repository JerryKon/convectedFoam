#ifndef _foam_
#define _foam_

#include <maya/MPxNode.h>
#include <maya/MFloatPoint.h>

#include "distort.h"

class foam : public MPxNode
{
	public:
                    foam();
    virtual         ~foam();

    virtual MStatus compute( const MPlug&, MDataBlock& );
	virtual void    postConstructor();

    static  void *  creator();
    static  MStatus initialize();
	void    limit( MFloatPoint &pt,const MFloatMatrix& mt);

	//  Id tag for use with binary file format
    static  MTypeId id;

	public:

	// Input attributes
    static MObject  aPointWorld;
    static MObject  aPlaceMat;
	static MObject  posArray;
	
	static MObject  percent;

	static MObject  turFrequency;
	static MObject  turPower;
	static MObject  turRoughness;

	static MObject  turSeed;

	// Output attributes
    static MObject  aOutColor;
    static MObject  aOutAlpha;

	static distort foamDistort;
};
#endif