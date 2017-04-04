//
//  pluginMain.cpp
//  shellMod
//
//  Created by Creative Case on 2015. 01. 03.
//  Copyright (c) 2015. Creative Case. All rights reserved.
//


#include "polyPaintImporter.h"

#include <maya/MFnPlugin.h>

MStatus initializePlugin( MObject obj )
{
	MStatus status;
    
	//icons_data_write();

	//MGlobal::executeCommand( mel_createShelf() );

	MFnPlugin fnPlugin( obj, "Creative Case", "0.5", "Any" );

	status = fnPlugin.registerCommand( "polyPaintImporter", polyPaintImporter::creator, polyPaintImporter::newSyntax );
	CHECK_MSTATUS_AND_RETURN_IT( status );
    
	return MS::kSuccess;
}


MStatus uninitializePlugin( MObject obj )
{
	MStatus status;




	//MGlobal::executeCommand( mel_deleteShelf() );




	MFnPlugin fnPlugin( obj );

	status = fnPlugin.deregisterCommand( "polyPaintImporter" );
	CHECK_MSTATUS_AND_RETURN_IT( status );
    
	return MS::kSuccess;
}

