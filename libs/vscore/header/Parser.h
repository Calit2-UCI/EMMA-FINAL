#ifndef _PARSER_H
#define _PARSER_H

#ifndef _STDIO_H
#include <stdio.h>
#endif

#ifndef _ARRAY_H
#include "Array.h"
#endif

class Scene;
class Node;
class Proto;
class Field;
class SFColor;
class SFNode;
class SFVec3f;
class SFInt32;
class SFFloat;
class SFRotation;
class SFString;
class SFTime;
class SFVec2f;
class FieldValue;
class String;

/**
* VRML parser.
* VRML parser.
* \defgroup VRMLParser
*/

/**
* VRML parser.
* Reads the VRML file and stores the parsed data (nodes, routes and Protos) into Scene object.
* \ingroup VRMLParser
*/
class Parser
{
public:
	Parser(Scene *scene, FILE *f);

	~Parser();

	/**
	* Parses the VRML file. File pointer was given as a parameter to
	* the Parser constructor. Routes, nodes and Protos are being created
	* and placed inside the scene (the parameter of the Parser constructor).
	*
	* @return 1 if some error has occured or 0 if the end of file was
	* reached
	*/
	int parseVRMLFile();

	/**
	* Gets the next word in the file content. Special case is when
	* the next word is a character '[', or ']', or '{', or '}',
	* or is equal to the parameter separator - it represents a separator
	* needed for additional checking.
	*
	* @param dest a string which will contain the next word in the file
	* content after executing this method
	* @param separator a character which represents a separator needed
	* for additional checking
	* @return the length of the next word (parameter dest) or 0 if there
	* is no more words (end of file content)
	*/
	int getNextWord(char *dest, char *separator);

	/**
	* Parses the body of the VRML node - content between '{' and '}',
	* including '{' and '}'.
	*
	* @param node a pointer to the node that will be filled with values
	* read from the file
	* @return 0 in case it comes across the end of the file content,
	* 1 if everything is OK, and -1 if there is some syntax error
	*/
	int parseNode(Node *node);

	/**
	* Parses the Proto declaration (content between '[' and ']',
	* including '[' and ']') and definition (content between '{' and '}',
	* including '{' and '}').
	*
	* @param proto a pointer to the Proto that will be filled with values
	* read from the file
	* @return 0 in case it comes across the end of the file content,
	* 1 if everything is OK, and -1 if there is some syntax error
	*/
	int parseProto(Proto *proto);

	/**
	* Parses the VRML route (characters after the word ROUTE).
	*
	* @return 0 in case it comes across the end of the file content,
	* 1 if everything is OK, and -1 if there is some error
	*/
	int parseRoute();

	/**
	* Parses the value of the field.
	*
	* @param fieldType a type of the field, i.e. SFBOOL
	* @param fieldName a name of the field, i.e. translation
	* @param minValue a minimum value that field can have
	* @param maxValue a maximum value that field can have
	* @param nodeType a type of the node that SFNode or MFNode field
	* contains (fields of other types have this value set to NODE_ANY)
	* @param temp a string that represents the first word of the file
	* content after the field name
	* @param flag a flag that is equal 0 in case it comes across the end
	* of the file content, 1 if everything is OK, and -1 if there is some
	* error
	* @return the parsed value of the field
	*/
	FieldValue * parseFieldValue(int fieldType, const char *fieldName,
		float minValue, float maxValue, int nodeType, char *temp, int *flag);
    
    

private:
	/**
	* Checks if the parameter letter is equal to the character 
	* '[', or ']', or '{', or '}', and if this is true it creates
	* a corresponding string (string that contains this character)
	* - parameter dest.
	*
	* @param letter a character that needs to be checked
	* @param dest a string which will contain the parameter letter
	* (that is equal to '[', or ']', or '{', or '}') after
	* executing this method
	* @return true if the parameter letter is equal to the character 
	* '[', or ']', or '{', or '}', else it returns false
	*/
	bool getSeparator(char letter, char *dest);

	/**
	* Jumps the next section in the file content, i.e. all the characters
	* between the parameter first and the parameter last. It is used for
	* parsing (jumping over) the unknown or incorrect node types or fields.
	*
	* @param first a string which contains the first word (separator) of
	* the section
	* @param last a string which contains the last word (separator) of
	* the section
	* @return false if there is no more words (end of file content) or the
	* first section character is missing, else it return true
	*/
	bool skipNextSection(const char *first, const char *last);

