/*
 * TreePreallocate.hpp
 *
 *  Created on: May 14, 2018
 *      Author: jschmieg
 */

#ifndef TREEPREALLOCATE_HPP_
#define TREEPREALLOCATE_HPP_

#include <libpmemobj++/make_persistent.hpp>
#include <libpmemobj++/make_persistent_array.hpp>
#include <libpmemobj++/p.hpp>
#include <libpmemobj++/persistent_ptr.hpp>
#include <libpmemobj++/pool.hpp>
#include <libpmemobj++/transaction.hpp>

#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>

using namespace pmem::obj;

//Number of keys slots inside Node (not key bits)
#define LEVEL_SIZE 4
// Number of key bits
#define KEY_SIZE 20

struct Node {

	explicit Node (bool _isEnd, int _depth) : isEnd(_isEnd), depth(_depth)
	{
		//children = make_persistent<Node[LEVEL_SIZE]>();
		if(isEnd)
		{
			values = make_persistent<int[LEVEL_SIZE]>();
		}
	}
	persistent_ptr<Node> children[LEVEL_SIZE];
	bool isEnd;
	int depth;
	persistent_ptr<int[LEVEL_SIZE]> values;		//only for leaves
};

class Tree {
public:
	Tree();
	int * findValueInNode(persistent_ptr<Node> current, int key);
	void allocateLevel(persistent_ptr<Node> current, int depth, int * count);
	persistent_ptr<Node> treeRoot;
/*struct root{
	persistent_ptr<Node> treeRoot;
};
*/
private:

	pool<Tree> _pm_pool;
};
#endif /* TREEPREALLOCATE_HPP_ */
