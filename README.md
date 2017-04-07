# cc-maya-polypaint_importer
Polypaint Importer for Maya

A free Zbrush Polypaint importer plug-in for Maya
---------------------------------------------------------------------------
Provided as is, with no warranty. Free to distribute, copy and modify
--------------------------------------------------------------------
Copyright 2016 Creative Case / casecreativevfx@gmail.com

Usage (Python command):
----------------------

- Select object from zbrush
- Type in the Python command (name of the source obj):

	import maya.cmds as cmds;
	
	cmds.polyPaintImporter("D:/vertcol.OBJ", gamma=2.2);
