
/**
 *
 * toqutree (pa3)
 * significant modification of a quadtree .
 * toqutree.cpp
 * This file will be used for grading.
 *
 */

#include "toqutree.h"

toqutree::Node::Node(pair<int,int> ctr, int dim, HSLAPixel a)
	:center(ctr),dimension(dim),avg(a),NW(NULL),NE(NULL),SE(NULL),SW(NULL)
	{}

toqutree::~toqutree(){
	clear(root);
}

toqutree::toqutree(const toqutree & other) {
	root = copy(other.root);
}


toqutree & toqutree::operator=(const toqutree & rhs){
	if (this != &rhs) {
		clear(root);
		root = copy(rhs.root);
	}
	return *this;
}

toqutree::toqutree(PNG & imIn, int k){ 

/* This constructor grabs the 2^k x 2^k sub-image centered */
/* in imIn and uses it to build a quadtree. It may assume  */
/* that imIn is large enough to contain an image of that size. */
	int w = imIn.width();
	int h = imIn.height();
	w = w/2;
	h = h/2;
	pair<int,int> corner = make_pair(w - (pow(2,k)/2), h - (pow(2,k)/2));
	size_t size = pow(2,k);
        PNG * edited = new PNG(size, size);
	makePNG(imIn, *edited, corner, size);
	root = buildTree(edited, k);
}


int toqutree::size() {
	return size_rec(root);
}

int toqutree::size_rec(Node * curr){
	if (curr != NULL){
		return 1 + size_rec(curr->SE) 
			 + size_rec(curr->SW)
			 + size_rec(curr->NE)
			 + size_rec(curr->NW);
	} else {
		return 0;
	}
}

toqutree::Node * toqutree::buildTree(PNG * im, int k) {

/* your code here */

// Note that you will want to practice careful memory use
// In this function. We pass the dynamically allocated image
// via pointer so that it may be released after it is used .
// similarly, at each level of the tree you will want to 
// declare a dynamically allocated stats object, and free it
// once you've used it to choose a split point, and calculated
// an average
    stats cur_stats(*im);
	Node * cur_node = new Node(make_pair(0,0), 0, HSLAPixel());
	int best_split_x = 0;
    int best_split_y = 0;
	if(k == 0){
		//cur_node->avg = *(im->getPixel(0,0));	
		cur_node->avg = cur_stats.getAvg(make_pair(0,0), make_pair(0,0));
		cur_node->dimension = 0;
		cur_node->center = make_pair(0,0);
		cur_node->NW = NULL;
		cur_node->NE = NULL;
		cur_node->SW = NULL;
		cur_node->SE = NULL;
		delete im;
		return cur_node;
	}
    int size = pow(2, k - 1);
	
	pair<int,int> best_split = find_split(size, cur_stats, k);
	best_split_x = get<0>(best_split);
	best_split_y = get<1>(best_split);
	
	pair<int,int> topL = make_pair(0,0);
	pair<int,int> bottomR = make_pair((2*size)-1,(2*size)-1);

	cur_node->avg = cur_stats.getAvg(topL, bottomR);
	printf("Current (non leaf) node H: %f, S: %f, L: %f //// \n", cur_node->avg.h, cur_node->avg.s, cur_node->avg.l);
	cur_node->dimension = k;
	pair<int,int> cent = make_pair(best_split_x, best_split_y);
	cur_node->center = cent;

	// Don't forget to delete stats!
	
	// Need to init 4 new smaller PNGS and call the function rec on them
	// Delete old PNG
	PNG * png1 = new PNG(size,size);
	PNG * png2 = new PNG(size,size);
	PNG * png3 = new PNG(size,size);
	PNG * png4 = new PNG(size,size);
	makePNG(*im,*png1,cent,size);
	pair<int,int> p2 = make_pair(best_split_x+size, best_split_y);
	makePNG(*im,*png2,p2,size);
    pair<int,int> p3 = make_pair(best_split_x, best_split_y + size);
	makePNG(*im,*png3,p3,size);
    pair<int,int> p4 = make_pair(best_split_x+size, best_split_y + size);
	makePNG(*im,*png4,p4,size);
	if(k > 0){
		cur_node->SE = buildTree(png1, k-1);
        cur_node->SW = buildTree(png2, k-1);
        cur_node->NE = buildTree(png3, k-1);
        cur_node->NW = buildTree(png4, k-1);
	}
	delete im;
	return cur_node;
}


