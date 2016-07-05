
#include "FBAPMapping.h"

namespace VisageSDK
{

FBAPMapping::FBAPMapping()
{
	name = "expression_neutral";
	refValue = 0;
}

FBAPMapping::FBAPMapping( const string& name )
{
	this->name = name;
	refValue = 0;
}

const string& FBAPMapping::getFBAPName() const
{
	return name;
}

void FBAPMapping::getMorphTarget( int i, string& meshId, string& morphTargetId ) const
{
	#ifdef _DEBUG
	assert( i >= 0 && i < numberOfMorphTargets() );
	#endif

	StringMap::const_iterator mt_iter = morphTargets.find( morphedMeshes[i] );

	meshId = mt_iter->first;
	morphTargetId = mt_iter->second;
}

bool FBAPMapping::getMorphTarget( const string& meshId, string& morphTargetId ) const
{
	StringMap::const_iterator mt_iter = morphTargets.find( meshId );

	if( mt_iter != morphTargets.end() )
	{
		morphTargetId = mt_iter->second;
		return true;
	}

	return false;
}

void FBAPMapping::mapToMorphTarget( const string& meshId, const string& morphTargetId )
{
	morphedMeshes.push_back( meshId );
	morphTargets[meshId] = morphTargetId;
}

int FBAPMapping::numberOfMorphTargets() const
{
	return (int)morphedMeshes.size();
}

bool FBAPMapping::isMappedToMorphTarget( const string &meshId, const string& morphTargetId ) const
{
	StringMap::const_iterator i = morphTargets.find( meshId );

	if( i != morphTargets.end() &&
		i->second == morphTargetId )
		return true;

	return false;
}

void FBAPMapping::getRefPosition( int i, string& boneId, Vector3& position ) const
{
	#ifdef _DEBUG
	assert( i >= 0 && i < numberOfTransBones() );
	#endif

	NodeTransMap::const_iterator bone_iter = transBonesRefPositions.find( transBones[i] );

	boneId = bone_iter->first;
	position = bone_iter->second;
}

bool FBAPMapping::getRefPosition( const string& boneId, Vector3& position ) const
{
	NodeTransMap::const_iterator bone_iter = transBonesRefPositions.find( boneId );

	if( bone_iter != transBonesRefPositions.end() )
	{
		position = bone_iter->second;
		return true;
	}

	return false;
}

void FBAPMapping::mapToBoneTranslation( const string& boneId, const Vector3& position )
{
	if( !isMappedToTransBone(boneId) )
		transBones.push_back( boneId );
	transBonesRefPositions[boneId] = position;
}

void FBAPMapping::unmapFromBoneTranslation( const string& boneId )
{
	if( !isMappedToTransBone(boneId) )
		return;

	transBonesRefPositions.erase(boneId);
	for( vector<string>::iterator i = transBones.begin(); i != transBones.end(); ++i )
		if( *i == boneId )
		{
			transBones.erase(i);
			break;
		}
}

int FBAPMapping::numberOfTransBones() const
{
	return (int)transBones.size();
}

bool FBAPMapping::isMappedToTransBone( const string &boneId ) const
{
	if( transBonesRefPositions.find( boneId ) != transBonesRefPositions.end() )
		return true;

	return false;
}

void FBAPMapping::getRefOrientation( int i, string& boneId, Quat& orientation ) const
{
	#ifdef _DEBUG
	assert( i >= 0 && i < numberOfRotBones() );
	#endif

	NodeRotMap::const_iterator bone_iter = rotBonesRefOrientations.find( rotBones[i] );

	boneId = bone_iter->first;
	orientation = bone_iter->second;
}
	
bool FBAPMapping::getRefOrientation( const string& boneId, Quat& orientation ) const
{
	NodeRotMap::const_iterator bone_iter = rotBonesRefOrientations.find( boneId );

	if( bone_iter != rotBonesRefOrientations.end() )
	{
		orientation = bone_iter->second;
		return true;
	}

	return false;
}

void FBAPMapping::mapToBoneRotation( const string& boneId, const Quat& orientation )
{
	if( !isMappedToRotBone(boneId) )
		rotBones.push_back( boneId );
	rotBonesRefOrientations[boneId] = orientation;
}

void FBAPMapping::unmapFromBoneRotation( const string& boneId )
{
	if( !isMappedToRotBone(boneId) )
		return;

	rotBonesRefOrientations.erase(boneId);
	for( vector<string>::iterator i = rotBones.begin(); i != rotBones.end(); ++i )
		if( *i == boneId )
		{
			rotBones.erase(i);
			break;
		}
}

int FBAPMapping::numberOfRotBones() const
{
	return (int)rotBones.size();
}

bool FBAPMapping::isMappedToRotBone( const string &boneId ) const
{
	if( rotBonesRefOrientations.find( boneId ) != rotBonesRefOrientations.end() )
		return true;

	return false;
}

int FBAPMapping::getRefValue() const
{
	return refValue;
}

void FBAPMapping::setRefValue( int refValue )
{
	this->refValue = refValue;
}

void FBAPMapping::unmap()
{
	morphTargets.clear();
	morphedMeshes.clear();
	transBonesRefPositions.clear();
	transBones.clear();
	rotBonesRefOrientations.clear();
	rotBones.clear();
	refValue = 0;
}

}
