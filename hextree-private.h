/**
 * @file hextree-private.h
 * @description Declaration of private HexTree functions
 */

/* Helper for Render */
void renderNodeAndSubnodes(PNG& img, Node * node, bool fullDepth, unsigned int maxLevel) const;

/* Updates PNG RGBAPixels to hold the average color */
void renderAvgOntoImg(PNG& img, Node* node) const;

/* Returns true if node doesn't have any non-null children */
bool AllNullChildren(Node* node) const;

/* Clears node and all its subnodes */
void ClearNode(Node * node);

/* Returns deep copy of node */
Node* CopyNode(Node * node);

/* Recursive helper for FlipHorizontal */
void FlipNodeHorizontal(Node * node);

void contributeToWeightedAvg(Node* child, unsigned int& totalR, unsigned int& totalG, unsigned int& totalB, double& totalA, unsigned int& totalPixels);

/* Translates X-coordinate */
unsigned int flipX(unsigned int x, unsigned int parentLeft, unsigned int width);

/* Clears subnodes */
void ClearSubnodes(Node* node);

/* Determines if this node should be pruned based on if all its children should be pruned */
bool ShouldPrune(Node* node, const RGBAPixel& rootColor, double tolerance);

/* Recursive helper function for Prune  */ 
void Prune(Node* node, double tolerance);


// bool PruneNode(Node* node, const RGBAPixel& rootColor, double tolerance);
