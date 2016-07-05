#ifndef _MFNODE_H
#define _MFNODE_H

#ifndef _ASSERT_H
#include <assert.h>
#endif

#ifndef _FIELDVALUE_H
#include "FieldValue.h"
#endif
#ifndef _NODE_H
#include "Node.h"
#endif
#ifndef _NODELIST_H
#include "NodeList.h"
#endif

/**
* MFNode field type.
* \ingroup VRMLParser
*/
class MFNode : public MFieldValue {
public:
			MFNode();
			MFNode(NodeList *value);
			MFNode(const MFNode &other);
    virtual	       ~MFNode();

    virtual int     	getType() const { return MFNODE; }
    virtual const char *getTypeName() const { return "MFNode"; }
    virtual void	write(FILE *f, int indent) const;
    virtual bool	equals(const FieldValue *value) const;
    int			getSize() const { return _value->size(); }
    NodeList	       *getValue() const { return _value; }
    Node	       *getValue(int index) const { assert(index >= 0 && index < _value->size()); return _value->get(index); }

    virtual int		getSFSize() const { return _value->size(); }
    virtual FieldValue *getSFValue(int index) const;
    virtual void	setSFValue(int index, FieldValue *value);

    virtual FieldValue *addNode(Node *node) const;
    virtual FieldValue *removeNode(Node *node) const;
    virtual FieldValue *copy() { return new MFNode(*this); }

private:
    NodeList	       *_value;
};

#endif // _MFNODE_H
