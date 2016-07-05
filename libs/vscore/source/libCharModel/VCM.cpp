
#include "Logger.h"
#include "VCM.h"

// C++
#ifdef _DEBUG
#include <assert.h>
#endif
#include <fstream>
#include <sstream>

// libXML
#include "libxml/xmlreader.h"
#include "libxml/xmlwriter.h"

namespace VisageSDK
{

/// File input/output:

bool VCM::loadFromFile( const string& filename )
{
	xmlTextReaderPtr vcmReader;

	if( ( vcmReader = xmlNewTextReaderFilename( filename.data() ) ) == NULL )
	{
		logFileReadError( filename );
		return false;
	}

	bool success = parseDocument( vcmReader );

	xmlFreeTextReader( vcmReader );

	return success;
}

bool VCM::saveToFile( const string& filename )
{
	xmlTextWriterPtr vcmWriter;

	if( ( vcmWriter = xmlNewTextWriterFilename( filename.data(), 0 ) ) == NULL )
	{
		logFileWriteError( filename );
		return false;
	}

	bool success = writeDocument( vcmWriter );

	xmlFreeTextWriter( vcmWriter );

	return success;
}

bool VCM::parseDocument( xmlTextReaderPtr vcmReader )
{
	string name;
	getCurNodeName( vcmReader, name );

	//skip comments
	while( xmlTextReaderRead( vcmReader ) == 1 && xmlTextReaderNodeType( vcmReader ) == 8 )
		continue;

	//found root node
	if( xmlTextReaderNodeType( vcmReader ) == 1
		&& getCurNodeName( vcmReader, name )
		&& ( name == "vcm" ) )
	{
		

		return parseNodeVcm( vcmReader );
	}

	

	logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
	return false;
}

bool VCM::parseNodeVcm( xmlTextReaderPtr vcmReader )
{
	string name, value;getCurNodeName( vcmReader, name );

	//parse attributes

	if( xmlTextReaderAttributeCount( vcmReader ) != 2 )
	{
		logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
		return false;
	}

	//check VCM version
	if( !getCurAttributeValue( vcmReader, "version", value ) || value != "1.0" )
	{
		logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
		return false;
	}
	

	//get character model id
	if( !getCurAttributeValue( vcmReader, "charId", value ) )
	{
		logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
		return false;
	}
	charId = value;
	
	
	//parse subelements

	if( xmlTextReaderIsEmptyElement( vcmReader ) )
		return true;

	while( xmlTextReaderRead( vcmReader ) == 1
		&& getCurNodeName( vcmReader, name )
		)
	{
		//parse feature points
		if( xmlTextReaderNodeType( vcmReader ) == 1
			&& ( name == "featurePoints" ) )
		{
			

			if( !parseNodeFeaturePoints( vcmReader ) )
			{
				return false;
			}

			continue;
		}
		//parse vertex lip assignments
		else if( xmlTextReaderNodeType( vcmReader ) == 1
			&& ( name == "vertexLipAssignments" ) )
		{
			

			if( !parseNodeVertexLipAssignments( vcmReader ) )
			{
				return false;
			}

			continue;
		}
		//parse body parts
		else if( xmlTextReaderNodeType( vcmReader ) == 1
			&& ( name == "bodyParts" ) )
		{
			

			if( !parseNodeBodyParts( vcmReader ) )
			{
				return false;
			}

			continue;
		}
		//parse animation parameter mappings
		else if( xmlTextReaderNodeType( vcmReader ) == 1
			&& ( name == "animationParameterMappings" ) )
		{
			

			if( !parseNodeAnimationParameterMappings( vcmReader ) )
			{
				return false;
			}

			continue;
		}
		//parse neutral pose
		else if( xmlTextReaderNodeType( vcmReader ) == 1
			&& ( name == "neutralPose" ) )
		{
			

			if( !parseNodeNeutralPose( vcmReader ) )
			{
				return false;
			}

			continue;
		}
		//reached end node?
		else if( xmlTextReaderNodeType( vcmReader ) == 15
			&& ( name == "vcm" ) )
		{
			

			return true;
		}
		//skip comments and whitespaces
		else if( xmlTextReaderNodeType( vcmReader ) == 8
			|| xmlTextReaderNodeType( vcmReader ) == 13
			|| xmlTextReaderNodeType( vcmReader ) == 14)
		{
			continue;
		}

		

		logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
		return false;
	}

	
	

	logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
	return false;
}

bool VCM::parseNodeFeaturePoints( xmlTextReaderPtr vcmReader )
{
	string name, value;getCurNodeName( vcmReader, name );

	//parse attributes

	if( xmlTextReaderAttributeCount( vcmReader ) != 0 )
	{
		logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
		return false;
	}
	
	//parse subelements

	if( xmlTextReaderIsEmptyElement( vcmReader ) )
		return true;

	while( xmlTextReaderRead( vcmReader ) == 1
		&& getCurNodeName( vcmReader, name )
		)
	{
		//parse feature point
		if( xmlTextReaderNodeType( vcmReader ) == 1
			&& ( name == "featurePoint" ) )
		{
			

			if( !parseNodeFeaturePoint( vcmReader ) )
			{
				return false;
			}

			continue;
		}
		//reached end node?
		else if( xmlTextReaderNodeType( vcmReader ) == 15
			&& ( name == "featurePoints" ) )
		{
			

			return true;
		}
		//skip comments and whitespaces
		else if( xmlTextReaderNodeType( vcmReader ) == 8
			|| xmlTextReaderNodeType( vcmReader ) == 13
			|| xmlTextReaderNodeType( vcmReader ) == 14)
		{
			continue;
		}

		

		logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
		return false;
	}

	
	

	logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
	return false;
}

bool VCM::parseNodeFeaturePoint( xmlTextReaderPtr vcmReader )
{
	string name, value;getCurNodeName( vcmReader, name );
	int fp_group, fp_index, v_index = -1;
	Vector3 v_pos;
	string mesh_id = "";

	//parse attributes

	int attr_cnt = 0; //number of attributes

	if( ( attr_cnt = xmlTextReaderAttributeCount( vcmReader ) ) != 4 &&
		attr_cnt != 2 )
	{
		logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
		return false;
	}

	//get feature point id
	if( !getCurAttributeValue( vcmReader, "id", value ) )
	{
		logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
		return false;
	}
	if( !FDP::FPIsValid( value.data() ) )
	{
		logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
		return false;
	}
	FDP::parseFPName( value.data(), fp_group, fp_index );
	
	if( attr_cnt == 4 )
	{
		//get mesh id
		if( !getCurAttributeValue( vcmReader, "meshId", value ) )
		{
			logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
			return false;
		}
		mesh_id = value;
	
		//get vertex index
		if( !getCurAttributeValue( vcmReader, "vertexIndex", value ) )
		{
			logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
			return false;
		}
		if( !parseInt( ( const char* )value.data(), v_index ) )
		{
			logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
			return false;
		}
	}
	
	//get vertex position
	if( !getCurAttributeValue( vcmReader, "vertexPosition", value ) )
	{
		logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
		return false;
	}
	if( !parseVector3( ( const char* )value.data(), v_pos ) )
	{
		logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
		return false;
	}
	
	//add new feature point
	if( featurePoints.FPIsDefined( fp_group, fp_index ) )
	{
		logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
		return false; //no duplicate feature point definitions
	}
	featurePoints.setFPPos( fp_group, fp_index, v_pos.x, v_pos.y, v_pos.z );
	if( mesh_id != "" ) featurePoints.setFPSurfVert( fp_group, fp_index, mesh_id, v_index );
	
	//parse subelements

	if( xmlTextReaderIsEmptyElement( vcmReader ) )
		return true;

	while( xmlTextReaderRead( vcmReader ) == 1
		&& getCurNodeName( vcmReader, name )
		)
	{
		//reached end node?
		if( xmlTextReaderNodeType( vcmReader ) == 15
			&& ( name == "featurePoint" ) )
		{
			

			return true;
		}
		//skip comments and whitespaces
		else if( xmlTextReaderNodeType( vcmReader ) == 8
			|| xmlTextReaderNodeType( vcmReader ) == 13
			|| xmlTextReaderNodeType( vcmReader ) == 14)
		{
			continue;
		}

		

		logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
		return false;
	}

	
	

	logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
	return false;
}

bool VCM::parseNodeVertexLipAssignments( xmlTextReaderPtr vcmReader )
{
	string name, value;getCurNodeName( vcmReader, name );

	//parse attributes

	if( xmlTextReaderAttributeCount( vcmReader ) != 0 )
	{
		logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
		return false;
	}
	
	//parse subelements

	if( xmlTextReaderIsEmptyElement( vcmReader ) )
		return true;

	while( xmlTextReaderRead( vcmReader ) == 1
		&& getCurNodeName( vcmReader, name )
		)
	{
		//parse upper lip vertices
		if( xmlTextReaderNodeType( vcmReader ) == 1
			&& ( name == "upperLipVertices" ) )
		{
			

			if( !parseNodeUpperLipVertices( vcmReader ) )
			{
				return false;
			}

			continue;
		}
		//parse lower lip vertices
		else if( xmlTextReaderNodeType( vcmReader ) == 1
			&& ( name == "lowerLipVertices" ) )
		{
			

			if( !parseNodeLowerLipVertices( vcmReader ) )
			{
				return false;
			}

			continue;
		}
		//reached end node?
		else if( xmlTextReaderNodeType( vcmReader ) == 15
			&& ( name == "vertexLipAssignments" ) )
		{
			

			return true;
		}
		//skip comments and whitespaces
		else if( xmlTextReaderNodeType( vcmReader ) == 8
			|| xmlTextReaderNodeType( vcmReader ) == 13
			|| xmlTextReaderNodeType( vcmReader ) == 14)
		{
			continue;
		}

		

		logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
		return false;
	}

	
	

	logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
	return false;
}

bool VCM::parseNodeUpperLipVertices( xmlTextReaderPtr vcmReader )
{
	string name, value; getCurNodeName( vcmReader, name );
	string mesh_id;
	IntArray vertex_indices;
	Vector3Array vertex_positions;

	//parse attributes

	if( xmlTextReaderAttributeCount( vcmReader ) != 1 )
	{
		logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
		return false;
	}

	//get mesh id
	if( !getCurAttributeValue( vcmReader, "meshId", value ) )
	{
		logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
		return false;
	}
	mesh_id = value;
	

	//parse subelements

	if( xmlTextReaderIsEmptyElement( vcmReader ) )
		return true;

	while( xmlTextReaderRead( vcmReader ) == 1
		&& getCurNodeName( vcmReader, name )
		)
	{
		//parse vertex list
		if( xmlTextReaderNodeType( vcmReader ) == 3 )
		{
			//get vertex lists
			getCurElementValue( vcmReader, value );
			if( !parseIntVector3Array( ( const char* )value.data(), vertex_indices, vertex_positions ) )
			{
				logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
				return false;
			}
			for( size_t i = 0; i < vertex_indices.size(); ++i )
			{
				FeaturePoint fp;
				fp.defined = 1;
				fp.pos[0] = vertex_positions[i].x;
				fp.pos[1] = vertex_positions[i].y;
				fp.pos[2] = vertex_positions[i].z;
				fp.surf = mesh_id;
				fp.vert = vertex_indices[i];
				featurePoints.addUpperLipVertex(fp);
			}

			continue;
		}
		//reached end node?
		else if( xmlTextReaderNodeType( vcmReader ) == 15
			&& ( name == "upperLipVertices" ) )
		{
			

			return true;
		}
		//skip comments and whitespaces
		else if( xmlTextReaderNodeType( vcmReader ) == 8
			|| xmlTextReaderNodeType( vcmReader ) == 13
			|| xmlTextReaderNodeType( vcmReader ) == 14)
		{
			continue;
		}

		

		logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
		return false;
	}

	
	

	logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
	return false;
}

bool VCM::parseNodeLowerLipVertices( xmlTextReaderPtr vcmReader )
{
	string name, value;getCurNodeName( vcmReader, name );
	string mesh_id;
	IntArray vertex_indices;
	Vector3Array vertex_positions;

	//parse attributes

	if( xmlTextReaderAttributeCount( vcmReader ) != 1 )
	{
		logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
		return false;
	}

	//get mesh id
	if( !getCurAttributeValue( vcmReader, "meshId", value ) )
	{
		logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
		return false;
	}
	mesh_id = value;
	

	//parse subelements

	if( xmlTextReaderIsEmptyElement( vcmReader ) )
		return true;

	while( xmlTextReaderRead( vcmReader ) == 1
		&& getCurNodeName( vcmReader, name )
		)
	{
		//parse vertex list
		if( xmlTextReaderNodeType( vcmReader ) == 3 )
		{
			//get vertex lists
			getCurElementValue( vcmReader, value );
			if( !parseIntVector3Array( ( const char* )value.data(), vertex_indices, vertex_positions ) )
			{
				logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
				return false;
			}
			for( size_t i = 0; i < vertex_indices.size(); ++i )
			{
				FeaturePoint fp;
				fp.defined = 1;
				fp.pos[0] = vertex_positions[i].x;
				fp.pos[1] = vertex_positions[i].y;
				fp.pos[2] = vertex_positions[i].z;
				fp.surf = mesh_id;
				fp.vert = vertex_indices[i];
				featurePoints.addLowerLipVertex(fp);
			}

			continue;
		}
		//reached end node?
		else if( xmlTextReaderNodeType( vcmReader ) == 15
			&& ( name == "lowerLipVertices" ) )
		{
			

			return true;
		}
		//skip comments and whitespaces
		else if( xmlTextReaderNodeType( vcmReader ) == 8
			|| xmlTextReaderNodeType( vcmReader ) == 13
			|| xmlTextReaderNodeType( vcmReader ) == 14)
		{
			continue;
		}

		

		logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
		return false;
	}

	
	

	logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
	return false;
}

bool VCM::parseNodeBodyParts( xmlTextReaderPtr vcmReader )
{
	string name, value;getCurNodeName( vcmReader, name );

	//parse attributes

	if( xmlTextReaderAttributeCount( vcmReader ) != 0 )
	{
		logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
		return false;
	}
	
	//parse subelements

	if( xmlTextReaderIsEmptyElement( vcmReader ) )
		return true;

	while( xmlTextReaderRead( vcmReader ) == 1
		&& getCurNodeName( vcmReader, name )
		)
	{
		//parse left eye
		if( xmlTextReaderNodeType( vcmReader ) == 1
			&& ( name == "leftEye" ) )
		{
			

			if( !parseNodeBodyPart( vcmReader, "leftEye" ) )
			{
				return false;
			}

			continue;
		}
		//parse right eye
		else if( xmlTextReaderNodeType( vcmReader ) == 1
			&& ( name == "rightEye" ) )
		{
			

			if( !parseNodeBodyPart( vcmReader, "rightEye" ) )
			{
				return false;
			}

			continue;
		}
		//parse upper teeth
		else if( xmlTextReaderNodeType( vcmReader ) == 1
			&& ( name == "upperTeeth" ) )
		{
			

			if( !parseNodeBodyPart( vcmReader, "upperTeeth" ) )
			{
				return false;
			}

			continue;
		}
		//parse lower teeth
		else if( xmlTextReaderNodeType( vcmReader ) == 1
			&& ( name == "lowerTeeth" ) )
		{
			

			if( !parseNodeBodyPart( vcmReader, "lowerTeeth" ) )
			{
				return false;
			}

			continue;
		}
		//parse tongue
		else if( xmlTextReaderNodeType( vcmReader ) == 1
			&& ( name == "tongue" ) )
		{
			

			if( !parseNodeBodyPart( vcmReader, "tongue" ) )
			{
				return false;
			}

			continue;
		}
		//parse face
		else if( xmlTextReaderNodeType( vcmReader ) == 1
			&& ( name == "face" ) )
		{
			

			if( !parseNodeBodyPart( vcmReader, "face" ) )
			{
				return false;
			}

			continue;
		}
		//parse skeleton
		else if( xmlTextReaderNodeType( vcmReader ) == 1
			&& ( name == "skeleton" ) )
		{

			if( !parseNodeSkeleton( vcmReader ) )
			{
				return false;
			}

			continue;
		}
		//reached end node?
		else if( xmlTextReaderNodeType( vcmReader ) == 15
			&& ( name == "bodyParts" ) )
		{

			return true;
		}
		//skip comments and whitespaces
		else if( xmlTextReaderNodeType( vcmReader ) == 8
			|| xmlTextReaderNodeType( vcmReader ) == 13
			|| xmlTextReaderNodeType( vcmReader ) == 14)
		{
			continue;
		}

		

		logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
		return false;
	}

	
	

	logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
	return false;
}

bool VCM::parseNodeBodyPart( xmlTextReaderPtr vcmReader, const string& bodyPartId )
{
	string name, value;getCurNodeName( vcmReader, name );
	StringArray* mesh_list;
	string* node_id;

	if( bodyPartId == "leftEye" )
	{
		mesh_list = &leftEyeMeshes;
		node_id = &leftEyeNode;
	}
	else if( bodyPartId == "rightEye" )
	{
		mesh_list = &rightEyeMeshes;
		node_id = &rightEyeNode;
	}
	else if( bodyPartId == "upperTeeth" )
	{
		mesh_list = &upperTeethMeshes;
		node_id = &upperTeethNode;
	}
	else if( bodyPartId == "lowerTeeth" )
	{
		mesh_list = &lowerTeethMeshes;
		node_id = &lowerTeethNode;
	}
	else if( bodyPartId == "tongue" )
	{
		mesh_list = &tongueMeshes;
		node_id = &tongueNode;
	}
	else if( bodyPartId == "face" )
	{
		mesh_list = &faceMeshes;
		node_id = &faceNode;
	}
	else
	{
		#ifdef _DEBUG
		assert(false);
		#endif
	}

	//parse attributes

	if( xmlTextReaderAttributeCount( vcmReader ) != 0 )
	{
		logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
		return false;
	}
	
	//parse subelements

	if( xmlTextReaderIsEmptyElement( vcmReader ) )
		return true;

	while( xmlTextReaderRead( vcmReader ) == 1
		&& getCurNodeName( vcmReader, name )
		)
	{
		//parse meshes
		if( xmlTextReaderNodeType( vcmReader ) == 1
			&& ( name == "meshes" ) )
		{
			

			if( !parseNodeBodyPartMeshes( vcmReader, *mesh_list ) )
			{
				return false;
			}

			continue;
		}
		//parse node
		else if( xmlTextReaderNodeType( vcmReader ) == 1
			&& ( name == "node" ) )
		{
			

			if( !parseNodeBodyPartNode( vcmReader, *node_id ) )
			{
				return false;
			}

			continue;
		}
		//reached end node?
		else if( xmlTextReaderNodeType( vcmReader ) == 15
			&& ( name == bodyPartId ) )
		{
			

			return true;
		}
		//skip comments and whitespaces
		else if( xmlTextReaderNodeType( vcmReader ) == 8
			|| xmlTextReaderNodeType( vcmReader ) == 13
			|| xmlTextReaderNodeType( vcmReader ) == 14)
		{
			continue;
		}

		

		logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
		return false;
	}

	
	

	logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
	return false;
}

bool VCM::parseNodeBodyPartMeshes( xmlTextReaderPtr vcmReader, StringArray& meshList )
{
	string name, value;getCurNodeName( vcmReader, name );
	string mesh_id;

	//parse attributes

	if( xmlTextReaderAttributeCount( vcmReader ) != 0 )
	{
		logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
		return false;
	}
	
	//parse subelements

	if( xmlTextReaderIsEmptyElement( vcmReader ) )
		return true;

	while( xmlTextReaderRead( vcmReader ) == 1
		&& getCurNodeName( vcmReader, name )
		)
	{
		//parse mesh
		if( xmlTextReaderNodeType( vcmReader ) == 1
			&& ( name == "mesh" ) )
		{
			

			if( !parseNodeBodyPartMesh( vcmReader, mesh_id ) )
			{
				return false;
			}

			if( meshListHasMesh( meshList, mesh_id ) )
			{
				logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
				return false; //no duplicate meshes
			}
			meshList.push_back( mesh_id );

			continue;
		}
		//reached end node?
		else if( xmlTextReaderNodeType( vcmReader ) == 15
			&& ( name == "meshes" ) )
		{
			

			return true;
		}
		//skip comments and whitespaces
		else if( xmlTextReaderNodeType( vcmReader ) == 8
			|| xmlTextReaderNodeType( vcmReader ) == 13
			|| xmlTextReaderNodeType( vcmReader ) == 14)
		{
			continue;
		}

		

		logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
		return false;
	}

	
	

	logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
	return false;
}

bool VCM::parseNodeBodyPartMesh( xmlTextReaderPtr vcmReader, string& meshId )
{
	string name, value;getCurNodeName( vcmReader, name );

	//parse attributes

	if( xmlTextReaderAttributeCount( vcmReader ) != 1 )
	{
		logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
		return false;
	}

	//get mesh id
	if( !getCurAttributeValue( vcmReader, "id", value ) )
	{
		logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
		return false;
	}
	meshId = value;
	
	
	//parse subelements

	if( xmlTextReaderIsEmptyElement( vcmReader ) )
		return true;

	while( xmlTextReaderRead( vcmReader ) == 1
		&& getCurNodeName( vcmReader, name )
		)
	{
		//reached end node?
		if( xmlTextReaderNodeType( vcmReader ) == 15
			&& ( name == "mesh" ) )
		{
			

			return true;
		}
		//skip comments and whitespaces
		else if( xmlTextReaderNodeType( vcmReader ) == 8
			|| xmlTextReaderNodeType( vcmReader ) == 13
			|| xmlTextReaderNodeType( vcmReader ) == 14)
		{
			continue;
		}

		

		logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
		return false;
	}

	
	

	logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
	return false;
}

bool VCM::parseNodeBodyPartNode( xmlTextReaderPtr vcmReader, string& nodeId )
{
	string name, value;getCurNodeName( vcmReader, name );

	//parse attributes

	if( xmlTextReaderAttributeCount( vcmReader ) != 1 )
	{
		logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
		return false;
	}

	//get node id
	if( !getCurAttributeValue( vcmReader, "id", value ) )
	{
		logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
		return false;
	}
	if( nodeId != "" )
	{
		logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
		return false; //no duplicate body part definitions
	}
	nodeId = value;
	
	
	//parse subelements

	if( xmlTextReaderIsEmptyElement( vcmReader ) )
		return true;

	while( xmlTextReaderRead( vcmReader ) == 1
		&& getCurNodeName( vcmReader, name )
		)
	{
		//reached end node?
		if( xmlTextReaderNodeType( vcmReader ) == 15
			&& ( name == "node" ) )
		{
			

			return true;
		}
		//skip comments and whitespaces
		else if( xmlTextReaderNodeType( vcmReader ) == 8
			|| xmlTextReaderNodeType( vcmReader ) == 13
			|| xmlTextReaderNodeType( vcmReader ) == 14)
		{
			continue;
		}

		

		logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
		return false;
	}

	
	

	logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
	return false;
}

bool VCM::parseNodeSkeleton( xmlTextReaderPtr vcmReader )
{
	string name, value;getCurNodeName( vcmReader, name );

	//parse attributes

	if( xmlTextReaderAttributeCount( vcmReader ) != 0 )
	{
		logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
		return false;
	}
	
	//parse subelements

	if( xmlTextReaderIsEmptyElement( vcmReader ) )
		return true;

	while( xmlTextReaderRead( vcmReader ) == 1
		&& getCurNodeName( vcmReader, name )
		)
	{
		//parse COM
		if( xmlTextReaderNodeType( vcmReader ) == 1
			&& ( name == "COM" ) )
		{
			

			if( !parseNodeCOM( vcmReader ) )
			{
				return false;
			}

			continue;
		}
		//reached end node?
		else if( xmlTextReaderNodeType( vcmReader ) == 15
			&& ( name == "skeleton" ) )
		{
			

			return true;
		}
		//skip comments and whitespaces
		else if( xmlTextReaderNodeType( vcmReader ) == 8
			|| xmlTextReaderNodeType( vcmReader ) == 13
			|| xmlTextReaderNodeType( vcmReader ) == 14)
		{
			continue;
		}

		

		logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
		return false;
	}

	
	

	logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
	return false;
}
	
bool VCM::parseNodeCOM( xmlTextReaderPtr vcmReader )
{
	string name, value;getCurNodeName( vcmReader, name );

	//parse attributes

	if( xmlTextReaderAttributeCount( vcmReader ) != 1 )
	{
		logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
		return false;
	}

	//get node id
	if( !getCurAttributeValue( vcmReader, "id", value ) )
	{
		logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
		return false;
	}
	if( skeletonCOM != "" )
	{
		logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
		return false; //no duplicate body part definitions
	}
	skeletonCOM = value;
	
	
	//parse subelements

	if( xmlTextReaderIsEmptyElement( vcmReader ) )
		return true;

	while( xmlTextReaderRead( vcmReader ) == 1
		&& getCurNodeName( vcmReader, name )
		)
	{
		//reached end node?
		if( xmlTextReaderNodeType( vcmReader ) == 15
			&& ( name == "COM" ) )
		{
			

			return true;
		}
		//skip comments and whitespaces
		else if( xmlTextReaderNodeType( vcmReader ) == 8
			|| xmlTextReaderNodeType( vcmReader ) == 13
			|| xmlTextReaderNodeType( vcmReader ) == 14)
		{
			continue;
		}

		

		logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
		return false;
	}

	
	

	logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
	return false;
}

bool VCM::parseNodeAnimationParameterMappings( xmlTextReaderPtr vcmReader )
{
	string name, value;getCurNodeName( vcmReader, name );

	//parse attributes

	if( xmlTextReaderAttributeCount( vcmReader ) != 0 )
	{
		logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
		return false;
	}
	
	//parse subelements

	if( xmlTextReaderIsEmptyElement( vcmReader ) )
		return true;

	while( xmlTextReaderRead( vcmReader ) == 1
		&& getCurNodeName( vcmReader, name )
		)
	{
		//parse FBAP mapping
		if( xmlTextReaderNodeType( vcmReader ) == 1
			&& ( name == "animationParameterMapping" ) )
		{
			

			if( !parseNodeAnimationParameterMapping( vcmReader ) )
			{
				return false;
			}

			continue;
		}
		//reached end node?
		else if( xmlTextReaderNodeType( vcmReader ) == 15
			&& ( name == "animationParameterMappings" ) )
		{
			

			return true;
		}
		//skip comments and whitespaces
		else if( xmlTextReaderNodeType( vcmReader ) == 8
			|| xmlTextReaderNodeType( vcmReader ) == 13
			|| xmlTextReaderNodeType( vcmReader ) == 14)
		{
			continue;
		}

		

		logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
		return false;
	}

	
	

	logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
	return false;
}

bool VCM::parseNodeAnimationParameterMapping( xmlTextReaderPtr vcmReader )
{
	string name, value;getCurNodeName( vcmReader, name );

	string param_name;
	int ref_val;

	//parse attributes

	if( xmlTextReaderAttributeCount( vcmReader ) != 2 )
	{
		logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
		return false;
	}

	//get param name
	if( !getCurAttributeValue( vcmReader, "paramName", value ) )
	{
		logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
		return false;
	}
	param_name = value;
	if( mappings.find( param_name ) != mappings.end() )
	{
		logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
		return false; //no duplicate param. mappings
	}
	mappings.insert( make_pair( param_name, FBAPMapping( param_name ) ) );
	

	//get ref value
	if( !getCurAttributeValue( vcmReader, "refValue", value ) )
	{
		logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
		return false;
	}
	if( !parseInt( ( const char* )value.data(), ref_val ) )
	{
		logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
		return false;
	}
	// TODO:
	// check if refValue is valid for visemes / expressions
	//
	mappings[param_name].setRefValue( ref_val );
	
	
	//parse subelements

	if( xmlTextReaderIsEmptyElement( vcmReader ) )
		return true;

	while( xmlTextReaderRead( vcmReader ) == 1
		&& getCurNodeName( vcmReader, name )
		)
	{
		//parse morph target
		if( xmlTextReaderNodeType( vcmReader ) == 1
			&& ( name == "morphTarget" ) )
		{
			

			string mesh_id, mt_id;

			if( !parseNodeMorphTarget( vcmReader, mesh_id, mt_id ) )
			{
				return false;
			}

			//map param. to morph target
			mappings[param_name].mapToMorphTarget( mesh_id, mt_id );

			continue;
		}
		//parse bone
		else if( xmlTextReaderNodeType( vcmReader ) == 1
			&& ( name == "bone" ) )
		{
			

			string node_id;
			Vector3 ref_pos;
			Quat ref_orient;

			if( xmlTextReaderGetAttribute( vcmReader, BAD_CAST "refOrientation" ) != NULL )
			{
				if( !parseNodeBoneRot( vcmReader, node_id, ref_orient ) )
					return false;

				if( mappings[param_name].isMappedToRotBone( node_id ) )
				{
					logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
					return false; //no duplicate bones
				}
				mappings[param_name].mapToBoneRotation( node_id, ref_orient );
			}
			else if( xmlTextReaderGetAttribute( vcmReader, BAD_CAST "refPosition" ) != NULL )
			{
				if( !parseNodeBoneTrans( vcmReader, node_id, ref_pos ) )
					return false;

				if( mappings[param_name].isMappedToTransBone( node_id ) )
				{
					logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
					return false; //no duplicate bones
				}
				mappings[param_name].mapToBoneTranslation( node_id, ref_pos );
			}
			else
			{
				logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
				return false;
			}

			continue;
		}
		//reached end node?
		else if( xmlTextReaderNodeType( vcmReader ) == 15
			&& ( name == "animationParameterMapping" ) )
		{
			

			return true;
		}
		//skip comments and whitespaces
		else if( xmlTextReaderNodeType( vcmReader ) == 8
			|| xmlTextReaderNodeType( vcmReader ) == 13
			|| xmlTextReaderNodeType( vcmReader ) == 14)
		{
			continue;
		}

		

		logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
		return false;
	}

	
	

	logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
	return false;
}

bool VCM::parseNodeMorphTarget( xmlTextReaderPtr vcmReader, string& meshId, string& mtId )
{
	string name, value;getCurNodeName( vcmReader, name );

	//parse attributes

	if( xmlTextReaderAttributeCount( vcmReader ) != 2 )
	{
		logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
		return false;
	}

	//get mesh id
	if( !getCurAttributeValue( vcmReader, "meshId", value ) )
	{
		logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
		return false;
	}
	meshId = value;
	

	//get morph target id
	if( !getCurAttributeValue( vcmReader, "id", value ) )
	{
		logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
		return false;
	}
	mtId = value;
	

	//parse subelements

	if( xmlTextReaderIsEmptyElement( vcmReader ) )
		return true;

	while( xmlTextReaderRead( vcmReader ) == 1
		&& getCurNodeName( vcmReader, name )
		)
	{
		//reached end node?
		if( xmlTextReaderNodeType( vcmReader ) == 15
			&& ( name == "morphTarget" ) )
		{
			

			return true;
		}
		//skip comments and whitespaces
		else if( xmlTextReaderNodeType( vcmReader ) == 8
			|| xmlTextReaderNodeType( vcmReader ) == 13
			|| xmlTextReaderNodeType( vcmReader ) == 14)
		{
			continue;
		}

		

		logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
		return false;
	}

	
	

	logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
	return false;
}

bool VCM::parseNodeBoneTrans( xmlTextReaderPtr vcmReader, string& boneId, Vector3& refPosition )
{
	string name, value;getCurNodeName( vcmReader, name );

	//parse attributes

	if( xmlTextReaderAttributeCount( vcmReader ) != 2 )
	{
		logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
		return false;
	}

	//get bone id
	if( !getCurAttributeValue( vcmReader, "id", value ) )
	{
		logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
		return false;
	}
	boneId = value;
	

	//get ref. position
	if( !getCurAttributeValue( vcmReader, "refPosition", value ) )
	{
		logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
		return false;
	}
	if( !parseVector3( ( const char* )value.data(), refPosition ) )
	{
		logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
		return false;
	}
	

	//parse subelements

	if( xmlTextReaderIsEmptyElement( vcmReader ) )
		return true;

	while( xmlTextReaderRead( vcmReader ) == 1
		&& getCurNodeName( vcmReader, name )
		)
	{
		//reached end node?
		if( xmlTextReaderNodeType( vcmReader ) == 15
			&& ( name == "bone" ) )
		{
			

			return true;
		}
		//skip comments and whitespaces
		else if( xmlTextReaderNodeType( vcmReader ) == 8
			|| xmlTextReaderNodeType( vcmReader ) == 13
			|| xmlTextReaderNodeType( vcmReader ) == 14)
		{
			continue;
		}

		

		logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
		return false;
	}

	
	

	logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
	return false;
}

bool VCM::parseNodeBoneRot( xmlTextReaderPtr vcmReader, string& boneId, Quat& refOrientation )
{
	string name, value;getCurNodeName( vcmReader, name );

	//parse attributes

	if( xmlTextReaderAttributeCount( vcmReader ) != 2 )
	{
		logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
		return false;
	}

	//get bone id
	if( !getCurAttributeValue( vcmReader, "id", value ) )
	{
		logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
		return false;
	}
	boneId = value;
	

	//get ref. orientation
	if( !getCurAttributeValue( vcmReader, "refOrientation", value ) )
	{
		logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
		return false;
	}
	if( !parseQuat( ( const char* )value.data(), refOrientation ) )
	{
		logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
		return false;
	}
	

	//parse subelements

	if( xmlTextReaderIsEmptyElement( vcmReader ) )
		return true;

	while( xmlTextReaderRead( vcmReader ) == 1
		&& getCurNodeName( vcmReader, name )
		)
	{
		//reached end node?
		if( xmlTextReaderNodeType( vcmReader ) == 15
			&& ( name == "bone" ) )
		{
			

			return true;
		}
		//skip comments and whitespaces
		else if( xmlTextReaderNodeType( vcmReader ) == 8
			|| xmlTextReaderNodeType( vcmReader ) == 13
			|| xmlTextReaderNodeType( vcmReader ) == 14)
		{
			continue;
		}

		

		logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
		return false;
	}

	
	

	logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
	return false;
}

bool VCM::parseNodeNeutralPose( xmlTextReaderPtr vcmReader )
{
	string name, value;getCurNodeName( vcmReader, name );

	//parse attributes

	if( xmlTextReaderAttributeCount( vcmReader ) != 0 )
	{
		logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
		return false;
	}
	
	//parse subelements

	if( xmlTextReaderIsEmptyElement( vcmReader ) )
		return true;

	while( xmlTextReaderRead( vcmReader ) == 1
		&& getCurNodeName( vcmReader, name )
		)
	{
		//parse bone transformation
		if( xmlTextReaderNodeType( vcmReader ) == 1
			&& ( name == "boneTransformation" ) )
		{
			

			if( !parseNodeBoneTransformation( vcmReader ) )
			{
				return false;
			}

			continue;
		}
		//reached end node?
		else if( xmlTextReaderNodeType( vcmReader ) == 15
			&& ( name == "neutralPose" ) )
		{
			

			return true;
		}
		//skip comments and whitespaces
		else if( xmlTextReaderNodeType( vcmReader ) == 8
			|| xmlTextReaderNodeType( vcmReader ) == 13
			|| xmlTextReaderNodeType( vcmReader ) == 14)
		{
			continue;
		}

		

		logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
		return false;
	}

	
	

	logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
	return false;
}

bool VCM::parseNodeBoneTransformation( xmlTextReaderPtr vcmReader )
{
	string name, value;getCurNodeName( vcmReader, name );

	string bone_id;
	Quat orientation;
	Vector3 position;

	//parse attributes
	
	//total number of attributes
	int attr_cnt1 = 0;
	//number of valid attributes
	int attr_cnt2 = 0;

	if( ( attr_cnt1 = xmlTextReaderAttributeCount( vcmReader ) ) != 2 && 
		attr_cnt1 != 3 )
	{
		logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
		return false;
	}

	//get bone id
	if( !getCurAttributeValue( vcmReader, "boneId", value ) )
	{
		logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
		return false;
	}
	++attr_cnt2;
	bone_id = value;
	

	//get orientation
	if( getCurAttributeValue( vcmReader, "orientation", value ) )
	{
		++attr_cnt2;

		if( !parseQuat( ( const char* )value.data(), orientation ) )
		{
			logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
			return false;
		}
		

		neutralPoseOrientations.insert( make_pair( bone_id, orientation ) );
	}

	//get position
	if( getCurAttributeValue( vcmReader, "position", value ) )
	{
		++attr_cnt2;

		if( !parseVector3( ( const char* )value.data(), position ) )
		{
			logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
			return false;
		}
		

		neutralPosePositions.insert( make_pair( bone_id, position ) );
	}
	if( attr_cnt1 != attr_cnt2 )
	{
		logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
		return false; //unexpected attributes found
	}

	//parse subelements

	if( xmlTextReaderIsEmptyElement( vcmReader ) )
		return true;

	while( xmlTextReaderRead( vcmReader ) == 1
		&& getCurNodeName( vcmReader, name )
		)
	{
		//reached end node?
		if( xmlTextReaderNodeType( vcmReader ) == 15
			&& ( name == "boneTransformation" ) )
		{
			

			return true;
		}
		//skip comments and whitespaces
		else if( xmlTextReaderNodeType( vcmReader ) == 8
			|| xmlTextReaderNodeType( vcmReader ) == 13
			|| xmlTextReaderNodeType( vcmReader ) == 14)
		{
			continue;
		}

		

		logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
		return false;
	}

	
	

	logVCMParseError( xmlTextReaderGetParserLineNumber( vcmReader ), name );
	return false;
}

bool VCM::parseInt( const char* valueStr, int& value )
{
	istringstream iss;

	iss.str( valueStr );
	if( iss.eof() ) return false;
	iss >> value;

	return true;
}

bool VCM::parseIntVector3Array( const char* valueStr, IntArray& value1, Vector3Array& value2 )
{
	istringstream iss;
	int t_val;
	float x_val, y_val, z_val;

	iss.str( valueStr );
	while(true)
	{
		if( iss.eof() ) break;
		
		// get int
		iss >> t_val;
		if( iss.eof() ) return false;

		// get Vector3
		iss >> x_val;
		if( iss.eof() ) return false;
		iss >> y_val;
		if( iss.eof() ) return false;
		iss >> z_val;

		value1.push_back(t_val);
		value2.push_back( Vector3( x_val, y_val, z_val ) );
	}

	return true;
}

bool VCM::parseVector3( const char* valueStr, Vector3& value )
{
	istringstream iss;

	iss.str( valueStr );
	if( iss.eof() ) return false;
	iss >> value.x;
	if( iss.eof() ) return false;
	iss >> value.y;
	if( iss.eof() ) return false;
	iss >> value.z;

	return true;
}

bool VCM::parseQuat( const char* valueStr, Quat& value )
{
	istringstream iss;

	iss.str( valueStr );
	if( iss.eof() ) return false;
	iss >> value.w;
	if( iss.eof() ) return false;
	iss >> value.x;
	if( iss.eof() ) return false;
	iss >> value.y;
	if( iss.eof() ) return false;
	iss >> value.z;

	return true;
}

bool VCM::getCurNodeName( xmlTextReaderPtr vcmReader, string& nodeName ) const
{
	xmlChar* name;

	if( ( name = xmlTextReaderName( vcmReader ) ) == NULL )
	{
		nodeName = "";
		return false;
	}

	nodeName = string( ( const char* )name );
	xmlFree(name);

	return true;
}

bool VCM::getCurElementValue( xmlTextReaderPtr vcmReader, string& nodeValue ) const
{
	xmlChar* value;

	if( ( value = xmlTextReaderValue( vcmReader ) ) == NULL )
	{
		nodeValue = "";
		return false;
	}

	nodeValue = string( ( const char* )value );
	xmlFree(value);

	return true;
}

bool VCM::getCurAttributeValue( xmlTextReaderPtr vcmReader, const string& attribName, string& nodeValue ) const
{
	xmlChar* value;

	if( ( value = xmlTextReaderGetAttribute( vcmReader, BAD_CAST attribName.data() ) ) == NULL )
	{
		nodeValue = "";
		return false;
	}

	nodeValue = string( ( const char* )value );
	xmlFree(value);

	return true;
}

bool VCM::writeDocument( xmlTextWriterPtr vcmWriter )
{
	//set indentation
	if( xmlTextWriterSetIndent( vcmWriter, 1 ) < 0 ||
		xmlTextWriterSetIndentString( vcmWriter, BAD_CAST " " ) < 0 )
	{
		logVCMSetIndentError();
		return false;
	}

	//start VCM file
	if( xmlTextWriterStartDocument( vcmWriter, "1.0", "UTF-8", NULL ) < 0 )
	{
		logVCMWriteError( "?xml" );
		return false;
	}

	//write root
	if( !writeNodeVcm( vcmWriter ) )
	{
		return false;
	}

	//end VCM file
	if( xmlTextWriterEndDocument( vcmWriter ) < 0 )
	{
		logVCMWriteError( "" );
		return false;
	}

	return true;
}

bool VCM::writeNodeVcm( xmlTextWriterPtr vcmWriter )
{
	//start root element
	if( xmlTextWriterStartElement( vcmWriter, BAD_CAST "vcm" ) < 0 )
	{
		logVCMWriteError( "vcm" );
		return false;
	}

	//write version
	if( xmlTextWriterWriteAttribute( vcmWriter, BAD_CAST "version", BAD_CAST VCM_VERSION_STR ) < 0 )
	{
		logVCMWriteError( "version" );
		return false;
	}

	//write character model id
	if( xmlTextWriterWriteAttribute( vcmWriter, BAD_CAST "charId", BAD_CAST charId.data() ) < 0 )
	{
		logVCMWriteError( "charId" );
		return false;
	}

	//write feature points
	if( !writeNodeFeaturePoints( vcmWriter ) )
	{
		return false;
	}

	//write vertex lip assignments
	if( !writeNodeVertexLipAssignments( vcmWriter ) )
	{
		return false;
	}

	//write body parts
	if( !writeNodeBodyParts( vcmWriter ) )
	{
		return false;
	}

	//write animation param. mappings
	if( !writeNodeAnimationParameterMappings( vcmWriter ) )
	{
		return false;
	}

	//write neutral pose
	if( !writeNodeNeutralPose( vcmWriter ) )
	{
		return false;
	}

	//end root element
	if( xmlTextWriterEndElement( vcmWriter ) < 0 )
	{
		logVCMWriteError( "vcm" );
		return false;
	}

	return true;
}

bool VCM::writeNodeFeaturePoints( xmlTextWriterPtr vcmWriter )
{
	//start feature points
	if( xmlTextWriterStartElement( vcmWriter, BAD_CAST "featurePoints" ) < 0 )
	{
		logVCMWriteError( "featurePoints" );
		return false;
	}

	//write each feature point
	for( int i = FDP::FP_START_GROUP_INDEX; i <= FDP::FP_END_GROUP_INDEX; ++i )
	{
		for( int j = 1; j <= FDP::groupSize(i); ++j )
		{
			if( !featurePoints.FPIsDefined( i, j ) ) continue;

			if( !writeNodeFeaturePoint( vcmWriter, FDP::getFPName( i, j ) ) )
			{
				return false;
			}
		}
	}

	//end feature points
	if( xmlTextWriterEndElement( vcmWriter ) < 0 )
	{
		logVCMWriteError("featurePoints" );
		return false;
	}

	return true;
}

bool VCM::writeNodeFeaturePoint( xmlTextWriterPtr vcmWriter, const string& featurePointId )
{
	string mesh_id;
	int v_ind;
	featurePoints.getFPSurfVert( featurePointId.data(), mesh_id, v_ind );
	Vector3 v_pos( featurePoints.getFPPos( featurePointId.data() )[0],
		featurePoints.getFPPos( featurePointId.data() )[1],
		featurePoints.getFPPos( featurePointId.data() )[2] );

	//start feature point
	if( xmlTextWriterStartElement( vcmWriter, BAD_CAST "featurePoint" ) < 0 )
	{
		logVCMWriteError( "featurePoint" );
		return false;
	}

	//write feature point id
	if( xmlTextWriterWriteAttribute( vcmWriter, BAD_CAST "id", 
		BAD_CAST featurePointId.data() )
		< 0 )
	{
		logVCMWriteError( "id" );
		return false;
	}

	//write mesh id
	if( mesh_id != "" && xmlTextWriterWriteAttribute( vcmWriter, BAD_CAST "meshId", 
		BAD_CAST mesh_id.data() )
		< 0 )
	{
		logVCMWriteError( "meshId" );
		return false;
	}

	//write vertex index
	string value_str;
	writeInt( v_ind, value_str );
	if( v_ind > -1 && xmlTextWriterWriteAttribute( vcmWriter, BAD_CAST "vertexIndex",
		BAD_CAST value_str.data() )
		< 0 )
	{
		logVCMWriteError( "vertexIndex" );
		return false;
	}

	//write vertex position
	writeVector3( v_pos, value_str );
	if( xmlTextWriterWriteAttribute( vcmWriter, BAD_CAST "vertexPosition",
		BAD_CAST value_str.data() )
		< 0 )
	{
		logVCMWriteError( "vertexPosition" );
		return false;
	}

	//end feature point
	if( xmlTextWriterEndElement( vcmWriter ) < 0 )
	{
		logVCMWriteError( "featurePoint" );
		return false;
	}

	return true;
}

bool VCM::writeNodeVertexLipAssignments( xmlTextWriterPtr vcmWriter )
{
	if( featurePoints.numberOfUpperLipVertices() == 0 &&
		featurePoints.numberOfLowerLipVertices() == 0 )
		return true;

	//start vertex lip assignments
	if( xmlTextWriterStartElement( vcmWriter, BAD_CAST "vertexLipAssignments" ) < 0 )
	{
		logVCMWriteError( "vertexLipAssignments" );
		return false;
	}

	map<string, IntArray> vertex_indices;
	map<string, Vector3Array> vertex_positions;

	// get vertex indices and positions
	for( int i = 0; i < featurePoints.numberOfUpperLipVertices(); ++i )
	{
		const FeaturePoint& fp = featurePoints.getUpperLipVertex(i);
		if( !fp.defined ) continue;

		vertex_indices[fp.surf].push_back(fp.vert);
		vertex_positions[fp.surf].push_back( Vector3( fp.pos[0], fp.pos[1], fp.pos[2] ) );
	}

	//write upper lip vertex assignments
	for( map<string, IntArray>::const_iterator i = vertex_indices.begin(); i != vertex_indices.end(); 
		++i )
	{
		if( !writeNodeUpperLipVertices( vcmWriter, i->first, i->second, vertex_positions[i->first] ) )
		{
			return false;
		}
	}

	vertex_indices.clear();
	vertex_positions.clear();

	// get vertex indices and positions
	for( int i = 0; i < featurePoints.numberOfLowerLipVertices(); ++i )
	{
		const FeaturePoint& fp = featurePoints.getLowerLipVertex(i);
		if( !fp.defined ) continue;

		vertex_indices[fp.surf].push_back(fp.vert);
		vertex_positions[fp.surf].push_back( Vector3( fp.pos[0], fp.pos[1], fp.pos[2] ) );
	}

	//write lower lip vertex assignments
	for( map<string, IntArray>::const_iterator i = vertex_indices.begin(); i != vertex_indices.end(); 
		++i )
	{
		if( !writeNodeLowerLipVertices( vcmWriter, i->first, i->second, vertex_positions[i->first] ) )
		{
			return false;
		}
	}

	//end vertex lip assignments
	if( xmlTextWriterEndElement( vcmWriter ) < 0 )
	{
		logVCMWriteError( "vertexLipAssignments" );
		return false;
	}

	return true;
}

bool VCM::writeNodeUpperLipVertices( xmlTextWriterPtr vcmWriter, const string& meshId,
									const IntArray& vertexIndices, const Vector3Array& vertexPositions )
{
	string vert_ind_str;

	if( !writeIntVector3Array( vertexIndices, vertexPositions, vert_ind_str ) )
	{
		logVCMWriteError( "upperLipVertices" );
		return false;
	}

	//start upper lip vertices
	if( xmlTextWriterStartElement( vcmWriter, BAD_CAST "upperLipVertices" ) < 0 )
	{
		logVCMWriteError( "upperLipVertices" );
		return false;
	}

	//write mesh id
	if( xmlTextWriterWriteAttribute( vcmWriter, BAD_CAST "meshId", BAD_CAST meshId.data() )
		< 0 )
	{
		logVCMWriteError( "meshId" );
		return false;
	}

	//write vertex indices
	if( xmlTextWriterWriteString( vcmWriter, BAD_CAST vert_ind_str.data() ) < 0 )
	{
		logVCMWriteError( "upperLipVertices" );
		return false;
	}

	//end upper lip vertices
	if( xmlTextWriterEndElement( vcmWriter ) < 0 )
	{
		logVCMWriteError( "upperLipVertices" );
		return false;
	}

	return true;
}

bool VCM::writeNodeLowerLipVertices( xmlTextWriterPtr vcmWriter, const string& meshId,
									const IntArray& vertexIndices, const Vector3Array& vertexPositions )
{
	string vert_ind_str;

	if( !writeIntVector3Array( vertexIndices, vertexPositions, vert_ind_str ) )
	{
		logVCMWriteError( "upperLipVertices" );
		return false;
	}

	//start lower lip vertices
	if( xmlTextWriterStartElement( vcmWriter, BAD_CAST "lowerLipVertices" ) < 0 )
	{
		logVCMWriteError( "lowerLipVertices" );
		return false;
	}

	//write mesh id
	if( xmlTextWriterWriteAttribute( vcmWriter, BAD_CAST "meshId", BAD_CAST meshId.data() )
		< 0 )
	{
		logVCMWriteError( "meshId" );
		return false;
	}

	//write vertex indices
	if( xmlTextWriterWriteString( vcmWriter, BAD_CAST vert_ind_str.data() ) < 0 )
	{
		logVCMWriteError( "lowerLipVertices" );
		return false;
	}

	//end lower lip vertices
	if( xmlTextWriterEndElement( vcmWriter ) < 0 )
	{
		logVCMWriteError( "lowerLipVertices" );
		return false;
	}

	return true;
}

bool VCM::writeNodeBodyParts( xmlTextWriterPtr vcmWriter )
{
	//start body parts
	if( xmlTextWriterStartElement( vcmWriter, BAD_CAST "bodyParts" ) < 0 )
	{
		logVCMWriteError( "bodyParts" );
		return false;
	}

	//write left eye
	if( !writeNodeBodyPart( vcmWriter, "leftEye", leftEyeMeshes, leftEyeNode ) )
	{
		return false;
	}

	//write right eye
	if( !writeNodeBodyPart( vcmWriter, "rightEye", rightEyeMeshes, rightEyeNode ) )
	{
		return false;
	}

	//write upper teeth
	if( !writeNodeBodyPart( vcmWriter, "upperTeeth", upperTeethMeshes, upperTeethNode ) )
	{
		return false;
	}

	//write lower teeth
	if( !writeNodeBodyPart( vcmWriter, "lowerTeeth", lowerTeethMeshes, lowerTeethNode ) )
	{
		return false;
	}

	//write tongue
	if( !writeNodeBodyPart( vcmWriter, "tongue", tongueMeshes, tongueNode ) )
	{
		return false;
	}

	//write face
	if( !writeNodeBodyPart( vcmWriter, "face", faceMeshes, faceNode ) )
	{
		return false;
	}

	//write skeleton
	if( !writeNodeSkeleton( vcmWriter ) )
	{
		return false;
	}

	//end body parts
	if( xmlTextWriterEndElement( vcmWriter ) < 0 )
	{
		logVCMWriteError( "bodyParts" );
		return false;
	}

	return true;
}

bool VCM::writeNodeBodyPart( xmlTextWriterPtr vcmWriter, const string& bodyPartId,
		const StringArray& meshList, const string& nodeId )
{
	//start body part
	if( xmlTextWriterStartElement( vcmWriter, BAD_CAST bodyPartId.data() ) < 0 )
	{
		logVCMWriteError( bodyPartId );
		return false;
	}

	//write meshes
	if( !writeNodeBodyPartMeshes( vcmWriter, meshList ) )
	{
		return false;
	}

	//write node
	if( !writeNodeBodyPartNode( vcmWriter, nodeId ) )
	{
		return false;
	}

	//end body part
	if( xmlTextWriterEndElement( vcmWriter ) < 0 )
	{
		logVCMWriteError( bodyPartId );
		return false;
	}

	return true;
}

bool VCM::writeNodeBodyPartMeshes( xmlTextWriterPtr vcmWriter, const StringArray& meshList )
{
	if( meshList.empty() )
		return true;

	//start meshes
	if( xmlTextWriterStartElement( vcmWriter, BAD_CAST "meshes" ) < 0 )
	{
		logVCMWriteError( "meshes" );
		return false;
	}

	//write meshes
	for( StringArray::const_iterator i = meshList.begin(); i != meshList.end(); ++i )
	{
		if( !writeNodeBodyPartMesh( vcmWriter, *i ) )
		{
			return false;
		}
	}

	//end meshes
	if( xmlTextWriterEndElement( vcmWriter ) < 0 )
	{
		logVCMWriteError( "meshes" );
		return false;
	}

	return true;
}

bool VCM::writeNodeBodyPartMesh( xmlTextWriterPtr vcmWriter, const string& meshId )
{
	if( meshId == "" )
		return true;

	//start mesh
	if( xmlTextWriterStartElement( vcmWriter, BAD_CAST "mesh" ) < 0 )
	{
		logVCMWriteError( "mesh" );
		return false;
	}

	//write mesh id
	if( xmlTextWriterWriteAttribute( vcmWriter, BAD_CAST "id", BAD_CAST meshId.data() ) < 0 )
	{
		logVCMWriteError( "id" );
		return false;
	}

	//end mesh
	if( xmlTextWriterEndElement( vcmWriter ) < 0 )
	{
		logVCMWriteError( "mesh" );
		return false;
	}

	return true;
}

bool VCM::writeNodeBodyPartNode( xmlTextWriterPtr vcmWriter, const string& nodeId )
{
	if( nodeId == "" )
		return true;

	//start node
	if( xmlTextWriterStartElement( vcmWriter, BAD_CAST "node" ) < 0 )
	{
		logVCMWriteError( "node" );
		return false;
	}

	//write node id
	if( xmlTextWriterWriteAttribute( vcmWriter, BAD_CAST "id", BAD_CAST nodeId.data() ) < 0 )
	{
		logVCMWriteError( "id" );
		return false;
	}

	//end node
	if( xmlTextWriterEndElement( vcmWriter ) < 0 )
	{
		logVCMWriteError( "node" );
		return false;
	}

	return true;
}

bool VCM::writeNodeSkeleton( xmlTextWriterPtr vcmWriter )
{
	//start skeleton
	if( xmlTextWriterStartElement( vcmWriter, BAD_CAST "skeleton" ) < 0 )
	{
		logVCMWriteError( "skeleton" );
		return false;
	}

	//write COM
	if( !writeNodeCOM( vcmWriter ) )
	{
		return false;
	}

	//end skeleton
	if( xmlTextWriterEndElement( vcmWriter ) < 0 )
	{
		logVCMWriteError( "skeleton" );
		return false;
	}

	return true;
}

bool VCM::writeNodeCOM( xmlTextWriterPtr vcmWriter )
{
	//start COM
	if( xmlTextWriterStartElement( vcmWriter, BAD_CAST "COM" ) < 0 )
	{
		logVCMWriteError( "COM" );
		return false;
	}

	//write node id
	if( xmlTextWriterWriteAttribute( vcmWriter, BAD_CAST "id", BAD_CAST skeletonCOM.data() ) < 0 )
	{
		logVCMWriteError( "id" );
		return false;
	}

	//end COM
	if( xmlTextWriterEndElement( vcmWriter ) < 0 )
	{
		logVCMWriteError( "COM" );
		return false;
	}

	return true;
}

bool VCM::writeNodeAnimationParameterMappings( xmlTextWriterPtr vcmWriter )
{
	if( mappings.empty() )
		return true;

	//start param. mappings
	if( xmlTextWriterStartElement( vcmWriter, BAD_CAST "animationParameterMappings" ) < 0 )
	{
		logVCMWriteError( "animationParameterMappings" );
		return false;
	}

	//write each param. mapping
	for( FBAPMappingsMap::const_iterator i = mappings.begin(); i != mappings.end(); ++i )
	{
		if( !writeNodeAnimationParameterMapping( vcmWriter, i->second ) )
			return false;
	}

	//end param. mappings
	if( xmlTextWriterEndElement( vcmWriter ) < 0 )
	{
		logVCMWriteError( "animationParameterMappings" );
		return false;
	}

	return true;
}

bool VCM::writeNodeAnimationParameterMapping( xmlTextWriterPtr vcmWriter,
											 const FBAPMapping& mapping )
{
	string ref_val_str;

	if( !writeInt( mapping.getRefValue(), ref_val_str ) )
	{
		logVCMWriteError( "animationParameterMapping" );
		return false;
	}

	//start param. mapping
	if( xmlTextWriterStartElement( vcmWriter, BAD_CAST "animationParameterMapping" ) < 0 )
	{
		logVCMWriteError( "animationParameterMapping" );
		return false;
	}

	//write param. name
	if( xmlTextWriterWriteAttribute( vcmWriter, BAD_CAST "paramName",
		BAD_CAST mapping.getFBAPName().data() ) < 0 )
	{
		logVCMWriteError( "paramName" );
		return false;
	}

	//write ref. value
	if( xmlTextWriterWriteAttribute( vcmWriter, BAD_CAST "refValue",
		BAD_CAST ref_val_str.data() ) < 0 )
	{
		logVCMWriteError( "refValue" );
		return false;
	}

	//write morph targets
	for( int i = 0; i < mapping.numberOfMorphTargets(); ++i )
	{
		string mesh_id, mt_id;
		mapping.getMorphTarget( i, mesh_id, mt_id );

		if( !writeNodeMorphTarget( vcmWriter, mesh_id, mt_id ) )
		{
			return false;
		}
	}
	
	//write trans. bones
	for( int i = 0; i < mapping.numberOfTransBones(); ++i )
	{
		string bone_id;
		Vector3 pos;

		mapping.getRefPosition( i, bone_id, pos );

		if( !writeNodeBoneTrans( vcmWriter, bone_id, pos ) )
		{
			return false;
		}
	}

	//write rot. bones
	for( int i = 0; i < mapping.numberOfRotBones(); ++i )
	{
		string bone_id;
		Quat orient;

		mapping.getRefOrientation( i, bone_id, orient );

		if( !writeNodeBoneRot( vcmWriter, bone_id, orient ) )
		{
			return false;
		}
	}

	//end param. mapping
	if( xmlTextWriterEndElement( vcmWriter ) < 0 )
	{
		logVCMWriteError( "animationParameterMapping" );
		return false;
	}

	return true;
}

bool VCM::writeNodeMorphTarget( xmlTextWriterPtr vcmWriter, const string& meshId, const string& mtId )
{
	//start morph target
	if( xmlTextWriterStartElement( vcmWriter, BAD_CAST "morphTarget" ) < 0 )
	{
		logVCMWriteError( "morphTarget" );
		return false;
	}

	//write mesh id
	if( xmlTextWriterWriteAttribute( vcmWriter, BAD_CAST "meshId", BAD_CAST meshId.data() )
		< 0 )
	{
		logVCMWriteError( "meshId" );
		return false;
	}

	//write morph target id
	if( xmlTextWriterWriteAttribute( vcmWriter, BAD_CAST "id", BAD_CAST mtId.data() )
		< 0 )
	{
		logVCMWriteError( "id" );
		return false;
	}

	//end morph target
	if( xmlTextWriterEndElement( vcmWriter ) < 0 )
	{
		logVCMWriteError( "morphTarget" );
		return false;
	}

	return true;
}

bool VCM::writeNodeBoneTrans( xmlTextWriterPtr vcmWriter, const string& boneId,
	const Vector3& refPosition )
{
	string ref_position_str;

	if( !writeVector3( refPosition, ref_position_str ) )
	{
		logVCMWriteError( "bone" );
		return false;
	}

	//start bone
	if( xmlTextWriterStartElement( vcmWriter, BAD_CAST "bone" ) < 0 )
	{
		logVCMWriteError( "bone" );
		return false;
	}

	//write bone id
	if( xmlTextWriterWriteAttribute( vcmWriter, BAD_CAST "id", BAD_CAST boneId.data() )
		< 0 )
	{
		logVCMWriteError( "id" );
		return false;
	}

	//write ref. position
	if( xmlTextWriterWriteAttribute( vcmWriter, BAD_CAST "refPosition",
		BAD_CAST ref_position_str.data() ) < 0 )
	{
		logVCMWriteError( "refPosition" );
		return false;
	}

	//end bone
	if( xmlTextWriterEndElement( vcmWriter ) < 0 )
	{
		logVCMWriteError( "bone" );
		return false;
	}

	return true;
}

bool VCM::writeNodeBoneRot( xmlTextWriterPtr vcmWriter, const string& boneId,
	const Quat& refOrientation )
{
	string ref_orientation_str;

	if( !writeQuat( refOrientation, ref_orientation_str ) )
	{
		logVCMWriteError( "bone" );
		return false;
	}

	//start bone
	if( xmlTextWriterStartElement( vcmWriter, BAD_CAST "bone" ) < 0 )
	{
		logVCMWriteError( "bone" );
		return false;
	}

	//write bone id
	if( xmlTextWriterWriteAttribute( vcmWriter, BAD_CAST "id", BAD_CAST boneId.data() )
		< 0 )
	{
		logVCMWriteError( "id" );
		return false;
	}

	//write ref. position
	if( xmlTextWriterWriteAttribute( vcmWriter, BAD_CAST "refOrientation",
		BAD_CAST ref_orientation_str.data() ) < 0 )
	{
		logVCMWriteError( "refOrientation" );
		return false;
	}

	//end bone
	if( xmlTextWriterEndElement( vcmWriter ) < 0 )
	{
		logVCMWriteError( "bone" );
		return false;
	}

	return true;
}

bool VCM::writeNodeNeutralPose( xmlTextWriterPtr vcmWriter )
{
	//start neutral pose
	if( xmlTextWriterStartElement( vcmWriter, BAD_CAST "neutralPose" ) < 0 )
	{
		logVCMWriteError( "neutralPose" );
		return false;
	}
	
	//write bone transformations
	for( NodeTransMap::const_iterator i = neutralPosePositions.begin();
		i != neutralPosePositions.end(); ++i )
	{
		if( !writeNodeBoneTransformation( vcmWriter, i->first ) )
		{
			return false;
		}
	}

	//write bone transformations
	for( NodeRotMap::const_iterator i = neutralPoseOrientations.begin();
		i != neutralPoseOrientations.end(); ++i )
	{
		if( neutralPosePositions.find( i->first ) != neutralPosePositions.end() )
			continue;

		if( !writeNodeBoneTransformation( vcmWriter, i->first ) )
		{
			return false;
		}
	}

	//end neutral pose
	if( xmlTextWriterEndElement( vcmWriter ) < 0 )
	{
		logVCMWriteError( "neutralPose" );
		return false;
	}

	return true;
}

bool VCM::writeNodeBoneTransformation( xmlTextWriterPtr vcmWriter, const string& boneId )
{
	string ref_position_str = "", ref_orientation_str = "";
	Vector3 ref_position;
	Quat ref_orientation;

	NodeTransMap::const_iterator np_trans_iter;
	if( ( np_trans_iter = neutralPosePositions.find( boneId ) ) != neutralPosePositions.end() )
	{
		ref_position = np_trans_iter->second;

		if( !writeVector3( ref_position, ref_position_str ) )
		{
			logVCMWriteError( "boneTransformation" );
			return false;
		}
	}

	NodeRotMap::const_iterator np_rot_iter;
	if( ( np_rot_iter = neutralPoseOrientations.find( boneId ) ) != neutralPoseOrientations.end() )
	{
		ref_orientation = np_rot_iter->second;
	
		if( !writeQuat( ref_orientation, ref_orientation_str ) )
		{
			logVCMWriteError( "boneTransformation" );
			return false;
		}
	}

	//start bone transf.
	if( xmlTextWriterStartElement( vcmWriter, BAD_CAST "boneTransformation" ) < 0 )
	{
		logVCMWriteError( "boneTransformation" );
		return false;
	}

	//write bone id
	if( xmlTextWriterWriteAttribute( vcmWriter, BAD_CAST "boneId", BAD_CAST boneId.data() )
		< 0 )
	{
		logVCMWriteError( "boneId" );
		return false;
	}

	//write ref. position
	if( ref_position_str != "" && xmlTextWriterWriteAttribute( vcmWriter, BAD_CAST "position",
		BAD_CAST ref_position_str.data() ) < 0 )
	{
		logVCMWriteError( "position" );
		return false;
	}

	//write ref. orientation
	if( ref_orientation_str != "" && xmlTextWriterWriteAttribute( vcmWriter, BAD_CAST "orientation",
		BAD_CAST ref_orientation_str.data() ) < 0 )
	{
		logVCMWriteError( "orientation" );
		return false;
	}
 
	//end bone transf.
	if( xmlTextWriterEndElement( vcmWriter ) < 0 )
	{
		logVCMWriteError( "boneTransformation" );
		return false;
	}

	return true;
}

bool VCM::writeInt( int value, string& valueStr )
{
	ostringstream oss;

	oss << value;
	valueStr = oss.str();
	
	return true;
}

bool VCM::writeIntVector3Array( const IntArray& value1, const Vector3Array& value2, string& valueStr )
{
	V_ASSERT( value1.size() == value2.size() );

	ostringstream oss;

	for( size_t i = 0; i < value1.size(); ++i )
	{
		oss << value1[i];
		oss << " ";
		oss << value2[i].x;
		oss << " ";
		oss << value2[i].y;
		oss << " ";
		oss << value2[i].z;
		if( ( i + 1 ) != value1.size() ) oss << " ";
	}
	
	valueStr = oss.str();

	return true;
}

bool VCM::writeVector3( const Vector3& value, string& valueStr )
{
	ostringstream oss;

	oss << value.x;
	oss << " ";
	oss << value.y;
	oss << " ";
	oss << value.z;

	valueStr = oss.str();

	return true;
}

bool VCM::writeQuat( const Quat& value, string& valueStr )
{
	ostringstream oss;

	oss << value.w;
	oss << " ";
	oss << value.x;
	oss << " ";
	oss << value.y;
	oss << " ";
	oss << value.z;

	valueStr = oss.str();

	return true;
}

void VCM::logFileReadError( const string& filename ) const
{
	V_LOG(
		string( "Error opening VCM file " + filename 
		+ " for reading." ).c_str() );
}

void VCM::logFileWriteError( const string& filename ) const
{
	V_LOG(
		string( "Error opening VCM file " + filename
		+ " for writing." ).c_str() );
}

void VCM::logVCMParseError( int lineNo, const string& nodeName ) const
{
	string lnstr = toString<int>(lineNo);

	V_LOG(
		string( "Error parsing VCM file at line " + lnstr
		+ ", node " + nodeName + "." ).c_str() );
}

void VCM::logVCMSetIndentError() const
{
	V_LOG( "Error setting VCM file indentation." );
}

void VCM::logVCMWriteError( const string& nodeName ) const
{
	V_LOG(
		string( "Error writing to VCM file, node "
		+ nodeName + "." ).c_str() );
}
}
