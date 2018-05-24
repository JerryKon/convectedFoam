
#include "cellShader.h"

#include <math.h>

#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h> 
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnMatrixAttribute.h>
#include <maya/MFloatVector.h>
#include <maya/MVector.h>
#include <maya/MFloatPoint.h>
#include <maya/MGlobal.h>

using namespace crystalCG;

// Static data

MTypeId Cell3D::id(0x81017);

// Attributes

MObject Cell3D::aNearValue;
MObject Cell3D::aFarValue;
MObject Cell3D::aPower;

MObject Cell3D::aPointWorld;
MObject Cell3D::aPlaceMat;

MObject Cell3D::frequency;
MObject Cell3D::jitter;
MObject Cell3D::seed;

MObject Cell3D::aOutColor;
MObject Cell3D::aOutAlpha;

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

void Cell3D::postConstructor( )
{
	setMPSafe(true);
}

Cell3D::Cell3D()
{
}

Cell3D::~Cell3D()
{
}

void * Cell3D::creator()
{
    return new Cell3D();
}

MStatus Cell3D::initialize()
{
    MFnMatrixAttribute mAttr;
    MFnNumericAttribute nAttr; 

	// Input attributes

    aPlaceMat = mAttr.create("placementMatrix", "pm", 
							 MFnMatrixAttribute::kFloat);
    MAKE_INPUT(mAttr);
	CHECK_MSTATUS( mAttr.setHidden(true) );

	// Implicit shading network attributes

    aPointWorld = nAttr.createPoint("pointWorld", "pw");
	MAKE_INPUT(nAttr);
    CHECK_MSTATUS( nAttr.setHidden(true) );


    aNearValue = nAttr.create("nearValue", "nv", MFnNumericData::kFloat,0.5f );
    MAKE_INPUT(nAttr);

    aFarValue = nAttr.create("farValue", "fv", MFnNumericData::kFloat,0.0f );
    MAKE_INPUT(nAttr);

    aPower = nAttr.create("colorPower", "cpwer", MFnNumericData::kFloat,1.2f );
    MAKE_INPUT(nAttr);

	frequency = nAttr.create( "frequency", "freq", MFnNumericData::kFloat,1.0f);
    MAKE_INPUT(nAttr);
	
	jitter = nAttr.create( "jitter", "jit", MFnNumericData::kFloat,0.8f);
    MAKE_INPUT(nAttr);
	nAttr.setMin(-1);
	nAttr.setMax(1);
	//--------------

	seed = nAttr.create( "seed", "seed", MFnNumericData::kInt,5);
	MAKE_INPUT(nAttr);

	// Create output attributes
    
    aOutColor = nAttr.createColor("outColor", "oc");
	MAKE_OUTPUT(nAttr);

    aOutAlpha = nAttr.create( "outAlpha", "oa", MFnNumericData::kFloat);
	MAKE_OUTPUT(nAttr);
	
	
	// Add attributes to the node database.
    CHECK_MSTATUS( addAttribute(aPointWorld) );
    CHECK_MSTATUS( addAttribute(aPlaceMat) );

    CHECK_MSTATUS( addAttribute(aNearValue) );
    CHECK_MSTATUS( addAttribute(aFarValue) );
	CHECK_MSTATUS( addAttribute(aPower) );

	CHECK_MSTATUS( addAttribute(frequency) );
	CHECK_MSTATUS( addAttribute(jitter) );
	CHECK_MSTATUS( addAttribute(seed) );

    CHECK_MSTATUS( addAttribute(aOutAlpha) );
    CHECK_MSTATUS( addAttribute(aOutColor) );

    // All input affect the output color and alpha
    CHECK_MSTATUS( attributeAffects (aPlaceMat, aOutColor) );
    CHECK_MSTATUS( attributeAffects (aPointWorld, aOutColor) );

    CHECK_MSTATUS( attributeAffects (aPlaceMat, aOutAlpha) );
    CHECK_MSTATUS( attributeAffects (aPointWorld, aOutAlpha) );

    CHECK_MSTATUS( attributeAffects (aNearValue, aOutColor) );
    CHECK_MSTATUS( attributeAffects (aFarValue, aOutColor) );
   
	CHECK_MSTATUS( attributeAffects (aNearValue, aOutAlpha) );
    CHECK_MSTATUS( attributeAffects (aFarValue, aOutAlpha) );

	CHECK_MSTATUS( attributeAffects (jitter, aOutColor) );
    CHECK_MSTATUS( attributeAffects (jitter, aOutAlpha) );

	CHECK_MSTATUS( attributeAffects (frequency, aOutColor) );
    CHECK_MSTATUS( attributeAffects (frequency, aOutAlpha) );
	
	CHECK_MSTATUS( attributeAffects (seed, aOutColor) );
    CHECK_MSTATUS( attributeAffects (seed, aOutAlpha) );

	CHECK_MSTATUS( attributeAffects (aPower, aOutColor) );
    CHECK_MSTATUS( attributeAffects (aPower, aOutAlpha) );

    return MS::kSuccess;
}


MStatus Cell3D::compute(const MPlug& plug, MDataBlock& block) 
{
    if ( (plug != aOutColor) && (plug.parent() != aOutColor) &&
         (plug != aOutAlpha)
       ) 
       return MS::kUnknownParameter;

	const float3& worldPos   = block.inputValue(aPointWorld).asFloat3();
    const MFloatMatrix& m    = block.inputValue(aPlaceMat).asFloatMatrix();

    float nearV = block.inputValue(aNearValue).asFloat();

    float farV = block.inputValue(aFarValue).asFloat();

    float powerData = block.inputValue(aPower).asFloat();

	int cellSeed  = block.inputValue( seed ).asInt();

	float cellFrequency   = block.inputValue( frequency ).asFloat();

	float cellJitter	  = block.inputValue( jitter ).asFloat();
	
	MFloatPoint q(worldPos[0], worldPos[1], worldPos[2]);
    q *= m;	

	MVector uvForce;

	jCell::setFrequency( cellFrequency );
	jCell::setJitter( cellJitter );
	jCell::setNearFar( nearV, farV );
	jCell::setSeed(cellSeed);

	float f0 = (float)jCell::GetValue( q.x,q.y, q.z,uvForce);
	
	f0 = pow(f0,powerData);

    MDataHandle outHandle = block.outputValue( aOutColor );
    MFloatVector & outColor = outHandle.asFloatVector();
    outColor = MFloatVector( f0,f0,f0 );
    outHandle.setClean();

    outHandle = block.outputValue(aOutAlpha);
    outHandle.asFloat() = f0;
    outHandle.setClean();

    return MS::kSuccess;
}
