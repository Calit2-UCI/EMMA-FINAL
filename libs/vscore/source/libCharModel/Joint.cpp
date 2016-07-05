
#include "Joint.h"
#include "Logger.h"
#include "Math3D.h"
#include "Skeleton.h"
#include "Mesh.h"

namespace VisageSDK
{

Joint::Joint( const string& id, Skeleton* owner )
{
	this->id = id;
	this->owner = owner;
	parent = NULL;

	initPos = Vector3( 0, 0, 0 );
	initOrient = Quat( 0, 0, 0 );
	initScal = Vector3( 1, 1, 1 );
	pos = initPos;
	orient = initOrient;
	scal = initScal;
}

Joint::~Joint()
{
}

const Vector3& Joint::getPosition() const
{
	return pos;
}

void Joint::setPosition( const Vector3& pos )
{
	this->pos = pos;
}

Vector3 Joint::getWorldPosition() const
{
	Vector3 wpos;

	if( parent != NULL )
		wpos = parent->getWorldPosition() + pos;
	else
		wpos = pos;

	return wpos;
}

const Vector3& Joint::getInitialPosition() const
{
	return initPos;
}

void Joint::setInitialPosition( const Vector3& pos )
{
	this->initPos = pos;
}

const Quat& Joint::getOrientation() const
{
	return orient;
}

void Joint::setOrientation( const Quat& orient )
{
	this->orient = orient;
}

Quat Joint::getWorldOrientation() const
{
	Quat worient;

	if( parent != NULL )
		worient = parent->getWorldOrientation() ^ orient;
	else
		worient = orient;

	return worient;
}

const Quat& Joint::getInitialOrientation() const
{
	return initOrient;
}

void Joint::setInitialOrientation( const Quat& orient )
{
	this->initOrient = orient;
}

const Vector3& Joint::getScale() const
{
	return scal;
}

void Joint::setScale( const Vector3& scal )
{
	this->scal = scal;
}

Vector3 Joint::getWorldScale() const
{
	Vector3 pscal;

	if( parent != NULL )
		pscal = parent->getWorldScale();
	else
		pscal = Vector3( 1, 1, 1 );

	return Vector3( pscal.x * scal.x, pscal.y * scal.y, pscal.z * scal.z );
}

const Vector3& Joint::getInitialScale() const
{
	return initScal;
}

void Joint::setInitialScale( const Vector3& scal )
{
	this->initScal = scal;
}

void Joint::translate( const Vector3& trans, bool relToInitial )
{
	pos = relToInitial ? ( initPos + trans ) : ( pos + trans );
}

void Joint::rotate( const Quat& rot, bool relToInitial )
{
	orient = relToInitial ? initOrient ^ rot : orient ^ rot;
}

void Joint::rotate( float yaw, float pitch, float roll, bool relToInitial )
{
	orient = relToInitial ? initOrient ^ Quat( yaw, pitch, roll ) :
		orient ^ Quat( yaw, pitch, roll );
}

void Joint::scale( const Vector3& scal, bool relToInitial )
{
	this->scal = relToInitial ?
		Vector3( initScal.x * scal.x, initScal.y * scal.y, initScal.z * scal.z )
		: Vector3( this->scal.x * scal.x, this->scal.y * scal.y, this->scal.z * scal.z );
}

Joint* Joint::getParent() const
{
	return parent;
}

void Joint::addChild( Joint* joint )
{
	V_ASSERT( joint != NULL );
	V_ASSERT( owner->getRoot() != joint );

	for( size_t i = 0; i < children.size(); ++i )
		if( children[i] == joint )
			return;

	joint->parent = this;
	children.push_back(joint);
}

void Joint::removeChild( int childIndex )
{
	V_ASSERT( childIndex >= 0 && childIndex < numberOfChildren() );

	children[childIndex]->parent = NULL;
	children.erase( children.begin() + childIndex );
}

void Joint::moveChild( int childIndex, Joint* joint )
{
	V_ASSERT( childIndex >= 0 && childIndex < numberOfChildren() );
	V_ASSERT( joint != NULL );

	Joint* cjoint = children[childIndex];
	children.erase( children.begin() + childIndex );
	joint->addChild(cjoint);
}

Joint* Joint::getChild( int childIndex ) const
{
	V_ASSERT( childIndex >= 0 && childIndex < numberOfChildren() );

	return children[childIndex];
}

int Joint::numberOfChildren() const
{
	return (int)children.size();
}

void Joint::attachMesh( Mesh* mesh )
{
	V_ASSERT( mesh != NULL );

	for( size_t i = 0; i < attachedMeshes.size(); ++i )
		if( attachedMeshes[i] == mesh )
			return;

	mesh->parent = this;
	attachedMeshes.push_back(mesh);
}

void Joint::detachMesh( int meshIndex )
{
	V_ASSERT( meshIndex >= 0 && meshIndex < numberOfAttachedMeshes() );

	attachedMeshes[meshIndex]->parent = NULL;
	attachedMeshes.erase( attachedMeshes.begin() + meshIndex );
}

Mesh* Joint::getAttachedMesh( int meshIndex )
{
	V_ASSERT( meshIndex >= 0 && meshIndex < numberOfAttachedMeshes() );

	return attachedMeshes[meshIndex];
}

int Joint::numberOfAttachedMeshes()
{
	return (int)attachedMeshes.size();
}

}
