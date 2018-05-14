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

using namespace std;

//Number of keys slots inside Node (not key bits)
#define LEVEL_SIZE 4
// Number of key bits
#define KEY_SIZE 24

int level_bits = log2(LEVEL_SIZE);
int tree_heigh = KEY_SIZE/level_bits;

struct Node {
	struct Node * children[LEVEL_SIZE];
	bool isEnd;
	int depth;
	int* values;		//only for leaves
	Node (bool _isEnd, int _depth)
	{
		isEnd = _isEnd;
		depth = _depth;
		if(isEnd)
		{
			values = new int[LEVEL_SIZE];
		}
	}
};


void allocateLevel(Node * current, int depth, int * count) {
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

		current->children[i] = new Node(isLast,depth);

		if(depth<tree_heigh)
		{
			//std::cout << "allocating level: "<< depth << " iter: " << i << std::endl;
			allocateLevel(current->children[i], depth, count);
		}

	}
}

int * findValueInNode(Node * current, int key) {

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
	struct Node *root = new Node(false, 0);
	int count = 0;
	allocateLevel(root,root->depth, &count);

	int* value;
	value = findValueInNode(root, 45678);
	std::cout << "value = "<< *value << std::endl;
	*value = 33333;
	value = findValueInNode(root, 45678);
	std::cout << "value = "<< *value << std::endl;

	return 0;
}
