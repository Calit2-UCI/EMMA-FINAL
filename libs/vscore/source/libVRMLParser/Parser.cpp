#include "stdafx.h"
#include "Parser.h"
#include "Scene.h"
#include "NodeList.h"
#include "Proto.h"
#include "FieldValue.h"
#include "Field.h"
#include "SFBool.h"
#include "SFColor.h"
#include "SFFloat.h"
#include "SFImage.h"
#include "SFInt32.h"
#include "SFNode.h"
#include "SFRotation.h"
#include "SFString.h"
#include "SFTime.h"
#include "SFVec2f.h"
#include "SFVec3f.h"
#include "MFColor.h"
#include "MFInt32.h"
#include "MFFloat.h"
#include "MFNode.h"
#include "MFRotation.h"
#include "MFString.h"
#include "MFTime.h"
#include "MFVec2f.h"
#include "MFVec3f.h"
#include "EventIn.h"
#include "EventOut.h"
#include "Field.h"
#include "ExposedField.h"
#include <math.h>
#include <stdlib.h>

Parser::Parser(Scene *scene, FILE *f)
{
	this->scene = scene;
	fseek(f, 0L, SEEK_END);
	long size = ftell(f);
	content = new char[size + 1];

	rewind(f);
        
    
	char c;
	int i = 0;
	while ((c = fgetc(f)) != EOF) content[i++] = c;
	content[i] = '\0';
	line = 1;

	standardNodes.append("Anchor");
	standardNodes.append("Appearance");
	standardNodes.append("AudioClip");
	standardNodes.append("Background");
	standardNodes.append("Billboard");
	standardNodes.append("Box");
	standardNodes.append("Collision");
	standardNodes.append("Color");
	standardNodes.append("ColorInterpolator");
	standardNodes.append("Cone");
	standardNodes.append("Coordinate");
	standardNodes.append("CoordinateInterpolator");
	standardNodes.append("Cylinder");
	standardNodes.append("CylinderSensor");
	standardNodes.append("DirectionalLight");
	standardNodes.append("ElevationGrid");
	standardNodes.append("Extrusion");
	standardNodes.append("Fog");
	standardNodes.append("FontStyle");
	standardNodes.append("Group");
	standardNodes.append("ImageTexture");
	standardNodes.append("IndexedFaceSet");
	standardNodes.append("IndexedLineSet");
	standardNodes.append("Inline");
	standardNodes.append("LOD");
	standardNodes.append("Material");
	standardNodes.append("MovieTexture");
	standardNodes.append("NavigationInfo");
	standardNodes.append("Normal");
	standardNodes.append("NormalInterpolator");
	standardNodes.append("OrientationInterpolator");
	standardNodes.append("PixelTexture");
	standardNodes.append("PlaneSensor");
	standardNodes.append("PointLight");
	standardNodes.append("PointSet");
	standardNodes.append("PositionInterpolator");
	standardNodes.append("ProximitySensor");
	standardNodes.append("ScalarInterpolator");
	standardNodes.append("Script");
	standardNodes.append("Shape");
	standardNodes.append("Sound");
	standardNodes.append("Sphere");
	standardNodes.append("SphereSensor");
	standardNodes.append("SpotLight");
	standardNodes.append("Switch");
	standardNodes.append("Text");
	standardNodes.append("TextureCoordinate");
	standardNodes.append("TextureTransform");
	standardNodes.append("TimeSensor");
	standardNodes.append("TouchSensor");
	standardNodes.append("Transform");
	standardNodes.append("Viewpoint");
	standardNodes.append("VisibilitySensor");
	standardNodes.append("WorldInfo");
}

Parser::~Parser()
{
	delete[] content;
}

int Parser::parseVRMLFile()
{
	position = 0;
	NodeList *nodes;
	char *temp = new char[100];
	char *name = new char[100];
	char *letter = new char;
	int flag;

	// reads characters until it gets to the VRML file header
	// which starts with the character '#' (ASCII code is 35)
	while (true)
	{
		do
		{
			if (content[position] == '\0')
			{
				delete [] temp;
				delete [] name;
				delete letter;
				return 0;
			}
			if (content[position] == '\n') line++;
			*letter = content[position++];
		} 
		while ((int)*letter != 35);

		// parses the VRML file header which has the following form:
		// #VRML V2.0 <encoding type> [optional comment] <line terminator>
		int tempPosition;
		int i;	//CHANGED BY I.K. declared iterator outside of loop
		for (i = 0; i < 10; i++)
		{
			tempPosition = position + i;
			if (content[tempPosition] == '\0')
			{
				delete [] temp;
				delete [] name;
				delete letter;
				return 0;
			}
			temp[i] = content[tempPosition];
		}
		temp[i] = '\0';

		// checks if this really is the header
		if (strcmp(temp, "VRML V2.0 ") == 0)
		{
			position += 10; 
			break;
		}
	}

	// reads the encoding type (there is no checking is this the
	// correct type, i.e. utf8)
	if (content[position] == '\0')
	{
		delete [] temp;
		delete [] name;
		delete letter;
		return 0;
	}
	else
	{
		*letter = content[position++];
	}

	// ASCII codes of the following characters are:
	// SPACE or ' '             32
	// CR or '\r'               13
	// LF or '\n'               10
	// HORIZONTAL TAB or '\t'    9
	// COMMA or ','             44
	//			'#'				35
	//			'['				91
	//			']'				93
	//			'{'			   123
	//			'}'			   125
	int i = 0;
	while (((int)*letter != 32) && ((int)*letter != 13) && 
		((int)*letter != 10) && ((int)*letter != 9))
	{
		temp[i++] = *letter;
		if (content[position] == '\0')
		{
			delete [] temp;
			delete [] name;
			delete letter;
			return 0;
		}
		*letter = content[position++];
	}
	position--;
	temp[i] = '\0';

	// checks if there is an encoding type (if not it returns 1, i.e.
	// it's an error)
	if (strcmp(temp, "") == 0)
	{
		scene->errorf((const char *) (String("Line ") += longToString(line)),
			": Incorrect header of the VRML file - encoding type missing or",
			"is not correct.\n", NULL);
		delete [] temp;
		delete [] name;
		delete letter;
		return 1;
	}

	// reads the optional comment (if there is one) and ignores it
	do
	{
		if (content[position] == '\0')
		{
			delete [] temp;
			delete [] name;
			delete letter;
			return 0;
		}
		if (content[position] == '\n') line++;
		*letter = content[position++];
	} 
	while (((int)*letter != 13) && ((int)*letter != 10));

	nodes = new NodeList();

	// parses the rest of the file
	while (getNextWord(temp, NULL) != 0)//Aqui anadir el primer NaradaNotificationCenter::postNotification(naradaNotifLoadingProgress)
	{
		if (strcmp(temp, "DEF") == 0)
		{
			if (getNextWord(name, NULL) == 0) break;
			if (isCorrect(name) == false)
			{
				scene->errorf((const char *) (String("Line ") +=
					longToString(line)), ": ", NULL);
				scene->invalidNode(name);
				delete nodes;
				delete [] temp;
				delete [] name;
				delete letter;
				return 1;
			}
			if (getNextWord(temp, NULL) == 0) break;

			if (scene->getProto(temp) != NULL)
			{
				Node *node = scene->createNode(temp);
				flag = parseNode(node);
				if (flag == 0)
				{
					break;
				}
				else if (flag == -1)
				{
					delete nodes;
					delete [] temp;
					delete [] name;
					delete letter;
					return 1;
				}

				Node *definedNode = scene->getNodeByName(name);
				if (definedNode != NULL)
				{
					if (definedNode != node)
					{
						// some other node already has that name
						/*
						scene->errorf((const char *) (String("Line ") +=
							longToString(line)), ": ", NULL);
						scene->invalidNode(name);
						scene->errorf("(Some other node already has that name)", NULL);
						delete nodes;
						delete [] temp;
						delete [] name;
						delete letter;
						return 1;
						*/

						String newName;

						while(true)
						{
							String tempName(name);
							tempName += generateRandomNumber();
							definedNode = scene->getNodeByName((const char *) tempName);
							if (definedNode == NULL)
							{
								newName += tempName;
								break;
							}
						}

						node->setName((const char *) newName);
						scene->def((const char *) newName, node);
					}
				}
				else
				{
					scene->def(name, node);
				}
				nodes->append(node);
			}
			else if (standardNodes.find(String(temp)) != -1)
			{
				scene->errorf((const char *) (String("Line ") +=
					longToString(line)), ": Standard node type that is ",
					(const char *) (String("not implemented - ") += temp),
					"\n", NULL);
				skipNextSection("{", "}");
			}
			else
			{
				scene->errorf((const char *) (String("Line ") +=
					longToString(line)), (const char *)
					(String(": Unknown node type - ") += temp), "\n",
					NULL);
				delete nodes;
				delete [] temp;
				delete [] name;
				delete letter;
				return 1;
			}
		}
		else if (scene->getProto(temp) != NULL)
		{
			Node *node = scene->createNode(temp);//Aqui habria que poner un NaradaNotificationCenter::postNotification, este es el caso de encontrarse un AnimationParameter
			flag = parseNode(node);
			if (flag == 0)
			{
				break;
			}
			else if (flag == -1)
			{
				delete nodes;
				delete [] temp;
				delete [] name;
				delete letter;
				return 1;
			}
			nodes->append(node);
		}
		else if (strcmp(temp, "PROTO") == 0)
		{
			if (getNextWord(name, NULL) == 0) break;
			if (scene->getProto(name) != NULL)
			{
				scene->errorf((const char *) (String("Line ") +=
					longToString(line)), (const char *)
					(String(": PROTO already defined - ") += name), "\n",
					NULL);
				skipNextSection("[", "]");
				skipNextSection("{", "}");
			}
			else
			{
				if (isCorrect(name) == false)
				{
					scene->errorf((const char *) (String("Line ") +=
						longToString(line)), (const char *)
						(String(": Invalid PROTO name - ") += name), "\n",
						NULL);
					delete nodes;
					delete [] temp;
					delete [] name;
					delete letter;
					return 1;
				}
				Proto *proto = new Proto(scene, name);
				flag = parseProto(proto);
				if (flag == 0)
				{
					delete proto;
					break;
				}
				else if (flag == -1)
				{
					delete proto;
					delete nodes;
					delete [] temp;
					delete [] name;
					delete letter;
					return 1;
				}
				scene->addProto(name, proto);
			}
		}
		else if (strcmp(temp, "ROUTE") == 0)
		{
			int flag = parseRoute();
			if (flag == 0)
			{
				break;
			}
			else if (flag == -1)
			{
				delete nodes;
				delete [] temp;
				delete [] name;
				delete letter;
				return 1;
			}
		}
		else if (standardNodes.find(String(temp)) != -1)
		{
			scene->errorf((const char *) (String("Line ") +=
				longToString(line)), ": Standard node type that is ",
				(const char *) (String("not implemented - ") += temp),
				"\n", NULL);
			skipNextSection("{", "}");
		}
		else
		{
			scene->errorf((const char *) (String("Line ") +=
				longToString(line)), (const char *)
				(String(": Unknown symbol - ") += temp), "\n", NULL);
			delete nodes;
			delete [] temp;
			delete [] name;
			delete letter;
			return 1;
		}
	}

	scene->setNodes(nodes);
	/*
	int size = nodes->size();

	for (int i = 0; i < size; i++)
	{
		Node *node = nodes->get(0);
		nodes->remove(0, 0);
		node->unref();
	}
	*/
	delete nodes;
	delete [] temp;
	delete [] name;
	delete letter;
	return 0;
}

