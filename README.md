# PNG_Compressor
## Intro
A PNG image compression algorithm developed as an assignment of UBC CPSC 221 course: Basic Algorithms and Data Structures in 2022.
<figure>
<img src = "https://github.com/Christopher-Eunho/PNG_Compressor/blob/main/PNG_Compressor/PNGcompressor.gif">  
<figcaption align = "center"><b>Demo of the program with sample images</b></figcaption>
</figure>


## Algorithm
This algorithm uses lossy image compression method. 
- The program creates a binary tree by partitioning the HSLA pixels of the image. 
- Elements of the tree are partitions of the image that contains the HSLA data of the partition and two children of sub-partitions if they exist. 
- Then the prune function manipulates the pixels within the same partition if they have close HSLA values. The closeness of pixcels is provided by user as an arguemnt 'tolerance' for prune function.
- Prune function recursively anayzes the partitions within the given node deletes its children in BFS traverse.
- After prune, the program creates an new PNG with average pixels of each partition. Depending on the size of the partitions, the blockiness of the image may vary.


## Installation & Demo
1. Download PNG_Compressor folder
2. Run "make testPTree" on command
3. The original images are in the 'originals' folder and compressed images are created in 'images' folder (See testPTree.cpp for details)
4. You can play with this program with your own images! (The images should be uncorrupted PNG files)
