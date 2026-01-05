/**
 * @file hextree.h
 * @description declaration of HexTree class used for storing image data
 */

#ifndef _HEXTREE_H_
#define _HEXTREE_H_

#include <utility>
#include "util/PNG.h"
#include "util/RGBAPixel.h"

using namespace std;
using namespace util;

/**
 * Like we had for PA1, the Node class *should be* private to the tree
 * class via the principle of encapsulation -- the end user does not
 * need to know our node-based implementation details.
 * Given for PA3, and made as a public class for convenience of testing and debugging.
 */
class Node {
public:
    Node(pair<unsigned int, unsigned int> ul, pair<unsigned int, unsigned int> lr, RGBAPixel a); // Node constructor

    pair<unsigned int, unsigned int> upLeft;   // image coordinates of upper-left corner of node's rectangular region
    pair<unsigned int, unsigned int> lowRight; // image coordinates of lower-right corner of node's rectangular region
    RGBAPixel avg;  // average color of node's rectangular region
    Node* A; // upper-left child
    Node* B; // upper-middle child
    Node* C; // upper-right child
    Node* D; // lower-left child
    Node* E; // lower-middle child
    Node* F; // lower-right child
};

/**
 * HexTree: This is a structure used in decomposing an image
 * into six rectangular regions.
 *
 */

class HexTree {
public:

    /* =============== start of given functions ====================*/

    /**
     * HexTree destructor.
     * Destroys all of the memory associated with the
     * current HexTree. This function ensures that
     * memory does not leak on destruction of a HexTree.
     */
    ~HexTree();

    /**
     * Copy constructor for a HexTree.
     *
     * @param other The HexTree  we are copying.
     */
    HexTree(const HexTree& other);

    /**
     * Counts the number of nodes in the tree
     */
    unsigned int CountNodes() const;

    /**
     * Counts the number of leaves in the tree
     */
    unsigned int CountLeaves() const;

    /* =============== end of given functions ====================*/

    /* =============== public PA3 FUNCTIONS =========================*/

    /**
     * Constructor that builds a HexTree out of the given PNG.
     * Every leaf in the tree corresponds to a pixel in the PNG.
     * Every non-leaf node corresponds to a rectangle of pixels
     * in the original PNG, represented by an (x,y) pair for the
     * upper left corner of the rectangle and an (x,y) pair for
     * lower right corner of the rectangle. In addition, the Node
     * stores a RGBAPixel representing the average colour over the
     * rectangle.
     *
     * The average colour for each node in your implementation MUST
     * be determined in constant time. HINT: this will lead to nodes
     * at shallower levels of the tree accumulating some error in their
     * average colour value, but we will accept this consequence in
     * exchange for faster tree construction.
     * Note that we will be looking for specific colour values in our
     * autograder, so if you instead perform a slow but accurate
     * average colour computation, you will likely fail the test cases!
     *
     * Every node's children correspond to a partition of the
     * node's rectangle into (up to) six smaller rectangles. The node's
     * rectangle is split evenly (or as close to evenly as possible)
     * along both horizontal and vertical axes. If an even split along
     * the vertical axis is not possible, the extra line(s) will be included
     * in either the middle section, or distributed to the left and right sections;
     * If an even split along the horizontal axis is not
     * possible, the extra line will be included in the upper side.
     * If a single-pixel-wide rectangle needs to be split, the left and right children
     * will be null.
     * If a single-pixel-tall rectangle needs to be split,
     * the lower children will be null.
     *
     * In this way, each of the children's rectangles together will have coordinates
     * that when combined, completely cover the original rectangle's image
     * region and do not overlap.
     */
    HexTree(const PNG& imIn);

    /**
     * Overloaded assignment operator for HexTree.
     * Part of the Big Three that we must define because the class
     * allocates dynamic memory. This depends on your implementation
     * of the copy and clear funtions.
     * You may refer to the assignment operator defined and given in PA1.
     * Make sure you understand what its implementation means.
     *
     * @param rhs The right hand side of the assignment statement.
     */
    HexTree& operator=(const HexTree& rhs);

    /**
     * Render returns a PNG image consisting of the pixels
     * stored in the tree. May be used on pruned trees. Draws
     * every leaf node's rectangle onto a PNG canvas using the
     * average colour stored in the node.
     * 
     * @param fulldepth whether to render each path fully to a leaf node,
     *                  or to only render down to maxlevel
     * @param maxlevel the maximum depth to render in the tree (ignored if fulldepth == true)
     *                 Beware that maxlevel might be larger than
     *                 the length of some paths in pruned trees
     */
    PNG Render(bool fulldepth, unsigned int maxlevel) const;

    /**
     *  Prune function trims subtrees as high as possible in the tree.
     *  A subtree is pruned (cleared) if all of the subtree's leaves are within
     *  tolerance of the average colour stored in the root of the subtree.
     *  Pruning criteria is evaluated on the original tree, not
     *  on any pruned subtree. (only expect that trees would be pruned once.)
     *
     *
     * @param tolerance maximum RGBA distance to qualify for pruning
     * @pre this tree has not previously been pruned, nor is copied from a previously pruned tree.
     */
    void Prune(double tolerance);

    /**
     *  FlipHorizontal rearranges the contents of the tree, so that
     *  its rendered image will appear mirrored across a vertical axis.
     *  This may be called on a previously pruned/flipped/rotated tree.
     *
     *  After flipping, the A/B/C/D/E/F pointers must map to what will be
     *  physically rendered in the respective NW/NE/SW/SE regions.
     */
    void FlipHorizontal();

    /* =============== end of public FUNCTIONS =========================*/

private:
    /*
     * Private member variables.
     *
     * You must use these as specified in the spec and may not rename them.
     */
    Node* root; // pointer to the root of the HexTree

    /* =================== private functions ============== */

    /**
     * Destroys all dynamically allocated memory associated with the
     * current HexTree object.
     */
    void Clear();

    /**
     * Copies the parameter other HexTree into the current HexTree.
     * Does not free any memory. Called by copy constructor and operator=
     * @param other The HexTree to be copied.
     */
    void Copy(const HexTree& other);

    /**
     * Private helper function for the constructor. Recursively builds
     * the tree according to the specification of the constructor.
     * @param img reference to the original input image.
     * @param ul upper left point of current node's rectangle.
     * @param lr lower right point of current node's rectangle.
     */
    Node* BuildNode(const PNG& img, pair<unsigned int, unsigned int> ul, pair<unsigned int, unsigned int> lr);

    /**
     * Private helper function for counting the total number of nodes in the tree. 
     * @param nd the root of the subtree whose nodes we want to count
     */
    unsigned int CountNodes(Node* nd) const;

    /**
     * Private helper function for counting the number of leaves in the tree.
     * @param nd the root of the subtree whose leaves we want to count
     */
    unsigned int CountLeaves(Node* nd) const;

    PNG grayscale(PNG image);

    
    /**
     * If you require more private member attributes and/or private functions,
     * declare them in hextree-private.h
     */
    #include "hextree-private.h"
};

#endif
