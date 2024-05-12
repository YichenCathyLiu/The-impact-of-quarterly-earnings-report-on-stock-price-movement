#include "Bootstrap.h"

#include <random>
#include <cmath>
#include <algorithm>
#include <iterator>
#include <vector>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
using namespace std;

Matrix Bootstrap::GetSampleAbnormalReturns() {
    vector<Stocks> stocks = GetRandomStocks(30);
    Matrix sample_abnormal_returns;

    for (const auto& stock : stocks) {
        map<string, double> stock_prices = stock.GetPrices();
        map<string, double> IWV_prices = iwv_data_.at("IWV").GetPrices();

        map<string, double> filtered_IWV_prices;
        for (const auto& date_price_pair : stock_prices) {
            if (IWV_prices.find(date_price_pair.first) != IWV_prices.end()) {
                filtered_IWV_prices[date_price_pair.first] = IWV_prices.at(date_price_pair.first);
            }
        }
        Vector stock_prices_values(stock_prices.size());
        Vector IWV_prices_values(filtered_IWV_prices.size());

        transform(stock_prices.begin(), stock_prices.end(), stock_prices_values.begin(), [](const pair<string, double>& p) { return p.second; });
        transform(filtered_IWV_prices.begin(), filtered_IWV_prices.end(), IWV_prices_values.begin(), [](const pair<string, double>& p) { return p.second; });
        
        Vector abnormal_returns = cal_AR(stock_prices_values, IWV_prices_values);
        sample_abnormal_returns.push_back(abnormal_returns);
    }
    return sample_abnormal_returns;

}

vector<Stocks> Bootstrap::GetRandomStocks(int n) {

    vector<Stocks> all_stocks;
    transform(group_data_.begin(), group_data_.end(), back_inserter(all_stocks), [](const pair<string, Stocks>& p) { return p.second; });
                
    vector<Stocks> new_all_stocks;
    for (const auto& stock : all_stocks) {
        map<string, double> stock_prices = stock.GetPrices();
        if (stock_prices.size() >= (unsigned)2*N_){
            new_all_stocks.push_back(stock);
        }
    }

    all_stocks = new_all_stocks;

    random_device rd;
    mt19937 g(rd());
    shuffle(all_stocks.begin(), all_stocks.end(), g);
    all_stocks.resize(n);

    return all_stocks;
}

void Bootstrap::PerformBootstrap() {
    // cout<<"PerformBootstrap start"<<endl;
    MatrixList samples;
    for (int i = 0; i < n_samples_; i++) {
        Matrix sample_abnormal_returns = GetSampleAbnormalReturns();
        samples.push_back(sample_abnormal_returns);
    }

    // cout<<"samples.size"<< samples.size()<<endl;
    // cout<<"cal_avg_AAR start"<<endl;
    avg_aar = cal_avg_AAR(samples);
    // cout<<"cal_avg_CAAR start"<<endl;
    avg_caar = cal_avg_CAAR(samples);
    // cout<<"cal_std_AAR start"<<endl;
    std_aar = cal_std_AAR(samples);
    // cout<<"cal_std_CAAR start"<<endl;
    std_caar = cal_std_CAAR(samples);
    // cout<<"PerformBootstrap end"<<endl;
}