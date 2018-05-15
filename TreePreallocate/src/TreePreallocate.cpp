//============================================================================
// Name        : TreePreallocate.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <cmath>
#include <climits>

#include "TreePreallocate.hpp"



using namespace std;


#define LAYOUT "rtree"

int level_bits = log2(LEVEL_SIZE);
int tree_heigh = KEY_SIZE/level_bits;


Tree::Tree()
{

		//treeRoot = _pm_pool.get_root()->treeRoot;
}


void Tree::allocateLevel(persistent_ptr<Node> current, int depth, int * count) {
	int i;
	depth++;
	//std::cout << "depth=" << depth <<std::endl;
	for (i=0; i<LEVEL_SIZE; i++)
	{
		bool isLast = false;
		if(depth==((tree_heigh)-1))
		{
			//std::cout << "isLast" <<std::endl;
			isLast = true;
		}
		if(depth==tree_heigh)
		{
			//std::cout << "last level " << depth << " count = " << *count << std::endl;
			current->values[i] = *count;	// just for tests
			(*count)++;
		}

		current->children[i] = make_persistent<Node>(isLast,depth);

		if(depth<tree_heigh)
		{
			//std::cout << "allocating level: "<< depth << " iter: " << i << std::endl;
			allocateLevel(current->children[i], depth, count);
		}

	}
}

int * Tree::findValueInNode(persistent_ptr<Node> current, int key) {

	std::cout << "current->depth = "<< current->depth << " tree_heigh="<<tree_heigh << std::endl;

	int key_shift = (2*(tree_heigh - current->depth-1));
	int key_calc = key >> key_shift;
	int mask = ~(UINT_MAX << level_bits);
	key_calc = key_calc & mask;
	std::cout << "key_calc = "<< key_calc << std::endl;

	if(current->depth!=(tree_heigh-1))
	{
		return findValueInNode(current->children[key_calc], key);
	}
	return &current->values[key_calc];
}

int main() {

	const char *path = "/mnt/pmem/test.pm";
			//pmem::obj::pool<struct root> pop;
			//persistent_ptr<Node> root;
	pool<Tree> _pm_pool;
	persistent_ptr<Node> treeRoot;
	if (!boost::filesystem::exists(path)) {
			std::cout << "Creating pool " << std::endl;
			try {
				_pm_pool = pool<Tree>::create(
				path, LAYOUT, 800*PMEMOBJ_MIN_POOL, S_IWUSR | S_IRUSR);
			} catch (pmem::pool_error &pe) {std::cout << "Error on create" << pe.what();}
			try {
				transaction::exec_tx(_pm_pool, [&] {
					//tree = new Tree();
					treeRoot = make_persistent<Node>(false, 0);
					_pm_pool.get_root()->treeRoot = treeRoot;
					int count = 0;
					_pm_pool.get_root()->allocateLevel(treeRoot, treeRoot->depth, &count);
				});
			} catch (std::exception &e) {
				std::cout << "Error " << e.what();
			}

		}
		else
		{
			std::cout << "Opening existing pool " << std::endl;
			_pm_pool = pool<Tree>::open(path, LAYOUT);
		}

	Tree * tree = _pm_pool.get_root().get();

	//tree->treeRoot = tree->_pm_pool.get_root();

	persistent_ptr<int> value;
	value = tree->findValueInNode(tree->treeRoot, 45678);
	std::cout << "value = "<< *value << std::endl;
	*value = 33333;
	value = tree->findValueInNode(tree->treeRoot, 45678);
	std::cout << "value = "<< *value << std::endl;
	//*value = 45678;
	try{
		_pm_pool.close();
	}catch(std::exception &e) {
		std::cout << "Error on close" << e.what();
	}

	return 0;
}
