#ifndef _NODE_LIST_H
#define _NODE_LIST_H

class Node;
#ifndef _ARRAY_H
#include "Array.h"
#endif

/**An Array of Nodes. Responsible of handling VRML97 MFNodes.
* \ingroup VRMLParser
*/
class NodeList : public Array<Node *> {
public:
	/**Appends hte specified node to the Array<Node*> .*/
    NodeList(Node *node) : Array<Node *>() { append(node); }
    NodeList() : Array<Node *>() {}

	/**Clears the flag of all the Nodes in the NodeList.*/
    void	    clearFlag(int flag) const;

	/**Sets the flag of all the Nodes in the NodeList to the specified flag.*/
    void	    setFlag(int flag) const;
};

#endif // _NODE_LIST_H
