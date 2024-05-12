#include <cmath>
#include <vector>
#include <string>
using namespace std;

// define dada structure
typedef vector<string> String;
typedef vector<double> Vector;
typedef vector<Vector> Matrix;
typedef vector<Matrix> MatrixList;

// overload operator
Vector operator-(const Vector& v1, const Vector& v2);
Vector& operator+=(Vector& v1, const Vector& v2);
Vector& operator/=(Vector& v, double divisor);
Vector operator+(const Vector& v1, const Vector& v2);

// calculations
Vector cal_DailyRet(Vector &prices);
// 3.c + 3.d    calculate the daily log returns using adjusted daily closing prices

Vector cal_AR(Vector &stock_prices, Vector &IWV_prices);
// 3.e  calculate abnormal return given the stock prices and benchmark prices

Vector cal_CumulativeRet(Vector &prices);
// 3.e  calculate the cumulative return from price series

Vector cal_AAR(const Matrix &matrix);
// 3.f  calculate AAR series from given abnormal return matrix

Vector cal_CAAR(const Matrix &matrix);
// 3.g  calculate CAAR value from given abnormal return matrix, return all cumulating vector

Vector cal_avg(const MatrixList& matrixList, Vector (*func)(const Matrix &matrix));
// pass function pointer to calculate both AAR and CAAR

Vector cal_avg_AAR(const MatrixList& matrixList);
// 3.h.i  calculate average AAR series from given abnormal return matrix list

Vector cal_avg_CAAR(const MatrixList& matrixList);
// 3.h.i  calculate average CAAR value from given abnormal return matrix list, return all cumulating vector

Vector cal_std(const MatrixList& matrixList, Vector (*func)(const Matrix &matrix));
// pass function pointer to calculate both AAR and CAAR

Vector cal_std_AAR(const MatrixList& matrixList);
// 3.h.ii   calculate std AAR series from given abnormal return matrix list

Vector cal_std_CAAR(const MatrixList& matrixList);
// 3.h.ii   calculate std CAAR value from given abnormal return matrix list, return all cumulating vector

