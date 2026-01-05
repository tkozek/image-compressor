/**
 * @file hextree.cpp
 * @description Implementation of HexTree class used for storing image data
 */

#include "hextree.h"

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
 * The average colour for each node is determined in constant time. 
 * This leads to nodes at shallower levels of the tree accumulating 
 * some error in their average colour value, but this consequence 
 * is accepted in exchange for faster tree construction.
 *
 * Every node's children correspond to a partition of the
 * node's rectangle into (up to) six smaller rectangles. The node's
 * rectangle is split evenly (or as close to evenly as possible)
 * along both horizontal and vertical axes. If an even split along
 * the vertical axis is not possible, the extra line(s) will be included
 * in either the middle section, or distributed to the left and right sections;
 * If an even split along the horizontal axis is not possible, the extra line 
 * will be included in the upper side. If a single-pixel-wide rectangle needs 
 * to be split, the left and right children will be null.
 * If a single-pixel-tall rectangle needs to be split,
 * the lower children will be null.
 *
 * In this way, each of the children's rectangles together will have coordinates
 * that when combined, completely cover the original rectangle's image
 * region and do not overlap.
 */

HexTree::HexTree(const PNG& imIn) {
    unsigned int width = imIn.width();
    unsigned int height = imIn.height();
	root = BuildNode(imIn, {0, 0}, {width - 1, height - 1});
}

/**
 * Overloaded assignment operator for HexTree.
 *
 * @param rhs The right hand side of the assignment statement.
 */
HexTree& HexTree::operator=(const HexTree& rhs) {
    if (this != &rhs) {
        Clear(); 
        Copy(rhs);
    }
	return *this;
}

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
PNG HexTree::Render(bool fulldepth, unsigned int maxlevel) const {
    if (!root) return PNG();
    pair<unsigned int, unsigned int> rootUL = root->upLeft;
    pair<unsigned int, unsigned int> rootLR = root->lowRight;

    unsigned int width = rootLR.first - rootUL.first + 1;
    unsigned int height = rootLR.second - rootUL.second + 1;
    PNG res = PNG(width, height);
    // if fulldepth, go to leaf node and render ever leaf node's average color over its span
    // otherwise, go to at most maxlevel, but if we hit a leaf first, still render its avg color over its span
    // if we hit maxlevel, just render that nodes avg over its span
    renderNodeAndSubnodes(res, root->A, fulldepth, maxlevel - 1);
    renderNodeAndSubnodes(res, root->B, fulldepth, maxlevel - 1);
    renderNodeAndSubnodes(res, root->C, fulldepth, maxlevel - 1);
    renderNodeAndSubnodes(res, root->D, fulldepth, maxlevel - 1);
    renderNodeAndSubnodes(res, root->E, fulldepth, maxlevel - 1);
    renderNodeAndSubnodes(res, root->F, fulldepth, maxlevel - 1);    
	return res;
}

/**
 * @param tolerance maximum RGBA distance to qualify for pruning
 * @pre this tree has not previously been pruned, nor is copied from a previously pruned tree.
 */
void HexTree::Prune(double tolerance) {
    if (!root) return;
    Prune(root, tolerance);
}


 
void HexTree::Prune(Node* node, double tolerance) {
    if (!node) return;

    if (ShouldPrune(node, node->avg, tolerance)) {
        ClearSubnodes(node); 
    } else {
        
        Prune(node->A, tolerance);
        Prune(node->B, tolerance);
        Prune(node->C, tolerance);
        Prune(node->D, tolerance);
        Prune(node->E, tolerance);
        Prune(node->F, tolerance);
    }
}

void HexTree::FlipHorizontal() {
    FlipNodeHorizontal(root);
}

void HexTree::Clear() {
    ClearNode(root);
    root = nullptr;
}

/**
 * @param other The HexTree to be copied.
 */
void HexTree::Copy(const HexTree& other) {
    root = CopyNode(other.root);
}

