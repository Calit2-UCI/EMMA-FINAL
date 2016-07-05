
#include "VisageCharModel.h"
#include "VCM.h"
#include <iostream>

using namespace std;

namespace VisageSDK
{

VisageCharModel::VisageCharModel( const string& id )
{
	this->id = id;
	skeleton = new Skeleton(id);

	fdp = new FDP(this);

	for( int i = 0; i < NVISEME; ++i )
		visemeMappings.insert( make_pair( i, FBAPMapping( VisemeNameString[i] ) ) );
	for( int i = 0; i < NEXP; ++i )
		expressionMappings.insert( make_pair( i, FBAPMapping( ExpressionNameString[i] ) ) );
	for( int i = 0; i < NFAP; ++i )
		fapMappings.insert( make_pair( i+1, FBAPMapping( FAPNameString[i] ) ) );
	for( int i = 0; i < NBAP; ++i )
		bapMappings.insert( make_pair( i+1, FBAPMapping( BAPNameString[i] ) ) );
}

VisageCharModel::~VisageCharModel()
{
	delete skeleton;
	for( int i = 0; i < (int)meshes.size(); ++i )
		delete meshes[i];

	delete fdp;
}

bool VisageCharModel::init()
{
	// destroy existing meshes and skeleton
	skeleton->deleteAllJoints();
	while( numberOfMeshes() > 0 )
		deleteMeshByIndex(0);

	// create meshes
	StringArray ms, mts;
	_getMeshes(ms);
	for( StringArray::const_iterator i = ms.begin(); i != ms.end(); ++i )
	{
		Mesh* mesh = createMesh(*i);
		_getMorphTargets( *i, mts );
		for( StringArray::const_iterator j = mts.begin(); j != mts.end(); ++j )
			mesh->createMorphTarget(*j);
		mts.clear();
	}

	// create skeleton
	string cn = _getModelRoot();
	if( cn == "" )
		return false;
	skeleton->_init(cn);
	_initSkeleton(cn);

	return true;
}

void VisageCharModel::_initSkeleton( const string& jointId )
{
	Joint* joint = skeleton->createJoint(jointId);
	if( joint == NULL )
		// joint exists, so it must be root joint!
		joint = skeleton->getRoot();

	// attach new joint to its parent
	string pj = _getNodeParent(jointId);
	if( pj != "" && joint != skeleton->getRoot() )
		skeleton->getJointById(pj)->addChild(joint);

	// initialize joint transformations
	joint->setInitialPosition( _getNodeInitialLocalPosition(jointId) );
	joint->setInitialOrientation( _getNodeInitialLocalOrientation(jointId) );
	joint->setInitialScale( _getNodeInitialLocalScale(jointId) );
	joint->setPosition( joint->getInitialPosition() );
	joint->setOrientation( joint->getInitialOrientation() );
	joint->setScale( joint->getInitialScale() );

	StringArray cjc;
	_getNodeChildren( jointId, cjc );
	for( StringArray::const_iterator i = cjc.begin(); i != cjc.end(); ++i )
		_initSkeleton(*i);
}

Mesh* VisageCharModel::createMesh( const string& meshId )
{
	if( hasMesh(meshId) )
		return getMeshById(meshId);

	Mesh* mesh = new Mesh(meshId);
	meshes.push_back(mesh);
	meshesById.insert( make_pair( meshId, mesh ) );

	return mesh;
}

void VisageCharModel::deleteMeshByIndex( int meshIndex )
{
	V_ASSERT( meshIndex >= 0 && meshIndex < numberOfMeshes() );

	deleteMeshById( meshes[meshIndex]->getId() );
}

void VisageCharModel::deleteMeshById( const string& meshId )
{
	if( !hasMesh(meshId) )
		return;

	Mesh* mesh = meshesById.find(meshId)->second;
	for( int i = 0; i < numberOfMeshes(); ++i )
		if( meshes[i]->getId() == meshId )
		{
			meshes.erase( meshes.begin() + i );
			break;
		}
	meshesById.erase(meshId);
	delete mesh;
}

bool VisageCharModel::hasMesh( const string& meshId ) const
{
	return meshesById.count(meshId) > 0;
}

Mesh* VisageCharModel::getMeshById( const string& meshId ) const
{
	map<string, Mesh*>::const_iterator i = meshesById.find(meshId);
	if( i == meshesById.end() )
		return NULL;

	return i->second;
}

Mesh* VisageCharModel::getMeshByIndex( int meshIndex ) const
{
	V_ASSERT( meshIndex >= 0 && meshIndex < numberOfMeshes() );

	return meshes[meshIndex];
}

int VisageCharModel::numberOfMeshes() const
{
	return (int)meshes.size();
}

void VisageCharModel::applyFBAPs( const FBAPs* fbaps )
{
	#ifdef _DEBUG
	assert( fbaps != NULL );
	#endif

	curFBAPs.merge(const_cast<FBAPs*>(fbaps));
}

void VisageCharModel::clearFBAPs()
{
	curFBAPs.reset();
}

void VisageCharModel::updateGeometry()
{
	int pval, ref_val;
	float t;

	// reset geometry to neutral pose
	skeleton->resetToInitialPose();
	for( int i = 0; i < numberOfMeshes(); ++i )
	{
		for( int j = 0; j < meshes[i]->numberOfMorphTargets(); ++j )
			meshes[i]->getMorphTargetByIndex(j)->setWeight(0);
	}

	// apply visemes
	t = ((float)curFBAPs.faps->fap1.blend) / 63;
	_applyFBAPToGeometry( visemeMappings[ curFBAPs.faps->fap1.viseme1 ], t );
	_applyFBAPToGeometry( visemeMappings[ curFBAPs.faps->fap1.viseme2 ], 1.f - t );

	// added by TP for testing
	V_LOG( "Renderer viseme1: %s, viseme2: %s, blend: %d",
		VisemeNameString[ curFBAPs.faps->fap1.viseme1 ],
		VisemeNameString[ curFBAPs.faps->fap1.viseme2 ],
		curFBAPs.faps->fap1.blend );
	//

	// apply expressions
	t = ((float)curFBAPs.faps->fap2.intensity1) / 63;
	_applyFBAPToGeometry( expressionMappings[ curFBAPs.faps->fap2.expression1 ], t );
	t = ((float)curFBAPs.faps->fap2.intensity2) / 63;
	_applyFBAPToGeometry( expressionMappings[ curFBAPs.faps->fap2.expression2 ], t );

	// apply LLFAPs
	for( int i = 3; i <= NFAP; ++i )
	{
		pval = curFBAPs.getFAP( i - 1 );
		if( pval == 0 )
			continue;
		ref_val = fapMappings[i].getRefValue();
		t = ((float)pval) / ref_val;

		_applyFBAPToGeometry( fapMappings[i], t );
	}

	// apply BAPs
	for( int i = 1; i <= NBAP; ++i )
	{
		pval = curFBAPs.getBAP( i - 1 );
		if( pval == 0 )
			continue;
		ref_val = bapMappings[i].getRefValue();
		t = ((float)pval) / ref_val;

		_applyFBAPToGeometry( bapMappings[i], t );
	}

	_updateGeometry();
}

void VisageCharModel::_applyFBAPToGeometry( const FBAPMapping& mapping, float t )
{
	string mid, mtid, jid;
	Vector3 rpos;
	Quat rorient;
	Mesh* mesh;
	MorphTarget* mt;
	Joint* joint;

	// apply morph targets
	int nmt = mapping.numberOfMorphTargets();
	for( int j = 0; j < nmt; ++j )
	{
		mapping.getMorphTarget( j, mid, mtid );
		mesh = getMeshById(mid);
		V_ASSERT( mesh != NULL );
		mt = mesh->getMorphTargetById(mtid); 
		V_ASSERT( mt != NULL );
		mt->setWeight(t);
	}

	// apply joint translations
	int nj = mapping.numberOfTransBones();
	for( int j = 0; j < nj; ++j )
	{
		mapping.getRefPosition( j, jid, rpos );
		joint = skeleton->getJointById(jid);
		joint->translate( ( rpos - joint->getInitialPosition() ) * t );
	}

	// apply joint orientations
	nj = mapping.numberOfRotBones();
	for( int j = 0; j < nj; ++j )
	{
		mapping.getRefOrientation( j, jid, rorient );
		joint = skeleton->getJointById(jid);
		joint->rotate( joint->getInitialOrientation().getInverse() ^
			joint->getInitialOrientation().slerp( rorient, t ) );
	}
}

bool VisageCharModel::loadFromFile( const string& filename )
{
	VCM* vcm = new VCM(this);
	if( !vcm->loadFromFile( filename ) )
	{
		delete vcm;
		return false;
	}

	//set char. id
	//id = vcm->charId;
    this->id = vcm->charId;

	//set feature points
	*fdp = vcm->featurePoints;
	//set body parts
	{
		string mesh_id, node_id;

		for( StringArray::const_iterator i = vcm->leftEyeMeshes.begin(); i != vcm->leftEyeMeshes.end(); ++i )
		{
			mesh_id = *i;

			if( !hasMesh(mesh_id) )
			{
				// can't find mesh
				V_LOG( string( "Error setting left eye mesh "
					+ mesh_id + ". Mesh doesn't exist." ).c_str() );
				continue;
			}

			leftEyeMeshes.push_back( mesh_id );
		}
		if( !skeleton->hasJoint(vcm->leftEyeNode) )
		{
			// can't find node
			V_LOG( string( "Error setting left eye node "
				+ vcm->leftEyeNode + ". Node doesn't exist." ).c_str() );
		}
		else leftEyeNode = vcm->leftEyeNode;

		for( StringArray::const_iterator i = vcm->rightEyeMeshes.begin(); i != vcm->rightEyeMeshes.end(); ++i )
		{
			mesh_id = *i;

			if( !hasMesh(mesh_id) )
			{
				// can't find mesh
				V_LOG( string( "Error setting right eye mesh "
					+ mesh_id + ". Mesh doesn't exist." ).c_str() );
				continue;
			}

			rightEyeMeshes.push_back( mesh_id );
		}
		if( !skeleton->hasJoint(vcm->rightEyeNode) )
		{
			// can't find node
			V_LOG( string( "Error setting right eye node "
				+ vcm->rightEyeNode + ". Node doesn't exist." ).c_str() );
		}
		else rightEyeNode = vcm->rightEyeNode;

		for( StringArray::const_iterator i = vcm->upperTeethMeshes.begin(); i != vcm->upperTeethMeshes.end(); ++i )
		{
			mesh_id = *i;

			if( !hasMesh(mesh_id) )
			{
				// can't find mesh
				V_LOG( string( "Error setting upper teeth mesh "
					+ mesh_id + ". Mesh doesn't exist." ).c_str() );
				continue;
			}

			upperTeethMeshes.push_back( mesh_id );
		}
		if( !skeleton->hasJoint(vcm->upperTeethNode) )
		{
			// can't find node
			V_LOG( string( "Error setting upper teeth node "
				+ vcm->upperTeethNode + ". Node doesn't exist." ).c_str() );
		}
		else upperTeethNode = vcm->upperTeethNode;

		for( StringArray::const_iterator i = vcm->lowerTeethMeshes.begin(); i != vcm->lowerTeethMeshes.end(); ++i )
		{
			mesh_id = *i;

			if( !hasMesh(mesh_id) )
			{
				// can't find mesh
				V_LOG( string( "Error setting lower teeth mesh "
					+ mesh_id + ". Mesh doesn't exist." ).c_str() );
				continue;
			}

			lowerTeethMeshes.push_back( mesh_id );
		}
		if( !skeleton->hasJoint(vcm->lowerTeethNode) )
		{
			// can't find node
			V_LOG( string( "Error setting lower teeth node "
				+ vcm->lowerTeethNode + ". Node doesn't exist." ).c_str() );
		}
		else lowerTeethNode = vcm->lowerTeethNode;

		for( StringArray::const_iterator i = vcm->tongueMeshes.begin(); i != vcm->tongueMeshes.end(); ++i )
		{
			mesh_id = *i;

			if( !hasMesh(mesh_id) )
			{
				// can't find mesh
				V_LOG( string( "Error setting tongue mesh "
					+ mesh_id + ". Mesh doesn't exist." ).c_str() );
				continue;
			}

			tongueMeshes.push_back( mesh_id );
		}
		if( !skeleton->hasJoint(vcm->tongueNode) )
		{
			// can't find node
			V_LOG( string( "Error setting tongue node "
				+ vcm->tongueNode + ". Node doesn't exist." ).c_str() );
		}
		else tongueNode = vcm->tongueNode;

		for( StringArray::const_iterator i = vcm->faceMeshes.begin(); i != vcm->faceMeshes.end(); ++i )
		{
			mesh_id = *i;

			if( !hasMesh(mesh_id) )
			{
				// can't find mesh
				V_LOG( string( "Error setting face mesh "
					+ mesh_id + ". Mesh doesn't exist." ).c_str() );
				continue;
			}

			faceMeshes.push_back( mesh_id );
		}
		if( !skeleton->hasJoint(vcm->faceNode) )
		{
			// can't find node
			V_LOG( string( "Error setting face node "
				+ vcm->faceNode + ". Node doesn't exist." ).c_str() );
		}
		else faceNode = vcm->faceNode;

		if( !skeleton->hasJoint(vcm->skeletonCOM) )
		{
			// can't find node
			V_LOG( string( "Error setting skeleton COM "
				+ vcm->skeletonCOM + ". Node doesn't exist." ).c_str() );
		}
		else skeletonCOM = vcm->skeletonCOM;
	}
	//set param. mappings
	for( FBAPMappingsMap::const_iterator i = vcm->mappings.begin(); i != vcm->mappings.end(); ++i )
	{
		FBAPMapping mapping( i->first );
		string mesh_id, mt_id, bone_id;
		Vector3 pos;
		Quat orient;

		for( int j = 0; j != i->second.numberOfMorphTargets(); ++j )
		{
			i->second.getMorphTarget( j, mesh_id, mt_id );

			if( !hasMesh(mesh_id) || !getMeshById(mesh_id)->hasMorphTarget(mt_id) )
			{
				// can't find mesh
				V_LOG( string( "Error mapping FBAP "
					+ i->first + " to mesh " + mesh_id
					+ ", morph target " + mt_id + ". Mesh or morph target don't exist." ).c_str() );
				continue;
			}

			mapping.mapToMorphTarget( mesh_id, mt_id );
		}

		for( int j = 0; j != i->second.numberOfTransBones(); ++j )
		{
			i->second.getRefPosition( j, bone_id, pos );

			if( !skeleton->hasJoint(bone_id) )
			{
				// can't find node
				V_LOG( string( "Error mapping FBAP "
					+ i->first + " to node " + bone_id
					+ ". Node doesn't exist." ).c_str() );
				continue;
			}

			mapping.mapToBoneTranslation( bone_id, pos );
		}

		for( int j = 0; j != i->second.numberOfRotBones(); ++j )
		{
			i->second.getRefOrientation( j, bone_id, orient );

			if( !skeleton->hasJoint(bone_id) )
			{
				// can't find node
				V_LOG( string( "Error mapping FBAP "
					+ i->first + " to node " + bone_id
					+ ". Node doesn't exist." ).c_str() );
				continue;
			}
			
			mapping.mapToBoneRotation( bone_id, orient );
		}

		mapping.setRefValue( i->second.getRefValue() );

		if( mapping.numberOfMorphTargets() != 0 || mapping.numberOfTransBones() != 0 || mapping.numberOfRotBones() != 0 )
			setFBAPMapping( mapping );
	}
	//set neutral pose
	for( NodeTransMap::const_iterator i = vcm->neutralPosePositions.begin(); i != vcm->neutralPosePositions.end(); ++i )
	{
		string node_id;
		Vector3 position;

		node_id = i->first;
		position = i->second;

		if( !skeleton->hasJoint(node_id) )
		{
			// can't find node
			V_LOG( string( "Error setting neutral pose for node "
				+ node_id + ". Node doesn't exist." ).c_str() );
			continue;
		}

		skeleton->getJointById(node_id)->setInitialPosition(position);
	}
	for( NodeRotMap::const_iterator i = vcm->neutralPoseOrientations.begin(); i != vcm->neutralPoseOrientations.end(); ++i )
	{
		string node_id;
		Quat orientation;

		node_id = i->first;
		orientation = i->second;

		if( !skeleton->hasJoint(node_id) )
		{
			// can't find node
			V_LOG( string( "Error setting neutral pose for node "
				+ node_id + ". Node doesn't exist." ).c_str() );
			continue;
		}

		skeleton->getJointById(node_id)->setInitialOrientation(orientation);
	}

	//delete VCM
	delete vcm;

	return true;
}

bool VisageCharModel::saveToFile( const string& filename ) const
{
	// save VCM

	VCM* vcm = new VCM( const_cast<VisageCharModel*>(this) );

	//set char. id
	vcm->charId = id;
	//set feature points
	vcm->featurePoints = *fdp;
	//set body parts
	{
		string mesh_id, node_id;

		for( StringArray::const_iterator i = leftEyeMeshes.begin(); i != leftEyeMeshes.end(); ++i )
		{
			mesh_id = *i;

			vcm->leftEyeMeshes.push_back( mesh_id );
		}
		vcm->leftEyeNode = leftEyeNode;

		for( StringArray::const_iterator i = rightEyeMeshes.begin(); i != rightEyeMeshes.end(); ++i )
		{
			mesh_id = *i;

			vcm->rightEyeMeshes.push_back( mesh_id );
		}
		vcm->rightEyeNode = rightEyeNode;

		for( StringArray::const_iterator i = upperTeethMeshes.begin(); i != upperTeethMeshes.end(); ++i )
		{
			mesh_id = *i;

			vcm->upperTeethMeshes.push_back( mesh_id );
		}
		vcm->upperTeethNode = upperTeethNode;

		for( StringArray::const_iterator i = lowerTeethMeshes.begin(); i != lowerTeethMeshes.end(); ++i )
		{
			mesh_id = *i;

			vcm->lowerTeethMeshes.push_back( mesh_id );
		}
		vcm->lowerTeethNode = lowerTeethNode;

		for( StringArray::const_iterator i = tongueMeshes.begin(); i != tongueMeshes.end(); ++i )
		{
			mesh_id = *i;

			vcm->tongueMeshes.push_back( mesh_id );
		}
		vcm->tongueNode = tongueNode;

		for( StringArray::const_iterator i = faceMeshes.begin(); i != faceMeshes.end(); ++i )
		{
			mesh_id = *i;

			vcm->faceMeshes.push_back( mesh_id );
		}
		vcm->faceNode = faceNode;

		vcm->skeletonCOM = skeletonCOM;
	}
	//set param. mappings
	for( int i = 1; i <= 68; ++i )
	{
		FBAPMapping mapping = getFAPMapping(i);

		if( mapping.getRefValue() != 0 )
			vcm->mappings.insert( make_pair( mapping.getFBAPName(), mapping ) );
	}
	for( int i = 0; i <= 14; ++i )
	{
		FBAPMapping mapping = getVisemeMapping(i);

		if( mapping.getRefValue() != 0 )
			vcm->mappings.insert( make_pair( mapping.getFBAPName(), mapping ) );
	}
	for( int i = 0; i <= 6; ++i )
	{
		FBAPMapping mapping = getExpressionMapping(i);

		if( mapping.getRefValue() != 0 )
			vcm->mappings.insert( make_pair( mapping.getFBAPName(), mapping ) );
	}
	for( int i = 1; i <= 186; ++i )
	{
		FBAPMapping mapping = getBAPMapping(i);

		if( mapping.getRefValue() != 0 )
			vcm->mappings.insert( make_pair( mapping.getFBAPName(), mapping ) );
	}
	//set neutral pose
	Joint* joint;
	for( int i = 0; i < skeleton->numberOfJoints(); ++i )
	{
		joint = skeleton->getJointByIndex(i);
		vcm->neutralPosePositions.insert( make_pair( joint->getId(), joint->getInitialPosition() ) );
		vcm->neutralPoseOrientations.insert( make_pair( joint->getId(), joint->getInitialOrientation() ) );
	}

	if( !vcm->saveToFile( filename ) )
	{
		delete vcm;
		return false;
	}

	//delete VCM

	delete vcm;

	return true;
}

FDP* VisageCharModel::getFeaturePoints()
{
	return fdp;
}

bool VisageCharModel::isLeftEyeMesh( const string& id ) const
{
	#ifdef _DEBUG
	assert( hasMesh(id) );
	#endif

	for( StringArray::const_iterator m_iter = leftEyeMeshes.begin();
		m_iter != leftEyeMeshes.end(); ++m_iter )
		if( *m_iter == id ) return true;

	return false;
}

void VisageCharModel::addLeftEyeMesh( const string& id )
{
	#ifdef _DEBUG
	assert( hasMesh(id) );
	#endif

	if( !isLeftEyeMesh(id) )
		leftEyeMeshes.push_back(id);
}

void VisageCharModel::removeLeftEyeMesh( const string& id )
{
	#ifdef _DEBUG
	assert( hasMesh(id) );
	#endif
	
	for( StringArray::iterator m_iter = leftEyeMeshes.begin();
		m_iter != leftEyeMeshes.end(); ++m_iter )
		if( *m_iter == id ) leftEyeMeshes.erase(m_iter);
}

int VisageCharModel::numberOfLeftEyeMeshes() const
{
	return static_cast<int>( leftEyeMeshes.size() );
}

const string& VisageCharModel::getLeftEyeMesh( int i ) const
{
	#ifdef _DEBUG
	assert( i >= 0 && i < numberOfLeftEyeMeshes() );
	#endif

	return leftEyeMeshes[i];
}

void VisageCharModel::setLeftEyeMesh( int i, const string& id )
{
	#ifdef _DEBUG
	assert( i >= 0 && i < numberOfLeftEyeMeshes() );
	#endif

	leftEyeMeshes[i] = id;
}

const string& VisageCharModel::getLeftEyeNode() const
{
	return leftEyeNode;
}

void VisageCharModel::setLeftEyeNode( const string& id )
{
	#ifdef _DEBUG
	assert( _hasNode(id) );
	#endif

	leftEyeNode = id;
}

bool VisageCharModel::isRightEyeMesh( const string& id ) const
{
	#ifdef _DEBUG
	assert( hasMesh(id) );
	#endif

	for( StringArray::const_iterator m_iter = rightEyeMeshes.begin();
		m_iter != rightEyeMeshes.end(); ++m_iter )
		if( *m_iter == id ) return true;

	return false;
}

void VisageCharModel::addRightEyeMesh( const string& id )
{
	#ifdef _DEBUG
	assert( hasMesh(id) );
	#endif

	if( !isRightEyeMesh(id) )
		rightEyeMeshes.push_back(id);
}

void VisageCharModel::removeRightEyeMesh( const string& id )
{
	#ifdef _DEBUG
	assert( hasMesh(id) );
	#endif
	
	for( StringArray::iterator m_iter = rightEyeMeshes.begin();
		m_iter != rightEyeMeshes.end(); ++m_iter )
		if( *m_iter == id ) rightEyeMeshes.erase(m_iter);
}

int VisageCharModel::numberOfRightEyeMeshes() const
{
	return static_cast<int>( rightEyeMeshes.size() );
}

const string& VisageCharModel::getRightEyeMesh( int i ) const
{
	#ifdef _DEBUG
	assert( i >= 0 && i < numberOfRightEyeMeshes() );
	#endif

	return rightEyeMeshes[i];
}

void VisageCharModel::setRightEyeMesh( int i, const string& id )
{
	#ifdef _DEBUG
	assert( i >= 0 && i < numberOfRightEyeMeshes() );
	#endif

	rightEyeMeshes[i] = id;
}

const string& VisageCharModel::getRightEyeNode() const
{
	return rightEyeNode;
}

void VisageCharModel::setRightEyeNode( const string& id )
{
	#ifdef _DEBUG
	assert( _hasNode(id) );
	#endif
	
	rightEyeNode = id;
}

bool VisageCharModel::isUpperTeethMesh( const string& id ) const
{
	#ifdef _DEBUG
	assert( hasMesh(id) );
	#endif

	for( StringArray::const_iterator m_iter = upperTeethMeshes.begin();
		m_iter != upperTeethMeshes.end(); ++m_iter )
		if( *m_iter == id ) return true;

	return false;
}

void VisageCharModel::addUpperTeethMesh( const string& id )
{
	#ifdef _DEBUG
	assert( hasMesh(id) );
	#endif

	if( !isUpperTeethMesh(id) )
		upperTeethMeshes.push_back(id);
}

void VisageCharModel::removeUpperTeethMesh( const string& id )
{
	#ifdef _DEBUG
	assert( hasMesh(id) );
	#endif
	
	for( StringArray::iterator m_iter = upperTeethMeshes.begin();
		m_iter != upperTeethMeshes.end(); ++m_iter )
		if( *m_iter == id ) upperTeethMeshes.erase(m_iter);
}

int VisageCharModel::numberOfUpperTeethMeshes() const
{
	return static_cast<int>( upperTeethMeshes.size() );
}

const string& VisageCharModel::getUpperTeethMesh( int i ) const
{
	#ifdef _DEBUG
	assert( i >= 0 && i < numberOfUpperTeethMeshes() );
	#endif

	return upperTeethMeshes[i];
}

void VisageCharModel::setUpperTeethMesh( int i, const string& id )
{
	#ifdef _DEBUG
	assert( i >= 0 && i < numberOfUpperTeethMeshes() );
	#endif

	upperTeethMeshes[i] = id;
}

const string& VisageCharModel::getUpperTeethNode() const
{
	return upperTeethNode;
}

void VisageCharModel::setUpperTeethNode( const string& id )
{
	#ifdef _DEBUG
	assert( _hasNode(id) );
	#endif

	upperTeethNode = id;
}

bool VisageCharModel::isLowerTeethMesh( const string& id ) const
{
	#ifdef _DEBUG
	assert( hasMesh(id) );
	#endif

	for( StringArray::const_iterator m_iter = lowerTeethMeshes.begin();
		m_iter != lowerTeethMeshes.end(); ++m_iter )
		if( *m_iter == id ) return true;

	return false;
}

void VisageCharModel::addLowerTeethMesh( const string& id )
{
	#ifdef _DEBUG
	assert( hasMesh(id) );
	#endif

	if( !isLowerTeethMesh(id) )
		lowerTeethMeshes.push_back(id);
}

void VisageCharModel::removeLowerTeethMesh( const string& id )
{
	#ifdef _DEBUG
	assert( hasMesh(id) );
	#endif
	
	for( StringArray::iterator m_iter = lowerTeethMeshes.begin();
		m_iter != lowerTeethMeshes.end(); ++m_iter )
		if( *m_iter == id ) lowerTeethMeshes.erase(m_iter);
}

int VisageCharModel::numberOfLowerTeethMeshes() const
{
	return static_cast<int>( lowerTeethMeshes.size() );
}

const string& VisageCharModel::getLowerTeethMesh( int i ) const
{
	#ifdef _DEBUG
	assert( i >= 0 && i < numberOfLowerTeethMeshes() );
	#endif

	return lowerTeethMeshes[i];
}

void VisageCharModel::setLowerTeethMesh( int i, const string& id )
{
	#ifdef _DEBUG
	assert( i >= 0 && i < numberOfLowerTeethMeshes() );
	#endif

	lowerTeethMeshes[i] = id;
}

const string& VisageCharModel::getLowerTeethNode() const
{
	return lowerTeethNode;
}

void VisageCharModel::setLowerTeethNode( const string& id )
{
	#ifdef _DEBUG
	assert( _hasNode(id) );
	#endif

	lowerTeethNode = id;
}

bool VisageCharModel::isTongueMesh( const string& id ) const
{
	#ifdef _DEBUG
	assert( hasMesh(id) );
	#endif

	for( StringArray::const_iterator m_iter = tongueMeshes.begin();
		m_iter != tongueMeshes.end(); ++m_iter )
		if( *m_iter == id ) return true;

	return false;
}

void VisageCharModel::addTongueMesh( const string& id )
{
	#ifdef _DEBUG
	assert( hasMesh(id) );
	#endif

	if( !isTongueMesh(id) )
		tongueMeshes.push_back(id);
}

void VisageCharModel::removeTongueMesh( const string& id )
{
	#ifdef _DEBUG
	assert( hasMesh(id) );
	#endif
	
	for( StringArray::iterator m_iter = tongueMeshes.begin();
		m_iter != tongueMeshes.end(); ++m_iter )
		if( *m_iter == id ) tongueMeshes.erase(m_iter);
}

int VisageCharModel::numberOfTongueMeshes() const
{
	return static_cast<int>( tongueMeshes.size() );
}

const string& VisageCharModel::getTongueMesh( int i ) const
{
	#ifdef _DEBUG
	assert( i >= 0 && i < numberOfTongueMeshes() );
	#endif

	return tongueMeshes[i];
}

void VisageCharModel::setTongueMesh( int i, const string& id )
{
	#ifdef _DEBUG
	assert( i >= 0 && i < numberOfTongueMeshes() );
	#endif

	tongueMeshes[i] = id;
}

const string& VisageCharModel::getTongueNode() const
{
	return tongueNode;
}

void VisageCharModel::setTongueNode( const string& id )
{
	#ifdef _DEBUG
	assert( _hasNode(id) );
	#endif

	tongueNode = id;
}

bool VisageCharModel::isFaceMesh( const string& id ) const
{
	#ifdef _DEBUG
	assert( hasMesh(id) );
	#endif

	for( StringArray::const_iterator m_iter = faceMeshes.begin();
		m_iter != faceMeshes.end(); ++m_iter )
		if( *m_iter == id ) return true;

	return false;
}

void VisageCharModel::addFaceMesh( const string& id )
{
	#ifdef _DEBUG
	assert( hasMesh(id) );
	#endif

	if( !isFaceMesh(id) )
		faceMeshes.push_back(id);
}

void VisageCharModel::removeFaceMesh( const string& id )
{
	#ifdef _DEBUG
	assert( hasMesh(id) );
	#endif
	
	for( StringArray::iterator m_iter = faceMeshes.begin();
		m_iter != faceMeshes.end(); ++m_iter )
		if( *m_iter == id ) faceMeshes.erase(m_iter);
}

int VisageCharModel::numberOfFaceMeshes() const
{
	return static_cast<int>( faceMeshes.size() );
}

const string& VisageCharModel::getFaceMesh( int i ) const
{
	#ifdef _DEBUG
	assert( i >= 0 && i < numberOfFaceMeshes() );
	#endif

	return faceMeshes[i];
}

void VisageCharModel::setFaceMesh( int i, const string& id )
{
	#ifdef _DEBUG
	assert( i >= 0 && i < numberOfFaceMeshes() );
	#endif

	faceMeshes[i] = id;
}

const string& VisageCharModel::getFaceNode() const
{
	return faceNode;
}

void VisageCharModel::setFaceNode( const string& id )
{
	#ifdef _DEBUG
	assert( _hasNode(id) );
	#endif

	faceNode = id;
}

const string& VisageCharModel::getSkeletonCOM() const
{
	return skeletonCOM;
}

void VisageCharModel::setSkeletonCOM( const string& id )
{
	#ifdef _DEBUG
	assert( _hasNode(id) );
	#endif

	skeletonCOM = id;
}

const FBAPMapping& VisageCharModel::getVisemeMapping( int no ) const
{
	#ifdef _DEBUG
	assert( no >= 0 && no < NVISEME );
	#endif

	return visemeMappings.find(no)->second;
}

const FBAPMapping& VisageCharModel::getExpressionMapping( int no ) const
{
	#ifdef _DEBUG
	assert( no >= 0 && no < NEXP );
	#endif

	return expressionMappings.find(no)->second;
}

const FBAPMapping& VisageCharModel::getFAPMapping( int id ) const
{
	#ifdef _DEBUG
	assert( id >= 1 && id <= NFAP );
	#endif

	return fapMappings.find(id)->second;
}

const FBAPMapping& VisageCharModel::getBAPMapping( int id ) const
{
	#ifdef _DEBUG
	assert( id >= 1 && id <= NBAP );
	#endif

	return bapMappings.find(id)->second;
}

const FBAPMapping& VisageCharModel::getFBAPMapping( const string& name ) const
{
	for( int i = 0; i < NVISEME; ++i )
		if( getVisemeMapping(i).getFBAPName() == name )
			return getVisemeMapping(i);

	for( int i = 0; i < NEXP; ++i )
		if( getExpressionMapping(i).getFBAPName() == name )
			return getExpressionMapping(i);

	for( int i = 1; i <= NFAP; ++i )
		if( getFAPMapping(i).getFBAPName() == name )
			return getFAPMapping(i);

	for( int i = 1; i <= NBAP; ++i )
		if( getBAPMapping(i).getFBAPName() == name )
			return getBAPMapping(i);

	#ifdef _DEBUG
	// non-existent FBAP
	assert(true);
	#endif

	return getFAPMapping(1);
}

void VisageCharModel::setFBAPMapping( const FBAPMapping& mapping )
{
	for( int i = 0; i < NVISEME; ++i )
		if( getVisemeMapping(i).getFBAPName() == mapping.getFBAPName() )
		{
			visemeMappings[i] = mapping;
			return;
		}

	for( int i = 0; i < NEXP; ++i )
		if( getExpressionMapping(i).getFBAPName() == mapping.getFBAPName() )
		{
			expressionMappings[i] = mapping;
			return;
		}

	for( int i = 1; i <= NFAP; ++i )
		if( getFAPMapping(i).getFBAPName() == mapping.getFBAPName() )
		{
			fapMappings[i] = mapping;
			return;
		}

	for( int i = 1; i <= NBAP; ++i )
		if( getBAPMapping(i).getFBAPName() == mapping.getFBAPName() )
		{
			bapMappings[i] = mapping;
			return;
		}
}

Vector3 VisageCharModel::_getNodeInitialWorldPosition( const string& id ) const
{
	if( id == _getModelRoot() )
		return _getNodeInitialLocalPosition(id);

	return _getNodeInitialWorldPosition( _getNodeParent(id) ) + _getNodeInitialLocalPosition(id);
}

Quat VisageCharModel::_getNodeInitialWorldOrientation( const string& id ) const
{
	if( id == _getModelRoot() )
		return _getNodeInitialLocalOrientation(id);

	return _getNodeInitialWorldOrientation( _getNodeParent(id) ) ^ _getNodeInitialLocalOrientation(id);
}

Vector3 VisageCharModel::_getNodeInitialWorldScale( const string& id ) const
{
	if( id == _getModelRoot() )
		return _getNodeInitialLocalScale(id);

	Vector3 ls = _getNodeInitialLocalScale(id);
	Vector3 ps = _getNodeInitialWorldScale( _getNodeParent(id) );
	return Vector3( ls.x * ps.x, ls.y * ps.y, ls.z * ps.z );
}

Vector3 VisageCharModel::_getNodeWorldPosition( const string& id ) const
{
	if( id == _getModelRoot() )
		return _getNodeLocalPosition(id);

	return _getNodeWorldPosition( _getNodeParent(id) ) + _getNodeLocalPosition(id);
}

Quat VisageCharModel::_getNodeWorldOrientation( const string& id ) const
{
	if( id == _getModelRoot() )
		return _getNodeLocalOrientation(id);

	return _getNodeWorldOrientation( _getNodeParent(id) ) ^ _getNodeLocalOrientation(id);
}

Vector3 VisageCharModel::_getNodeWorldScale( const string& id ) const
{
	if( id == _getModelRoot() )
		return _getNodeLocalScale(id);

	Vector3 ls = _getNodeLocalScale(id);
	Vector3 ps = _getNodeWorldScale( _getNodeParent(id) );
	return Vector3( ls.x * ps.x, ls.y * ps.y, ls.z * ps.z );
}

}