int Parser::getNextWord(char *dest, char *separator)
{
	char *letter = new char;
	*dest = '\0';

	// ignores the voids, i.e. CR, LF, tabs, spaces and commas
	do
	{
		if (content[position] == '\0')
		{
			delete letter;
			return 0;
		}
		if (content[position] == '\n') line++;
		*letter = content[position++];
	} 
	while (((int)*letter == 32) || ((int)*letter == 13) || 
		((int)*letter == 10) || ((int)*letter == 9) || ((int)*letter == 44));

	// if it reads a character like '[', ']', '{', or '}'
	// it returns that character (separator needed for additional
	// checking)
	if (getSeparator(*letter, dest) == true)
	{
		delete letter;
		return 1;
	}

	// if it reads a character equal to the parameter saparator
	// it returns that character
	if ((separator != NULL) && (*letter == *separator))
	{
		*dest = *letter;
		*(dest + 1) = '\0';
		delete letter;
		return 1;
	}

	// ignores comments which start with '#'
	while ((int)*letter == 35)
	{
		do
		{
			if (content[position] == '\0')
			{
				delete letter;
				return 0;
			}
			if (content[position] == '\n') line++;
			*letter = content[position++];
		}
		while (((int)*letter != 10) && ((int)*letter != 13));

		// ignores the voids, i.e. CR, LF, tabs, spaces and commas
		do
		{
			if (content[position] == '\0')
			{
				delete letter;
				return 0;
			}
			if (content[position] == '\n') line++;
			*letter = content[position++];
		} 
		while (((int)*letter == 32) || ((int)*letter == 13) || 
			((int)*letter == 10) || ((int)*letter == 9) ||
			((int)*letter == 44));
	}

	// if it reads a character like '[', ']', '{', or '}'
	// it returns that character
	if (getSeparator(*letter, dest) == true)
	{
		delete letter;
		return 1;
	}

	// if it reads a character equal to the parameter saparator
	// it returns that character
	if ((separator != NULL) && (*letter == *separator))
	{
		*dest = *letter;
		*(dest + 1) = '\0';
		delete letter;
		return 1;
	}

	// reads the next word
	*dest = *letter;
	*letter = content[position];
	int i = 1;

	while ((*letter != '\0') && ((int)*letter != 32) &&
		((int)*letter != 13) &&	((int)*letter != 10) &&
		((int)*letter != 9) && ((int)*letter != 35) &&
		((int)*letter != 44) &&	((int)*letter != 91) &&
		((int)*letter != 93) && ((int)*letter != 123) &&
		((int)*letter != 125))
	{
		if ((separator != NULL) && (*letter == *separator)) break;

		*(dest + i) = *letter;
		i++;
		position++;
		*letter = content[position];
	}

	*(dest + i) = '\0';
	delete letter;
	return i;
}

bool Parser::getSeparator(char letter, char *dest)
{
	if (((int)letter == 91) || ((int)letter == 93) ||
		((int)letter == 123) ||	((int)letter == 125))
	{
		*dest = letter;
		*(dest + 1) = '\0';
		return true;
	}

	return false;
}

int Parser::parseNode(Node *node)
{
	char *temp = new char[100];
	char *name = new char[100];
	int flag;

	// end of file content
	if (getNextWord(temp, NULL) == 0)
	{
		delete [] temp;
		delete [] name;
		return 0;
	}

	// syntax error
	if (strcmp(temp, "{") != 0)
	{
		delete [] temp;
		delete [] name;
		return -1;
	}

	if (getNextWord(temp, NULL) == 0)
	{
		delete [] temp;
		delete [] name;
		return 0;
	}

	while (strcmp(temp, "}") != 0)
	{
		int index;
		// parses field
		if ((index = node->getProto()->lookupField(temp)) != INVALID_INDEX)
		{
			if (getNextWord(temp, NULL) == 0)
			{
				delete [] temp;
				delete [] name;
				return 0;
			}
			if (strcmp(temp, "IS") == 0)
			{
				if (getNextWord(temp, NULL) == 0)
				{
					delete [] temp;
					delete [] name;
					return 0;
				}
				int protoField;
				if ((protoField = node->getProto()->lookupField(temp))
					== INVALID_INDEX)
				{
					scene->errorf((const char *) (String("Line ") +=
						longToString(line)), ": Invalid IS statement - ",
						(const char *) (String("incorrect Proto field name ")
						+= name), "\n", NULL);
				}

				int type1 = node->getField(index)->getType();
				int type2 = node->getProto()->getField(protoField)->getType();
				if (type1 != type2)
				{
					scene->errorf((const char *) (String("Line ") +=
						longToString(line)), ": Invalid IS statement - field",
						" types don't match.\n", NULL);
				}

				scene->setIS(temp, index);
			}
			else
			{
				Field *field = node->getProto()->getField(index);
				int *flag = new int;
				FieldValue *value = parseFieldValue(field->getType(),
					field->getName(), field->getMin(), field->getMax(),
					field->getNodeType(), temp, flag);

				if (*flag != 1)
				{
					delete [] temp;
					delete [] name;
					return *flag;
				}
				node->setField(index, value);
				delete flag;
			}
		}

		/*
		// parses eventIn or eventOut in case of IS statements
		else if ((index = node->lookupEventIn(temp)) != INVALID_INDEX)
		{
		}
		else if ((index = node->lookupEventOut(temp)) != INVALID_INDEX)
		{
		}
		*/

		// parses PROTO
		else if (strcmp(temp, "PROTO") == 0)
		{
			if (getNextWord(name, NULL) == 0)
			{
				delete [] temp;
				delete [] name;
				return 0;
			}
			if (scene->getProto(name) != NULL)
			{
				scene->errorf((const char *) (String("Line ") +=
					longToString(line)), (const char *)
					(String(": PROTO already defined - ") += name), "\n",
					NULL);
				skipNextSection("[", "]");
				skipNextSection("{", "}");
			}
			else
			{
				if (isCorrect(name) == false)
				{
					scene->errorf((const char *) (String("Line ") +=
						longToString(line)), (const char *)
						(String(": Invalid PROTO name - ") += name), "\n",
						NULL);
					delete [] temp;
					delete [] name;
					return -1;
				}
				Proto *proto = new Proto(scene, name);
				flag = parseProto(proto);
				if (flag != 1)
				{
					delete [] temp;
					delete [] name;
					return flag;
				}
				scene->addProto(name, proto);
			}
		}
		// parses ROUTE
		else if (strcmp(temp, "ROUTE") == 0)
		{
			int flag = parseRoute();
			if (flag != 1)
			{
				delete [] temp;
				delete [] name;
				return flag;
			}
		}
		else
		{
			scene->errorf((const char *) (String("Line ") +=
				longToString(line)), (const char *)
				(String(": Unknown symbol - ") += temp), "\n",
				NULL);
			delete [] temp;
			delete [] name;
			return -1;
		}

		if (getNextWord(temp, NULL) == 0)
		{
			delete [] temp;
			delete [] name;
			return 0;
		}
	}

	delete [] temp;
	delete [] name;
	return 1;
}

