#include "Stocks.h"
#include <vector>
#include <iostream>


using namespace std;
using namespace fre;

class Bootstrap {
private:
    Matrix GetSampleAbnormalReturns();
    vector<Stocks> GetRandomStocks(int n);

    const map<string, Stocks>& group_data_;
    const map<string, Stocks>& iwv_data_;
    unsigned N_;
    int n_samples_;
    Vector avg_aar;
    Vector avg_caar;
    Vector std_aar;
    Vector std_caar;
public:
    Bootstrap(const map<string, Stocks>& group_data, const map<string, Stocks>& iwv_data, int N, int n_samples)
        : group_data_(group_data), iwv_data_(iwv_data), N_(N), n_samples_(n_samples) { 
            cout << "Bootstrap start" << endl;
    }
    void PerformBootstrap();
    const Vector& GetAvgAAR() const { return avg_aar; }
    const Vector& GetAvgCAAR() const { return avg_caar; }
    const Vector& GetStdAAR() const { return std_aar; }
    const Vector& GetStdCAAR() const { return std_caar; }
};