	/**
	* Checks the name of the node, or (not standard) Proto, or field, or
	* eventIn, or eventOut. If it contains any of the prohibited characters
	* it is incorrect.
	*
	* @param name a string that needs to be checked
	* @return false if the parameter name is incorrect, else it returns true
	*/
	bool isCorrect(char *name);

	/**
	* Parses the SFColor value.
	*
	* @param temp a string that represents the first word of the file
	* content after the (SFColor) field name
	* @param minValue a minimum value that SFColor field can have
	* @param maxValue a maximum value that SFColor field can have
	* @param flag a flag that is 0 in case it comes across the end of the file
	* content, 1 if everything is OK, and -1 if there is some error
	* @return the parsed SFColor value or NULL if it comes across the end of
	* the file or if there is some error
	*/
	SFColor * getSFColorValue(char *temp, float minValue, float maxValue,
		int *flag);

	/**
	* Parses the SFInt32 value.
	*
	* @param temp a string that represents the first word of the file
	* content after the (SFInt32) field name
	* @param minValue a minimum value that SFInt32 field can have
	* @param maxValue a maximum value that SFInt32 field can have
	* @param flag a flag that is 1 if everything is OK or -1 if there is some
	* error
	* @return the parsed SFInt32 value or NULL if it comes across the end of
	* the file or if there is some error
	*/
	SFInt32 * getSFInt32Value(char *temp, float minValue, float maxValue,
		int *flag);

	/**
	* Parses the SFFloat value.
	*
	* @param temp a string that represents the first word of the file
	* content after the (SFFloat) field name
	* @param minValue a minimum value that SFFloat field can have
	* @param maxValue a maximum value that SFFloat field can have
	* @param flag a flag that is 1 if everything is OK or -1 if there is some
	* error
	* @return the parsed SFFloat value or NULL if it comes across the end of
	* the file or if there is some error
	*/
	SFFloat * getSFFloatValue(char *temp, float minValue, float maxValue,
		int *flag);

	/**
	* Parses the SFNode value.
	*
	* @param temp a string that represents the first word of the file
	* content after the (SFNode) field name
	* @param nodeType a type of the node that SFNode field contains, it is used
	* to validate the scene hierarchy
	* @param flag a flag that is 0 in case it comes across the end of the file
	* content, 1 if everything is OK, and -1 if there is some error
	* @return the parsed Node value or NULL if it comes across the end of
	* the file or if there is some error
	*/
	Node * getFieldNode(char *temp, int nodeType, int *flag);

	/**
	* Parses the SFNode value.
	*
	* @param temp a string that represents the first word of the file
	* content after the (SFNode) field name
	* @param nodeType a type of the node that SFNode field contains, it is used
	* to validate the scene hierarchy
	* @param flag a flag that is 0 in case it comes across the end of the file
	* content, 1 if everything is OK, and -1 if there is some error
	* @return the parsed SFNode value or NULL if it comes across the end of
	* the file or if there is some error
	*/
	SFNode * getSFNodeValue(char *temp, int nodeType, int *flag);

	/**
	* Parses the SFRotation value.
	*
	* @param temp a string that represents the first word of the file
	* content after the (SFRotation) field name
	* @param flag a flag that is 0 in case it comes across the end of the file
	* content, 1 if everything is OK, and -1 if there is some error
	* @return the parsed SFRotation value or NULL if it comes across the end of
	* the file or if there is some error
	*/
	SFRotation * getSFRotationValue(char *temp, int *flag);

	/**
	* Parses the SFString value.
	*
	* @param temp a string that represents the first word of the file
	* content after the (SFString) field name
	* @param flag a flag that is 0 in case it comes across the end of the file
	* content, 1 if everything is OK, and -1 if there is some error
	* @return the parsed SFString value or NULL if it comes across the end of
	* the file or if there is some error
	*/
	SFString * getSFStringValue(char *temp, int *flag);

	/**
	* Parses the SFTime value.
	*
	* @param temp a string that represents the first word of the file
	* content after the (SFTime) field name
	* @param flag a flag that is 1 if everything is OK or -1 if there is some
	* error
	* @return the parsed SFTime value or NULL if it comes across the end of
	* the file or if there is some error
	*/
	SFTime * getSFTimeValue(char *temp, int *flag);

	/**
	* Parses the SFVec2f value.
	*
	* @param temp a string that represents the first word of the file
	* content after the (SFVec2f) field name
	* @param flag a flag that is 0 in case it comes across the end of the file
	* content, 1 if everything is OK, and -1 if there is some error
	* @return the parsed SFVec2f value or NULL if it comes across the end of
	* the file or if there is some error
	*/
	SFVec2f * getSFVec2fValue(char *temp, int *flag);

