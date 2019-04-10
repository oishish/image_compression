Check out ReadMe.pdf for some images, correctly compiled Latex and things

# Image_Compression

This is an algorithm meant to compress images using a quadtree and a process of pruning the tree based on similarity between nodes and children.



A process of entropy minimization is run to find the ideal splitting point 

Where entropy is defined as follows

-\sum_{i = 0}^{35} p_i \log_2p_i


Where $p_i$ indicates the *fraction* of pixels within a given range of hues defined by

 $h \geq i * 10$ and $h<(i+1)*10$



Problem specification with more images and better explanations (although this spec may be taken down soon):

https://www.ugrad.cs.ubc.ca/~cs221/2018W2/mps/p3/



**Note**    —    I wrote this algorithm as an assignment for one of my CS classes, not all of the code is mine (much of it is not). Also, of course, the ideas behind the algorithm are definitely not mine.