/**
 * @param img reference to the original input image.
 * @param ul upper left point of current node's rectangle.
 * @param lr lower right point of current node's rectangle.
 */
Node* HexTree::BuildNode(const PNG& img, pair<unsigned int, unsigned int> ul, pair<unsigned int, unsigned int> lr) {
    unsigned int farLeft = ul.first;
    unsigned int farRight = lr.first;
    unsigned int top = ul.second;
    unsigned int bottom = lr.second;

    unsigned int width = farRight - farLeft + 1;
    unsigned int height = bottom - top + 1;
// leaf
    if (width == 1 && height == 1) {
        return new Node(ul, lr, *(img.getPixel(farLeft, top)));
    }

    unsigned int aWidth, bWidth, cWidth;
    aWidth = bWidth = cWidth = width / 3;
    if (width % 3 == 1) {
        bWidth += 1;
    } else if (width % 3 == 2) {
        aWidth += 1;
        cWidth += 1;
    }

    unsigned int aStart = farLeft;
    unsigned int aEnd = aStart + aWidth - 1;
    unsigned int bStart = aEnd + 1;
    unsigned int bEnd = bStart + bWidth - 1;
    unsigned int cStart = bEnd + 1;
    unsigned int cEnd = cStart + cWidth - 1;

    unsigned int midY = top + (height - 1) / 2;
    unsigned int botY = midY + 1;

    Node * A, * B, *C, *D, *E, *F;
    A = B = C = D = E = F = nullptr;


    if (height == 1) {
        if (aWidth > 0) A = BuildNode(img, {aStart, top}, {aEnd, top});
        if (bWidth > 0) B = BuildNode(img, {bStart, top}, {bEnd, top});
        if (cWidth > 0) C = BuildNode(img, {cStart, top}, {cEnd, top});
    } else if (width == 1) {
        B = BuildNode(img, {farLeft, top}, {farLeft, midY});
        E = BuildNode(img, {farLeft, botY}, {farLeft, bottom});
    } else {
        if (aWidth > 0) {
            A = BuildNode(img, {aStart, top}, {aEnd, midY});
            D = BuildNode(img, {aStart, botY}, {aEnd, bottom});
        }
        if (bWidth > 0) {
            B = BuildNode(img, {bStart, top}, {bEnd, midY});
            E = BuildNode(img, {bStart, botY}, {bEnd, bottom});
        }
        if (cWidth > 0) {
            C = BuildNode(img, {cStart, top}, {cEnd, midY});
            F = BuildNode(img, {cStart, botY}, {cEnd, bottom});
        }
    }
    unsigned int totalR = 0;
    unsigned int totalG = 0;
    unsigned int totalB = 0;
    double totalA = 0;
    unsigned int totalPixels = 0;

    contributeToWeightedAvg(A, totalR, totalG, totalB, totalA, totalPixels);
    contributeToWeightedAvg(B, totalR, totalG, totalB, totalA, totalPixels);
    contributeToWeightedAvg(C, totalR, totalG, totalB, totalA, totalPixels);
    contributeToWeightedAvg(D, totalR, totalG, totalB, totalA, totalPixels);
    contributeToWeightedAvg(E, totalR, totalG, totalB, totalA, totalPixels);
    contributeToWeightedAvg(F, totalR, totalG, totalB, totalA, totalPixels);

    unsigned char avgR = totalR / totalPixels;
    unsigned char avgG = totalG / totalPixels;
    unsigned char avgB = totalB / totalPixels;
    double avgA = totalA / totalPixels;

    Node* curr = new Node(ul, lr, RGBAPixel(avgR, avgG, avgB, avgA));
    curr->A = A;
    curr->B = B;
    curr->C = C;
    curr->D = D;
    curr->E = E;
    curr->F = F;

    return curr;
}

/*** PRIVATE MEMBER FUNCTIONS  ***/

