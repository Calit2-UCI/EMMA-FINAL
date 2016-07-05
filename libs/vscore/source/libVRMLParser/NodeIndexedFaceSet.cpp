#include "stdafx.h"
#include "NodeIndexedFaceSet.h"
#include "SFNode.h"
#include "SFBool.h"
#include "MFInt32.h"
#include "SFFloat.h"
#include "ExposedField.h"
//#include "MFVec3f.h"
#include "Field.h"
//#include "NodeNormal.h"

NodeIndexedFaceSet::NodeIndexedFaceSet(Scene *scene, Proto *proto):
	Node(scene, proto)
{
}

NodeIndexedFaceSet::~NodeIndexedFaceSet()
{
}

void NodeIndexedFaceSet::setField(int index, FieldValue *value)
{
	assert((index >= 0) && (index < _numFields));
	if (_fields[index] != NULL) _fields[index]->unref();

	if (value->getType() == SFNODE)
	{
		SFNode *v = (SFNode *) value;
		Node *node = v->getValue();
		if (node) node->addParent(this, index);
	}

	_fields[index] = value;
}

/*
void NodeIndexedFaceSet::getNormals(MFVec3f *coords, MFInt32 *coordIndex)
{
}

void NodeIndexedFaceSet::generateNormals(MFVec3f *coords, MFInt32 *coordIndex)
{
}

void NodeIndexedFaceSet::smoothNormals(const MFVec3f *coords,
				  const MFInt32 *coordIndex, 
				  const Vec3f *faceNormals)
{
}

void NodeIndexedFaceSet::draw()
{
}
*/

ProtoIndexedFaceSet::ProtoIndexedFaceSet(Scene *scene):
	Proto(scene, "IndexedFaceSet")
{
	addEventIn(MFINT32, "set_colorIndex");
	addEventIn(MFINT32, "set_coordIndex");
	addEventIn(MFINT32, "set_normalIndex");
	addEventIn(MFINT32, "set_texCoordIndex");

	ExposedField *exposedField = new ExposedField(SFNODE,
		"color", new SFNode(NULL), -FLT_MAX, FLT_MAX, NODE_COLOR);
	addExposedField(exposedField);

	exposedField = new ExposedField(SFNODE, "coord",
		new SFNode(NULL), -FLT_MAX, FLT_MAX, NODE_COORDINATE);
	addExposedField(exposedField);

	exposedField = new ExposedField(SFNODE, "normal",
		new SFNode(NULL), -FLT_MAX, FLT_MAX, NODE_NORMAL);
	addExposedField(exposedField);

	exposedField = new ExposedField(SFNODE, "texCoord",
		new SFNode(NULL), -FLT_MAX, FLT_MAX,
		NODE_TEXTURE_COORDINATE);
	addExposedField(exposedField);

	addField(SFBOOL, "ccw", new SFBool(true));
	addField(MFINT32, "colorIndex", new MFInt32(), -1);
	addField(SFBOOL, "colorPerVertex", new SFBool(true));
	addField(SFBOOL, "convex", new SFBool(true));
	addField(MFINT32, "coordIndex", new MFInt32(), -1);
	addField(SFFLOAT, "creaseAngle", new SFFloat(0), 0);
	addField(MFINT32, "normalIndex", new MFInt32(), -1);
	addField(SFBOOL, "normalPerVertex", new SFBool(true));
	addField(SFBOOL, "solid", new SFBool(true));
	addField(MFINT32, "texCoordIndex", new MFInt32(), -1);
}
