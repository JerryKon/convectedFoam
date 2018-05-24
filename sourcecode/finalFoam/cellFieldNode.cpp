
#include "cellFieldNode.h"

#include <math.h>
#include <maya/MPlug.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnVectorArrayData.h>
#include <maya/MFnDoubleArrayData.h>
#include <maya/MGlobal.h>

MTypeId     cellField::id( 0x81019 );

MObject     cellField::constraintY;        
MObject     cellField::phase;        
MObject     cellField::frequency; 
MObject     cellField::jitter;        
MObject     cellField::seed; 
MObject     cellField::aNearValue;        
MObject     cellField::aFarValue; 
MObject		cellField::aPower;

#define McheckErr(stat, msg)		\
	if ( MS::kSuccess != stat )		\
	{								\
		cerr << msg;				\
		return MS::kFailure;		\
	}


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

cellField::cellField() {}
cellField::~cellField() {}

MStatus cellField::compute( const MPlug& plug, MDataBlock& block )
{
	MStatus status;
 
	if( !(plug == mOutputForce) )
        return( MS::kUnknownParameter );

	// get the logical index of the element this plug refers to.
	//
	int multiIndex = plug.logicalIndex( &status );
	McheckErr(status, "ERROR in plug.logicalIndex.\n");

	// Get input data handle, use outputArrayValue since we do not
	// want to evaluate both inputs, only the one related to the
	// requested multiIndex. Evaluating both inputs at once would cause
	// a dependency graph loop.
	//
	MArrayDataHandle hInputArray = block.outputArrayValue( mInputData, &status );
	McheckErr(status,"ERROR in hInputArray = block.outputArrayValue().\n");

	status = hInputArray.jumpToElement( multiIndex );
	McheckErr(status, "ERROR: hInputArray.jumpToElement failed.\n");

	// get children of aInputData.
	//
	MDataHandle hCompond = hInputArray.inputValue( &status );
	McheckErr(status, "ERROR in hCompond=hInputArray.inputValue\n");

	MDataHandle hPosition = hCompond.child( mInputPositions );
	MObject dPosition = hPosition.data();
	MFnVectorArrayData fnPosition( dPosition );
	MVectorArray points = fnPosition.array( &status );
	McheckErr(status, "ERROR in fnPosition.array(), not find points.\n");

	MDataHandle hVelocity = hCompond.child( mInputVelocities );
	MObject dVelocity = hVelocity.data();
	MFnVectorArrayData fnVelocity( dVelocity );
	MVectorArray velocities = fnVelocity.array( &status );
	McheckErr(status, "ERROR in fnVelocity.array(), not find velocities.\n");

	MDataHandle hMass = hCompond.child( mInputMass );
	MObject dMass = hMass.data();
	MFnDoubleArrayData fnMass( dMass );
	MDoubleArray masses = fnMass.array( &status );
	McheckErr(status, "ERROR in fnMass.array(), not find masses.\n");

	// ----------------------Compute the output force.

	MVectorArray forceArray;

	getForceArray(block,points,velocities,masses,forceArray);

	// get output data handle
	//
	MArrayDataHandle hOutArray = block.outputArrayValue( mOutputForce, &status);
	McheckErr(status, "ERROR in hOutArray = block.outputArrayValue.\n");
	MArrayDataBuilder bOutArray = hOutArray.builder( &status );
	McheckErr(status, "ERROR in bOutArray = hOutArray.builder.\n");

	// get output force array from block.
	//
	MDataHandle hOut = bOutArray.addElement(multiIndex, &status);
	McheckErr(status, "ERROR in hOut = bOutArray.addElement.\n");

	MFnVectorArrayData fnOutputForce;
	MObject dOutputForce = fnOutputForce.create( forceArray, &status );
	McheckErr(status, "ERROR in dOutputForce = fnOutputForce.create\n");

	// update data block with new output force data.
	//
	hOut.set( dOutputForce );
	block.setClean( plug );

	return MS::kSuccess;
}

void* cellField::creator()

{
	return new cellField();
}

MStatus cellField::initialize()
		