	/**
	* Parses the SFVec3f value.
	*
	* @param temp a string that represents the first word of the file
	* content after the (SFVec3f) field name
	* @param fieldName a name of the field, i.e. translation
	* @param minValue a minimum value that field can have
	* @param maxValue a maximum value that field can have
	* @param flag a flag that is 0 in case it comes across the end of the file
	* content, 1 if everything is OK, and -1 if there is some error
	* @return the parsed SFVec3f value or NULL if it comes across the end of
	* the file or if there is some error
	*/
	SFVec3f * getSFVec3fValue(char *temp, const char *fieldName, float minValue,
		float maxValue, int *flag);

	/**
	* Converts a string (parameter word) into a corresponding float value. If
	* the word contains any character that isn't a digit (or more than one dot
	* or minus) method will return 0.0 and flag will be set to false.
	*
	* @param word a string that will be converted to a float value
	* @param flag an indicator that is true if everything is OK or false if
	* a string contains characters that a float number shouldn't contain
	* @return a float value calculated from the parameter word
	*/
	float getFloatValue(char *word, bool *flag);

	/**
	* Converts a string (parameter word) into a corresponding double value. If
	* the word contains any character that isn't a digit (or more than one dot
	* or minus) method will return 0.0 and flag will be set to false.
	*
	* @param word a string that will be converted to a double value
	* @param flag an indicator that is true if everything is OK or false if
	* a string contains characters that a double number shouldn't contain
	* @return a double value calculated from the parameter word
	*/
	double getDoubleValue(char *word, bool *flag);

	/**
	* Converts a string (parameter word) into a corresponding integer value.
	* If the word contains any character that isn't a digit (or more than one
	* minus) method will return 0 and flag will be set to false.
	*
	* @param word a string that will be converted to an integer value
	* @param flag an indicator that is true if everything is OK or false if
	* a string contains characters that an integer number shouldn't contain
	* @return a integer value calculated from the parameter word
	*/
	int getIntValue(char *word, bool *flag);

	/**
	* Converts a string (parameter word) into a corresponding unsigned integer
	* value. Digits can be decimal or hexadecimal. If the word contains any
	* character that isn't a digit (decimal or hexadecimal) method will
	* return 0 and flag will be set to false. The same thing will happen
	* if the word contains more then number of components given by the
	* parameter depth.
	*
	* @param word a string that will be converted to an integer value
	* @param depth a number of components that the pixel value given by
	* word can contain
	* @param flag an indicator that is true if everything is OK or false if
	* a string contains characters that an integer number shouldn't contain
	* @return a integer value calculated from the parameter word
	*/
	int getPixelValue(char *word, int depth, bool *flag);

	/**
	* Converts a string (parameter word) into a corresponding integer value.
	* Digits can be decimal or hexadecimal. If the word contains any
	* character that isn't a digit (decimal or hexadecimal) method will
	* return 0 and flag will be set to false.
	*
	* @param word a string that will be converted to an integer value
	* @param flag an indicator that is true if everything is OK or false if
	* a string contains characters that an integer number shouldn't contain
	* @return a integer value calculated from the parameter word
	*/
	int getInt32Value(char *word, bool *flag);

	/**
	* Converts a long number into a corresponding string. It is used
	* for displaying number of file line in which error has occured while
	* parsing it.
	*
	* @param number a number that is being converted into string
	* @return a string that corresponds to the parameter number
	*/
	String longToString(long number);

	/**
	* Generates random number and converts it to a string.
	*
	* @return a string that corresponds to the parameter number
	*/
	String generateRandomNumber();

	/**
	* An Attribute that points to a scene which 
	* will be built from the content of the VRML file.
	*/
	Scene *scene;
	/**
	* An attribute that contains a content of the VRML file 
	* (all characters that are stored there).
	*/
	char *content;

	/**
	* An attribute which represents the current position in the
	* (VRML file) content.
	*/
	int position;

	/**
	* An attribute which represents the current line in the
	* (VRML file) content.
	*/
	long line;

	/**
	* An attribute that contains names of the standard VRML nodes.
	* It is used to check whether the node in the VRML file is standard
	* or (if it's not a PROTO node) is unknown.
	*/
	Array<String> standardNodes;
    

};

#endif // _PARSER_H
