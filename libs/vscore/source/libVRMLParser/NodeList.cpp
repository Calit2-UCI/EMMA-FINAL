#include "stdafx.h"
#include "NodeList.h"
#include "Node.h"

void NodeList::clearFlag(int flag) const
{
	int i;

	for (i = 0; i < size(); i++)
	{
		Node *node = get(i);
		node->clearFlag(flag);
	}
}

void NodeList::setFlag(int flag) const
{
	int i;

	for (i = 0; i < size(); i++)
	{
		Node *node = get(i);
		node->setFlag(flag);
	}
}
