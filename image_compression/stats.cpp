
#include "stats.h"

stats::stats(PNG & im){
	// TODO:
        //      - init sumHueX, sumHueY : cos(h), sin(h)
        //      - init sumSat, sumLum : as it says
        // Done - init hist : hist[x-1][y] + hist[x][y-1] - hist[x-1][y-1] + cur          
	hist = vector<vector<vector<int>>>(im.width(), vector<vector<int>>(im.height(), 
                                                                       vector<int>(36,0)));
	sumHueX = vector<vector<double>>(im.width(), vector<double>(im.height(), 0.));
    sumHueY = vector<vector<double>>(im.width(), vector<double>(im.height(), 0.));
    sumLum = vector<vector<double>>(im.width(), vector<double>(im.height(), 0.));
	sumSat = vector<vector<double>>(im.width(), vector<double>(im.height(), 0.));
	/*
	for (unsigned int n = 0; n < im.width(); n++){
		for (unsigned int m = 0; m < im.height(); m++){
			printf("%f ", sumSat[n][m]);
		}
		printf("\n");
	}*/
	// printf("Image size: %u, %u \n", im.width(), im.height());
	for (unsigned int i = 0; i < im.width(); i++){
		bool igt = i > 0;
    		for (unsigned int j = 0; j < im.height(); j++){
			bool jgt = j > 0;
			// Init histogram
			for(int k = 0; k <= 35; k++){
                double one = 0;
                double two = 0;
                double three = 0;
				if(igt)
					one = hist[i - 1][j][k];
                if(jgt)
					two = hist[i][j - 1][k];
                if(igt && jgt)
					three = hist[i- 1][j - 1][k];
				hist[i][j][k] = one + two - three; // if problem, its because
                          				// hist is ints, we used doubles 
			}
			double cur_hue = im.getPixel(i,j)->h;
			int pix_k = cur_hue/10;
			hist[i][j][pix_k]++;
			// Init sumHueX, sumHueY, sumSat, sumLum
			double sum_hue_x = cos(cur_hue * (PI/180.));
			double sum_hue_y = sin(cur_hue * (PI/180.));
			double sum_l = im.getPixel(i,j)->l;
			double sum_s = im.getPixel(i,j)->s;
 			if(igt){ 
				//printf("x is larger than zero, x = %d\n",i);
				sum_hue_x += sumHueX[i - 1][j];
				sum_hue_y += sumHueY[i - 1][j];
				sum_l     += sumLum [i - 1][j];
				sum_s     += sumSat [i - 1][j];
			}
			if(jgt){
				//printf("y is larger than zero, y = %d\n",j);	
				sum_hue_x += sumHueX[i][j - 1];
				sum_hue_y += sumHueY[i][j - 1];
				sum_l     += sumLum [i][j - 1];
				sum_s     += sumSat [i][j - 1];
			}
			if(jgt && igt){
				sum_hue_x -= sumHueX[i - 1][j - 1];
				sum_hue_y -= sumHueY[i - 1][j - 1];
				sum_l     -= sumLum [i - 1][j - 1];
				sum_s     -= sumSat [i - 1][j - 1];
			}

			sumHueX[i][j] = sum_hue_x;
			sumHueY[i][j] = sum_hue_y;
			sumLum [i][j] = sum_l;
			sumSat [i][j] = sum_s;
		} 
	}

}

long stats::rectArea(pair<int,int> ul, pair<int,int> lr){

	long result = ((get<0>(lr) - get<0>(ul)) + 1) * ((get<1>(lr) - get<1>(ul)) + 1);

        return result;

}