FieldValue * Parser::parseFieldValue(int fieldType, const char *fieldName,
									 float minValue, float maxValue,
									 int nodeType, char *temp, int *flag)
{
	*flag = 1;
	FieldValue *newValue = NULL;
	int *intValue = NULL;
	int i;
	bool *isOK = new bool;
	int width, height, depth, size;
	MFColor *colors = NULL;
	MFInt32 *integers = NULL;
	MFFloat *floats = NULL;
	MFNode *nodes = NULL;
	MFRotation *rotations = NULL;
	MFString *strings = NULL;
	MFVec2f *vectors2f = NULL;
	MFVec3f *vectors3f = NULL;
	MFTime *times = NULL;

	switch (fieldType)
	{
		case SFBOOL:
			if (strcmp(temp, "TRUE") == 0)
				newValue = new SFBool(true);
			else if (strcmp(temp, "FALSE") == 0)
				newValue = new SFBool(false);
			else
			{
				scene->errorf((const char *) (String("Line ") +=
					longToString(line)), (const char *)
					(String(": Incorrect SFBool field value ") += temp),
					"\n", NULL);
				*flag = -1;
				delete isOK;
				return NULL;
			}
			break;

		case SFCOLOR:
			newValue = getSFColorValue(temp, minValue, maxValue, flag);
			if (*flag != 1)
			{
				delete isOK;
				return NULL;
			}
			break;

		case SFIMAGE:
			width = getIntValue(temp, isOK);
			if (*isOK == false)
			{
				scene->errorf((const char *) (String("Line ") +=
					longToString(line)), (const char *)
					(String(": Incorrect integer value ") += temp), "\n",
					NULL);
				*flag = -1;
				delete isOK;
				return NULL;
			}

			if (getNextWord(temp, NULL) == 0)
			{
				*flag = 0;
				delete isOK;
				return NULL;
			}
			height = getIntValue(temp, isOK);
			if (*isOK == false)
			{
				scene->errorf((const char *) (String("Line ") +=
					longToString(line)), (const char *)
					(String(": Incorrect integer value ") += temp), "\n",
					NULL);
				*flag = -1;
				delete isOK;
				return NULL;
			}

			if (getNextWord(temp, NULL) == 0)
			{
				*flag = 0;
				delete isOK;
				return NULL;
			}
			depth = getIntValue(temp, isOK);
			if (*isOK == false)
			{
				scene->errorf((const char *) (String("Line ") +=
					longToString(line)), (const char *)
					(String(": Incorrect integer value ") += temp), "\n",
					NULL);
				*flag = -1;
				delete isOK;
				return NULL;
			}

			if (((width != 0) || (height != 0) || (depth != 0)) &&
				(width * height * depth == 0))
			{
				scene->errorf((const char *) (String("Line ") +=
					longToString(line)), ": Incorrect SFImage value.\n",
					NULL);
				*flag = -1;
				delete isOK;
				return NULL;
			}

			if ((depth < 1) || (depth > 4))
			{
				scene->errorf((const char *) (String("Line ") +=
					longToString(line)), ": Incorrect SFImage value, wrong ",
					(const char *)(String("number of components - ") += temp), "\n", NULL);
				*flag = -1;
				delete isOK;
				return NULL;
			}

			if (width != 0)
			{
				size = width * height;
				intValue = new int[size];

				for (i = 0; i < size; i++)
				{
					if (getNextWord(temp, NULL) == 0)
					{
						*flag = 0;
						delete isOK;
						delete [] intValue;
						return NULL;
					}
					intValue[i] = getPixelValue(temp, depth, isOK);
					if (*isOK == false)
					{
						scene->errorf((const char *) (String("Line ") +=
							longToString(line)), (const char *)
							(String(": Incorrect pixel value ") += temp),
							"\n", NULL);
						*flag = -1;
						delete isOK;
						delete [] intValue;
						return NULL;
					}
				}
			}
			else
			{
				intValue = NULL;
			}

			newValue = new SFImage(width, height, depth, intValue);
			delete [] intValue;
			break;

		case SFINT32:
			newValue = getSFInt32Value(temp, minValue, maxValue, flag);
			if (*flag != 1)
			{
				delete isOK;
				return NULL;
			}
			break;

		case SFFLOAT:
			newValue = getSFFloatValue(temp, minValue, maxValue, flag);
			if (*flag != 1)
			{
				delete isOK;
				return NULL;
			}
			break;

		case SFNODE:
			newValue = getSFNodeValue(temp, nodeType, flag);
			if (*flag != 1)
			{
				delete isOK;
				return NULL;
			}
			if (newValue == NULL) newValue = new SFNode(NULL);
			break;

		case SFROTATION:
			newValue = getSFRotationValue(temp, flag);
			if (*flag != 1)
			{
				delete isOK;
				return NULL;
			}
			break;

		case SFSTRING:
			newValue = getSFStringValue(temp, flag);
			if (*flag != 1)
			{
				delete isOK;
				return NULL;
			}
			break;

		case SFTIME:
			newValue = getSFTimeValue(temp, flag);
			if (*flag != 1)
			{
				delete isOK;
				return NULL;
			}
			break;

		case SFVEC2F:
			newValue = getSFVec2fValue(temp, flag);
			if (*flag != 1)
			{
				delete isOK;
				return NULL;
			}
			break;

		case SFVEC3F:
			newValue = getSFVec3fValue(temp, fieldName, minValue, maxValue, flag);
			if (*flag != 1)
			{
				delete isOK;
				return NULL;
			}
			break;

		case MFCOLOR:
			if (strcmp(temp, "[") != 0)
			{
				scene->errorf((const char *) (String("Line ") +=
					longToString(line)),
					": Incorrect MFColor value - \"[\" missing.\n", NULL);
				*flag = -1;
				delete isOK;
				return NULL;
			}

			colors = new MFColor();
			if (getNextWord(temp, NULL) == 0)
			{
				*flag = 0;
				delete isOK;
				delete colors;
				return NULL;
			}

			i = 0;
			while (strcmp(temp, "]") != 0)
			{
				SFColor *color =
					getSFColorValue(temp, minValue, maxValue, flag);
				if (*flag != 1)
				{
					delete isOK;
					delete colors;
					return NULL;
				}
				colors->setSFValue(i, color);
				i++;
				delete color;
				if (getNextWord(temp, NULL) == 0)
				{
					*flag = 0;
					delete isOK;
					delete colors;
					return NULL;
				}
			}

			newValue = colors;
			break;

		case MFINT32:
			if (strcmp(temp, "[") != 0)
			{
				scene->errorf((const char *) (String("Line ") +=
					longToString(line)),
					": Incorrect MFInt32 value - \"[\" missing.\n", NULL);
				*flag = -1;
				delete isOK;
				return NULL;
			}

			integers = new MFInt32();
			if (getNextWord(temp, NULL) == 0)
			{
				*flag = 0;
				delete isOK;
				delete integers;
				return NULL;
			}

			i = 0;
			while (strcmp(temp, "]") != 0)
			{
				SFInt32 *integer =
					getSFInt32Value(temp, minValue, maxValue, flag);
				if (*flag != 1)
				{
					delete isOK;
					delete integers;
					return NULL;
				}
				integers->setSFValue(i, integer);
				i++;
				delete integer;
				if (getNextWord(temp, NULL) == 0)
				{
					*flag = 0;
					delete isOK;
					delete integers;
					return NULL;
				}
			}

			newValue = integers;
			break;

		case MFFLOAT:
			if (strcmp(temp, "[") != 0)
			{
				scene->errorf((const char *) (String("Line ") +=
					longToString(line)),
					": Incorrect MFFloat value - \"[\" missing.\n", NULL);
				*flag = -1;
				delete isOK;
				return NULL;
			}

			floats = new MFFloat();
			if (getNextWord(temp, NULL) == 0)
			{
				*flag = 0;
				delete isOK;
				delete floats;
				return NULL;
			}

			i = 0;
			while (strcmp(temp, "]") != 0)
			{
				SFFloat *floatVal =
					getSFFloatValue(temp, minValue, maxValue, flag);
				if (*flag != 1)
				{
					delete isOK;
					delete floats;
					return NULL;
				}
				floats->setSFValue(i, floatVal);
				i++;
				delete floatVal;
				if (getNextWord(temp, NULL) == 0)
				{
					*flag = 0;
					delete isOK;
					delete floats;
					return NULL;
				}
			}

			newValue = floats;
			break;

		case MFNODE:
			if (strcmp(temp, "[") != 0)
			{
				scene->errorf((const char *) (String("Line ") +=
					longToString(line)),
					": Incorrect MFNode value - \"[\" missing.\n", NULL);
				*flag = -1;
				delete isOK;
				return NULL;
			}

			nodes = new MFNode();
			if (getNextWord(temp, NULL) == 0)
			{
				*flag = 0;
				delete isOK;
				delete nodes;
				return NULL;
			}

			i = 0;
			while (strcmp(temp, "]") != 0)
			{
				Node *nodeVal = getFieldNode(temp, nodeType, flag);
				if (*flag != 1)
				{
					delete isOK;
					delete nodes;
					return NULL;
				}

				if (nodeVal != NULL)
				{
					nodes->getValue()->set(i, nodeVal);
					i++;
				}

				if (getNextWord(temp, NULL) == 0)
				{
					*flag = 0;
					delete isOK;
					delete nodes;
					return NULL;
				}
			}

			newValue = nodes;
			break;

		case MFROTATION:
			if (strcmp(temp, "[") != 0)
			{
				scene->errorf((const char *) (String("Line ") +=
					longToString(line)),
					": Incorrect MFRotation value - \"[\" missing.\n", NULL);
				*flag = -1;
				delete isOK;
				return NULL;
			}

			rotations = new MFRotation();
			if (getNextWord(temp, NULL) == 0)
			{
				*flag = 0;
				delete isOK;
				delete rotations;
				return NULL;
			}

			i = 0;
			while (strcmp(temp, "]") != 0)
			{
				SFRotation *rotation = getSFRotationValue(temp, flag);
				if (*flag != 1)
				{
					delete isOK;
					delete rotations;
					return NULL;
				}
				rotations->setSFValue(i, rotation);
				i++;
				delete rotation;
				if (getNextWord(temp, NULL) == 0)
				{
					*flag = 0;
					delete isOK;
					delete rotations;
					return NULL;
				}
			}

			newValue = rotations;
			break;

		case MFSTRING:
			if (strcmp(temp, "[") != 0)
			{
				strings = new MFString();
				SFString *string = getSFStringValue(temp, flag);
				if (*flag != 1)
				{
					delete isOK;
					delete strings;
					return NULL;
				}
				strings->setSFValue(0, string);
				delete string;
			}
			else
			{
				strings = new MFString();
				if (getNextWord(temp, NULL) == 0)
				{
					*flag = 0;
					delete isOK;
					delete strings;
					return NULL;
				}

				i = 0;
				while (strcmp(temp, "]") != 0)
				{
					SFString *string = getSFStringValue(temp, flag);
					if (*flag != 1)
					{
						delete isOK;
						delete strings;
						return NULL;
					}
					strings->setSFValue(i, string);
					i++;
					delete string;
					if (getNextWord(temp, NULL) == 0)
					{
						*flag = 0;
						delete isOK;
						delete strings;
						return NULL;
					}
				}
			}

			newValue = strings;
			break;

		case MFTIME:
			if (strcmp(temp, "[") != 0)
			{
				scene->errorf((const char *) (String("Line ") +=
					longToString(line)),
					": Incorrect MFTime value - \"[\" missing.\n", NULL);
				*flag = -1;
				delete isOK;
				return NULL;
			}

			times = new MFTime();
			if (getNextWord(temp, NULL) == 0)
			{
				*flag = 0;
				delete isOK;
				delete times;
				return NULL;
			}

			i = 0;
			while (strcmp(temp, "]") != 0)
			{
				SFTime *time = getSFTimeValue(temp, flag);
				if (*flag != 1)
				{
					delete isOK;
					delete times;
					return NULL;
				}
				times->setSFValue(i, time);
				i++;
				delete time;
				if (getNextWord(temp, NULL) == 0)
				{
					*flag = 0;
					delete isOK;
					delete times;
					return NULL;
				}
			}

			newValue = times;
			break;

		case MFVEC2F:
			if (strcmp(temp, "[") != 0)
			{
				scene->errorf((const char *) (String("Line ") +=
					longToString(line)),
					": Incorrect MFVec2f value - \"[\" missing ", NULL);
				*flag = -1;
				delete isOK;
				return NULL;
			}

			vectors2f = new MFVec2f();
			if (getNextWord(temp, NULL) == 0)
			{
				*flag = 0;
				delete isOK;
				delete vectors2f;
				return NULL;
			}

			i = 0;
			while (strcmp(temp, "]") != 0)
			{
				SFVec2f *vector2f = getSFVec2fValue(temp, flag);
				if (*flag != 1)
				{
					delete isOK;
					delete vectors2f;
					return NULL;
				}
				vectors2f->setSFValue(i, vector2f);
				i++;
				delete vector2f;
				if (getNextWord(temp, NULL) == 0)
				{
					*flag = 0;
					delete isOK;
					delete vectors2f;
					return NULL;
				}
			}

			newValue = vectors2f;
			break;

		case MFVEC3F:
			if (strcmp(temp, "[") != 0)
			{
				scene->errorf((const char *) (String("Line ") +=
						longToString(line)),
						": Incorrect MFVec3f value - \"[\" missing.\n", NULL);
				*flag = -1;
				delete isOK;
				return NULL;
			}

			vectors3f = new MFVec3f();
			if (getNextWord(temp, NULL) == 0)
			{
				*flag = 0;
				delete isOK;
				delete vectors3f;
				return NULL;
			}
			i = 0;
			
			while (strcmp(temp, "]") != 0)
			{
				SFVec3f *vector3f =
					getSFVec3fValue(temp, fieldName, minValue, maxValue, flag);
				if (*flag != 1)
				{
					delete isOK;
					delete vectors3f;
					return NULL;
				}
				vectors3f->setSFValue(i, vector3f);
				i++;
				vector3f->unref();

				if (getNextWord(temp, NULL) == 0)
				{
					*flag = 0;
					delete isOK;
					delete vectors3f;
					return NULL;
				}
			}

			newValue = vectors3f;
			break;
	}

	delete isOK;

	return newValue;
}

