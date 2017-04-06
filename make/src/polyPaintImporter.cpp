
 //==========================================================================
 //Copyright 2016 Creative Case / casecreativevfx@gmail.com

 //Provided as is, with no warranty.
 //Free to distribute, copy and modify
 //==========================================================================

 //Usage (Python command):
	//					- Select object from zbrush
	//					- Type in the Python command (name of the source obj):
	//					- cmds.polyPaintImporter("D:/vertcol.OBJ", gamma=2.2)

 //==========================================================================





#include "polyPaintImporter.h"

polyPaintImporter::polyPaintImporter()
{
}


void* polyPaintImporter::creator()
{
	return new polyPaintImporter;
}

bool polyPaintImporter::isUndoable() const
{
	return true;
}

MSyntax polyPaintImporter::newSyntax()
{
	MSyntax syntax;

	syntax.addFlag( "-ga", "-gamma", MSyntax::kDouble );
	syntax.setObjectType( MSyntax::kSelectionList, 1, 1 );

	syntax.useSelectionAsDefault( true );

	syntax.enableEdit( false );
	syntax.enableQuery( false );

	return syntax;
}


MPlug polyPaintImporter::findPlug(MDagPath pathName, MString plugName)
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

MDagPath polyPaintImporter::getCurrSelectionTransformDAG()
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

MStatus polyPaintImporter::getShapeNodeFromTransformDAG(MDagPath& path)
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


	// Read all the flag arguments
	MArgDatabase argData( syntax(), args, &status );
	//CHECK_MSTATUS_AND_RETURN_IT( status );

	// Variables
	MSelectionList selectionList;
	MStringArray selectionStrings;
	MStringArray argList;
	MString selectionString;
	MColor currCol;
	vertexList.clear();

	double gamma = 1.0;

	if ( argData.isFlagSet( "gamma" ) )
	{
		gamma = argData.flagArgumentDouble( "gamma", 0 ); 
		MGlobal::displayInfo(MString() + "[PolyPaintImporter] Gamma: " + gamma);
	}


	// Get current transform selection
	path = getCurrSelectionTransformDAG();

	MGlobal::displayInfo("[PolyPaintImporter] Selected " + path.partialPathName());


	// Get the shape node from the transofrm dag path
	getShapeNodeFromTransformDAG(path);


	// Get te currently selected objects as string
	selectionList.getSelectionStrings(selectionStrings);
	MString objPath = args.asString(0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// If not object is given
	if (objPath.length() ==0){
		MGlobal::displayWarning("[PolyPaintImporter] No path found");
		return MStatus::kFailure;
	}


	// Find the displaycolor plug
	MPlug pDisplColor = findPlug(path, "displayColors");
	pDisplColor.setBool(true);


	if (objPath.length() !=0){

		string line;
		int found;
		int id = 0;
		ifstream myfile( objPath.asChar() );

		if (myfile.is_open())
		{



			MGlobal::displayInfo("[PolyPaintImporter] Reading file...");


			// Read the obj file
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

						float rgb_r = float(r1)/255.0f;
						float rgb_g = float(g1)/255.0f;
						float rgb_b = float(b1)/255.0f;

						// Multiply by gamma
						rgb_r *= float(gamma);
						rgb_g *= float(gamma);
						rgb_b *= float(gamma);

						// Set the color values
						MColor currCol(rgb_r,rgb_g,rgb_b,1.0f);

						// Append to the list
						colors.append(currCol);
						vertexList.append(id);

						id++;
					}




				}
			}
			myfile.close();
		}

	}

	// Store original vertex color
	MFnMesh mFn(path);

	MGlobal::displayInfo(MString() + path.partialPathName());

	status = mFn.getVertexColors(orig_colors,NULL, NULL);

	// Apply the colors
	if (colors.length() != 0)
	{
		MGlobal::displayInfo("[PolyPaintImporter] coloring mesh...");
		status = mFn.setVertexColors(colors,vertexList);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MGlobal::displayInfo("[PolyPaintImporter] Done");
	}



	return redoIt();

}


MStatus polyPaintImporter::redoIt()
{
	MStatus status;


	// restore the original colors
	if (vertexList.length() != 0)
	{

		MFnMesh mFn(path);

		mFn.clearColors();

		status = mFn.setVertexColors(colors,vertexList);
		CHECK_MSTATUS_AND_RETURN_IT(status);
	}




	return MS::kSuccess;
}

MStatus polyPaintImporter::undoIt()
{
	MStatus status;

	// Restore the initial state
	status = m_DGMod.undoIt();
	CHECK_MSTATUS_AND_RETURN_IT( status );

	// restore the original colors
	MFnMesh mFn(path);

	mFn.clearColors();

	status = mFn.setVertexColors(orig_colors,vertexList);
	CHECK_MSTATUS_AND_RETURN_IT(status);


	return MS::kSuccess;
}