bool HexTree::ShouldPrune(Node* node, const RGBAPixel& rootColor, double tolerance) {
    if (!node) return true;  // Null node prunes by defaultf

    if (AllNullChildren(node)) {
        return node->avg.distanceTo(rootColor) <= tolerance;
    }

    return ShouldPrune(node->A, rootColor, tolerance) &&
           ShouldPrune(node->B, rootColor, tolerance) &&
           ShouldPrune(node->C, rootColor, tolerance) &&
           ShouldPrune(node->D, rootColor, tolerance) &&
           ShouldPrune(node->E, rootColor, tolerance) &&
           ShouldPrune(node->F, rootColor, tolerance);
}


void HexTree::renderNodeAndSubnodes(PNG& img, Node * node, bool fullDepth, unsigned int maxLevel) const {
    if (!node) return; // check null node 
    if ((!fullDepth && maxLevel == 0) || (AllNullChildren(node))) {
// render if we are at our desired level, or we're going to full depth and are at a leaf
        renderAvgOntoImg(img, node);
    } else if (fullDepth || maxLevel > 0) { 
// if going fullDepth or arent' at the maxLevel then recursively call
// if some are leaves then we still arent there yet, and they will be caught by the condition before this in their own call
        renderNodeAndSubnodes(img, node->A, fullDepth, maxLevel - 1);
        renderNodeAndSubnodes(img, node->B, fullDepth, maxLevel - 1);
        renderNodeAndSubnodes(img, node->C, fullDepth, maxLevel - 1);
        renderNodeAndSubnodes(img, node->D, fullDepth, maxLevel - 1);
        renderNodeAndSubnodes(img, node->E, fullDepth, maxLevel - 1);
        renderNodeAndSubnodes(img, node->F, fullDepth, maxLevel - 1);
    } 
}


void HexTree::renderAvgOntoImg(PNG& img, Node* node) const {
    pair<unsigned int, unsigned int> nodeUL = node->upLeft;
    pair<unsigned int, unsigned int> nodeLR = node->lowRight;

    unsigned int leftStart = nodeUL.first;
    unsigned int rightEnd = nodeLR.first;

    unsigned int topStart = nodeUL.second;
    unsigned int bottomEnd = nodeLR.second;

    RGBAPixel average = node->avg; 
    for (unsigned int j = topStart; j <= bottomEnd; ++j) {
        for (unsigned int i = leftStart; i <= rightEnd; i++) {
            *img.getPixel(i, j) = average;
        }
    }
}

bool HexTree::AllNullChildren(Node* node) const {
    return (node->A == nullptr && node->B == nullptr && node->C == nullptr && node->D == nullptr 
     && node->E == nullptr && node->F == nullptr);
}

/** Adds contribution of a child node to the running weighted average.
 * Each color channel (R, G, B, A) is multiplied by the number of pixels in this child
 *  and added to the running totals. The total number of pixels is also updated
   @param child Pointer to the child node to include in the weighted average.
 * @param totalR Accumulated total for the red channel.
 * @param totalG Accumulated total for the green channel.
 * @param totalB Accumulated total for the blue channel.
 * @param totalA Accumulated total for the alpha channel.
 * @param totalPixels Accumulated total number of pixels.
 */
void HexTree::contributeToWeightedAvg(Node* child, unsigned int& totalR, unsigned int& totalG, unsigned int& totalB,
        double& totalA, unsigned int& totalPixels) {
        if (child == nullptr) return;

        unsigned int width = child->lowRight.first - child->upLeft.first + 1;
        unsigned int height = child->lowRight.second - child->upLeft.second + 1;
        unsigned int numNodes = width * height;

        totalR += child->avg.r * numNodes;
        totalG += child->avg.g * numNodes;
        totalB += child->avg.b * numNodes;
        totalA += child->avg.a * numNodes;
        totalPixels += numNodes;
}

void HexTree::ClearNode(Node * node) {
    if (node == nullptr) return;
    ClearSubnodes(node);
    delete node;
    node = nullptr;    
}

