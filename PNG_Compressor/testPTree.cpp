/*
*  File:        testPTree.cpp
*  Description: Basic test cases for PTree class, CPSC 221 2021W2 PA3
*               
*               Recommended for you to modify and/or add test cases!
* 
*  Date:        2022-03-03 02:46
*/

// #define CATCH_CONFIG_MAIN
#include <iostream>
#include <vector>
// #include "cs221util/catch.hpp"

#include "cs221util/HSLAPixel.h"
#include "cs221util/PNG.h"

#include "ptree.h"
#include "hue_utils.h"

using namespace cs221util;
using namespace std;

int main(){

  PNG source1;
  source1.readFromFile("originals/img1.png");
  PTree tree1(source1);
  tree1.Prune(0.05);
  PNG output1 = tree1.Render();
  output1.writeToFile("images/prune-img1.png");


  PNG source2;
  source2.readFromFile("originals/img2.png");
  PTree tree2(source2);
  tree2.Prune(0.05);
  PNG output2 = tree2.Render();
  output2.writeToFile("images/prune-img2.png");

  PNG source3;
  source3.readFromFile("originals/img3.png");
  PTree tree3(source3);
  tree3.Prune(0.05);
  PNG output3 = tree3.Render();
  output3.writeToFile("images/prune-img3.png");
  
  PNG source4;
  source4.readFromFile("originals/img4.png");
  PTree tree4(source4);
  tree4.Prune(0.05);
  PNG output4 = tree4.Render();
  output4.writeToFile("images/prune-img4.png");

  PNG source5;
  source5.readFromFile("originals/img5.png");
  PTree tree5(source5);
  tree5.Prune(0.05);
  PNG output5 = tree5.Render();
  output5.writeToFile("images/prune-img5.png");


  PNG source6;
  source6.readFromFile("originals/img6.png");
  PTree tree6(source6);
  tree6.Prune(0.05);
  PNG output6 = tree6.Render();
  output6.writeToFile("images/prune-img6.png");


}

