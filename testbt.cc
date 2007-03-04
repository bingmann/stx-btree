// $Id$

#include <string>
#include <stdlib.h>
#include <sys/time.h>

#include <map>
#include <set>
#include <iostream>
#include <fstream>

#include "btree.h"

#include <assert.h>

double timestamp()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec * 0.000001;
}

struct testx
{
    unsigned int a, b;

    testx()
    {
    }

    testx(unsigned int _a)
	: a(_a), b(0)
    {
    }

    inline bool operator==(const struct testx &o) const
    {
	return a == o.a;
    }
};

inline std::ostream& operator<< (std::ostream &o, const struct testx &t)
{
    return o << t.a;
}

struct testcomp
{
    bool operator()(const struct testx &a, const struct testx &b) const
    {
	return a.a < b.a;
    }
};

typedef btree<unsigned int, unsigned int> btree_type;

template class btree<unsigned int, unsigned int>;

//template class btree<struct testx, int>;
template class std::map<struct testx, int, testcomp>;

int main()
{
    srand(34234235);

    double ts1 = timestamp();

#if 1

    btree_type bt;

    std::ifstream dumpof("dump.bt");

    assert( bt.restore(dumpof) );

        for(btree_type::iterator bi = bt.begin(); bi != bt.end(); ++bi)
    {
	std::cout << bi.key() << " ";
    }
    std::cout << "\n";

#elif 1

    btree_type bt;

    int innum = 320000;
    int ratio = 100000;

    typedef std::multiset<testx, testcomp> multiset_type;
    multiset_type set;

    srand(34234235);
    for(int i = 0; i < innum; i++)
    {
	unsigned int k = rand() % ratio;
	unsigned int v = 234;

	//assert(bt.size() == set.size());
	assert( bt.insert2(k, v).second );
	//set.insert(k);
	//assert( bt.count(k) == set.count(k) );

	//assert(bt.size() == set.size());
    }

    for(btree_type::iterator bi = bt.begin(); bi != bt.end(); ++bi)
    {
	std::cout << bi.key() << " ";
    }
    std::cout << "\n";

    srand(34234235);
    for(int i = 0; i < innum; i++)
    {
	unsigned int k = rand() % ratio;

	assert( bt.exists(k) );
    }

    std::ofstream dumpof("dump.bt");

    bt.dump(dumpof);

    if (0)
    {
	srand(34234235);
	for(int i = 0; i < innum; i++)
	{
	    unsigned int k = rand() % ratio;

	    if (set.find(k) != set.end())
	    {
		assert( bt.size() == set.size() );

		assert( bt.exists(k) );
		assert( bt.erase(k) );
		set.erase( set.find(k) );

		assert( bt.size() == set.size() );
	    }
	}

	assert(bt.empty());
    }

#elif 1

    btree<unsigned int, unsigned int> bt;

    std::cout << "insert\n";
    srand(34234235);
    for(int i = 0; i < 4000000; i++)
	bt.insert(rand(), 0);

    // ts1 = timestamp();
    assert( bt.size() == 4000000 );

    std::cout << "check\n";
    srand(34234235);
    for(int i = 0; i < 4000000; i++)
	bt.exists(rand());

    std::cout << "erase\n";
    srand(34234235);
    for(int i = 0; i < 4000000; i++)
	bt.erase(rand());

    assert(bt.empty());

#else

    typedef std::multimap<unsigned int, unsigned int> multimap_type;
    multimap_type map;

    std::cout << "insert\n";
    srand(34234235);
    for(int i = 0; i < 4000000; i++)
	map.insert( multimap_type::value_type(rand(), 0) );

    // ts1 = timestamp();
    assert( map.size() == 4000000 );

    std::cout << "check\n";
    srand(34234235);
    for(int i = 0; i < 4000000; i++)
	map.find(rand());

    std::cout << "erase\n";
    srand(34234235);
    for(int i = 0; i < 4000000; i++)
	map.erase( map.find(rand()) );

    assert( map.empty() );

/*
    srand(34234235);
    for(int i = 0; i < 1000000; i++)
	map[rand()] = rand();
*/
#endif

    double ts2 = timestamp();

    std::cerr << "ts: " << (ts2-ts1);
}
