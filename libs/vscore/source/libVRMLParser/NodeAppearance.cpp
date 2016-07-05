#include "stdafx.h"
#include "NodeAppearance.h"
#include "SFNode.h"
#include "ExposedField.h"

NodeAppearance::NodeAppearance(Scene *scene, Proto *proto):
	Node(scene, proto)
{
}

/*
void NodeAppearance::bind()
{
}

void NodeAppearance::unbind()
{
}
*/

ProtoAppearance::ProtoAppearance(Scene *scene): 
	Proto(scene, "Appearance")
{
	ExposedField *exposedField = new ExposedField(SFNODE,
		"material", new SFNode(NULL), -FLT_MAX, FLT_MAX,
		NODE_MATERIAL);
	addExposedField(exposedField);

	exposedField = new ExposedField(SFNODE, "texture",
		new SFNode(NULL), -FLT_MAX, FLT_MAX, NODE_TEXTURE);
	addExposedField(exposedField);

	exposedField = new ExposedField(SFNODE, "textureTransform",
		new SFNode(NULL), -FLT_MAX, FLT_MAX, NODE_TEXTURE_TRANSFORM);
	addExposedField(exposedField);
}
