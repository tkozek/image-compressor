/**
 * @file hextree-given.cpp
 * @description partial implementation of HexTree class used for storing image data
 */

#include "hextree.h"

 /**
  * Node constructor.
  * Assigns appropriate values to all attributes.
  */
Node::Node(pair<unsigned int, unsigned int> ul, pair<unsigned int, unsigned int> lr, RGBAPixel a) {
	upLeft = ul;
	lowRight = lr;
	avg = a;

	A = nullptr;
	B = nullptr;
	C = nullptr;
	D = nullptr;
	E = nullptr;
	F = nullptr;
}

/**
 * HexTree destructor.
 * Destroys all of the memory associated with the
 * current HexTree. This function should ensure that
 * memory does not leak on destruction of a HexTree.
 */
HexTree::~HexTree() {
	Clear();
}

/**
 * Copy constructor for a HexTree. 
 * @param other The HexTree  we are copying.
 */
HexTree::HexTree(const HexTree& other) {
	Copy(other);
}

/**
 * Counts the number of nodes in the tree
 */
unsigned int HexTree::CountNodes() const {
	return CountNodes(root);
}

/**
 * Counts the number of leaves in the tree
 */
unsigned int HexTree::CountLeaves() const {
	return CountLeaves(root);
}

/**
 * Private helper function for counting the total number of nodes in the tree. 
 * @param nd the root of the subtree whose nodes we want to count
 */
unsigned int HexTree::CountNodes(Node* nd) const {
	if (nd == nullptr)
		return 0;
	else
		return 1 + CountNodes(nd->A) + CountNodes(nd->B) + CountNodes(nd->C) + CountNodes(nd->D) + CountNodes(nd->E) + CountNodes(nd->F);
}

/**
 * Private helper function for counting the number of leaves in the tree.
 * @param nd the root of the subtree whose leaves we want to count
 */
unsigned int HexTree::CountLeaves(Node* nd) const {
	if (nd == nullptr)
		return 0;
	else {
		if (nd->A == nullptr && nd->B == nullptr && nd->C == nullptr && nd->D == nullptr && nd->E == nullptr && nd->F == nullptr)
			return 1;
		else
			return CountLeaves(nd->A) + CountLeaves(nd->B) + CountLeaves(nd->C) + CountLeaves(nd->D) + CountLeaves(nd->E) + CountLeaves(nd->F);
	}
}