
#include "Skeleton.h"
#include "Logger.h"
#include "Joint.h"

namespace VisageSDK
{

Skeleton::Skeleton( const string& id )
{
	this->id = id;
	root = NULL;
}

Skeleton::~Skeleton()
{
	for( int i = 0; i < numberOfJoints(); ++i )
		delete joints[i];
}

void Skeleton::_init( const string& rootId )
{
	for( int i = 0; i < numberOfJoints(); ++i )
		delete joints[i];

	// create root joint
	Joint* joint = new Joint( rootId, this );
	joints.push_back(joint);
	jointsById.insert( make_pair( rootId, joint ) );
	root = joint;
}

Joint* Skeleton::createJoint( const string& jointId )
{
	V_ASSERT( root != NULL );

	if( jointsById.count(jointId) > 0 )
		return NULL; // joint already exists

	Joint* joint = new Joint( jointId, this );
	joints.push_back(joint);
	jointsById.insert( make_pair( jointId, joint ) );

	return joint;
}

void Skeleton::deleteJointByIndex( int jointIndex )
{
	V_ASSERT( root != NULL );
	V_ASSERT( jointIndex > 0 && jointIndex < numberOfJoints() );

	deleteJointById( joints[jointIndex]->getId() );
}

void Skeleton::deleteJointById( const string& jointId )
{
	V_ASSERT( root != NULL );
	V_ASSERT( root->getId() != jointId );

	if( !hasJoint(jointId) )
		return;

	// detach the joint from the hierarchy
	Joint* joint = getJointById(jointId);
	while( joint->numberOfChildren() > 0 )
		joint->removeChild(0);
	Joint* p_joint = joint->getParent();
	for( int i = 0; p_joint != NULL && i < p_joint->numberOfChildren(); ++i )
		if( p_joint->getChild(i) == joint )
		{
			p_joint->removeChild(i);
			break;
		}

	// delete the joint
	for( vector<Joint*>::iterator i = joints.begin(); i != joints.end(); ++i )
		if( *i == joint )
		{
			joints.erase(i);
			break;
		}
	jointsById.erase(jointId);
	delete joint;
}

void Skeleton::deleteAllJoints()
{
	while( numberOfJoints() > 1 )
		deleteJointByIndex(1);
}

bool Skeleton::hasJoint( const string& jointId ) const
{
	if( jointsById.count(jointId) > 0 )
		return true;

	return false;
}

Joint* Skeleton::getJointByIndex( int jointIndex ) const
{
	V_ASSERT( jointIndex >= 0 && jointIndex < numberOfJoints() );

	return joints[jointIndex];
}

Joint* Skeleton::getJointById( const string& jointId ) const
{
	if( !hasJoint(jointId) )
		return NULL;

	return jointsById.find(jointId)->second;
}

int Skeleton::numberOfJoints() const
{
	return (int)joints.size();
}

void Skeleton::resetToInitialPose()
{
	Joint* joint;

	for( int i = 0; i < numberOfJoints(); ++i )
	{
		joint = getJointByIndex(i);
		joint->setPosition( joint->getInitialPosition() );
		joint->setOrientation( joint->getInitialOrientation() );
		joint->setScale( joint->getInitialScale() );
	}
}

}
