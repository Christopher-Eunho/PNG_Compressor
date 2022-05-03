/*
*  File:        ptree-private.h
*  Description: Private functions for the PTree class
*  Date:        2022-03-06 03:30
*
*               DECLARE YOUR PTREE PRIVATE MEMBER FUNCTIONS HERE
*/

#ifndef _PTREE_PRIVATE_H_
#define _PTREE_PRIVATE_H_

/////////////////////////////////////////////////
// DEFINE YOUR PRIVATE MEMBER FUNCTIONS HERE
//
// Just write the function signatures.
//
// Example:
//
// Node* MyHelperFunction(int arg_a, bool arg_b);
//
/////////////////////////////////////////////////

typedef pair<unsigned int, unsigned int> pairUI;

HSLAPixel GetAveragePixel(const PNG &im, const pairUI ul, const unsigned int w, const unsigned int h) const;
unsigned int NumLeaves(const Node *node) const;
unsigned int Size(const Node *node) const;
void Clear(Node *node) const;
void Render(PNG &img, const Node *node) const;
Node* Copy(const Node* other) const;
void Prune(Node *node, const double tolerance) const;
bool ShouldPrune(const Node *node, const HSLAPixel rootPixel, const double tolerance) const;
void FlipHorizontal(Node* node, const unsigned int width) const;
void FlipVertical(Node* node, const unsigned int height) const;

#endif