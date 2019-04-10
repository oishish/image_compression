#define CATCH_CONFIG_MAIN
#include "cs221util/catch.hpp"
#include <vector>
#include <sys/stat.h>
#include <iostream>
#include "cs221util/PNG.h"
#include "cs221util/HSLAPixel.h"
#include "stats.h"
#include "toqutree.h"

using namespace std;
using namespace cs221util;
/*

TEST_CASE("stats::basic rectArea","[weight=1][part=stats]"){

    PNG data; data.resize(2,2);

    stats s(data);
    pair<int,int> ul(0,0);
    pair<int,int> lr(1,1);
    long result = s.rectArea(ul,lr);

    REQUIRE(result == 4);
    
}

TEST_CASE("stats::basic getAvg","[weight=1][part=stats]"){
    PNG data; data.resize(2,2);
    for (int i = 0; i < 2; i ++){
        for (int j = 0; j < 2; j++){
            HSLAPixel * p = data.getPixel(i,j);
            p->h = 135*j + i * 90;
            p->s = 1.0;
            p->l = 0.5;
            p->a = 1.0;
        }
    }
    stats s(data);
    pair<int,int> ul(0,0);
    pair<int,int> lr(1,1);
    HSLAPixel result = s.getAvg(ul,lr);
    HSLAPixel expected(112.5,1.0, 0.5);
    REQUIRE(result == expected);
}

TEST_CASE("stats::basic entropy","[weight=1][part=stats]"){
    PNG data; data.resize(2,2);
    for (int i = 0; i < 2; i ++){
        for (int j = 0; j < 2; j++){
            HSLAPixel * p = data.getPixel(i,j);
            p->h = 135*j + i * 90;
            p->s = 1.0;
            p->l = 0.5;
            p->a = 1.0;
        }
    }
    stats s(data);
    pair<int,int> ul(0,0);
    pair<int,int> lr(1,1);
    long result = s.entropy(ul,lr);
    REQUIRE(result == 2);
}

TEST_CASE("Small average", "[weight=1][part=toqutree]"){
    PNG img;
    img.readFromFile("images/tester.png");
    stats s(img);
    pair<int,int> ULcorner = make_pair(0,0);
    pair<int,int> RLcorner = make_pair(1,1);
    PNG ret_val(1,1);
    *(ret_val.getPixel(0,0)) = (s.getAvg(ULcorner,RLcorner));
    ret_val.writeToFile("images/testerResult.png");

    REQUIRE(1);
}

TEST_CASE("toqutree::basic ctor render","[weight=1][part=toqutree]"){
    PNG img;
    img.readFromFile("images/stanleySquare.png");

    toqutree t1(img,9);

    PNG out = t1.render();
    out.convert();

    out.writeToFile("images/stanleySquareTestOut.png");
    
    

    REQUIRE(out==img);
}

TEST_CASE("toqutree::basic copy","[weight=1][part=toqutree]"){
    PNG img;
    img.readFromFile("images/geo.png");

    toqutree t1(img,5);
    toqutree t1copy(t1);

    PNG out = t1copy.render();

    REQUIRE(out==img);
}

TEST_CASE("toqutree::basic prune","[weight=1][part=toqutree]"){
    PNG img;
    img.readFromFile("images/ada.png");
    
    toqutree t1(img,9);

    t1.prune(0.05);
    PNG result = t1.render();

    PNG expected; expected.readFromFile("images/adaPrune.05.png");
    result.convert();
    result.writeToFile("images/adaTEST.png");
    REQUIRE(expected==result);
}
*/
TEST_CASE("toqutree::basic splitting","[weight=1][part=toqutree]"){

	PNG img;
    img.readFromFile("images/bigCase4.png");
	stats s(img);
	toqutree t(img, 3);
    PNG out(3,3);
    t.makePNG(img, out, make_pair(3,1), 3);
    out.writeToFile("images/bigCase4TEST.png");
	REQUIRE(1);
}

/*
TEST_CASE("toqutree::basic splitting #2","[weight=1][part=toqutree]"){
 
        PNG img(4,4);
        for(int i = 0; i < 2; i++){
                for(int j = 0; j < 2; j++){
                        if(i < 2 && j < 2)
                               *(img.getPixel(i,j)) = HSLAPixel(250,1,1,1);
                        else if(i > 2 && j < 2)
                               *(img.getPixel(i,j)) = HSLAPixel(200,1,1,1);
                        else if(i > 2 && j < 2)
                               *(img.getPixel(i,j)) = HSLAPixel(150,1,1,1);
			else if()                       
			else
                               *(img.getPixel(i,j)) = HSLAPixel(100,1,1,1);
                }
        }
        stats s(img);
        toqutree t(img, 2);
        pair<int,int> split = t.find_split(2, s, 2);
        REQUIRE(get<0>(split) == 0);
        REQUIRE(get<1>(split) == 0);
}

*/

