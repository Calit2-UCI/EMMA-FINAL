
#include "MorphTarget.h"
#include "Logger.h"
#include "Mesh.h"

namespace VisageSDK
{

MorphTarget::MorphTarget( const string& id, Mesh* owner )
{
	this->id = id;
	this->owner = owner;
}

MorphTarget::~MorphTarget()
{
}

float MorphTarget::getWeight() const
{
	return weight;
}

void MorphTarget::setWeight( float weight )
{
	this->weight = weight;
}

}