{

	MFnNumericAttribute nAttr;
	MStatus				stat;

	aNearValue = nAttr.create("nearValue", "nv", MFnNumericData::kDouble,0.8 );
    MAKE_INPUT(nAttr);

    aFarValue = nAttr.create("farValue", "fv", MFnNumericData::kDouble,0.0 );
    MAKE_INPUT(nAttr);

	aPower = nAttr.create("forcePower", "fpwer", MFnNumericData::kDouble,1.2 );
    MAKE_INPUT(nAttr);

	frequency = nAttr.create( "frequency", "freq", MFnNumericData::kDouble,1);
    MAKE_INPUT(nAttr);

	constraintY = nAttr.create( "constraintY", "cy", MFnNumericData::kBoolean,true);
    MAKE_INPUT(nAttr);

	phase = nAttr.create( "phase", "phase", MFnNumericData::k3Double,0);
    MAKE_INPUT(nAttr);
	
	jitter = nAttr.create( "jitter", "jit", MFnNumericData::kDouble,0.8);
    MAKE_INPUT(nAttr);
	nAttr.setMin(-1);
	nAttr.setMax(1);

	seed = nAttr.create( "seed", "seed", MFnNumericData::kInt,5);
	MAKE_INPUT(nAttr);

	CHECK_MSTATUS( addAttribute(aNearValue) );
    CHECK_MSTATUS( addAttribute(aFarValue) );
	CHECK_MSTATUS( addAttribute(aPower) );

	CHECK_MSTATUS( addAttribute(frequency) );
	CHECK_MSTATUS( addAttribute(jitter) );
	CHECK_MSTATUS( addAttribute(seed) );

	CHECK_MSTATUS( addAttribute(constraintY) );
	CHECK_MSTATUS( addAttribute(phase) );

    CHECK_MSTATUS( attributeAffects (aNearValue, mOutputForce) );
    CHECK_MSTATUS( attributeAffects (aFarValue, mOutputForce) );
    CHECK_MSTATUS( attributeAffects (frequency, mOutputForce) );
    CHECK_MSTATUS( attributeAffects (jitter, mOutputForce) );
    CHECK_MSTATUS( attributeAffects (seed, mOutputForce) );
    CHECK_MSTATUS( attributeAffects (constraintY, mOutputForce) );
    CHECK_MSTATUS( attributeAffects (phase, mOutputForce) );
    CHECK_MSTATUS( attributeAffects (aPower, mOutputForce) );
	return MS::kSuccess;

}

void cellField::getForceArray(MDataBlock& block,
			  const MVectorArray &points,
			  const MVectorArray &velocities,
			  const MDoubleArray &masses,
			  MVectorArray &outputForce)
{

	if( points.length() != velocities.length() )
		return;

	outputForce.clear();

	unsigned int particleSize = points.length();

	double nearV	    = block.inputValue(aNearValue).asDouble();

    double farV		    = block.inputValue(aFarValue).asDouble();
	
	double powerData = block.inputValue(aPower).asDouble();
	
	int cellSeed	= block.inputValue( seed ).asInt();

	double cellFrequency   = block.inputValue( frequency ).asDouble();

	double cellJitter	  = block.inputValue( jitter ).asDouble();
	
	bool constraint		  = block.inputValue( constraintY ).asBool();
	
	double3& phaseXYZ	  = block.inputValue( phase ).asDouble3();
	
	double mag			  = block.inputValue( mMagnitude ).asDouble();

	double atten		  = block.inputValue( mAttenuation ).asDouble();
	
	MVector force,point;
	double value,px,py,pz;

	jCell::setFrequency( cellFrequency );
	jCell::setJitter( cellJitter );
	jCell::setNearFar( nearV, farV );
	jCell::setSeed(cellSeed);

	if( !constraint )
	{
		for( unsigned int i=0;i<particleSize;i++ )
		{
			px = points[i].x + phaseXYZ[0];
			py = points[i].y + phaseXYZ[1];
			pz = points[i].z + phaseXYZ[2];

			value = jCell::GetValue(px,py,pz,force);

			value = pow(value,(double)powerData);

		    force = force.normal() * ( 1 - value );

			force *= mag;

			outputForce.append( force );

		}
	}
	else
	{
		py = phaseXYZ[1];

		for( unsigned int i=0;i<particleSize;i++ )
		{
			px = points[i].x + phaseXYZ[0];
			pz = points[i].z + phaseXYZ[2];

			value = jCell::GetValue(px,py,pz,force);

			value = pow(value,(double)powerData);

		    force = force.normal() * ( 1 - value );

			force *= mag;

			force.y = 0;

			outputForce.append( force );

		}
	}

}