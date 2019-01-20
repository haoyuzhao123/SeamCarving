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

Image transpose(const Image &img) {
    int w = img[0].size();
    int h = img.size();
    Image imgnew = vector<vector<Tuple> >(w, vector<Tuple>(h, Tuple()));
    for (int i = 0; i < h; i++) {
        for(int j = 0; j < w; j++) {
            imgnew[j][i].x = img[i][j].x;
            imgnew[j][i].y = img[i][j].y;
            imgnew[j][i].z = img[i][j].z;
        }
    }
    return imgnew;
}

Matrix transpose(const Matrix &mat) {
    int w = mat[0].size();
    int h = mat.size();
    Matrix matnew = vector<vector<double> >(w, vector<double>(h, 0.0));
    for (int i = 0; i < h; i++) {
        for(int j = 0; j < w; j++) {
            matnew[j][i] = mat[i][j];
        }
    }
    return matnew;
}

Image deleteline(const Image &img, const vector<int> &idx) {
    Image newimg = img;
    for(int i = 0; i < img.size(); i++) {
        newimg[i].erase(newimg[i].begin() + idx[i]);
    }
    return newimg;
}

Matrix deleteline(const Matrix &mat, const vector<int> &idx) {
    Matrix newmat = mat;
    for(int i = 0; i < mat.size(); i++) {
        newmat[i].erase(newmat[i].begin() + idx[i]);
    }
}

Image insert(const Image &img, const vector<int> &idx) {
    Image newimg = img;
    for(int i = 0; i < img.size(); i++) {
        newimg[i].insert(newimg[i].begin() + idx[i], Tuple(255,0,0));
    }
    return newimg;
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
    //printf("test5\n");
    Matrix dp = vector<vector<double> >(h, vector<double>(w, 0));
    for(int j = 0; j < w; j++) {
        //printf("%d\n", j);
        //printf("%lf\n", mat[0][j]);
        dp[0][j] = mat[0][j];
    }
    //printf("test6\n");
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
    //printf("test7\n");
    double minval = 1e5;
    int idx = -1;
    for (int j = 0; j < w; j++) {
        if(dp[h-1][j] < minval) {
            idx = j;
            minval = dp[h-1][j];
        }
    }
    //printf("idx: %d\n", idx);
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
    // if we should mark the deleted lines
    bool flag = true;
    int w, h, bpp;
    unsigned char * img = stbi_load("Images/7.jpg", &w, &h, &bpp, 3);
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

    
    //printf("test1\n");
    Matrix gray = rgbToGray(img_data);
    //printf("test2\n");
    Matrix dx = energy(gray);
    //printf("test3\n");
    //printf("%lf\n", dx[0][0]);

    int delcol = 60;
    int delrow = 60;
    vector<vector<int> > idxcol;
    vector<vector<int> > idxrow;

    for(int i = 0; i < delcol; i++) {
        if (i % 10 == 0)
            printf("delete column: %d \n", i);
        vector<int> idx = seamcarving(dx);
        idxcol.push_back(idx);
        img_data = deleteline(img_data, idx);
        dx = energy(rgbToGray(img_data));
    }

    img_data = transpose(img_data);
    dx = energy(rgbToGray(img_data));
    printf("testtest\n");
    for(int i = 0; i < delrow; i++) {
        if (i % 10 == 0)
            printf("delete row: %d \n", i);
        vector<int> idx = seamcarving(dx);
        idxrow.push_back(idx);
        img_data = deleteline(img_data, idx);
        dx = energy(rgbToGray(img_data));
    }

    img_data = transpose(img_data);

    //printf("test4\n");
    /*
    for(int i = 0; i < h; i++) {
        int j = idx[i];
        img_data[i][j].x = 255;
        img_data[i][j].y = 0;
        img_data[i][j].z = 0;
    }
    }
    */
   
    if(flag) {
        img_data = transpose(img_data);
        for(int i = idxrow.size()-1; i >= 0; i--) {
            img_data = insert(img_data, idxrow[i]);
        }
        img_data = transpose(img_data);
        for(int i = idxcol.size()-1; i >= 0; i--) {
            img_data = insert(img_data, idxcol[i]);
        }
    }

    //write image
    if(!flag) {
        w = w - delcol;
        h = h - delrow;
    }
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
    stbi_write_png("del7mark.png", w, h, 3, newimg, w*3);
    delete [] newimg;
    return 0;
}