SFColor * Parser::getSFColorValue(char *temp, float minValue,
								  float maxValue, int *flag)
{
	float *floatValue = new float[3];
	bool *isOK = new bool;
	*flag = 1;

	for (int i = 0; i < 3; i++)
	{
		floatValue[i] = getFloatValue(temp, isOK);

		if (*isOK == false)
		{
			scene->errorf((const char *) (String("Line ") +=
				longToString(line)), (const char *)
				(String(": Incorrect float value ") += temp), "\n", NULL);
			*flag = -1;
			delete isOK;
			delete [] floatValue;
			return NULL;
		}

		if ((i != 2) && (getNextWord(temp, NULL) == 0))
		{
			*flag = 0;
			delete isOK;
			delete [] floatValue;
			return NULL;
		}
	}

	SFColor *value = new SFColor(floatValue);
//	value->clamp(minValue, maxValue);
	delete [] floatValue;
	delete isOK;
	return value;
}

SFInt32 * Parser::getSFInt32Value(char *temp, float minValue,
								  float maxValue, int *flag)
{
	*flag = 1;
	bool *isOK = new bool;
	int intValue = getInt32Value(temp, isOK);

	if (*isOK == false)
	{
		scene->errorf((const char *) (String("Line ") += longToString(line)),
			(const char *) (String(": Incorrect integer value ") += temp),
			"\n", NULL);
		*flag = -1;
		delete isOK;
		return NULL;
	}

	SFInt32 *value = new SFInt32(intValue);
//	value->clamp(minValue, maxValue);
	delete isOK;
	return value;
}

SFFloat * Parser::getSFFloatValue(char *temp, float minValue, float maxValue,
								  int *flag)
{
	*flag = 1;
	bool *isOK = new bool;
	float floatValue = getFloatValue(temp, isOK);

	if (*isOK == false)
	{
		scene->errorf((const char *) (String("Line ") += longToString(line)),
			(const char *) (String(": Incorrect float value ") += temp), "\n",
			NULL);
		*flag = -1;
		delete isOK;
		return NULL;
	}

	SFFloat *value = new SFFloat(floatValue);
//	value->clamp(minValue, maxValue);
	delete isOK;
	return value;
}