HSLAPixel stats::getAvg(pair<int,int> ul, pair<int,int> lr){
	/*
    double sum_h_x;
	double sum_h_y;
	double sum_l;
	double sum_s;

    sum_h_x = sumHueX[get<0>(lr)][get<1>(lr)];
    sum_h_y = sumHueY[get<0>(lr)][get<1>(lr)];
	sum_l   = sumLum[get<0>(lr)][get<1>(lr)];
	sum_s   = sumSat[get<0>(lr)][get<1>(lr)];
	
	double area = (get<0>(lr) + 1) * (get<1>(lr) + 1);
	double h_new = (180./PI) * atan2(sum_h_y, sum_h_x);
	if (h_new < 0)
		h_new += 360;
	return HSLAPixel(h_new, sum_s/area, sum_l/area, 1.0);
	*/








	

	long n = rectArea(ul, lr);
	bool xgt = get<0>(ul) > 0;
    bool ygt = get<1>(ul) > 0;

    // Set equal to largest box
    double sum_hue_x = sumHueX[get<0>(lr)][get<1>(lr)];
    double sum_hue_y = sumHueY[get<0>(lr)][get<1>(lr)];
    double sum_l     = sumLum [get<0>(lr)][get<1>(lr)];
    double sum_s     = sumSat [get<0>(lr)][get<1>(lr)];
    // If xgt, subtract off a part of unused box
    if(xgt){
		sum_hue_x -= sumHueX[get<0>(ul) - 1][get<1>(lr)];
                sum_hue_y -= sumHueY[get<0>(ul) - 1][get<1>(lr)];
		sum_l     -= sumLum [get<0>(ul) - 1][get<1>(lr)];
		sum_s     -= sumSat [get<0>(ul) - 1][get<1>(lr)];	
	}

	if(ygt){
		sum_hue_x -= sumHueX[get<0>(lr)][get<1>(ul) - 1];
		sum_hue_y -= sumHueY[get<0>(lr)][get<1>(ul) - 1];
		sum_l     -= sumLum [get<0>(lr)][get<1>(ul) - 1];
		sum_s     -= sumSat [get<0>(lr)][get<1>(ul) - 1];
	}

	if(xgt && ygt){
		sum_hue_x += sumHueX[get<0>(ul) - 1][get<1>(ul) - 1];
		sum_hue_y += sumHueY[get<0>(ul) - 1][get<1>(ul) - 1];
		sum_l     += sumLum [get<0>(ul) - 1][get<1>(ul) - 1];
		sum_s     += sumSat [get<0>(ul) - 1][get<1>(ul) - 1];
	}

	double sum_h_x = sum_hue_x / n; 
        double sum_h_y = sum_hue_y / n;

	double h_new = d_mod((((180./PI) * atan2(sum_h_y, sum_h_x)) + 360.), 360.);
        double l_new = sum_l / n;
        double s_new = sum_s / n;
        printf("H: %f, S: %f, L: %f calculated by getAvg ////\n", h_new, s_new, l_new);
	double a_new = 1.0;
        return HSLAPixel(h_new,s_new,l_new,a_new);

}

// mod that works on doubles -- cannot handle negative values
double stats::d_mod(double one, double two){
	while (one > two)
		one -= two;
	return one;
}

vector<int> stats::buildHist(pair<int,int> ul, pair<int,int> lr){
	
	vector<int> cur_hist(36,0);
        // init big square
	for (int k = 0; k <= 35; k++)
		cur_hist[k] += hist[get<0>(lr)][get<1>(lr)][k];

        int xgt = get<0>(ul) > 0; 
        int ygt = get<1>(ul) > 0;
	if(xgt){
		for (int k = 0; k <= 35; k++)
			cur_hist[k] -= hist[get<0>(ul) - 1][get<1>(lr)][k]; 
	}
        if(ygt){
		for (int k = 0; k <= 35; k++)
                        cur_hist[k] -= hist[get<0>(lr)][get<1>(ul) - 1][k];
	}
        if(xgt && ygt){
		for (int k = 0; k <= 35; k++)
                        cur_hist[k] += hist[get<0>(ul) - 1][get<1>(ul) - 1][k];
	}

	return cur_hist;
}

// takes a distribution and returns entropy
// partially implemented so as to avoid rounding issues.
double stats::entropy(vector<int> & distn,int area){

    double entropy = 0.;


    for (int i = 0; i < 36; i++) {
        if (distn[i] > 0 ) 
            entropy += ((double) distn[i]/(double) area) 
                                    * log2((double) distn[i]/(double) area);
    }

    return  -1 * entropy;

}




double stats::entropy(pair<int,int> ul, pair<int,int> lr){

	// Call other entropy function with vector of hue values (from hist)
        // and with area (given by rectArea)
	//return entropy(buildHist(ul, lr), rectArea(ul, lr));
	long area = rectArea(ul, lr);
	vector<int> distn = buildHist(ul, lr);
	return entropy(distn, (int) area);
}
