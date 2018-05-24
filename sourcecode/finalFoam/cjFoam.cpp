#include "cjFoam.h"

#include <maya/MTypeId.h>
#include <maya/MPlug.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h> 
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFloatVector.h>
#include <maya/MGlobal.h>
#include <maya/MFnVectorArrayData.h>
#include <maya/MFnDoubleArrayData.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MTime.h>
#include <maya/MVectorArray.h>
#include <maya/MFloatPoint.h>

using namespace crystalCG;

// Static data
MTypeId cjFoam::id(0x81021);

// Attributes
MObject cjFoam::baseBound;
MObject cjFoam::aUVCoord;
MObject cjFoam::posArray;
MObject cjFoam::radiusArray;
MObject cjFoam::sampleR;
MObject cjFoam::samplePercent;

MObject cjFoam::foamType;
MObject cjFoam::turbulence;
MObject cjFoam::turFrequency;
MObject cjFoam::turPower;
MObject cjFoam::turRoughness;
MObject cjFoam::turSeed;

MObject cjFoam::startTime;
MObject cjFoam::currentTime;

MObject cjFoam::aOutColor;
MObject cjFoam::aOutAlpha;

#ifndef PI
#define PI 3.1415926f
#endif
#define SixtyDgree (PI/3.0f)
#define ThirtyDgree (PI/6.0f)

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

void cjFoam::postConstructor( )
{
	//setMPSafe(true);
}

cjFoam::cjFoam()
{
}

cjFoam::~cjFoam()
{
}

void * cjFoam::creator()
{
    return new cjFoam();
}

