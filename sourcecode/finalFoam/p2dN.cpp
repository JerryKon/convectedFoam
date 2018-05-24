#include "p2dN.h"

#include <maya/MTypeId.h>
#include <maya/MPlug.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h> 
#include <maya/MFnNumericAttribute.h>
#include <maya/MFloatPoint.h>

using namespace crystalCG;

// Static data
MTypeId p2dN::id(0x81022);

// Attributes
MObject p2dN::aUVCoord;

MObject p2dN::turFrequency;
MObject p2dN::turPower;
MObject p2dN::turRoughness;
MObject p2dN::turSeed;

MObject p2dN::aOutUV;

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

void p2dN::postConstructor( )
{
	//setMPSafe(false);
}

p2dN::p2dN()
{
}

p2dN::~p2dN()
{
}

void * p2dN::creator()
{
    return new p2dN();
}

MStatus p2dN::initialize()
{
    MFnNumericAttribute nAttr; 
	MStatus status;

	//-----------tur attr

	MObject child1 = nAttr.create( "uCoord", "u", MFnNumericData::kFloat,0,&status);
	CHECK_MSTATUS( status );
    MObject child2 = nAttr.create( "vCoord", "v", MFnNumericData::kFloat,0,&status);
	CHECK_MSTATUS( status );

    aUVCoord = nAttr.create( "uvCoord","uv", child1, child2);
	MAKE_INPUT(nAttr);
    CHECK_MSTATUS(nAttr.setHidden(true) );

	child1 = nAttr.create( "outU", "ou", MFnNumericData::kFloat,0,&status);
	CHECK_MSTATUS( status );
	MAKE_OUTPUT(nAttr);
    child2 = nAttr.create( "outV", "ov", MFnNumericData::kFloat,0,&status);
	CHECK_MSTATUS( status );
	MAKE_OUTPUT(nAttr);

    aOutUV = nAttr.create( "outUV","ouv", child1, child2);
    MAKE_OUTPUT(nAttr);

	child1 = nAttr.create( "turFreU", "tfu", MFnNumericData::kFloat,5,&status);
	CHECK_MSTATUS( status );
    child2 = nAttr.create( "turFreV", "tfv", MFnNumericData::kFloat,5,&status);
	CHECK_MSTATUS( status );

    turFrequency = nAttr.create( "turbulenceFrequency","tf", child1, child2);
	MAKE_INPUT(nAttr);

	child1 = nAttr.create( "turPowU", "tpu", MFnNumericData::kFloat,0.1f,&status);
	CHECK_MSTATUS( status );
    child2 = nAttr.create( "turPowV", "tpv", MFnNumericData::kFloat,0.1f,&status);
	CHECK_MSTATUS( status );

    turPower = nAttr.create( "turbulencePower","tp", child1, child2);
	MAKE_INPUT(nAttr);

	turRoughness = nAttr.create("turbulenceRoughness", "turrou", MFnNumericData::kInt,1 );
    MAKE_INPUT(nAttr);

	turSeed = nAttr.create("turbulenceSeed", "turr", MFnNumericData::kInt,112 );
    MAKE_INPUT(nAttr);

	// Add attributes to the node database.
	CHECK_MSTATUS( addAttribute(aUVCoord) );
	
	CHECK_MSTATUS( addAttribute(turFrequency) );
	CHECK_MSTATUS( addAttribute(turPower) );
	CHECK_MSTATUS( addAttribute(turRoughness) );
	CHECK_MSTATUS( addAttribute(turSeed) );

    CHECK_MSTATUS( addAttribute(aOutUV) );

    // All input affect the output color and alpha
    CHECK_MSTATUS( attributeAffects (turFrequency, aOutUV) );

	CHECK_MSTATUS( attributeAffects (turPower, aOutUV) );

	CHECK_MSTATUS( attributeAffects (turRoughness, aOutUV) );

	CHECK_MSTATUS( attributeAffects (turSeed, aOutUV) );

	CHECK_MSTATUS( attributeAffects (aUVCoord, aOutUV) );

    return MS::kSuccess;
}

MStatus p2dN::compute(const MPlug& plug, MDataBlock& block) 
{
	MStatus status;

    if ( (plug != aOutUV) && (plug.parent() != aOutUV) ) 
       return MS::kUnknownParameter;

	//---------------变形属性

	float2& turF = block.inputValue(turFrequency).asFloat2();

    float2& turP = block.inputValue(turPower).asFloat2();

	int turR = block.inputValue(turRoughness).asInt();

	int turS = block.inputValue(turSeed).asInt();
	
	//-----------------得到uv及边界
    float2 & uv   = block.inputValue( aUVCoord  ,&status).asFloat2();
	CHECK_MSTATUS(status);
    
	MDataHandle outHandle  = block.outputValue( aOutUV  ,&status);
	CHECK_MSTATUS(status);
	
	MFloatPoint sap(uv[0],0, uv[1]);

	distort::setSeed(turS);
	distort::SetTurRoughness(turR);
	distort::SetTurFrequency(turF[0],0,turF[1]);
	distort::SetTurPower(turP[0],0,turP[1]);
	distort::getValue( sap.x,sap.y,sap.z );

    outHandle.set( sap.x,sap.z );
    outHandle.setClean();

    return MS::kSuccess;
}