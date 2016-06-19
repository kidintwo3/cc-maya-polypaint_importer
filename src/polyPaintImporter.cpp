//
// ==========================================================================
// Copyright 2016 Creative Case / casecreativevfx@gmail.com
//
// Provided as is, with no warranty.
// Free to distribute, copy and modify
// ==========================================================================
//
// Usage (MEL command):
//						- Select object from zbrush
//						- Type in the MEL command (name of the source obj):
//						- polyPaintImporter "C:/test.OBJ"
//
// ==========================================================================
//
// PS: If you compile it or improve it send it to me I'll upload it to Gumroad
//



#include <maya/MIOStream.h>

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>



#include <maya/MSimple.h>
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
#include <maya/MPlug.h>

using namespace std;

DeclareSimpleCommand( polyPaintImporter,  "Creative Case" , "0.3");



MPlug findPlug(MDagPath pathName, MString plugName)
{
    MDagPath m_currShape = pathName;
    m_currShape.extendToShape();
    
    MFnDagNode fn_DagMod( m_currShape );
    MPlug plug_currPlugArray(fn_DagMod.findPlug( plugName ));
    
    MPlug plug_currPlug;
    
    if (plug_currPlugArray.isArray() == true){
        int count = plug_currPlugArray.numConnectedElements();
        plug_currPlug = plug_currPlugArray.elementByLogicalIndex(count);
    }
    
    if (plug_currPlugArray.isArray() == false){
        plug_currPlug = plug_currPlugArray;
    }
    
    return plug_currPlug;
}

MDagPath getCurrSelectionTransformDAG()
{
	MSelectionList selectedObjects;
	MDagPath mDagPath;
	MGlobal::getActiveSelectionList(selectedObjects);

	for (unsigned int i = 0; i < selectedObjects.length(); i++)
	{
		selectedObjects.getDagPath(i, mDagPath);

		if (mDagPath.apiType() == MFn::kTransform)
		{
			return mDagPath;
		}
	}



	return mDagPath;
}

MStatus getShapeNodeFromTransformDAG(MDagPath& path)
{
	MStatus status;

	if (path.apiType() == MFn::kMesh)
	{
		return MS::kSuccess;
	}

	unsigned int numShapes;
	status = path.numberOfShapesDirectlyBelow(numShapes);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	for (unsigned int i = 0; i < numShapes; ++i)
	{
		status = path.extendToShapeDirectlyBelow(i);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		if (!path.hasFn(MFn::kMesh))
		{
			path.pop();
			continue;
		}

		MFnDagNode fnNode(path, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);
		if (!fnNode.isIntermediateObject())
		{
			return MS::kSuccess;
		}
		path.pop();
	}

	MGlobal::displayWarning(MString() + "[PolyPaintImporter] Selection is not a mesh");

	return MS::kFailure;

}

MStatus polyPaintImporter::doIt( const MArgList& args)
{
	MStatus status;


	MSelectionList selectionList;
	MDagPath path;
	MStringArray selectionStrings;
	MStringArray argList;
	MString selectionString;
	MColor currCol;
	MColorArray colors;
	MIntArray vertexList;

	path = getCurrSelectionTransformDAG();

	MGlobal::displayInfo("[PolyPaintImporter] Selected " + path.partialPathName());

	getShapeNodeFromTransformDAG(path);


	MFnMesh mFn(path);

	selectionList.getSelectionStrings(selectionStrings);
	MString objPath = args.asString(0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	if (objPath.length() ==0){
		MGlobal::displayWarning("[PolyPaintImporter] No path found");
		return MStatus::kFailure;
	}

	MPlug pDisplColor = findPlug(path, "displayColors");
	pDisplColor.setBool(true);


	// MGlobal::displayInfo("[PolyPaintImporter] Opening file.. " + objPath.length());

	if (objPath.length() !=0){

		string line;
		int found;
		int id = 0;
		ifstream myfile( objPath.asChar() );

		if (myfile.is_open())
		{



			MGlobal::displayInfo("[PolyPaintImporter] Reading file...");

			while ( getline (myfile,line) )
			{
				if ((found = line.find("#MRGB")) != line.npos)
				{

					string currSentence = line.substr(6,line.length());

					for(int x=0; x < currSentence.length()-1; x+=8){

						string currString = currSentence.substr(x,8);

						string r = currString.substr(2,2);
						string g = currString.substr(4,2);
						string b = currString.substr(6,2);

						unsigned int r1;
						unsigned int g1;
						unsigned int b1;
						stringstream ss;
						ss << hex << r;
						ss >> r1;
						stringstream ss1;
						ss1 << hex << g;
						ss1 >> g1;
						stringstream ss2;
						ss2 << hex << b;
						ss2 >> b1;

						MColor currCol(r1/255.0f,g1/255.0f,b1/255.0f,1.0f);


						colors.append(currCol);
						vertexList.append(id);

						id++;
					}




				}
			}
			myfile.close();
		}

	}

	if (colors.length() != 0)
	{
		MGlobal::displayInfo("[PolyPaintImporter] coloring mesh...");
		status = mFn.setVertexColors(colors,vertexList);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MGlobal::displayInfo("[PolyPaintImporter] Done");
	}



	return MS::kSuccess;

}
