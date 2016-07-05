#ifndef _STACK_H
#define _STACK_H

#ifndef _ARRAY_H
#include "Array.h"
#endif

namespace VrmlParser
{
template < class T >
class Stack {
public:
    Stack() { _top = 0; }

    void push

    (T t) { _data[_top++] = t; }

    T pop() { return _data[--_top]; }

    T peek() const { return _data[_top - 1]; }

    int empty() const { return _top == 0; }

private:
    Array < T > _data;
    int _top;
};
}
#endif  // _STACK_H
