#ifndef _LIST_H
#define _LIST_H

#ifndef NULL
#define NULL 0L
#endif

template<class T>
class List {
public:
    class Iterator
    {
		friend class List<T>;
		public:
				Iterator(const T &t) : _value(t)
					{ _prev = _next = NULL; }

		Iterator       *prev()		{ return _prev; }
		const Iterator *prev() const	{ return _prev; }
		Iterator       *next()		{ return _next; }
		const Iterator *next() const	{ return _next; }

		const T	       &item() const	{ return _value; }
		T	        item()		{ return _value; }

		private:
		Iterator	       *_prev;
		Iterator	       *_next;
		T			_value;
    };



    List()
    {
    	_head = _tail = NULL; _length = 0;
    }


    List(const List<T> &l)
    {
    	_head = _tail = NULL; _length = 0;
		for (Iterator *i = l.first(); i != NULL; i = i->next())
			append(i->item());
	}


    ~List()
    { 
    	removeAll();
    }
    

    void append(const T &t)
    {
		Iterator   *i = new Iterator(t);
		i->_prev = _tail;
		if (_tail) {
			_tail->_next = i;
		} else {
			_head = i;
		}
		_tail = i;
		_length++;
    }

    void insert(const T &t)
    {
		Iterator   *i = new Iterator(t);
		i->_next = _head;
		if (_head) {
			_head->_prev = i;
		} else {
			_tail = i;
		}
		_head = i;
		_length++;
    }

    void remove(Iterator *i)
    {
		if (i->_prev) {
			i->_prev->_next = i->_next;
		} else {
			_head = i->_next;
		}
		if (i->_next) {
			i->_next->_prev = i->_prev;
		} else {
			_tail = i->_prev;
		}
		delete i;
		_length--;
    }

    Iterator *find(const T &t) const
    {
		for (Iterator *i = _head; i != NULL; i = i->next()) {
			if (i->item() == t) return i;
		}
		return NULL;
    }

    Iterator *get(int index) const
    {
		Iterator *i;
		for (i = _head; i != NULL && index > 0; i = i->next())
			index--;
		return i;
    }

    void removeAll()
    {
		Iterator		*i, *j;

		for (i = _head; i != NULL; i = j) {
		   j = i->next();
		   delete i;
		}
    }

    void appendList(List<T> *list)
    {
		for (Iterator *i = list->first(); i != NULL;
			 i = i->next()) {
			append(i->item());
		}
    }

    void insertList(List<T> *list)
    {
		for (Iterator *i = list->last(); i != NULL;
			 i = i->prev()) {
			insert(i->item());
		}
    }

    Iterator *first() const { return _head; }
    Iterator *last() const { return _tail; }
    int size() const { return _length; }

    int operator==(const List<T> &list)
    {
		Iterator	*i, *j;

		for(i = list.first(), j = _head; i != NULL && j != NULL;
			i = i->next(), j = j->next())
		{
			if (i->item() != j->item()) return 0;
		}
		return i != NULL && j != NULL;
    }

private:
    Iterator	       *_head;
    Iterator	       *_tail;
    int			_length;
};
#endif // _LIST_H
