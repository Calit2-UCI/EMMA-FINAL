#ifndef _SFNODE_H
#define _SFNODE_H

#ifndef _FIELDVALUE_H
#include "FieldValue.h"
#endif

class Element;
class Node;

/**
* SFNode field type.
* \ingroup VRMLParser
*/
class SFNode : public FieldValue {
public:
			SFNode(Node *value);
			SFNode(const SFNode &other);
    virtual	       ~SFNode();

    virtual int         getType() const { return SFNODE; }
    virtual const char *getTypeName() const { return "SFNode"; }
    virtual void	write(FILE *f, int indent) const;
    virtual bool	equals(const FieldValue *value) const;
    virtual FieldValue *copy() { return new SFNode(*this); }

    Node	       *getValue() const { return _value; }
    virtual FieldValue *addNode(Node *node) const;
    virtual FieldValue *removeNode(Node *node) const;

private:
    Node	       *_value;
};

#endif // _SFNODE_H
