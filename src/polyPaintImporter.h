//
//  ShellModCommand.h
//  shellMod
//
//  Created by Hunyadi János on 2015. 01. 03..
//  Copyright (c) 2015. Janos Hunyadi. All rights reserved.
//

#ifndef POLYPAINTIMPORTERCOMMAND_H
#define POLYPAINTIMPORTERCOMMAND_H

#ifdef __linux__ 
    #include <maya/MArgDatabase.h>
#else
    #include <maya/MArgDataBase.h>
#endif

#include <maya/MArgList.h>

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

#include <maya/MSelectionList.h>
#include <maya/MString.h>
#include <maya/MStringArray.h>
#include <maya/MColor.h>
#include <maya/MColorArray.h>
#include <maya/MIntArray.h>
#include <maya/MDagPath.h>
#include <maya/MGlobal.h>
#include <maya/MFnMesh.h>
#include <maya/MStatus.h>
#include <maya/MFnSet.h>

#include <maya/MSyntax.h>
#include <maya/MObject.h>

#include <maya/MGlobal.h>
#include <maya/MPlug.h>
#include <maya/MPxCommand.h>

using namespace std;

class polyPaintImporter : public MPxCommand
{
public:
    polyPaintImporter();
    virtual MStatus		doIt( const MArgList& argList );
    virtual MStatus		redoIt();
    virtual MStatus		undoIt();
    virtual bool		isUndoable() const;
    static void*		creator();
    static MSyntax		newSyntax();


private:

	MPlug				findPlug(MDagPath pathName, MString plugName);
	MDagPath			getCurrSelectionTransformDAG();
	MStatus				getShapeNodeFromTransformDAG(MDagPath& path);

	MDGModifier			m_DGMod;
	MFnDependencyNode	m_DEPNode;

	MColorArray			orig_colors;
	MColorArray			colors;

	MDagPath			path;
	MIntArray			vertexList;

};


#endif