MStatus cjFoam::initialize()
{
    MFnNumericAttribute nAttr; 
	MFnTypedAttribute typAttr;
	MStatus status;

	MFnEnumAttribute enuAttr;

	foamType = enuAttr.create("foamType","ft",0,&status);
	enuAttr.addField("Circle",0);
	enuAttr.addField("Poly",1);
	CHECK_MSTATUS(status);
	MAKE_INPUT(enuAttr);

    aOutColor = nAttr.createColor("outColor", "ocolor");
	MAKE_OUTPUT(nAttr);

    aOutAlpha = nAttr.create( "outAlpha", "oalpha", MFnNumericData::kFloat);
	MAKE_OUTPUT(nAttr);

	//-------------------------unitAttr
	MFnUnitAttribute uAttr;

	startTime = uAttr.create("startTime","sT",MFnUnitAttribute::kTime,0,&status);
	CHECK_MSTATUS(status);
	MAKE_INPUT(uAttr);
    currentTime = uAttr.create("currentTime","cT",MFnUnitAttribute::kTime,0,&status);
	CHECK_MSTATUS(status);
	MAKE_INPUT(uAttr);

	sampleR = nAttr.create("sampleRadius", "sara", MFnNumericData::kFloat,1 );
    MAKE_INPUT(nAttr);
	nAttr.setRenderSource(true);
	samplePercent = nAttr.create("samplePercent", "samper", MFnNumericData::kFloat,0.7f );
    MAKE_INPUT(nAttr);	
	nAttr.setRenderSource(true);

	MObject child1 = nAttr.create( "baseWidth", "bw", MFnNumericData::kFloat,10,&status);
	CHECK_MSTATUS( status );
	nAttr.setMin(0);
	nAttr.setRenderSource(false);
    MObject child2 = nAttr.create( "baseHeight", "bh", MFnNumericData::kFloat,10,&status);
	CHECK_MSTATUS( status );
	nAttr.setMin(0);
	nAttr.setRenderSource(false);
    baseBound = nAttr.create( "baseBound","babo", child1, child2);
	MAKE_INPUT(nAttr);
	nAttr.setRenderSource(false);

	//-----------tur attr
	turbulence = nAttr.create("turbulence", "tur", MFnNumericData::kBoolean,true );
    MAKE_INPUT(nAttr);
	nAttr.setRenderSource(false);

	turFrequency = nAttr.create("turbulenceFrequency", "turf", MFnNumericData::kFloat,5 );
    MAKE_INPUT(nAttr);
	nAttr.setRenderSource(true);
	
	turPower = nAttr.create("turbulencePower", "turpow", MFnNumericData::kFloat,0.1f );
    MAKE_INPUT(nAttr);
	nAttr.setRenderSource(true);
	
	turRoughness = nAttr.create("turbulenceRoughness", "turrou", MFnNumericData::kInt,1 );
    MAKE_INPUT(nAttr);
	nAttr.setRenderSource(false);

	turSeed = nAttr.create("turbulenceSeed", "turr", MFnNumericData::kInt,143 );
    MAKE_INPUT(nAttr);
	nAttr.setRenderSource(false);

	child1 = nAttr.create( "uCoordinate", "uc", MFnNumericData::kFloat,0,&status);
	CHECK_MSTATUS( status );
	nAttr.setRenderSource(true);
    child2 = nAttr.create( "vCoordinate", "vc", MFnNumericData::kFloat,0,&status);
	CHECK_MSTATUS( status );
	nAttr.setRenderSource(true);

    aUVCoord = nAttr.create( "uvCoordinate","uvc", child1, child2);
	nAttr.setRenderSource(true);
	MAKE_INPUT(nAttr);
    CHECK_MSTATUS(nAttr.setHidden(true) );

	MVectorArray defaultVectArray;
    MFnVectorArrayData vectArrayDataFn;
	vectArrayDataFn.create( defaultVectArray );
	posArray = typAttr.create( "positionArray", "pa", MFnData::kVectorArray,vectArrayDataFn.object());
	typAttr.setHidden(true);

	MFnDoubleArrayData doubArrayDataFn;
	doubArrayDataFn.create(&status);
    CHECK_MSTATUS( status );
	radiusArray = typAttr.create( "radiusArray", "rarray", MFnData::kDoubleArray,doubArrayDataFn.object(),&status);
	CHECK_MSTATUS( status );
	MAKE_INPUT(typAttr);
	CHECK_MSTATUS( typAttr.setHidden(true) );

	// Add attributes to the node database.
	CHECK_MSTATUS( addAttribute(foamType) );
	CHECK_MSTATUS( addAttribute(posArray) );
	CHECK_MSTATUS( addAttribute(radiusArray) );
	CHECK_MSTATUS( addAttribute(sampleR) );
	CHECK_MSTATUS( addAttribute(samplePercent) );	

	CHECK_MSTATUS( addAttribute(baseBound) );
	CHECK_MSTATUS( addAttribute(aUVCoord) );
	
	CHECK_MSTATUS( addAttribute(turbulence) );
	CHECK_MSTATUS( addAttribute(turFrequency) );
	CHECK_MSTATUS( addAttribute(turPower) );
	CHECK_MSTATUS( addAttribute(turRoughness) );
	CHECK_MSTATUS( addAttribute(turSeed) );

    CHECK_MSTATUS( addAttribute(aOutAlpha) );
    CHECK_MSTATUS( addAttribute(aOutColor) );

    CHECK_MSTATUS( addAttribute(startTime) );
    CHECK_MSTATUS( addAttribute(currentTime) );

    // All input affect the output color and alpha
    CHECK_MSTATUS( attributeAffects (aUVCoord, aOutColor) );
    CHECK_MSTATUS( attributeAffects (baseBound, aOutColor) );

    CHECK_MSTATUS( attributeAffects (aUVCoord, aOutAlpha) );
    CHECK_MSTATUS( attributeAffects (baseBound, aOutAlpha) );

    CHECK_MSTATUS( attributeAffects (posArray, aOutColor) );
    CHECK_MSTATUS( attributeAffects (posArray, aOutAlpha) );

	CHECK_MSTATUS( attributeAffects (radiusArray, aOutColor) );
    CHECK_MSTATUS( attributeAffects (radiusArray, aOutAlpha) );

	CHECK_MSTATUS( attributeAffects (sampleR, aOutColor) );
    CHECK_MSTATUS( attributeAffects (sampleR, aOutAlpha) );

	CHECK_MSTATUS( attributeAffects (currentTime, aOutColor) );
    CHECK_MSTATUS( attributeAffects (currentTime, aOutAlpha) );

	CHECK_MSTATUS( attributeAffects (startTime, aOutColor) );
    CHECK_MSTATUS( attributeAffects (startTime, aOutAlpha) );

    CHECK_MSTATUS( attributeAffects (turFrequency, aOutColor) );
    CHECK_MSTATUS( attributeAffects (turFrequency, aOutAlpha) );

	CHECK_MSTATUS( attributeAffects (turPower, aOutColor) );
    CHECK_MSTATUS( attributeAffects (turPower, aOutAlpha) );

	CHECK_MSTATUS( attributeAffects (turRoughness, aOutColor) );
    CHECK_MSTATUS( attributeAffects (turRoughness, aOutAlpha) );

	CHECK_MSTATUS( attributeAffects (turSeed, aOutColor) );
    CHECK_MSTATUS( attributeAffects (turSeed, aOutAlpha) );

	CHECK_MSTATUS( attributeAffects (samplePercent, aOutColor) );
    CHECK_MSTATUS( attributeAffects (samplePercent, aOutAlpha) );

	CHECK_MSTATUS( attributeAffects (foamType, aOutColor) );
    CHECK_MSTATUS( attributeAffects (foamType, aOutAlpha) );
	
	CHECK_MSTATUS( attributeAffects (turbulence, aOutColor) );
    CHECK_MSTATUS( attributeAffects (turbulence, aOutAlpha) );

    return MS::kSuccess;
}