void toqutree::makePNG(PNG & og, PNG & other, pair<int,int> ul, int size){
	for(unsigned int i = get<0>(ul); i < get<0>(ul) + size; i++){
		for(unsigned int j = get<1>(ul); j < get<1>(ul) + size; j++){
			*(other.getPixel((i-get<0>(ul)),(j - get<1>(ul)))) =
			*(og.getPixel((i%(size*2)),(j%(size*2))));
		}
	}
}

PNG toqutree::render(){

// My algorithm for this problem included a helper function
// that was analogous to Find in a BST, but it navigated the 
// quadtree, instead.
        unsigned int w_h = pow(2, (root->dimension));
	PNG cur_png = PNG(w_h, w_h);
	for(unsigned int i = 0; i < cur_png.width(); i++){
        for(unsigned int j = 0; j < cur_png.height(); j++){
			HSLAPixel this_pixel = render_find(root, i, j);
			*(cur_png.getPixel(i,j)) = this_pixel;
			double hue = this_pixel.h;
			printf("Render pixel hue //// %f\n",hue);
		}
	}
	return cur_png;

}

// render_find locates the pixel at (x, y) and returns its value
HSLAPixel toqutree::render_find(Node * root, int x_s, int y_s){
	int spl_x = get<0>(root->center);
	int spl_y = get<1>(root->center);
	int size = pow(2, root->dimension-1);
	if(root->NW == NULL
           && root->NE == NULL
           && root->SE == NULL
           && root->SW == NULL){
		return (root->avg);
	} 
	if(x_s >= spl_x + size || x_s < spl_x){
		// in SW or NW
		if(y_s  >= spl_y + size || y_s < spl_y){
			// NW = 4
			pair<int,int> new_coord = rel_coord(spl_x, spl_y, size, x_s, y_s, 4); 
			return render_find(root->NW, get<0>(new_coord), get<1>(new_coord));
		} else { 
			// SW = 2
			pair<int,int> new_coord = rel_coord(spl_x, spl_y, size, x_s, y_s, 2);
			return render_find(root->SW, get<0>(new_coord), get<1>(new_coord));
		}	
	} else {
		// in SE or NE
		if(y_s  >= spl_y + size || y_s < spl_y){
			// NE = 3
			pair<int,int> new_coord = rel_coord(spl_x, spl_y, size, x_s, y_s, 3);
			return render_find(root->NE, get<0>(new_coord), get<1>(new_coord));
		} else {
			// SE = 1 
			pair<int,int> new_coord = rel_coord(spl_x, spl_y, size, x_s, y_s, 1);
			return render_find(root->SE, get<0>(new_coord), get<1>(new_coord));
		}
	}

}
/* Generates the optimal splitting point in a PNG using the stats object,
 * and the size of the image
 *
 */

