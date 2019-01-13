// the cpp file for seam carving
#include <cstdio>
#include <vector>
#include <cmath>

using namespace std;

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

const double eps = 1e-8;

class Tuple {
    public:
        int x, y, z;
        Tuple(int x = 0, int y = 0, int z = 0) {
            this -> x = x;
            this -> y = y; 
            this -> z = z;
        }
};

typedef vector<vector<Tuple> > Image;
typedef vector<vector<double> > Matrix;

//Matrix dp = vector<vector<double> >(1000, vector<double>(1500,0.0));

double min(double a, double b) {
    return ((a < b) ? a : b);
}
double min(double a, double b, double c) {
    if (a < b && a < c) {
        return a;
    } else if (b < c) {
        return b;
    } else {
        return c;
    }
}

Matrix rgbToGray(const Image &img) {
    int h = img.size();
    int w = img[0].size();
    Matrix res = vector<vector<double> >(h, vector<double>(w, 0.0));
    for(int i = 0; i < h; i++) {
        for(int j = 0; j < w; j++) {
            res[i][j] = 0.299 * img[i][j].x + 0.587 * img[i][j].y + 0.114 * img[i][j].z;
        }
    }
    return res;
}

Matrix energy(const Matrix &mat) {
    int h = mat.size();
    int w = mat[0].size();
    Matrix res = vector<vector<double> >(h, vector<double>(w, 0.0));
    for(int i= 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            if(j == w) {
                res[i][j] = mat[i][j];
            } else {
                res[i][j] = fabs(mat[i][j+1] - mat[i][j]);
            }
        }
    }
    return res;
}

vector<int> seamcarving(Matrix mat) {
    // return the line with smalleast energy
    // represented by indices of each row
    int h = mat.size();
    int w = mat[0].size();
    printf("test5\n");
    Matrix dp = vector<vector<double> >(h, vector<double>(w, 0));
    for(int j = 0; j < w; j++) {
        //printf("%d\n", j);
        //printf("%lf\n", mat[0][j]);
        dp[0][j] = mat[0][j];
    }
    printf("test6\n");
    for(int i = 1; i < h; i++) {
        for(int j = 0; j < w; j++) {
            if(j == 0) {
                dp[i][j] = min(dp[i-1][j] + mat[i][j], dp[i-1][j+1] + mat[i][j]);
            } else if(j == w-1) {
                dp[i][j] = min(dp[i-1][j] + mat[i][j], dp[i-1][j-1] + mat[i][j]);
            } else {
                dp[i][j] = min(dp[i-1][j-1] + mat[i][j], dp[i-1][j] + mat[i][j], dp[i-1][j+1] + mat[i][j]);
            }
        }
    }
    printf("test7\n");
    double minval = 1e5;
    int idx = -1;
    for (int j = 0; j < w; j++) {
        if(dp[h-1][j] < minval) {
            idx = j;
            minval = dp[h-1][j];
        }
    }
    printf("idx: %d\n", idx);
    //backtracking
    vector<int> res = vector<int>(h, 0);
    res[h-1] = idx;
    int id;
    for (int ctr = h-1; ctr > 0; ctr--) {
        id = res[ctr];
        if(fabs(dp[ctr-1][id] + mat[ctr][id] - dp[ctr][id]) < eps) {
            res[ctr-1] = id;
        } else if(id > 0 && fabs(dp[ctr-1][id-1] + mat[ctr][id] - dp[ctr][id]) < eps) {
            res[ctr-1] = id-1;
        } else if(id < w-1 && fabs(dp[ctr-1][id+1] + mat[ctr][id] - dp[ctr][id]) < eps) {
            res[ctr-1] = id+1;
        } else {
            printf("error happens!\n");
        }
    }
    return res;
}

int main() {
    //read image
    int w, h, bpp;
    unsigned char * img = stbi_load("Images/1.jpg", &w, &h, &bpp, 3);
    int ctr = 0;
    printf("%d %d\n", w, h);
    vector<vector<Tuple> > img_data = vector<vector<Tuple> >();
	for(int i = 0; i < h; i++) {
	    vector<Tuple> v;
	    for(int j = 0; j < w; j++) {
	        Tuple col = Tuple();
	        col.x = img[ctr];
	        ctr++;
	        col.y = img[ctr];
	        ctr++;
	        col.z = img[ctr];
	        ctr++;
	        v.push_back(col);
	    }
	    img_data.push_back(v);
    }
    stbi_image_free(img);

    printf("test1\n");
    Matrix gray = rgbToGray(img_data);
    printf("test2\n");
    Matrix dx = energy(gray);
    printf("test3\n");
    printf("%lf\n", dx[0][0]);
    vector<int> idx = seamcarving(dx);
    printf("test4\n");

    for(int i = 0; i < h; i++) {
        int j = idx[i];
        img_data[i][j].x = 255;
        img_data[i][j].y = 0;
        img_data[i][j].z = 0;
    }


    //write image
    unsigned char * newimg;
    newimg = new unsigned char[w * h * 3];
    int counter = 0;
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            newimg[counter++] = img_data[i][j].x;
            newimg[counter++] = img_data[i][j].y;
            newimg[counter++] = img_data[i][j].z;
        }
    }
    stbi_write_png("test123.png", w, h, 3, newimg, w*3);
    delete [] newimg;
}