Node * Parser::getFieldNode(char *temp, int nodeType, int *flag)
{
	char *name = new char[100];
	Node *fieldNode;
	*flag = 1;

	if (strcmp(temp, "NULL") == 0)
	{
		fieldNode = NULL;
	}
	else if (strcmp(temp, "DEF") == 0)
	{
		if (getNextWord(name, NULL) == 0)
		{
			*flag = 0;
			delete [] name;
			return NULL;
		}
		if (isCorrect(name) == false)
		{
			scene->errorf((const char *) (String("Line ") +=
				longToString(line)), ": ", NULL);
			scene->invalidNode(name);
			*flag = -1;
			delete [] name;
			return NULL;
		}
		if (getNextWord(temp, NULL) == 0)
		{
			*flag = 0;
			delete [] name;
			return NULL;
		}

		if (scene->getProto(temp) != NULL)
		{
			fieldNode = scene->createNode(temp);
			*flag = parseNode(fieldNode);
			if (*flag != 1)
			{
				fieldNode->unref();
				delete [] name;
				return NULL;
			}

			Node *definedNode = scene->getNodeByName(name);
			if (definedNode != NULL)
			{
				if (definedNode != fieldNode)
				{
					// some other node already has that name
					/*
					scene->errorf((const char *) (String("Line ") +=
						longToString(line)), ": ", NULL);
					scene->invalidNode(name);
					scene->errorf("(Some other node already has that name)", NULL);
					fieldNode->unref();
					*flag = -1;
					delete [] name;
					return NULL;
					*/

					String newName;

					while(true)
					{
						String tempName(name);
						tempName += generateRandomNumber();
						definedNode = scene->getNodeByName((const char *) tempName);
						if (definedNode == NULL)
						{
							newName += tempName;
							break;
						}
					}

					fieldNode->setName((const char *) newName);
					scene->def((const char *) newName, fieldNode);
				}
			}
			else
			{
				scene->def(name, fieldNode);
			}
		}
		else if (standardNodes.find(String(temp)) != -1)
		{
			scene->errorf((const char *) (String("Line ") +=
				longToString(line)), ": Standard node type that is ",
				(const char *) (String("not implemented - ") += temp),
				"\n", NULL);
			skipNextSection("{", "}");
			delete [] name;
			return NULL;
		}
		else
		{
			scene->errorf((const char *) (String("Line ") +=
				longToString(line)), (const char *)
				(String(": Unknown node type - ") += temp), "\n",
				NULL);
			*flag = -1;
			delete [] name;
			return NULL;
		}
	}
	else if (strcmp(temp, "USE") == 0)
	{
		if (getNextWord(name, NULL) == 0)
		{
			*flag = 0;
			delete [] name;
			return NULL;
		}

		if (scene->getNodeByName(name) == NULL)
		{
			// there is no node that has that name
			scene->errorf((const char *) (String("Line ") +=
				longToString(line)), ": ", NULL);
			scene->invalidNode(name);
			scene->errorf("(There is no node that has that name)", NULL);
			*flag = -1;
			delete [] name;
			return NULL;
		}
		else
		{
			fieldNode = scene->use(name);
		}
	}
	else if (scene->getProto(temp) != NULL)
	{
		fieldNode = scene->createNode(temp);
		*flag = parseNode(fieldNode);
		if (*flag != 1)
		{
			fieldNode->unref();
			delete [] name;
			return NULL;
		}
	}
	else if (standardNodes.find(String(temp)) != -1)
	{
		scene->errorf((const char *) (String("Line ") +=
			longToString(line)), ": Standard node type that is ",
			(const char *) (String("not implemented - ") += temp),
			"\n", NULL);
		skipNextSection("{", "}");
		delete [] name;
		return NULL;
	}
	else
	{
		scene->errorf((const char *) (String("Line ") +=
			longToString(line)), (const char *)
			(String(": Unknown node type - ") += temp), "\n",
			NULL);
		*flag = -1;
		delete [] name;
		return NULL;
	}

	if ((nodeType != NODE_ANY) && (nodeType != fieldNode->getNodeClass()))
	{
		scene->errorf((const char *) (String("Line ") += longToString(line)),
			": Scene hierarchy is not valid, ", (const char *)
			(String("incorrect node type - ") += fieldNode->toString()),
			"\n", NULL);
		*flag = -1;
		fieldNode->unref();
		delete [] name;
		return NULL;
	}

	delete [] name;
	return fieldNode;
}

SFNode * Parser::getSFNodeValue(char *temp, int nodeType, int *flag)
{
	Node *fieldNode = getFieldNode(temp, nodeType, flag);

	if (*flag != 1) return NULL;

	SFNode *value = new SFNode(fieldNode);

//	if (fieldNode != NULL) fieldNode->unref();
	return value;
}

SFRotation * Parser::getSFRotationValue(char *temp, int *flag)
{
	float *floatValue = new float[4];
	bool *isOK = new bool;
	*flag = 1;

	for (int i = 0; i < 4; i++)
	{
		floatValue[i] = getFloatValue(temp, isOK);
		if (*isOK == false)
		{
			scene->errorf((const char *) (String("Line ") +=
				longToString(line)), (const char *)
				(String(": Incorrect float value ") += temp), "\n", NULL);
			*flag = -1;
			delete [] floatValue;
			delete isOK;
			return NULL;
		}
		if ((i != 3) && (getNextWord(temp, NULL) == 0))
		{
			*flag = 0;
			delete [] floatValue;
			delete isOK;
			return NULL;
		}
	}
/*
	// first three values should define normalized vector (vector
	// length should be 1)
	if ((pow(floatValue[0], 2) + pow(floatValue[1], 2) +
		pow(floatValue[2], 2)) != 1)
	{
		scene->errorf((const char *) (String("Line ") += longToString(line)),
			": Incorrect SFRotation value - not normalized rotation axis ",
			"vector.\n", NULL);
		*flag = -1;
		return NULL;
	}
*/
	SFRotation *value = new SFRotation(floatValue);
	delete [] floatValue;
	delete isOK;
	return value;
}

SFString * Parser::getSFStringValue(char *temp, int *flag)
{
	*flag = 1;
	String string;

	if (*temp != '\"')
	{
		scene->errorf((const char *) (String("Line ") += longToString(line)),
			": Incorrect string value - \" missing.\n", NULL);
		*flag = -1;
		return NULL;
	}

	// goes back to the character after the first character '\"'
	position -= (int)strlen(temp);	//CHANGED BY I.K. added explicit cast
	position++;

//	int i = 0;
	while (true)
	{
		if (content[position] == '\"')
		{
			position++;
			break;
		}
		else if (content[position] == '\\' &&
			((content[position + 1] == '\"') ||
			(content[position + 1] == '\\')))
		{
			string += content[position + 1];
			position += 2;
		}
		else if (content[position] == '\0')
		{
			*flag = 0;
			return NULL;
		}
		else
		{
			string += content[position];
			if (content[position] == '\n') line++;
			position++;
		}
	}

	return new SFString(string);
}

SFTime * Parser::getSFTimeValue(char *temp, int *flag)
{
	*flag = 1;
	bool *isOK = new bool;

	double doubleValue = getDoubleValue(temp, isOK);
	if (*isOK == false)
	{
		scene->errorf((const char *) (String("Line ") +=
			longToString(line)), (const char *)
			(String(": Incorrect double value ") += temp), "\n", NULL);
		*flag = -1;
		delete isOK;
		return NULL;
	}

	SFTime *value = new SFTime(doubleValue);
	delete isOK;
	return value;
}

SFVec2f * Parser::getSFVec2fValue(char *temp, int *flag)
{
	*flag = 1;
	float *floatValue = new float[2];
	bool *isOK = new bool;

	floatValue[0] = getFloatValue(temp, isOK);
	if (*isOK == false)
	{
		scene->errorf((const char *) (String("Line ") += longToString(line)),
			(const char *) (String(": Incorrect float value ") += temp),
			"\n", NULL);
		*flag = -1;
		delete [] floatValue;
		delete isOK;
		return NULL;
	}

	if (getNextWord(temp, NULL) == 0)
	{
		*flag = 0;
		delete [] floatValue;
		delete isOK;
		return NULL;
	}

	floatValue[1] = getFloatValue(temp, isOK);
	if (*isOK == false)
	{
		scene->errorf((const char *) (String("Line ") += longToString(line)),
			(const char *) (String(": Incorrect float value ") += temp),
			"\n", NULL);
		*flag = -1;
		delete [] floatValue;
		delete isOK;
		return NULL;
	}

	SFVec2f *value = new SFVec2f(floatValue);
	delete [] floatValue;
	delete isOK;
	return value;
}

SFVec3f * Parser::getSFVec3fValue(char *temp, const char *fieldName,
								  float minValue, float maxValue,
								  int *flag)
{
	float *floatValue = new float[3];
	bool *isOK = new bool;
	bool isMinusOne = true;
	*flag = 1;

	for (int i = 0; i < 3; i++)
	{
		floatValue[i] = getFloatValue(temp, isOK);
		if (*isOK == false)
		{
			scene->errorf((const char *) (String("Line ") +=
				longToString(line)), (const char *)
				(String(": Incorrect float value ") += temp), "\n", NULL);
			*flag = -1;
			delete [] floatValue;
			delete isOK;
			return NULL;
		}
		if (floatValue[i] != -1) isMinusOne = false;
		if ((i != 2) && (getNextWord(temp, NULL) == 0))
		{
			*flag = 0;
			delete [] floatValue;
			delete isOK;
			return NULL;
		}
	}

	SFVec3f *value = new SFVec3f(floatValue[0], floatValue[1],
		floatValue[2]);
	/*
	if (strcmp(fieldName, "bboxSize") == 0)
	{
		if (isMinusOne == false)
			value->clamp(minValue, maxValue);
	}
	else
	{
		value->clamp(minValue, maxValue);
	}
	*/
	delete [] floatValue;
	delete isOK;
	return value;
}

float Parser::getFloatValue(char *word, bool *flag)
{
	int wordSize = 0;
	int dotIndex = 0;
	int positive = 0;
	int negative = 0;
	int e = 0;
	float value = 0.0;
	*flag = true;

	// goes to the end of the word or to the character 'e' or 'E'
	while ((*(word + wordSize) != '\0') && (*(word + wordSize) != 'e') &&
		(*(word + wordSize) != 'E')) wordSize++;

	// searches the index of the dot inside the word
	while ((*(word + dotIndex) != '.') && dotIndex != wordSize) dotIndex++;
	
	// checks whether the number is negative or not
	int i = 0;
	if (*word == '-')
	{
		negative = 1;
		i = 1;
	}
	else if (*word == '+')
	{
		positive = 1;
		i = 1;
	}

	for (int p = dotIndex - 1 - negative - positive; i < wordSize; i++, p--)
	{
		if (i == dotIndex) i++;
		// checks if these characters are really digits
		if ((*(word + i) < '0') || (*(word + i) > '9'))
		{
			*flag = false;
			return 0.0;
		}
		value = value + (float) ((*(word + i) - '0') * pow(10.0f, p));
	}

	// if there is a character 'e' or 'E'
	if ((*(word + wordSize) == 'e') || (*(word + wordSize) == 'E'))
	{
		char *temp = new char[16];
		int i = 0;
		
		while (*(word + wordSize + 1 + i) != '\0')
		{
			*(temp + i) = *(word + wordSize + 1 + i);
			i++;
		}

		*(temp + i) = '\0';
		e = getIntValue(temp, flag);
		delete [] temp;
	}

	// checks if these characters are really digits
	if (*flag == false) return 0.0;

	value *= (float) pow(10.0f, e);
	if (negative == 1) value *= (-1);

	if (value)
	{
		value = (float)atof(word);

		if (!value)
		{
			*flag = false;
			return 0.0;
		}
	}

	return value;
}