MStatus cjFoam::compute(const MPlug& plug, MDataBlock& block) 
{
	MStatus status;

    if ( (plug != aOutColor) && (plug.parent() != aOutColor) &&
         (plug != aOutAlpha)
       ) 
       return MS::kUnknownParameter;
	
	MDataHandle outColorHandle = block.outputValue( aOutColor ,&status);
	CHECK_MSTATUS(status);
	MFloatVector& outColor = outColorHandle.asFloatVector();

	MDataHandle outAlphaHandle = block.outputValue(aOutAlpha ,&status);
	CHECK_MSTATUS(status);
	float& outAlpha = outAlphaHandle.asFloat() ;

	float finalColor = 0,finalAlpha=0;

	//-----------------------------检测是否有粒子连接
	MObject posObj  = block.inputValue(posArray,&status).data();
	CHECK_MSTATUS(status);
	MFnVectorArrayData dataVectorArrayFn(posObj);
	
	MObject radiusObj   = block.inputValue(radiusArray,&status).data();
	CHECK_MSTATUS(status);
	MFnDoubleArrayData radiusArrayFn(radiusObj);
	
	//-----------------
	MVectorArray inPosAry;
	CHECK_MSTATUS(inPosAry.clear());
	inPosAry = dataVectorArrayFn.array(&status);
	CHECK_MSTATUS(status);
	int posArysize = (int)inPosAry.length();//粒子数量
	
	MDoubleArray radiusAry;
	CHECK_MSTATUS(radiusAry.clear());
	radiusAry = radiusArrayFn.array(&status);
	CHECK_MSTATUS(status);
	int radiusArySize = (int)radiusAry.length();//粒子数量
	
	//------------------------------获得必须的每粒子属性
	if( posArysize == radiusArySize &&  
		radiusArySize != 0 && 
		posArysize != 0 )
	{	
			
		//-------------------------------获取时间属性
		//---start time
		MDataHandle StartTime_Hnd      = block.inputValue(startTime,&status); 
		CHECK_MSTATUS(status);
		MTime      StartTime_D         = StartTime_Hnd.asTime();
		double     StartTime_Data      = StartTime_D.as(MTime::kSeconds);

		//---current time
		MDataHandle CurrentTime_Hnd    = block.inputValue(currentTime,&status); 
		CHECK_MSTATUS(status);
		MTime      CurrentTime_D       = CurrentTime_Hnd.asTime();
		double     CurrentTime_Data    = CurrentTime_D.as(MTime::kSeconds);
		
		if( CurrentTime_Data >= StartTime_Data )
		{	
			MDataHandle Type_Hnd           = block.inputValue(foamType,&status);               
			CHECK_MSTATUS(status);
			short         Type_Data        = Type_Hnd.asShort();

			//---------------变形属性
			bool turData = block.inputValue(turbulence).asBool();

			float turF = block.inputValue(turFrequency).asFloat();

			float turP = block.inputValue(turPower).asFloat();

			int turR = block.inputValue(turRoughness).asInt();

			int turS = block.inputValue(turSeed).asInt();
			
			//----------------得到采样半径、抗锯齿
			float sampleRadiusData   = block.inputValue( sampleR  ,&status).asFloat();
			CHECK_MSTATUS(status);
			float samplePerData   = block.inputValue( samplePercent  ,&status).asFloat();
			CHECK_MSTATUS(status);
			
			//-----------------得到uv及边界
			float2 & uv   = block.inputValue( aUVCoord  ,&status).asFloat2();
			CHECK_MSTATUS(status);
			uv[0] -= floorf(uv[0]);
			uv[1] -= floorf(uv[1]);

			float2 & bound  = block.inputValue( baseBound ,&status).asFloat2();
			CHECK_MSTATUS(status);
			float leftEdge  = -bound[0]/2.0f; //z
			float lowEdge   = -bound[1]/2.0f; //x
			MFloatPoint samplePoint( (lowEdge + uv[1]*bound[1]), 0, (leftEdge + uv[0]*bound[0]) );		
			
			if(turData)
			{
				distort::setSeed(turS);

				distort::SetTurRoughness(turR);

				distort::SetTurFrequency(turF,turF,turF);
				
				distort::SetTurPower(turP,turP,turP);
				
				distort::getValue( samplePoint.x,samplePoint.y,samplePoint.z );
			}

			//----------------------------------------------
			finalColor = getColor(  samplePoint,
									inPosAry,
									radiusAry,
									Type_Data,
									samplePerData,
									sampleRadiusData);

			if( finalColor >1 ) finalColor=1;

			finalAlpha = finalColor ;
		}//end if( CurrentTime_Data >= StartTime_Data )
	}//end if(  pSys.isValid() )

    outColor = MFloatVector(finalColor,finalColor,finalColor) ;
    outColorHandle.setClean();

    outAlpha = finalAlpha ;
    outAlphaHandle.setClean();

    return MS::kSuccess;
}

