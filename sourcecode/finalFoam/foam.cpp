#include "foam.h"
#include "noise.h"

#include <maya/MString.h>
#include <maya/MTypeId.h>
#include <maya/MPlug.h>
#include <maya/MPlugArray.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h> 
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnMatrixAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFloatVector.h>
#include <maya/MFloatPoint.h>
#include <maya/MSelectionList.h>
#include <maya/MFnParticleSystem.h>
#include <maya/MGlobal.h>
#include <maya/MVectorArray.h>
#include <maya/MFnVectorArrayData.h>


using namespace crystalCG;

// Static data
MTypeId foam::id(0x81018);
distort foam::foamDistort;

// Attributes

MObject foam::aPointWorld;
MObject foam::aPlaceMat;
MObject foam::posArray;
MObject foam::percent;
MObject foam::turSeed;

MObject foam::turFrequency;
MObject foam::turPower;
MObject foam::turRoughness;

MObject foam::aOutColor;
MObject foam::aOutAlpha;

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

void foam::postConstructor( )
{
	setMPSafe(true);
}

foam::foam()
{
}

foam::~foam()
{
}

void * foam::creator()
{
    return new foam();
}

MStatus foam::initialize()
{
    MFnMatrixAttribute mAttr;
    MFnNumericAttribute nAttr; 
	MFnTypedAttribute typAttr;

	// Input attributes

    aPlaceMat = mAttr.create("placementMatrix", "pm", 
							 MFnMatrixAttribute::kFloat);
    MAKE_INPUT(mAttr);
	CHECK_MSTATUS( mAttr.setHidden(true) );

	// Implicit shading network attributes

    aPointWorld = nAttr.createPoint("pointWorld", "pw");
	MAKE_INPUT(nAttr);
    CHECK_MSTATUS( nAttr.setHidden(true) );

    aOutColor = nAttr.createColor("outColor", "oc");
	MAKE_OUTPUT(nAttr);

    aOutAlpha = nAttr.create( "outAlpha", "oa", MFnNumericData::kFloat);
	MAKE_OUTPUT(nAttr);

	//-----------tur attr
	turFrequency = nAttr.create("turblenceFrequency", "tf", MFnNumericData::kFloat,0.2f );
    MAKE_INPUT(nAttr);
	
	turPower = nAttr.create("turblencePower", "tp", MFnNumericData::kFloat,0.1f );
    MAKE_INPUT(nAttr);
	
	turRoughness = nAttr.create("turblenceRoughness", "tr", MFnNumericData::kInt,1 );
    MAKE_INPUT(nAttr);
	
	turSeed = nAttr.create("turblenceSeed", "tseed", MFnNumericData::kInt,1 );
    MAKE_INPUT(nAttr);

	percent = nAttr.create("percent", "pert", MFnNumericData::kFloat,0.1f );
    MAKE_INPUT(nAttr);
	
	MVectorArray defaultVectArray;
    MFnVectorArrayData vectArrayDataFn;
	vectArrayDataFn.create( defaultVectArray );
	posArray = typAttr.create( "positionArray", "pa", MFnData::kVectorArray,vectArrayDataFn.object());
	typAttr.setHidden(true);

	// Add attributes to the node database.
    CHECK_MSTATUS( addAttribute(aPointWorld) );
    CHECK_MSTATUS( addAttribute(aPlaceMat) );
	CHECK_MSTATUS( addAttribute(posArray) );
	CHECK_MSTATUS( addAttribute(percent) );

	CHECK_MSTATUS( addAttribute(turFrequency) );
	CHECK_MSTATUS( addAttribute(turPower) );
	CHECK_MSTATUS( addAttribute(turRoughness) );
	CHECK_MSTATUS( addAttribute(turSeed) );

    CHECK_MSTATUS( addAttribute(aOutAlpha) );
    CHECK_MSTATUS( addAttribute(aOutColor) );

    // All input affect the output color and alpha
    CHECK_MSTATUS( attributeAffects (aPlaceMat, aOutColor) );
    CHECK_MSTATUS( attributeAffects (aPointWorld, aOutColor) );

    CHECK_MSTATUS( attributeAffects (aPlaceMat, aOutAlpha) );
    CHECK_MSTATUS( attributeAffects (aPointWorld, aOutAlpha) );

    CHECK_MSTATUS( attributeAffects (posArray, aOutAlpha) );
    CHECK_MSTATUS( attributeAffects (posArray, aOutAlpha) );

	CHECK_MSTATUS( attributeAffects (turFrequency, aOutColor) );
    CHECK_MSTATUS( attributeAffects (turFrequency, aOutAlpha) );
	
	CHECK_MSTATUS( attributeAffects (turPower, aOutColor) );
    CHECK_MSTATUS( attributeAffects (turPower, aOutAlpha) );

	CHECK_MSTATUS( attributeAffects (turRoughness, aOutColor) );
    CHECK_MSTATUS( attributeAffects (turRoughness, aOutAlpha) );

	CHECK_MSTATUS( attributeAffects (percent, aOutColor) );
    CHECK_MSTATUS( attributeAffects (percent, aOutAlpha) );

	CHECK_MSTATUS( attributeAffects (turSeed, aOutColor) );
    CHECK_MSTATUS( attributeAffects (turSeed, aOutAlpha) );

    return MS::kSuccess;
}