double Parser::getDoubleValue(char *word, bool *flag)
{
	int wordSize = 0;
	int dotIndex = 0;
	int negative = 0;
	int positive = 0;
	int e = 0;
	double value = 0.0;
	*flag = true;

	// goes to the end of the word or to the character 'e' or 'E'
	while ((*(word + wordSize) != '\0') && (*(word + wordSize) != 'e') &&
		(*(word + wordSize) != 'E')) wordSize++;

	// searches the index of the dot inside the word
	while ((*(word + dotIndex) != '.') && dotIndex != wordSize) dotIndex++;
	
	// checks whether the number is negative or not
	int i = 0;
	if (*word == '-')
	{
		negative = 1;
		i = 1;
	}
	else if (*word == '+')
	{
		positive = 1;
		i = 1;
	}

	for (int p = dotIndex - 1 - negative - positive; i < wordSize; i++, p--)
	{
		if (i == dotIndex) i++;
		// checks if these characters are really digits
		if ((*(word + i) < '0') || (*(word + i) > '9'))
		{
			*flag = false;
			return 0.0;
		}
		value = value + ((*(word + i) - '0') * pow(10.0, (double) p));
	}

	// if there is a character 'e' or 'E'
	if ((*(word + wordSize) == 'e') || (*(word + wordSize) == 'E'))
	{
		char *temp = new char[16];
		int i = 0;
		
		while (*(word + wordSize + 1 + i) != '\0')
		{
			*(temp + i) = *(word + wordSize + 1 + i);
			i++;
		}

		*(temp + i) = '\0';
		e = getIntValue(temp, flag);
		delete [] temp;
	}
	
	// checks if these characters are really digits
	if (*flag == false) return 0.0;

	value *= pow(10.0, (double) e);
	if (negative == 1) value *= (-1);

	if (value)
	{
		value = atof(word);

		if (!value)
		{
			*flag = false;
			return 0.0;
		}
	}

	return value;
}

int Parser::getIntValue(char *word, bool *flag)
{
	int wordSize = 0;
	int value = 0;
	int negative = 0;
	int positive = 0;
	*flag = true;
	
	while (*(word + wordSize) != '\0') wordSize++;
	
	int i = 0;
	if (*word == '-')
	{
		negative = 1;
		i = 1;
	}
	else if (*word == '+')
	{
		positive = 1;
		i = 1;
	}

	for (; i < wordSize; i++)
	{
		// checks if these characters are really digits
		if ((*(word + i) < '0') || (*(word + i) > '9'))
		{
			*flag = false;
			return 0;
		}
		value = value +
			(int) ((*(word + i) - '0') * pow(10.0f, wordSize - 1 - i));
	}

	if (negative == 1) value *= (-1);

	if (value)
	{
		value = atoi(word);

		if (!value)
		{
			*flag = false;
			return 0;
		}
	}
	
	return value;	
}

int Parser::getPixelValue(char *word, int depth, bool *flag)
{
	int value = 0;
	*flag = true;
	int wordSize = 0;

	// checks if digits are hexadecimal or decimal
	if (strncmp(word, "0x", 2) == 0)
	{
		while (*(word + 2 + wordSize) != '\0') wordSize++;

		// checks if the depth (number of components) is OK
		if (wordSize > depth * 2)
		{
			*flag = false;
			return 0;
		}

		int digit;
		int potention = 1;

		for (int i = wordSize + 1; i > 1; i--)
		{
			if ((*(word + i) >= '0') && (*(word + i) <= '9'))
			{
				digit = *(word + i) - '0';
			}
			else if ((*(word + i) >= 'A') && (*(word + i) <= 'F'))
			{
				digit = *(word + i) - 'A' + 10;
			}
			else if ((*(word + i) >= 'a') && (*(word + i) <= 'f'))
			{
				digit = *(word + i) - 'a' + 10;
			}
			else
			{
				*flag = false;
				return 0;
			}

			value += digit * potention;
			potention *= 16;
		}
	}
	else
	{
		// checks if this is an unsigned value and if the depth
		// (number of components) is OK
		if ((*word == '-') ||
			(strlen(word) > (unsigned int) (pow(2.0f, depth * 8) - 1)))
		{
			*flag = false;
			return 0;
		}

		value = getIntValue(word, flag);

		// checks if these characters are really digits
		if (*flag == false) return 0;
	}

	return value;
}

int Parser::getInt32Value(char *word, bool *flag)
{
	int value = 0;
	*flag = true;
	int wordSize = 0;
	bool negative = false;
	int startIndex = 0;
	bool hexadecimal = false;

	// checks if digits are hexadecimal or decimal
	if (word[0] == '-')
	{
		negative = true;
		if (strncmp(word + 1, "0x", 2) == 0)
		{
			hexadecimal = true;
			startIndex += 3;
		}
	}
	else
	{
		if (strncmp(word + 1, "0x", 2) == 0)
		{
			hexadecimal = true;
			startIndex += 2;
		}
	}

	// digits are hexadecimal
	if (hexadecimal == true)
	{
		while (*(word + startIndex + wordSize) != '\0') wordSize++;

		int digit;
		int potention = 1;

		for (int i = wordSize + startIndex - 1; i >= startIndex; i--)
		{
			if ((*(word + i) >= '0') && (*(word + i) <= '9'))
			{
				digit = *(word + i) - '0';
			}
			else if ((*(word + i) >= 'A') && (*(word + i) <= 'F'))
			{
				digit = *(word + i) - 'A' + 10;
			}
			else if ((*(word + i) >= 'a') && (*(word + i) <= 'f'))
			{
				digit = *(word + i) - 'a' + 10;
			}
			else
			{
				*flag = false;
				return 0;
			}

			value += digit * potention;
			potention *= 16;
		}

		if (negative == true)
		{
			value *= -1;
		}
	}
	// digits are decimal
	else
	{
		value = getIntValue(word, flag);

		// checks if these characters are really digits
		if (*flag == false)	return 0;
	}

	return value;
}