pair<int,int> toqutree::find_split(int size, stats cur_stats, int k){
    int best_split_x;
	int best_split_y;
	double best_entr_total = 100000;  // super large as starting condition
	for (int i = 0; i < size; i++){
		for (int j = 0; j < size; j++){
			// Make (up to 8) hist for this splitting point
			// Then condense down into 4 vectors for running entropy
			//  __________________
			// |   .         .    |
            // | 4 .    3    .  4 |
            // | . . . . . . . . .|
            // |   .         .    |
            // | 2 .    1    .  2 |
            // |   .         .    |
            // | . . . . . . . . .|
            // | 4 .    3    .  4 |
            // --------------------
			vector<int> hist1;
 			vector<int> hist2;
			vector<int> hist3;
			vector<int> hist4;
			pair<int,int> one = make_pair(i, j);
			pair<int,int> two = make_pair(i+size, j);
			pair<int,int> thr = make_pair(i, j+size);
			pair<int,int> fou = make_pair(i+size, j+size);
			pair<int,int> one_lr = make_pair(i+size-1, j+size-1);
			//hist 1
			hist1 = cur_stats.buildHist(one, one_lr);		
			
			if(i == 0 && j == 0){
				// hist 2
				pair<int,int> two_lr = make_pair((2*size)-1, size-1);
				hist2 = cur_stats.buildHist(two, two_lr);
				// hist 3
				pair<int,int> thr_lr = make_pair(size-1, (2*size)-1);
				hist3 = cur_stats.buildHist(thr, thr_lr);
				// hist 4 
				pair<int,int> fou_lr = make_pair((2*size)-1, (2*size)-1);
				hist4 = cur_stats.buildHist(fou, fou_lr);
			} else if(i == 0){
				// hist 2
				pair<int,int> two_lr = make_pair((2*size)-1,j+size-1);
                                hist2 = cur_stats.buildHist(two, two_lr);
				// hist 3
				pair<int,int> thr_lr = make_pair(size-1, (2*size)-1);
				hist3 = cur_stats.buildHist(thr, thr_lr);
				pair<int,int> thr2 = make_pair(0,0);
				pair<int,int> thr2_lr = make_pair(size-1,j-1);
				vector<int> hist3_2 = cur_stats.buildHist(thr2, thr2_lr);
				// hist 4
				pair<int,int> fou_lr = make_pair((2*size)-1, (2*size)-1);
				hist4 = cur_stats.buildHist(fou, fou_lr);
				pair<int,int> fou2 = make_pair(size, 0);
				pair<int,int> fou2_lr = make_pair((2*size)-1, j-1);
				vector<int> hist4_2 = cur_stats.buildHist(fou2, fou2_lr);
				// combine hists for 3 & 4
				for(int n = 0; n <=35; n++){
					hist3[n] += hist3_2[n];
					hist4[n] += hist4_2[n];
				}	
			} else if(j == 0){
				// hist 2
				pair<int,int> two_lr = make_pair((2*size)-1, size-1);
				hist2 = cur_stats.buildHist(two, two_lr);
				pair<int,int> two2 = make_pair(0,0);
				pair<int,int> two2_lr = make_pair(i-1,size-1);
				vector<int> hist2_2 = cur_stats.buildHist(two2, two2_lr);
				// hist 3
				pair<int,int> thr_lr = make_pair(i+size-1, (2*size)-1);
				hist3 = cur_stats.buildHist(thr,thr_lr);
				// hist 4
				pair<int,int> fou_lr = make_pair((2*size)-1, (2*size)-1);
				hist4 = cur_stats.buildHist(fou, fou_lr);
				pair<int,int> fou2 = make_pair(0,size);
				pair<int,int> fou2_lr = make_pair(i-1 ,(2*size)-1);
				vector<int> hist4_2 = cur_stats.buildHist(fou2, fou2_lr);
				for(int n = 0; n <=35; n++){
                                        hist2[n] += hist2_2[n];
                                        hist4[n] += hist4_2[n];
                                }
			} else {
		        	// hist 2
				pair<int,int> two_lr = make_pair((2*size)-1, j+size-1);
				hist2 = cur_stats.buildHist(two, two_lr);
				pair<int,int> two2 = make_pair(0, j);
				pair<int,int> two2_lr = make_pair(i-1,j+size-1);
				vector<int> hist2_2 = cur_stats.buildHist(two2, two2_lr);
				// hist 3
                                pair<int,int> thr_lr = make_pair(i+size-1,(2*size)-1);
                                hist3= cur_stats.buildHist(thr, thr_lr);
                                pair<int,int> thr2 = make_pair(i,0);
                                pair<int,int> thr2_lr = make_pair(i+size-1,j-1);
                                vector<int> hist3_2 = cur_stats.buildHist(thr2, thr2_lr);
				// hist 4
				pair<int,int> fou_lr = make_pair((2*size)-1, (2*size)-1);
				hist4 = cur_stats.buildHist(fou, fou_lr);
				pair<int,int> fou2 = make_pair(i+size,0);
				pair<int,int> fou2_lr = make_pair((2*size)-1,j-1);
				vector<int> hist4_2 = cur_stats.buildHist(fou2,fou2_lr);
				pair<int,int> fou3 = make_pair(0,0);
				pair<int,int> fou3_lr = make_pair(i-1,j-1);
				vector<int> hist4_3 = cur_stats.buildHist(fou3,fou3_lr);
				pair<int,int> fou4 = make_pair(0,j+size-1);
                                pair<int,int> fou4_lr = make_pair(i-1,(2*size)-1);
                                vector<int> hist4_4 = cur_stats.buildHist(fou4,fou4_lr);
				for(int n = 0; n <=35; n++){
                                        hist2[n] += hist2_2[n];
					hist3[n] += hist3_2[n];
                                        hist4[n] += hist4_2[n] + hist4_3[n] + hist4_4[n];
                                }
			}
			double entr1 = cur_stats.entropy(hist1, pow(2,2*k));
			double entr2 = cur_stats.entropy(hist2, pow(2,2*k));
			double entr3 = cur_stats.entropy(hist3, pow(2,2*k));
			double entr4 = cur_stats.entropy(hist4, pow(2,2*k));
			double entr_tot = (entr1 + entr2 + entr3 + entr4)/4;
			if (entr_tot < best_entr_total){
				best_entr_total = entr_tot;
				best_split_x = i;
				best_split_y = j;
			}
		}
	}
	return make_pair(best_split_x, best_split_y);		
}

