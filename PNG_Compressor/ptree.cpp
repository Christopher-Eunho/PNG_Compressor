/*
*  File:        ptree.cpp
*  Description: Implementation of a partitioning tree class for CPSC 221 PA3
*  Date:        2022-03-03 01:53
*
*               ADD YOUR PRIVATE FUNCTION IMPLEMENTATIONS TO THE BOTTOM OF THIS FILE
*/

#include "ptree.h"
#include "hue_utils.h" // useful functions for calculating hue averages

using namespace cs221util;
using namespace std;

// The following definition may be convenient, but is not necessary to use
typedef pair<unsigned int, unsigned int> pairUI;

/////////////////////////////////
// PTree private member functions
/////////////////////////////////

/*
*  Destroys all dynamically allocated memory associated with the current PTree object.
*  You may want to add a recursive helper function for this!
*  POST: all nodes allocated into the heap have been released.
*/
void PTree::Clear() {
  Clear(root);
  root = nullptr;
}

/*
*  Copies the parameter other PTree into the current PTree.
*  Does not free any memory. Should be called by copy constructor and operator=.
*  You may want a recursive helper function for this!
*  PARAM: other - the PTree which will be copied
*  PRE:   There is no dynamic memory associated with this PTree.
*  POST:  This PTree is a physically separate copy of the other PTree.
*/
void PTree::Copy(const PTree& other) {
  root = Copy(other.root);
}

/*
*  Private helper function for the constructor. Recursively builds the tree
*  according to the specification of the constructor.
*  You *may* change this if you like, but we have provided here what we
*  believe will be sufficient to use as-is.
*  PARAM:  im - full reference image used for construction
*  PARAM:  ul - upper-left image coordinate of the currently building Node's image region
*  PARAM:  w - width of the currently building Node's image region
*  PARAM:  h - height of the currently building Node's image region
*  RETURN: pointer to the fully constructed Node
*/
Node* PTree::BuildNode(PNG& im, pair<unsigned int, unsigned int> ul, unsigned int w, unsigned int h) {
  if (w * h == 0) {
    return nullptr;
  }
  if (w * h == 1) {
    HSLAPixel pixel = *im.getPixel(ul.first, ul.second);
    Node *node = new Node(ul, w, h, pixel);
    return node;
  }
  HSLAPixel averagePixel = GetAveragePixel(im, ul, w, h);
  const bool isWide = w >= h;
  Node *leftChild = isWide ? BuildNode(im, ul, w / 2, h) : BuildNode(im, ul, w, h / 2);
  Node *rightChild = isWide ? BuildNode(im, {ul.first + w / 2, ul.second}, w / 2 + (w % 2), h) : 
                              BuildNode(im, {ul.first, ul.second + h / 2}, w, h / 2 + (h % 2));
  Node *node = new Node(ul, w, h, averagePixel, leftChild, rightChild);
  return node;
}

////////////////////////////////
// PTree public member functions
////////////////////////////////

/*
*  Constructor that builds the PTree using the provided PNG.
*
*  The PTree represents the sub-image (actually the entire image) from (0,0) to (w-1, h-1) where
*  w-1 and h-1 are the largest valid image coordinates of the original PNG.
*  Each node corresponds to a rectangle of pixels in the original PNG, represented by
*  an (x,y) pair for the upper-left corner of the rectangle, and two unsigned integers for the
*  number of pixels on the width and height dimensions of the rectangular sub-image region the
*  node defines.
*
*  A node's two children correspond to a partition of the node's rectangular region into two
*  equal (or approximately equal) size regions which are either tiled horizontally or vertically.
*
*  If the rectangular region of a node is taller than it is wide, then its two children will divide
*  the region into vertically-tiled sub-regions of equal height:
*  +-------+
*  |   A   |
*  |       |
*  +-------+
*  |   B   |
*  |       |
*  +-------+
*
*  If the rectangular region of a node is wider than it is tall, OR if the region is exactly square,
*  then its two children will divide the region into horizontally-tiled sub-regions of equal width:
*  +-------+-------+
*  |   A   |   B   |
*  |       |       |
*  +-------+-------+
*
*  If any region cannot be divided exactly evenly (e.g. a horizontal division of odd width), then
*  child B will receive the larger half of the two subregions.
*
*  When the tree is fully constructed, each leaf corresponds to a single pixel in the PNG image.
* 
*  For the average colour, this MUST be computed separately over the node's rectangular region.
*  Do NOT simply compute this as a weighted average of the children's averages.
*  The functions defined in hue_utils.h and implemented in hue_utils.cpp will be very useful.
*  Computing the average over many overlapping rectangular regions sounds like it will be
*  inefficient, but as an exercise in theory, think about the asymptotic upper bound on the
*  number of times any given pixel is included in an average calculation.
* 
*  PARAM: im - reference image which will provide pixel data for the constructed tree's leaves
*  POST:  The newly constructed tree contains the PNG's pixel data in each leaf node.
*/
PTree::PTree(PNG& im) {
  root = BuildNode(im, {0, 0}, im.width(), im.height());
}