int Parser::parseProto(Proto *proto)
{
	char *temp = new char[100];
	char *name = new char[100];
	int *flag = new int;

	// parses PROTO declaration
	if (getNextWord(temp, NULL) == 0)
	{
		delete [] temp;
		delete [] name;
		delete flag;
		return 0;
	}
	if (strcmp(temp, "[") != 0)
	{
		scene->errorf((const char *) (String("Line ") += longToString(line)),
			": Incorrect PROTO declaration - \"[\" missing.\n", NULL);
		delete [] temp;
		delete [] name;
		delete flag;
		return -1;
	}

	if (getNextWord(temp, NULL) == 0)
	{
		delete [] temp;
		delete [] name;
		delete flag;
		return 0;
	}

	while (strcmp(temp, "]") != 0)
	{
		Element *element;

		if (strcmp(temp, "eventIn") == 0)
		{
			if (getNextWord(temp, NULL) == 0)
			{
				delete [] temp;
				delete [] name;
				delete flag;
				return 0;
			}
			int type = scene->getFieldType(temp);
			if ((type == 0) && (strcmp(temp, "SFBool") != 0))
			{
				scene->errorf((const char *) (String("Line ") +=
					longToString(line)), (const char *)
					(String(": Unknown eventIn type - ") += temp), "\n",
					NULL);
				delete [] temp;
				delete [] name;
				delete flag;
				return -1;
			}

			if (getNextWord(name, NULL) == 0)
			{
				delete [] temp;
				delete [] name;
				delete flag;
				return 0;
			}
			if (isCorrect(name) == false)
			{
				scene->errorf((const char *) (String("Line ") +=
					longToString(line)), (const char *)
					(String(": Incorrect eventIn name - ") += name), "\n",
					NULL);
				delete [] temp;
				delete [] name;
				delete flag;
				return -1;
			}

			element = new EventIn(type, name);
		}
		else if (strcmp(temp, "eventOut") == 0)
		{
			if (getNextWord(temp, NULL) == 0)
			{
				delete [] temp;
				delete [] name;
				delete flag;
				return 0;
			}
			int type = scene->getFieldType(temp);
			if ((type == 0) && (strcmp(temp, "SFBool") != 0))
			{
				scene->errorf((const char *) (String("Line ") +=
					longToString(line)), (const char *)
					(String(": Unknown eventOut type - ") += temp), "\n",
					NULL);
				delete [] temp;
				delete [] name;
				delete flag;
				return -1;
			}

			if (getNextWord(name, NULL) == 0)
			{
				delete [] temp;
				delete [] name;
				delete flag;
				return 0;
			}
			if (isCorrect(name) == false)
			{
				scene->errorf((const char *) (String("Line ") +=
					longToString(line)), (const char *)
					(String(": Incorrect eventOut name - ") += name), "\n",
					NULL);
				delete [] temp;
				delete [] name;
				delete flag;
				return -1;
			}

			element = new EventOut(type, name);
		}
		else if (strcmp(temp, "field") == 0)
		{
			if (getNextWord(temp, NULL) == 0)
			{
				delete [] temp;
				delete [] name;
				delete flag;
				return 0;
			}
			int type = scene->getFieldType(temp);
			if ((type == 0) && (strcmp(temp, "SFBool") != 0))
			{
				scene->errorf((const char *) (String("Line ") +=
					longToString(line)), (const char *)
					(String(": Unknown field type - ") += temp), "\n", NULL);
				delete [] temp;
				delete [] name;
				delete flag;
				return -1;
			}

			if (getNextWord(name, NULL) == 0)
			{
				delete [] temp;
				delete [] name;
				delete flag;
				return 0;
			}
			if (isCorrect(name) == false)
			{
				scene->errorf((const char *) (String("Line ") +=
					longToString(line)), (const char *)
					(String(": Incorrect field name - ") += name), "\n",
					NULL);
				delete [] temp;
				delete [] name;
				delete flag;
				return -1;
			}

			if (getNextWord(temp, NULL) == 0)
			{
				delete [] temp;
				delete [] name;
				delete flag;
				return 0;
			}
			FieldValue *value = parseFieldValue(type, name, -FLT_MAX,
				FLT_MAX, NODE_ANY, temp, flag);
			if (*flag != 1)
			{
				delete [] temp;
				delete [] name;
				int f = *flag;
				delete flag;
				return f;
			}
			
			element = new Field(type, name, value);
		}
		else if (strcmp(temp, "exposedField") == 0)
		{
			if (getNextWord(temp, NULL) == 0)
			{
				delete [] temp;
				delete [] name;
				delete flag;
				return 0;
			}
			int type = scene->getFieldType(temp);
			if ((type == 0) && (strcmp(temp, "SFBool") != 0))
			{
				scene->errorf((const char *) (String("Line ") +=
					longToString(line)), (const char *)
					(String(": Unknown exposedField type - ") += temp), "\n",
					NULL);
				delete [] temp;
				delete [] name;
				delete flag;
				return -1;
			}

			if (getNextWord(name, NULL) == 0)
			{
				delete [] temp;
				delete [] name;
				delete flag;
				return 0;
			}
			if (isCorrect(name) == false)
			{
				scene->errorf((const char *) (String("Line ") +=
					longToString(line)), (const char *)
					(String(": Incorrect exposedField name - ") += name),
					"\n", NULL);
				delete [] temp;
				delete [] name;
				delete flag;
				return -1;
			}

			if (getNextWord(temp, NULL) == 0)
			{
				delete [] temp;
				delete [] name;
				delete flag;
				return 0;
			}
			FieldValue *value = parseFieldValue(type, name, -FLT_MAX,
				FLT_MAX, NODE_ANY, temp, flag);
			if (*flag != 1)
			{
				delete [] temp;
				delete [] name;
				int f = *flag;
				delete flag;
				return f;
			}

			element = new ExposedField(type, name, value);
		}
		else
		{
			scene->errorf((const char *) (String("Line ") +=
				longToString(line)), (const char *)
				(String(": Unknown element type - ") += temp), "\n", NULL);
			delete [] temp;
			delete [] name;
			delete flag;
			return -1;
		}

		proto->addElement(element);
		if (getNextWord(temp, NULL) == 0)
		{
			delete [] temp;
			delete [] name;
			delete flag;
			return 0;
		}
	}

	// parses PROTO definition
	if (getNextWord(temp, NULL) == 0)
	{
		delete [] temp;
		delete [] name;
		delete flag;
		return 0;
	}
	if (strcmp(temp, "{") != 0)
	{
		scene->errorf((const char *) (String("Line ") +=
			longToString(line)),
			": Incorrect PROTO definition - \"{\" missing.\n", NULL);
		delete [] temp;
		delete [] name;
		delete flag;
		return -1;
	}

	if (getNextWord(temp, NULL) == 0)
	{
		delete [] temp;
		delete [] name;
		delete flag;
		return 0;
	}

	Node *primaryNode = NULL;
	NodeList *nodes = new NodeList();

	while (strcmp(temp, "}") != 0)
	{
		// parses a Node
		if (strcmp(temp, "DEF") == 0)
		{
			if (getNextWord(name, NULL) == 0)
			{
				delete [] temp;
				delete [] name;
				delete flag;
				if (primaryNode != NULL) primaryNode->unref();
				delete nodes;
				return 0;
			}
			if (isCorrect(name) == false)
			{
				scene->errorf((const char *) (String("Line ") +=
					longToString(line)), ": ", NULL);
				scene->invalidNode(name);
				delete [] temp;
				delete [] name;
				delete flag;
				if (primaryNode != NULL) primaryNode->unref();
				delete nodes;
				return -1;
			}
			if (getNextWord(temp, NULL) == 0)
			{
				delete [] temp;
				delete [] name;
				delete flag;
				if (primaryNode != NULL) primaryNode->unref();
				delete nodes;
				return 0;
			}

			if (scene->getProto(temp) != NULL)
			{
				Node *node = scene->createNode(temp);
				*flag = parseNode(node);
				if (*flag != 1)
				{
					delete [] temp;
					delete [] name;
					int f = *flag;
					delete flag;
					if (primaryNode != NULL) primaryNode->unref();
					delete nodes;
					if (node != NULL) node->unref();
					return f;
				}

				Node *definedNode = scene->getNodeByName(name);
				if (definedNode != NULL)
				{
					if (definedNode != node)
					{
						// some other node already has that name
						/*
						scene->errorf((const char *) (String("Line ") +=
							longToString(line)), ": ", NULL);
						scene->invalidNode(name);
						scene->errorf("(Some other node already has that name)", NULL);
						delete [] temp;
						delete [] name;
						delete flag;
						if (primaryNode != NULL) primaryNode->unref();
						delete nodes;
						if (node != NULL) node->unref();
						return -1;
						*/

						String newName;

						while(true)
						{
							String tempName(name);
							tempName += generateRandomNumber();
							definedNode = scene->getNodeByName((const char *) tempName);
							if (definedNode == NULL)
							{
								newName += tempName;
								break;
							}
						}

						node->setName((const char *) newName);
						scene->def((const char *) newName, node);
					}
				}
				else
				{
					scene->def(name, node);
				}

				if (primaryNode == NULL) primaryNode = node;
				else nodes->append(node);
			}
			else if (standardNodes.find(String(temp)) != -1)
			{
				scene->errorf((const char *) (String("Line ") +=
					longToString(line)), ": Standard node type that is ",
					(const char *) (String("not implemented - ") += temp),
					"\n", NULL);
				skipNextSection("{", "}");
			}
			else
			{
				scene->errorf((const char *) (String("Line ") +=
					longToString(line)), (const char *)
					(String(": Unknown node type - ") += temp), "\n",
					NULL);
				delete [] temp;
				delete [] name;
				delete flag;
				if (primaryNode != NULL) primaryNode->unref();
				delete nodes;
				return -1;
			}
		}
		else if (scene->getProto(temp) != NULL)
		{
			Node *node = scene->createNode(temp);
			*flag = parseNode(node);
			if (*flag != 1)
			{
				delete [] temp;
				delete [] name;
				int f = *flag;
				delete flag;
				if (primaryNode != NULL) primaryNode->unref();
				delete nodes;
				if (node != NULL) node->unref();
				return f;
			}

			if (primaryNode == NULL) primaryNode = node;
			else nodes->append(node);
		}
		// parses a Proto
		else if (strcmp(temp, "PROTO") == 0)
		{
			if (getNextWord(name, NULL) == 0)
			{
				delete [] temp;
				delete [] name;
				delete flag;
				if (primaryNode != NULL) primaryNode->unref();
				delete nodes;
				return 0;
			}
			if (scene->getProto(name) != NULL)
			{
				scene->errorf((const char *) (String("Line ") +=
					longToString(line)), (const char *)
					(String(": PROTO already defined - ") += name), "\n",
					NULL);
				skipNextSection("[", "]");
				skipNextSection("{", "}");
			}
			else
			{
				if (isCorrect(name) == false)
				{
					scene->errorf((const char *) (String("Line ") +=
						longToString(line)), (const char *)
						(String(": Invalid PROTO name - ") += name), "\n",
						NULL);
					delete [] temp;
					delete [] name;
					delete flag;
					if (primaryNode != NULL) primaryNode->unref();
					delete nodes;
					return -1;
				}
				Proto *newProto = new Proto(scene, name);
				*flag = parseProto(newProto);
				if (*flag != 1)
				{
					delete [] temp;
					delete [] name;
					int f = *flag;
					delete flag;
					if (primaryNode != NULL) primaryNode->unref();
					delete nodes;
					delete newProto;
					return f;
				}
				scene->addProto(name, newProto);
			}
		}
		// parses a Route
		else if (strcmp(temp, "ROUTE") == 0)
		{
			*flag = parseRoute();
			if (*flag != 1)
			{
				delete [] temp;
				delete [] name;
				int f = *flag;
				delete flag;
				if (primaryNode != NULL) primaryNode->unref();
				delete nodes;
				return f;
			}
		}
		else if (standardNodes.find(String(temp)) != -1)
		{
			scene->errorf((const char *) (String("Line ") +=
				longToString(line)), ": Standard node type that is ",
				(const char *) (String("not implemented - ") += temp),
				"\n", NULL);
			skipNextSection("{", "}");
		}
		else
		{
			scene->errorf((const char *) (String("Line ") +=
				longToString(line)), (const char *)
				(String(": Unknown node type - ") += temp), "\n",
				NULL);
			delete [] temp;
			delete [] name;
			delete flag;
			if (primaryNode != NULL) primaryNode->unref();
			delete nodes;
			return -1;
		}

		if (getNextWord(temp, NULL) == 0)
		{
			delete [] temp;
			delete [] name;
			delete flag;
			if (primaryNode != NULL) primaryNode->unref();
			delete nodes;
			return 0;
		}
	}

	if (primaryNode == NULL)
	{
		scene->errorf((const char *) (String("Line ") += longToString(line)),
			"Invalid Proto definition - there must be at least one node ",
			"statement.\n", NULL);
		delete [] temp;
		delete [] name;
		delete flag;
		delete nodes;
		return -1;
	}
	else
	{
		proto->define(primaryNode, nodes);
	}

	delete [] temp;
	delete [] name;
	delete flag;
	return 1;
}

