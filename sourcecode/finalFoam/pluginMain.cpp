
#include "cellFieldNode.h"
#include "cellShader.h"
#include "cjFoam.h"
#include "cjpc.h"
#include "foamCache.h"

#include <maya/MFnPlugin.h>
#include <maya/MGlobal.h>

MStatus initializePlugin( MObject obj )

{ 
	MStatus   status;
	MFnPlugin plugin( obj, "jerry", "1.0", "Any");

    const MString UserClassify( "texture/3d" );

	MString command( "if( `window -exists createRenderNodeWindow` ) {refreshCreateRenderNodeWindow(\"" );
	
	CHECK_MSTATUS( plugin.registerNode("CJCell", Cell3D::id,
						Cell3D::creator, Cell3D::initialize,
						MPxNode::kDependNode, &UserClassify) );

    CHECK_MSTATUS( plugin.registerNode("CJFoam", cjFoam::id,
						cjFoam::creator, cjFoam::initialize,
						MPxNode::kLocatorNode) );

    CHECK_MSTATUS( plugin.registerNode("CJFCache", foamCache::id,
						foamCache::creator, foamCache::initialize) );

	CHECK_MSTATUS( plugin.registerNode( "CJCellField", cellField::id, cellField::creator,
								  cellField::initialize,MPxNode::kFieldNode ) );

	CHECK_MSTATUS( plugin.registerCommand("foamCache", cjpc::creator,cjpc::newSyntax) );

	//--------------------
	MString command1 = command + UserClassify;
    command1 += "\");}\n";

    CHECK_MSTATUS( MGlobal::executeCommand( command1 ) );

	std::cout<<"Foam: version 1.0 for Maya 10.0 loaded."<<endl;
	
	CHECK_MSTATUS( MGlobal::executeCommand( "foamMenu" ) );

	return MS::kSuccess;
}

MStatus uninitializePlugin( MObject obj)

{
	MStatus   status;
	MFnPlugin plugin( obj );

	const MString UserClassify( "texture/3d" );

	MString command( "if( `window -exists createRenderNodeWindow` ) {refreshCreateRenderNodeWindow(\"" );

	CHECK_MSTATUS( plugin.deregisterNode( cellField::id ) );

    CHECK_MSTATUS( plugin.deregisterNode( Cell3D::id ) );
   
	CHECK_MSTATUS( plugin.deregisterNode( cjFoam::id ) );

	CHECK_MSTATUS( plugin.deregisterNode( foamCache::id ) );

	CHECK_MSTATUS( plugin.deregisterCommand("foamCache" ) );

	MString command1 = command + UserClassify;
    command1 += "\");}\n";

	return MS::kSuccess;
}