// get relative coordinate in child, give parent info
pair<int,int> toqutree::rel_coord(int spl_x, int spl_y, int size, int cx, int cy, int node){
	int new_x = 0;
	int new_y = 0;
	// NODE decryption
        // SE = 1
	// SW = 2
	// NE = 3
	// NW = 4
	switch(node){
		case 1: // SE
			new_x = cx - spl_x;
			new_y = cy - spl_y;
			break;
		case 2: // SW
			new_x = modulus(cx - spl_x - size, size); 
			new_y = cy - spl_y;
			break;
		case 3: // NE
			new_x = cx - spl_x;
			new_y = modulus(cy - spl_y - size, size);
			break;
		case 4: // NW
			new_x = modulus(cx - spl_x - size, size);
			new_y = modulus(cy - spl_y - size, size);
			break;
		default:
			printf("ERROR:toqtr::rel_coord, default case should be unreachable");
	}
	pair<int,int> ret_val = make_pair(new_x, new_y);
	return ret_val;
}

int toqutree::modulus(int first, int second){
	while(first < 0)
		first += second;
	return first%second;
}

/* Iterating through the tree (Attempt #1)
	if(root->NW != NULL
           && root->NE != NULL
           && root->SE != NULL
           && root->SW !=NULL){
        	render(root->NE, img, (x + get<0>(root->center))%((int) pow(2, root->dimension)), 
                                      (y + get<1>(root->center) + pow(2,root->dimension - 1))%((int) pow(2, root->dimension)));
		render(root->NW, img, (x + get<0>(root->center) + pow(2,root->dimension - 1))%((int) pow(2, root->dimension)),
                                      (y + get<1>(root->center) + pow(2,root->dimension - 1))%((int) pow(2, root->dimension)));
		render(root->SE, img, (x + get<0>(root->center))%((int) pow(2, root->dimension)),
                                      (y + get<1>(root->center)))%((int) pow(2, root->dimension));
		render(root->SW, img, (x + get<0>(root->center) + pow(2,root->dimension - 1))%((int) pow(2, root->dimension)), 
                                      (y + get<1>(root->center))%((int) pow(2, root->dimension)));
	} else if(root->NW == NULL
                  && root->NE == NULL
                  && root->SE == NULL
                  && root->SW ==NULL){
		for (int i = x; i < x + pow(2, root->dimension); i++){
			for (int j = y; j < y + pow(2, root->dimension); j++){
				int real_i = i%((int) img.width());
				int real_j = j%((int) img.width());
				counter++;
				*(img.getPixel(real_i, real_j)) = root->avg;
			}
		}
	}
	return img;
*/


/* oops, i left the implementation of this one in the file! */
void toqutree::prune(double tol){
	// Call recursive implementation
	prune_rec(root, tol);
	//prune_rec(root, 0.005);
	printf("Tolerance %f\n", tol);
	
}

/* recursive implementation of prune */
void toqutree::prune_rec(Node * root, double tol){
	// base case
	if(root->NW == NULL 
           && root->NE == NULL
           && root->SW == NULL
           && root->SE == NULL)
		return;
	// rec case
	double dist1 = (root->avg).dist(root->NW->avg);
	double dist2 = (root->avg).dist(root->NE->avg);
	double dist3 = (root->avg).dist(root->SW->avg);
	double dist4 = (root->avg).dist(root->SE->avg);
	printf("Dist1: %f, Dist2: %f, Dist3: %f, Dist4: %f\n", dist1,dist2,dist3,dist4);
	if(dist1 <= tol
	   && dist2 <= tol
	   && dist3 <= tol
	   && dist4 <= tol){
		// all within tol => remove subtrees
		// ***TODO***
		// clear memory associated with old subtrees
		delete root->NW;
	    delete root->NE;
	    delete root->SW;
	    delete root->SE;
		root->NW = NULL;
		root->NE = NULL;
		root->SW = NULL;
		root->SE = NULL;
		return;
	} else {
		prune_rec(root->NW, tol);
		prune_rec(root->NE, tol);
		prune_rec(root->SW, tol);
		prune_rec(root->SE, tol);
	}
}


/* called by destructor and assignment operator*/
void toqutree::clear(Node * & curr){
	if(curr == NULL)
		return;
	clear(curr->NW);
	clear(curr->NE);
	clear(curr->SW);
	clear(curr->SE);
	delete curr;
}

/* done */
/* called by assignment operator and copy constructor */
toqutree::Node * toqutree::copy(const Node * other) {
	if(other == NULL)
		return NULL;
	Node * curr = new Node(other->center, other->dimension, other->avg);
	curr->NW = copy(other->NW);
	curr->NE = copy(other->NE);
	curr->SW = copy(other->SW);
	curr->SE = copy(other->SE);
	return curr;
	
}