void    cjFoam::draw(M3dView &view, 
						 const MDagPath &path, 
						 M3dView::DisplayStyle style, 
						 M3dView::DisplayStatus)
{
	MObject thisNode = thisMObject();
    MPlug plug(thisNode, baseBound);
    float width,height;

    plug.child(0).getValue(width);
    plug.child(1).getValue(height);
	
	view.beginGL(); 
	glPushAttrib(GL_CURRENT_BIT);

		glBegin(GL_LINES);
			glColor3f(1,0,0);
			glVertex3f( -height/2.0f,0,-width/2.0f);
			glVertex3f( -height/2.0f,0,width/2.0f);

			glColor3f(0,0,1);
			glVertex3f( -height/2.0f,0,width/2.0f);
			glVertex3f( height/2.0f,0,width/2.0f);
			
			glVertex3f( height/2.0f,0,width/2.0f);
			glVertex3f( height/2.0f,0,-width/2.0f);
			
			glColor3f(0,1,0);
			glVertex3f( height/2.0f,0,-width/2.0f);
			glVertex3f( -height/2.0f,0,-width/2.0f);
		glEnd();

	glPopAttrib();
	view.endGL();
}


bool  cjFoam::isBounded()
{
	return false;
}

float  cjFoam::getColor(  const MFloatPoint &spt,
						  const MVectorArray& posAry,
						  const MDoubleArray& radius,
						  short ps,
						  float sp,
						  float spr)
{
	float angle=0,newdist=0,dist=0;

	MFloatPoint particlePoint;

	MFloatVector s2p ; 

	float result = 0;

	for( unsigned i = 0;i< radius.length();i++)
	{
		particlePoint = MFloatPoint( (float)posAry[i].x,0,(float)posAry[i].z );
		
		s2p = spt - particlePoint;						
		
		dist = s2p.length();

		newdist = spr * (float)radius[i];

		if( ps ==1 )
		{
			angle = s2p.angle( MFloatVector(1,0,0) );

			//-------得到正六边形计算夹角
			angle = fmod( angle,SixtyDgree );
			
			if( angle > ThirtyDgree )
				angle -= ThirtyDgree;
			else if( angle < ThirtyDgree )
				angle = ThirtyDgree - angle;

			newdist /= cos( angle );
		}
		
		if( dist <= newdist )
			result += (float)( 1 - smoothstep( newdist * sp,newdist,dist ) ) ;
	}//end for(unsigned int i = 0;i< arysize;i++)

	return result;
}