/*
*  Copy constructor
*  Builds a new tree as a copy of another tree.
*
*  PARAM: other - an existing PTree to be copied
*  POST:  This tree is constructed as a physically separate copy of other tree.
*/
PTree::PTree(const PTree& other) {
  Copy(other);
}

/*
*  Assignment operator
*  Rebuilds this tree as a copy of another tree.
*
*  PARAM: other - an existing PTree to be copied
*  POST:  If other is a physically different tree in memory, all pre-existing dynamic
*           memory in this tree is deallocated and this tree is reconstructed as a
*           physically separate copy of other tree.
*         Otherwise, there is no change to this tree.
*/
PTree& PTree::operator=(const PTree& other) {
  if (this != &other) {
    Clear();
    Copy(other);
  }
  return *this;
}

/*
*  Destructor
*  Deallocates all dynamic memory associated with the tree and destroys this PTree object.
*/
PTree::~PTree() {
  Clear();
}

/*
*  Traverses the tree and puts the leaf nodes' color data into the nodes'
*  defined image regions on the output PNG.
*  For non-pruned trees, each leaf node corresponds to a single pixel that will be coloured.
*  For pruned trees, each leaf node may cover a larger rectangular region that will be
*  entirely coloured using the node's average colour attribute.
*
*  You may want to add a recursive helper function for this!
*
*  RETURN: A PNG image of appropriate dimensions and coloured using the tree's leaf node colour data
*/
PNG PTree::Render() const {
  PNG img = PNG(root->width, root->height);
  Render(img, root);
  return img;
}

/*
*  Trims subtrees as high as possible in the tree. A subtree is pruned
*  (its children are cleared/deallocated) if ALL of its leaves have colour
*  within tolerance of the subtree root's average colour.
*  Pruning criteria should be evaluated on the original tree, and never on a pruned
*  tree (i.e. we expect that Prune would be called on any tree at most once).
*  When processing a subtree, you should determine if the subtree should be pruned,
*  and prune it if possible before determining if it has subtrees that can be pruned.
* 
*  You may want to add (a) recursive helper function(s) for this!
*
*  PRE:  This tree has not been previously pruned (and is not copied/assigned from a tree that has been pruned)
*  POST: Any subtrees (as close to the root as possible) whose leaves all have colour
*        within tolerance from the subtree's root colour will have their children deallocated;
*        Each pruned subtree's root becomes a leaf node.
*/
void PTree::Prune(double tolerance) {
  Prune(root, tolerance);
}

/*
*  Returns the total number of nodes in the tree.
*  This function should run in time linearly proportional to the size of the tree.
*
*  You may want to add a recursive helper function for this!
*/
int PTree::Size() const {
  return Size(root);
}

/*
*  Returns the total number of leaf nodes in the tree.
*  This function should run in time linearly proportional to the size of the tree.
*
*  You may want to add a recursive helper function for this!
*/
int PTree::NumLeaves() const {
  return NumLeaves(root);
}

/*
*  Rearranges the nodes in the tree, such that a rendered PNG will be flipped horizontally
*  (i.e. mirrored over a vertical axis).
*  This can be achieved by manipulation of the nodes' member attribute(s).
*  Note that this may possibly be executed on a pruned tree.
*  This function should run in time linearly proportional to the size of the tree.
*
*  You may want to add a recursive helper function for this!
*
*  POST: Tree has been modified so that a rendered PNG will be flipped horizontally.
*/
void PTree::FlipHorizontal() {
  FlipHorizontal(root, root->width);
}

