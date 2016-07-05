#include "stdafx.h"
#include "NodeGroup.h"
#include "MFNode.h"
#include "SFVec3f.h"
#include "ExposedField.h"
#include "Field.h"

NodeGroup::NodeGroup(Scene *scene, Proto *proto):
	Node(scene, proto)
{
}

// these two methods (predraw and draw) are left
// empty because we do not use them for drawing
/*
void NodeGroup::preDraw()
{
}

void NodeGroup::draw()
{
}
*/

ProtoGroup::ProtoGroup(Scene *scene):
	Proto(scene, "Group")
{
	addEventIn(MFNODE, "addChildren");
	addEventIn(MFNODE, "removeChildren");

	addExposedField(MFNODE, "children", new MFNode(), NODE_CHILD);
	ExposedField *exposedField = 
		getExposedField(_exposedFields.size() - 1);
	exposedField->getValue()->ref();
	// field's flags are set to FF_HIDDEN because this is a hidden field
	Field *field = new Field(MFNODE, "children",
		exposedField->getValue(), NULL,	-FLT_MAX, FLT_MAX,
		NODE_CHILD,	exposedField->getFlags() | FF_HIDDEN);
	exposedField->setField(_fields.size());
	_fields.append(field);

	addField(SFVEC3F, "bboxCenter", new SFVec3f(0, 0, 0));
	//  range is (0,infinity) or -1,-1,-1
	addField(SFVEC3F, "bboxSize", new SFVec3f(-1, -1, -1), 0);
}
