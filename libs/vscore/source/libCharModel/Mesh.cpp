
#include "Mesh.h"
#include "Logger.h"
#include "MorphTarget.h"
#include "Skeleton.h"
#include "Joint.h"

namespace VisageSDK
{

Mesh::Mesh( const string& id )
{
	this->id = id;
	parent = NULL;
}

Mesh::~Mesh()
{
	for( int i = 0; i < numberOfMorphTargets(); ++i )
		delete morphTargets[i];
}

Joint* Mesh::getParent() const
{
	return parent;
}

MorphTarget* Mesh::createMorphTarget( const string& mtId )
{
	if( morphTargetsById.count(mtId) > 0 )
		return NULL;

	MorphTarget* mt = new MorphTarget( mtId, this );
	morphTargets.push_back(mt);
	morphTargetsById.insert( make_pair( mtId, mt ) );

	return mt;
}

void Mesh::deleteMorphTargetByIndex( int mtIndex )
{
	V_ASSERT( mtIndex >= 0 && mtIndex < numberOfMorphTargets() );

	deleteMorphTargetById( morphTargets[mtIndex]->getId() );
}

void Mesh::deleteMorphTargetById( const string& mtId )
{
	if( !hasMorphTarget(mtId) )
		return;

	// delete the mt
	MorphTarget* mt = morphTargetsById[mtId];
	for( vector<MorphTarget*>::iterator i = morphTargets.begin(); i != morphTargets.end(); ++i )
		if( *i == mt )
		{
			morphTargets.erase(i);
			break;
		}
	morphTargetsById.erase(mtId);
	delete mt;
}

bool Mesh::hasMorphTarget( const string& mtId ) const
{
	if( morphTargetsById.count(mtId) > 0 )
		return true;

	return false;
}

MorphTarget* Mesh::getMorphTargetByIndex( int mtIndex ) const
{
	V_ASSERT( mtIndex >= 0 && mtIndex < numberOfMorphTargets() );

	return morphTargets[mtIndex];
}

MorphTarget* Mesh::getMorphTargetById( const string& mtId ) const
{
	if( !hasMorphTarget(mtId) )
		return NULL;

	return morphTargetsById.find(mtId)->second;
}

int Mesh::numberOfMorphTargets() const
{
	return (int)morphTargets.size();
}

void Mesh::resetMorphTargetWeights()
{
	for( int i = 0; i < numberOfMorphTargets(); ++i )
		getMorphTargetByIndex(i)->setWeight(0);
}

}
