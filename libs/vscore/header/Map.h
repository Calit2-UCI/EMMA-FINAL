#ifndef _MAP_H
#define _MAP_H

#ifndef _LIST_H
#include "List.h"
#endif

#define MAP_DEFAULT_MAX_LOAD	2
#define MAP_DEFAULT_WIDTH	16

extern int hash(int key);

template<class K, class D>
class Map {
public:
    class Pair {
		public:
				Pair	(K key, D data) { _key = key; _data = data; }

		K	   &getKey() { return _key; }
		D	   &getData() { return _data; }

		const K	   &getKey() const { return _key; }
		const D	   &getData() const { return _data; }

		private:
		K		_key;
		D		_data;
    };

    typedef List<Pair *> Chain;

//public:

    Map(int maxLoad = MAP_DEFAULT_MAX_LOAD)
    {
    	_maxLoad = maxLoad;
    	_chains = NULL;
    	_width = _size = 0;
    	resize(MAP_DEFAULT_WIDTH);
    }

    ~Map()
    {
    	typename Chain::Iterator  *j;


		for (int i = 0; i < _width; i++)
		{
			for (j = _chains[i].first(); j != NULL; j = j->next())
			{
				Pair *p = j->item();
				delete p;
			}
		}
	delete[] _chains;
    }

//public:
    D		       &operator[](K key) {
			    Pair *p;
			    typename Chain::Iterator *i;
			    Chain &chain = _chains[hash(key) % _width];
			    for (i = chain.first(); i != NULL; i = i->next()) {
				p = i->item();
				if (p->getKey() == key) {
				    return p->getData();
				}
			    }
			    p = new Pair(key, 0);
			    chain.append(p);
			    _size++;
			    if ( _size > _width * _maxLoad )
				resize( _width * 2 );
			    return p->getData();
			}
    int			width() { return _width; }
    const Chain        &chain(int num) { return _chains[num]; }
    const Chain        &chain(int num) const { return _chains[num]; }

private:
    void		resize(int width);

private:
    int			_size;
    int			_width;
    int			_maxLoad;
    Chain	       *_chains;
};

template<class K, class D>
inline void		Map<K, D>::resize(int width) {
			    Chain *newChains = new Chain[width];
			    typename Chain::Iterator *j;
			    for (int i = 0; i < _width; i++) {
				for (j = _chains[i].first(); j != NULL;
				     j = j->next()) {
				    Pair *p = j->item();
				    newChains[hash(p->getKey()) % width]
					      .append(p);
				}
			    }
			    delete[] _chains;
			    _chains = newChains;
			    _width = width;
			}


#endif // _MAP_H
