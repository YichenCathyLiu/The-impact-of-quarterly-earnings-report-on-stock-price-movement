#include "Calculation.h"
#include <vector>
#include <numeric>
#include <stdexcept>
#include <iostream>

using namespace std;

// Overload a few arithmetic operators for vector/matrix.
Vector operator-(const Vector& v1, const Vector& v2) {
    if (v1.size() != v2.size()) {
        throw std::invalid_argument("Vector size mismatch.");
    }
    Vector result(v1.size());
    for (unsigned int i = 0; i < v1.size(); i++) {
        result[i] = v1[i] - v2[i];
    }
    return result;
}

Vector operator+(const Vector& v1, const Vector& v2) {
    if (v1.size() != v2.size()) {
        throw std::invalid_argument("Vector size mismatch.");
    }
    Vector result(v1.size());
    for (unsigned int i = 0; i < v1.size(); i++) {
        result[i] = v1[i] + v2[i];
    }
    return result;
}


 Vector& operator+=(Vector& v1, const Vector& v2) {
    if (v1.size() != v2.size()) {
        throw std::invalid_argument("Vector size mismatch.");
    }
    for (unsigned int i = 0; i < v1.size(); i++) {
        v1[i] += v2[i];
    }
    return v1;
}

Vector& operator/=(Vector& v, double divisor) {
    if (divisor == 0) {
        throw std::invalid_argument("Division by zero.");
    }
    for (unsigned int i = 0; i < v.size(); i++) {
        v[i] /= divisor;
    }
    return v;
}

// Use member functions or independent functions for all calculations. 
// Calculate daily return
Vector cal_DailyRet(Vector &prices) {    
    Vector daily_ret;
    double returns = 0.0;
    int n = prices.size();
    for (int i = 0; i <= n-2; i++) {
        returns = log(prices[i+1]/ prices[i]); // log return
        daily_ret.push_back(returns);
    }
    return daily_ret;
}

// Calculate cumulative return
Vector cal_CumulativeRet(Vector &prices) {
    Vector daily_ret = cal_DailyRet(prices);
    Vector cum_ret;
    double returns = 0.0;
    int n = daily_ret.size();
    for (int i = 0; i < n; i++) {
        returns += daily_ret[i];
        cum_ret.push_back(returns);
    }
    return cum_ret;
}

// Calculate abnormal return
Vector cal_AR(Vector &stock_prices, Vector &IWV_prices) {     
	Vector stock_ret = cal_DailyRet(stock_prices);
	Vector IWV_ret = cal_DailyRet(IWV_prices);
	Vector abnormal_ret = stock_ret - IWV_ret;
	return abnormal_ret;
}

// Calculate average abnormal return
Vector cal_AAR(const Matrix &matrix) {
    Vector aar = matrix[0]; // initialize the result with the first stock
    for(size_t i = 1; i < matrix.size() ; i++) { // add from the second stock
        aar += matrix[i]; // Using overloaded operator+ for vector + vector
    }
    aar /= static_cast<double>(matrix.size()); // Using overloaded operator/
    return aar;  
}

// Calculate cumulative average abnormal return
Vector cal_CAAR(const Matrix &matrix) {
    Vector aar = matrix[0];
    aar = cal_AAR(matrix);
    // int length = static_cast<int>(aar.size());
    Vector caar; 
    double result = 0.0;
    for (size_t i = 0; i < aar.size(); i++) {
        result += aar[i];
        caar.push_back(result);
    }
    return caar;
}

// pass a function pointer to calculate avg of both AAR and CAAR
Vector cal_avg(const MatrixList& matrixList, Vector (*func)(const Matrix &matrix)) {
    if (matrixList.empty()) return Vector();
    Vector avg = func(matrixList[0]); // initialize with the first stock
    for (size_t i = 1; i < matrixList.size(); i++) {
        avg += func(matrixList[i]);  // Using overloaded operator+=
    }
    avg /= static_cast<double>(matrixList.size());  // Using overloaded operator/
    return avg; 
}

// Calculate average AAR
Vector cal_avg_AAR(const MatrixList& matrixList) {
    return cal_avg(matrixList, cal_AAR);
}

// Calculate average CAAR
Vector cal_avg_CAAR(const MatrixList& matrixList) {
    return cal_avg(matrixList, cal_CAAR);
}

// pass a function pointer to calculate std of both AAR and CAAR
Vector cal_std(const MatrixList& matrixList, Vector (*func)(const Matrix &matrix)) {
    int sampling_num = static_cast<int>(matrixList.size());        // The number of samplings (40)
    int length = static_cast<int>(matrixList[0][0].size());    // The length of AAR vector 
    Matrix result_matrix;  // initialize an empty matrix to store each sampling's AAR vector
    // Matrix current_matrix;
    Vector current;
    for(int i = 0; i < sampling_num; i++) {
        current = func(matrixList[i]);
        result_matrix.push_back(current);
    }
    
    Vector std(length); // initialize an empty vector to store the standard deviation of each column
    
    for(int j = 0; j < length; j++) {
        Vector col_j; // get the j-th column of result_matrix
        for(int i = 0; i < sampling_num; i++) {
            col_j.push_back(result_matrix[i][j]);
        }
        // calculate the standard deviation of the j-th column and store it in std_arr
        double mean = accumulate(col_j.begin(), col_j.end(), 0.0) / sampling_num;
        double variance = 0.0;
        for (int i = 0; i < sampling_num; i++) {
            variance += pow(col_j[i] - mean, 2);
        }
        variance /= (sampling_num - 1.0);
        double stddev = sqrt(variance);
        std[j] = stddev;
    } 
    return std;
}

// Calculate standard deviation of AAR
Vector cal_std_AAR(const MatrixList& matrixList) {
    return cal_std(matrixList, cal_AAR);
}

// Calculate standard deviation of CAAR
Vector cal_std_CAAR(const MatrixList& matrixList) {
    return cal_std(matrixList, cal_CAAR);
}

