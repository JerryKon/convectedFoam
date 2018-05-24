#include "cjpc.h"

#include <maya/MArgDatabase.h>

#include <maya/MDGModifier.h>

#include <maya/MPlug.h>

#include <maya/MGlobal.h>

#include <maya/MSelectionList.h>

#include <maya/MDoubleArray.h>

#include <maya/MVectorArray.h>

#include <maya/MFnDependencyNode.h>

#include <maya/MFnVectorArrayData.h>

#include <math.h>

#include <iostream>

#include <fstream>

#include <stdlib.h>

using namespace std;

void cjpc::writeCache( const char* pathName,const MVectorArray &pos,const MDoubleArray &radius )
{
	ofstream os;
	os.open( pathName,ios_base::binary|ios::out );
	
	int length = (int)pos.length();

	//---------写入粒子数目
	os.write( (char*) &length,4);
	
	//---------写入粒子位置
	for( int i=0;i<length;i++ )
	{
		os.write( (char*) &pos[i].x,8 );
		os.write( (char*) &pos[i].y,8 );
		os.write( (char*) &pos[i].z,8 );
	}
	
	//--------写入粒子foamRadius数据
	for( int i=0;i<length;i++ )
	{
		double a = radius[i];

		os.write( (char*) &a,8 );
	}

	os.close();
}

cjpc::cjpc()

{
	time = 0;

	pathN.clear();

	particleN.clear();

	pSys.setObject(MObject::kNullObj);

}

MStatus cjpc::getRadiusPP( MDoubleArray& radiusAry )
{
	MStatus status;

	MString foamRadius("foamRadiusPP");

	if( !pSys.isPerParticleDoubleAttribute(foamRadius,&status) )
		return MS::kFailure;

	CHECK_MSTATUS(status);

	int pn = pSys.getPerParticleAttribute(foamRadius,radiusAry,&status);

	CHECK_MSTATUS(status);

	return status;

}

MStatus cjpc::getPosPP( MVectorArray& posAry ,const MObject &obj,const MFnDependencyNode &dgfn )
{
	MStatus status;

	MObject posObj;

	MPlug posPlug = dgfn.findPlug("position",false,&status);

	posPlug.getValue( posObj );

	MFnVectorArrayData dataVectorArrayFn(posObj);

	posAry = dataVectorArrayFn.array(&status);

	CHECK_MSTATUS(status);

	return status;

}

cjpc::~cjpc()

{}

void *cjpc::creator()

{
   return new cjpc;
}

MSyntax cjpc::newSyntax()

{

   MSyntax syntax;

   CHECK_MSTATUS( syntax.addFlag("-t", "-time", MSyntax::kLong) );
	
   CHECK_MSTATUS( syntax.addFlag("-p", "-path", MSyntax::kString) );

   CHECK_MSTATUS( syntax.addFlag("-n", "-name", MSyntax::kString) );
   
   return syntax;

}

bool cjpc::isUndoable() const
{
   return false;
}


MStatus cjpc::doIt(const MArgList& args)

{
	MStatus status;

	MArgDatabase argData(syntax(), args);

	if( argData.isFlagSet("-time") )
	{

	  CHECK_MSTATUS( argData.getFlagArgument("-time", 0, time) );

	} 
	
	if( argData.isFlagSet( "-path") )
	{

		CHECK_MSTATUS( argData.getFlagArgument("-path", 0, pathN) );

	}

	if( argData.isFlagSet( "-name" ) )
	{

		CHECK_MSTATUS( argData.getFlagArgument("-name", 0, particleN) );

	}

	MSelectionList selList;

	MObject particleNode; 

	MFnDependencyNode fnDG;

	selList.clear();

	selList.add( particleN );
	
	selList.getDependNode( 0 , particleNode );

	fnDG.setObject( particleNode );
	
	MString type = fnDG.typeName();

	if( type != "particle" && type != "nParticle")
	{

		displayError( " you let me down,it's not a particle!" );
		
		return MS::kFailure;
	}

	MVectorArray particlePos;

	CHECK_MSTATUS(particlePos.clear());

	MDoubleArray particleRadius;

	CHECK_MSTATUS(particleRadius.clear());

	CHECK_MSTATUS( getPosPP( particlePos ,particleNode,fnDG ) );

	CHECK_MSTATUS( pSys.setObject( particleNode ) );

	CHECK_MSTATUS( getRadiusPP( particleRadius ) );

	MString pathName;

	pathName =( pathN + "." + time + ".jpc" );

	const char* p = pathName.asChar();

	writeCache( p,particlePos,particleRadius );

	return MS::kSuccess;
}