void HexTree::ClearSubnodes(Node* node) {
    if (node == nullptr) return;
    ClearNode(node->A);
    node->A = nullptr;
    ClearNode(node->B);
    node->B = nullptr;
    ClearNode(node->C);
    node->C = nullptr;
    ClearNode(node->D);
    node->D = nullptr;
    ClearNode(node->E);
    node->E = nullptr;
    ClearNode(node->F);
    node->F = nullptr;
}

Node* HexTree::CopyNode(Node * node) {
    if (node == nullptr) return nullptr;

    Node * newNode = new Node(node->upLeft, node->lowRight, node->avg);

    newNode->A = CopyNode(node->A);
    newNode->B = CopyNode(node->B);
    newNode->C = CopyNode(node->C);
    newNode->D = CopyNode(node->D);
    newNode->E = CopyNode(node->E);
    newNode->F = CopyNode(node->F);
    
    return newNode;
}

void HexTree::FlipNodeHorizontal(Node* node) {
    if (node == nullptr) return;

    FlipNodeHorizontal(node->A);
    FlipNodeHorizontal(node->B);
    FlipNodeHorizontal(node->C);
    FlipNodeHorizontal(node->D);
    FlipNodeHorizontal(node->E);
    FlipNodeHorizontal(node->F);

    unsigned int left = node->upLeft.first;
    unsigned int right = node->lowRight.first;

    auto flip = [left, right](unsigned int x) {
        return right - (x - left);
    };

    auto adjustBounds = [&](Node* child) {
        if (child == nullptr) return;
        unsigned int ul_x = child->upLeft.first;
        unsigned int lr_x = child->lowRight.first;
        child->upLeft.first = flip(lr_x);
        child->lowRight.first = flip(ul_x);
    };

    Node* tmp = node->A;
    node->A = node->C;
    node->C = tmp;

    tmp = node->D;
    node->D = node->F;
    node->F = tmp;

    adjustBounds(node->A);
    adjustBounds(node->B);
    adjustBounds(node->C);
    adjustBounds(node->D);
    adjustBounds(node->E);
    adjustBounds(node->F);
}


/**
 * Flips a horizontal coordinate x within a rectangle of given width.
 * Returns the mirrored x-coordinate relative to the rectangle's left edge.
 *
 * @param x The original x-coordinate to flip.
 * @param parentLeft The x-coordinate of the left edge of the parent rectangle.
 * @param width The width of the rectangle to flip within.
 * @return The flipped x-coordinate.
 */
unsigned int HexTree::flipX(unsigned int x, unsigned int parentLeft, unsigned int width) {
    return parentLeft + (width - (x - parentLeft));
}

/**
 * Returns an image that has been transformed to grayscale.
 *
 * We are still representing the image using RGBA, but each pixel
 * will be adjusted so that it is a shade of gray with equal RGB values.
 *
 * While setting each RGB channel to the simple average may seem
 * intuitive, the resulting image will not be visually accurate due to the
 * human eye's varying sensitivity to different wavelengths.
 * Therefore the grayscale value for each channel will instead be weighted
 * by the perceived luminosity as 0.299*R + 0.587*G + 0.114*B.
 *
 *
 * @return The grayscale image.
 */
PNG grayscale(PNG image) {
  /// This function is partially written for you so you can see how to
  /// interact with our PNG class.
  for (unsigned x = 0; x < image.width(); x++) {
    for (unsigned y = 0; y < image.height(); y++) {
      RGBAPixel* pixel = image.getPixel(x, y);

      double adjusted_red = 0.299 * pixel->r;
      double adjusted_green = 0.587 * pixel->g;
      double adjusted_blue =  0.114 * pixel->b;
      double grayvalue = adjusted_red + adjusted_green + adjusted_blue; 
      pixel->r = (unsigned char) grayvalue;
      pixel->g = (unsigned char) grayvalue;
      pixel->b = (unsigned char) grayvalue;
    }
  }
  return image;
}