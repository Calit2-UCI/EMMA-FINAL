#include "stdafx.h"
#include "NodeShape.h"
#include "SFNode.h"
#include "ExposedField.h"

NodeShape::NodeShape(Scene *scene, Proto *proto):
	Node(scene, proto)
{
    NaradaNotificationCenter::postNotification(naradaNotificationLoadProgressReport);
}

/*
void NodeShape::draw()
{
}

bool NodeShape::isLit() const
{
	SFNode *shapeField = (SFNode *) getField(0);
	Node *appearance = shapeField->getValue();
	SFNode *appearanceField = (SFNode *) getField(0);
	Node *material = appearanceField->getValue();

	if (material == NULL) return false;
	else return true;
}

void NodeShape::doSpecularPass(Node *appearance, Node *geometry)
{
}
*/

ProtoShape::ProtoShape(Scene *scene):
	Proto(scene, "Shape")
{
	ExposedField *exposedField = new ExposedField(SFNODE,
		"appearance", new SFNode(NULL), -FLT_MAX, FLT_MAX,
		NODE_APPEARANCE);
	addExposedField(exposedField);

	exposedField = new ExposedField(SFNODE, "geometry",
		new SFNode(NULL), -FLT_MAX, FLT_MAX, NODE_GEOMETRY);
	addExposedField(exposedField);
}