/*
*  Like the function above, rearranges the nodes in the tree, such that a rendered PNG
*  will be flipped vertically (i.e. mirrored over a horizontal axis).
*  This can be achieved by manipulation of the nodes' member attribute(s).
*  Note that this may possibly be executed on a pruned tree.
*  This function should run in time linearly proportional to the size of the tree.
*
*  You may want to add a recursive helper function for this!
*
*  POST: Tree has been modified so that a rendered PNG will be flipped vertically.
*/
void PTree::FlipVertical() {
  FlipVertical(root, root->height);
}

/*
    *  Provides access to the root of the tree.
    *  Dangerous in practice! This is only used for testing.
    */
Node* PTree::GetRoot() {
  return root;
}

//////////////////////////////////////////////
// PERSONALLY DEFINED PRIVATE MEMBER FUNCTIONS
//////////////////////////////////////////////

HSLAPixel PTree::GetAveragePixel(const PNG &im, const pairUI ul, const unsigned int w, const unsigned int h) const {
  struct HSLAAggregator {
    double h_x = 0;
    double h_y = 0;
    double s = 0;
    double l = 0;
    double a = 0;
    int c = 0;
    void add(HSLAPixel *pixel) {
      h_x += Deg2X(pixel->h);
      h_y += Deg2Y(pixel->h);
      s += pixel->s;
      l += pixel->l;
      a += pixel->a;
      c++;
    }
    double hAvg() { return XY2Deg(h_x / (double) c, h_y / (double) c); }
    double sAvg() { return s / (double) c; }
    double lAvg() { return l / (double) c; }
    double aAvg() { return a / (double) c; }
  };

  HSLAAggregator agg;
  for (unsigned int y = ul.second; y < ul.second + h; y++) {
    for (unsigned int x = ul.first; x < ul.first + w; x++) {
      HSLAPixel *pixel = im.getPixel(x, y);
      agg.add(pixel);
    }
  }
  HSLAPixel averagePixel = HSLAPixel(agg.hAvg(), agg.sAvg(), agg.lAvg(), agg.aAvg());
  return averagePixel;
}

unsigned int PTree::NumLeaves(const Node *node) const {
  return !node ? 0 : (!node->A && !node->B ? 1 : NumLeaves(node->A) + NumLeaves(node->B));
}

unsigned int PTree::Size(const Node *node) const {
  return !node ? 0 : 1 + Size(node->A) + Size(node->B);
}

void PTree::Clear(Node *node) const {
  if (node) {
    Clear(node->A);
    Clear(node->B);
    delete node;
  }
}

void PTree::Render(PNG &img, const Node *node) const {
  if (!node) {
    return;
  }
  else if (!node->A && !node->B) {
    for (unsigned int y = node->upperleft.second; y < node->upperleft.second + node->height; y++) {
      for (unsigned int x = node->upperleft.first; x < node->upperleft.first + node->width; x++) {
        *img.getPixel(x, y) = node->avg;
      }
    }
    return;
  }
  Render(img, node->A);
  Render(img, node->B);
}

Node* PTree::Copy(const Node* other) const {
  return !other ? nullptr : new Node(other->upperleft, other->width, other->height, other->avg, Copy(other->A), Copy(other->B));
}

void PTree::Prune(Node *node, const double tolerance) const {
  if (!node) {
    return;
  }
  else if (ShouldPrune(node, node->avg, tolerance)) {
    Clear(node->A);
    node->A = nullptr;
    Clear(node->B);
    node->B = nullptr;
  }
  else {
    Prune(node->A, tolerance);
    Prune(node->B, tolerance);
  }
}

bool PTree::ShouldPrune(const Node *node, const HSLAPixel rootPixel, const double tolerance) const {
  return node &&
         !node->A && !node->B ? 
         node->avg.dist(rootPixel) <= tolerance : 
         ShouldPrune(node->A, rootPixel, tolerance) && ShouldPrune(node->B, rootPixel, tolerance);
}

// TODO: make this work with pruned trees
void PTree::FlipHorizontal(Node* node, const unsigned int width) const {
  if (!node) {
    return;
  }
  else if (!node->A && !node->B) {
    node->upperleft.first = width - 1 - node->upperleft.first;
    return;
  }
  FlipHorizontal(node->A, width);
  FlipHorizontal(node->B, width);
}

// TODO: make this work with pruned trees
void PTree::FlipVertical(Node* node, const unsigned int height) const {
  if (!node) {
    return;
  }
  else if (!node->A && !node->B) {
    node->upperleft.second = height - 1 - node->upperleft.second;
    return;
  }
  FlipVertical(node->A, height);
  FlipVertical(node->B, height);
}