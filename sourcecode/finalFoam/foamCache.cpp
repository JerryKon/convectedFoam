#include "foamCache.h"

#include <maya/MTypeId.h>
#include <maya/MPlug.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h> 
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFloatVector.h>
#include <maya/MGlobal.h>
#include <maya/MFnVectorArrayData.h>
#include <maya/MFnDoubleArrayData.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MTime.h>
#include <maya/MVectorArray.h>
#include <maya/MFloatPoint.h>
#include <iostream>
#include <fstream>

using namespace std;

// Static data
MTypeId foamCache::id(0x81018);

// Attributes
MObject foamCache::cacheName;
MObject foamCache::time;
MObject foamCache::offset;
MObject foamCache::outPos;
MObject foamCache::outRadius;


#define MAKE_INPUT(attr)	\
    CHECK_MSTATUS( attr.setKeyable(true) );		\
    CHECK_MSTATUS( attr.setStorable(true) );	\
    CHECK_MSTATUS( attr.setReadable(true) );	\
    CHECK_MSTATUS( attr.setWritable(true) );

#define MAKE_OUTPUT(attr)	\
    CHECK_MSTATUS( attr.setKeyable(false) ); 	\
    CHECK_MSTATUS( attr.setStorable(false) );	\
    CHECK_MSTATUS( attr.setReadable(true) );	\
    CHECK_MSTATUS( attr.setWritable(false) );

foamCache::foamCache()
{
}

foamCache::~foamCache()
{
}

void * foamCache::creator()
{
    return new foamCache();
}

MStatus foamCache::initialize()
{
    MFnNumericAttribute nAttr; 
	MFnTypedAttribute typAttr;
	MStatus status;

	cacheName = typAttr.create("cacheName","cn",MFnData::kString);
	MAKE_INPUT(typAttr);

	time = nAttr.create("time","t",MFnNumericData::kInt,0,&status);
	CHECK_MSTATUS(status);
	MAKE_INPUT(nAttr);
	offset = nAttr.create("offset","o",MFnNumericData::kInt,0,&status);
	CHECK_MSTATUS(status);
	MAKE_INPUT(nAttr);

	MVectorArray defaultVectArray;
    MFnVectorArrayData vectArrayDataFn;
	vectArrayDataFn.create( defaultVectArray );
	outPos = typAttr.create( "outPosition", "op", MFnData::kVectorArray,vectArrayDataFn.object(),&status);
    CHECK_MSTATUS( status );	
	MAKE_OUTPUT(typAttr);
	typAttr.setHidden(true);

	MFnDoubleArrayData doubArrayDataFn;
	doubArrayDataFn.create(&status);
    CHECK_MSTATUS( status );
	outRadius = typAttr.create( "outRadius", "or", MFnData::kDoubleArray,doubArrayDataFn.object(),&status);
	CHECK_MSTATUS( status );
	MAKE_OUTPUT(typAttr);
	CHECK_MSTATUS( typAttr.setHidden(true) );

	// Add attributes to the node database.
	CHECK_MSTATUS( addAttribute(cacheName) );
	CHECK_MSTATUS( addAttribute(time) );
	CHECK_MSTATUS( addAttribute(offset) );
	CHECK_MSTATUS( addAttribute(outPos) );
	CHECK_MSTATUS( addAttribute(outRadius) );

    // All input affect the output color and alpha
    CHECK_MSTATUS( attributeAffects (cacheName, outPos) );
    CHECK_MSTATUS( attributeAffects (cacheName, outRadius) );
    CHECK_MSTATUS( attributeAffects (time, outPos) );
    CHECK_MSTATUS( attributeAffects (time, outRadius) );
    CHECK_MSTATUS( attributeAffects (offset, outPos) );
    CHECK_MSTATUS( attributeAffects (offset, outRadius) );

    return MS::kSuccess;
}

MStatus foamCache::compute(const MPlug& plug, MDataBlock& block) 
{
	MStatus status;

    if ( (plug != outPos) && (plug != outRadius)
       ) 
       return MS::kUnknownParameter;
	
	MDataHandle caH = block.inputValue( cacheName ,&status);
	CHECK_MSTATUS(status);
	MString path = caH.asString();
		
	int offsetData = block.inputValue(offset).asInt();
	int timeData = block.inputValue(time).asInt();
	int frame = timeData + offsetData;

	//-----------------------------
	MDataHandle	posH = block.outputValue(outPos,&status);
	CHECK_MSTATUS(status);
	MObject posObj  = posH.data();
	MFnVectorArrayData dataVectorArrayFn(posObj);
	
	MDataHandle radiusH = block.outputValue(outRadius,&status);
	CHECK_MSTATUS(status);
	MObject radiusObj   = radiusH.data();
	MFnDoubleArrayData radiusArrayFn(radiusObj);

	//-----------------
	MVectorArray inPosAry;
	inPosAry.clear();
	MDoubleArray radiusAry;
	radiusAry.clear();

	path += ".";
	path += frame;
	path += ".jpc";

	readCache(path.asChar(),inPosAry,radiusAry);

	dataVectorArrayFn.set( inPosAry );

	posH.setClean();

	radiusArrayFn.set(radiusAry);

	radiusH.setClean();

    return MS::kSuccess;
}

void foamCache::readCache( const char* pathName ,MVectorArray &pos,MDoubleArray &radius)
{
	ifstream is( pathName,ios_base::binary|ios::in );
	
	int count=0;
	double posAry[3];
	double rad;

	if(is)
	{
		//--------得到粒子数量
		is.read((char*) &count,4);

		//---------读取第三行得到粒子foamradius
		for(int i=0;i<count;i++)
		{
			is.read( (char*) &posAry,24 );
			pos.append( MVector(posAry[0],posAry[1],posAry[2]) );
		}
		
		//---------读取第三行得到粒子foamradius
		for(int i=0;i<count;i++)
		{
			is.read( (char*) &rad,8 );
			radius.append( rad );
		}
	}

	is.close();
}