MStatus foam::compute(const MPlug& plug, MDataBlock& block) 
{
	MStatus status;

    if ( (plug != aOutColor) && (plug.parent() != aOutColor) &&
         (plug != aOutAlpha)
       ) 
       return MS::kUnknownParameter;

    const float3& worldPos   = block.inputValue(aPointWorld).asFloat3();
    const MFloatMatrix& m    = block.inputValue(aPlaceMat).asFloatMatrix();
	MObject posObj     = block.inputValue(posArray).data();
	
	float turF = block.inputValue(turFrequency).asFloat();

    float turP = block.inputValue(turPower).asFloat();
    
	float perData = block.inputValue(percent).asFloat();
	
	int turR = block.inputValue(turRoughness).asInt();

	int turSeedData = block.inputValue(turSeed).asInt();	

	foamDistort.setSeed(turSeedData);
	foamDistort.SetTurFrequency( turF );
	foamDistort.SetTurPower(turP);
	foamDistort.SetTurRoughness(turR);

	MFnVectorArrayData dataVectorArrayFn(posObj);
    MVectorArray inPosAry;
	inPosAry.clear();
	inPosAry = dataVectorArrayFn.array();

	MObject  particleObj;
	MFnParticleSystem pSys;

	MPlug arrayPlug( thisMObject(),posArray );
	MPlugArray plugArray;
	plugArray.clear();
	arrayPlug.connectedTo( plugArray,true,false,&status);
	CHECK_MSTATUS( status );

	if( plugArray.length() ==0 )
		return MS::kFailure;

	particleObj = plugArray[0].node();

	if( particleObj.apiType() != MFn::kParticle )
		return MS::kFailure;

	if ( pSys.setObject(particleObj)!= MS::kSuccess )
		return MS::kFailure;

	if( ! pSys.isValid() )
        return MS::kFailure;

	MDoubleArray radiusArry;
	radiusArry.clear();
	MString foamRadius("foamRadiusPP");

	if( !pSys.isPerParticleDoubleAttribute(foamRadius) )
		return MS::kFailure;	

	MFloatPoint q(worldPos[0],0/*worldPos[1]*/, worldPos[2]);

	//limit(q,m);

	foamDistort.getValue( q.x,q.y,q.z );

	int myID = -1;

	float dist=100000000000.0f,minDist=100000000000.0f;
	unsigned int arysize = inPosAry.length();

	for(unsigned int i = 0;i< arysize;i++)
	{
		dist = MFloatPoint(q.x,0,q.z).distanceTo( MFloatPoint((float)inPosAry[i].x,0,(float)inPosAry[i].z) );

		if( dist<minDist )
		{
			minDist = dist;

			myID = (int)i;
		}
	}

	if( myID == -1 )
		return MS::kFailure;

	unsigned int fs=0;
	fs = pSys.getPerParticleAttribute(foamRadius,radiusArry,&status);
	CHECK_MSTATUS( status );

	float f0 = 0,idValue=0;
	
	//compute
	if( fs>0)
		idValue = (float)radiusArry[myID];

	f0 = 1- (float)smoothstep( idValue*( 1-perData ),idValue,minDist);

    MDataHandle outHandle = block.outputValue( aOutColor );
    MFloatVector & outColor = outHandle.asFloatVector();
    outColor = MFloatVector(f0,f0,f0) ;
    outHandle.setClean();

    outHandle = block.outputValue(aOutAlpha);
    outHandle.asFloat() = f0;
    outHandle.setClean();

    return MS::kSuccess;
}

void foam::limit( MFloatPoint &pt,const MFloatMatrix& mt)
{
	   pt *= mt;									// Convert into solid space
	   	  
}