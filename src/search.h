#ifndef SEARCH_H
#define SEARCH_H

#include <iostream>
#include <cmath>
#include <vector>

#include <uv.h>
#include <node_api.h>

#include <Eigen/Dense>

typedef Eigen::Map<Eigen::Matrix<float, -1, -1, Eigen::RowMajor> > MatrixChannel;

typedef struct {
    unsigned int rows;
    unsigned int cols;
    unsigned int channels;
    float *k;
    float *r;
    float *g;
    float *b;
    float *a;
} Cargo;

typedef struct {
    unsigned int rows;
    unsigned int cols;
    unsigned int channels;
    MatrixChannel k;
    MatrixChannel r;
    MatrixChannel g;
    MatrixChannel b;
    MatrixChannel a;
} Matrix;

typedef struct {
    unsigned int row;
    unsigned int col;
    double accuracy;
} Match;

struct AsyncBaton {
    uv_work_t request;
    napi_callback callback;
    Cargo *m1;
    Cargo *m2;
    unsigned int colorTolerance;
    unsigned int pixelTolerance;
    std::vector<Match> result;
};

void searchDo(uv_work_t *request);
void searchAfter(uv_work_t *request);
std::vector<Match> search(Matrix &m1, Matrix &m2, unsigned int colorTolerance, unsigned int pixelTolerance);
Eigen::RowVectorXf stdDev(MatrixChannel &m);

#endif