int Parser::parseRoute()
{
	char *temp = new char[100];
	char *src = new char[100];
	char *dest = new char[100];
	char *eventOut = new char[100];
	char *eventIn = new char[100];
	char *dot = new char;
	*dot = '.';

	if (getNextWord(src, dot) == 0)
	{
		delete [] src;
		delete [] dest;
		delete [] eventOut;
		delete [] eventIn;
		delete [] temp;
		delete dot;
		return 0;
	}
	/*
	Node *srcNode = scene->getNodeByName(src);
	if (srcNode == NULL)
	{
		scene->errorf((const char *) (String("Line ") +=
			longToString(line)), ": ", NULL);
		scene->invalidNode(src);
		scene->errorf("(There is no node that has that name)", NULL);
		delete [] src;
		delete [] dest;
		delete [] eventOut;
		delete [] eventIn;
		delete [] temp;
		delete dot;
		return -1;
	}
	*/

	if ((getNextWord(temp, dot) != 1) || (strcmp(temp, ".") != 0))
	{
		scene->errorf((const char *) (String("Line ") += longToString(line)),
			": Invalid ROUTE - dot missing.\n", NULL);
		delete [] src;
		delete [] dest;
		delete [] eventOut;
		delete [] eventIn;
		delete [] temp;
		delete dot;
		return -1;
	}

	if (getNextWord(eventOut, NULL) == 0)
	{
		delete [] src;
		delete [] dest;
		delete [] eventOut;
		delete [] eventIn;
		delete [] temp;
		delete dot;
		return 0;
	}
	/*
	int eventOutIndex = srcNode->lookupEventOut(eventOut);
	if (eventOutIndex == INVALID_INDEX)
	{
		scene->errorf((const char *) (String("Line ") += longToString(line)),
			(const char *) (String(
			": Invalid ROUTE, incorrect EventOut name - ") += eventOut), "\n",
			NULL);
		delete [] src;
		delete [] dest;
		delete [] eventOut;
		delete [] eventIn;
		delete [] temp;
		delete dot;
		return -1;
	}
	*/

	if (getNextWord(temp, NULL) == 0)
	{
		delete [] src;
		delete [] dest;
		delete [] eventOut;
		delete [] eventIn;
		delete [] temp;
		delete dot;
		return 0;
	}
	if (strcmp(temp, "TO") != 0)
	{
		scene->errorf((const char *) (String("Line ") += longToString(line)),
			": Invalid ROUTE - TO missing.\n", NULL);
		delete [] src;
		delete [] dest;
		delete [] eventOut;
		delete [] eventIn;
		delete [] temp;
		delete dot;
		return -1;
	}

	if (getNextWord(dest, dot) == 0)
	{
		delete [] src;
		delete [] dest;
		delete [] eventOut;
		delete [] eventIn;
		delete [] temp;
		delete dot;
		return 0;
	}
	/*
	Node *destNode = scene->getNodeByName(dest);
	if (destNode == NULL)
	{
		scene->errorf((const char *) (String("Line ") +=
			longToString(line)), ": ", NULL);
		scene->invalidNode(dest);
		scene->errorf("(There is no node that has that name)", NULL);
		delete [] src;
		delete [] dest;
		delete [] eventOut;
		delete [] eventIn;
		delete [] temp;
		delete dot;
		return -1;
	}
	*/

	if ((getNextWord(temp, dot) != 1) || (strcmp(temp, ".") != 0))
	{
		scene->errorf((const char *) (String("Line ") += longToString(line)),
			": Invalid ROUTE - dot missing.\n", NULL);
		delete [] src;
		delete [] dest;
		delete [] eventOut;
		delete [] eventIn;
		delete [] temp;
		delete dot;
		return -1;
	}

	if (getNextWord(eventIn, NULL) == 0)
	{
		delete [] src;
		delete [] dest;
		delete [] eventOut;
		delete [] eventIn;
		delete [] temp;
		delete dot;
		return 0;
	}
	/*
	int eventInIndex = destNode->lookupEventIn(eventIn);
	if (eventInIndex == INVALID_INDEX)
	{
		scene->errorf((const char *) (String("Line ") += longToString(line)),
			(const char *) (String(
			": Invalid ROUTE, incorrect EventIn name - ") += eventIn), "\n",
			NULL);
		delete [] src;
		delete [] dest;
		delete [] eventOut;
		delete [] eventIn;
		delete [] temp;
		delete dot;
		return -1;
	}

	if (scene->validRoute(
		srcNode, eventOutIndex, destNode, eventInIndex) == false)
	{
		scene->errorf((const char *) (String("Line ") += longToString(line)),
			": Invalid ROUTE - EventIn and EventOut types don't match.\n",
			NULL);
		delete [] src;
		delete [] dest;
		delete [] eventOut;
		delete [] eventIn;
		delete [] temp;
		delete dot;
		return -1;
	}
	else
	{
		scene->addRoute(
			srcNode, eventOutIndex, destNode, eventInIndex);
	}
	*/

	delete [] src;
	delete [] dest;
	delete [] eventOut;
	delete [] eventIn;
	delete [] temp;
	delete dot;
	return 1;
}

bool Parser::skipNextSection(const char *first, const char *last)
{
	int counter = 0;
	char *word = new char[200];

	while (getNextWord(word, NULL) != 0)
	{
		if (strcmp(word, first) == 0) counter++;
		if (strcmp(word, last) == 0) counter--;
		if (counter < 0)
		{
			delete [] word;
			return false;
		}
		else if (counter == 0)
		{
			delete [] word;
			return true;
		}
	}

	delete [] word;
	return false;
}

bool Parser::isCorrect(char *name)
{
	char firstChar = *name;

	if (((firstChar >= 48) && (firstChar <= 57)) ||
		((firstChar >= 0) && (firstChar <= 32)) ||
		(firstChar == 34) || (firstChar == 35) ||
		(firstChar == 39) || (firstChar == 43) ||
		(firstChar == 44) || (firstChar == 45) ||
		(firstChar == 46) || (firstChar == 91) ||
		(firstChar == 92) || (firstChar == 93) ||
		(firstChar == 123) || (firstChar == 125) ||
		(firstChar == 127)) return false;

	char restChar = *(name + 1);
	int i = 2;

	while (restChar != '\0')
	{
		if (((restChar >= 0) && (restChar <= 32)) ||
		(restChar == 34) || (restChar == 35) ||
		(restChar == 39) || (restChar == 44) ||
		(restChar == 46) || (restChar == 91) ||
		(restChar == 92) || (restChar == 93) ||
		(restChar == 123) || (restChar == 125) ||
		(restChar == 127)) return false;

		restChar = *(name + i);
		i++;
	}

	return true;
}

String Parser::longToString(long number)
{
	String string;
	long value = number;

	if (value == 0) return string += '0';

	if (value < 0)
	{
		string += "-";
		value *= -1;
	}

	long potention = (long) pow(10.0, floor(log10((double) value)));

	while (potention != 0)
	{
		int digit = (int) value / potention;
		string += (char) (digit + '0');
		value %= potention;
		potention /= 10;
	}

	return string;
}

String Parser::generateRandomNumber()
{
	String name;
	int randomNumber;
	char digits[9];

	randomNumber = (int) ((float) rand() / (RAND_MAX + 1) * 
		(999999999 - 100000000 + 1) + 100000000);

	int i;	//CHANGED BY I.K. declared iterator outside of loop
	for (i = 0; i < 9; i++)
	{
		digits[i] = (randomNumber % 10) + '0';
		randomNumber /= 10;
	}
	for (i = 8; i >= 0; i--)
	{
		name += digits[i];
	}

	return name;
}
