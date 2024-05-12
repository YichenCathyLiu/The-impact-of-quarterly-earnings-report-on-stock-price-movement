#pragma once
#include <vector>
#include <cmath>
#include <string>
#include <iostream>
#include <algorithm>
#include "DataExtraction.h"

using namespace std;
using namespace fre;


class StockClassifier {
    private:
        static bool CompareStocks(const pair<string, Stocks>& a, const pair<string, Stocks>& b);
        map<string, Stocks> Beat_third;
        map<string, Stocks> Meet_third;
        map<string, Stocks> Miss_third;
    public:
        StockClassifier() {}
        void ClassifyStocks();
        map<string, Stocks> GetBeat() { return Beat_third; }
        map<string, Stocks> GetMeet() { return Meet_third; }
        map<string, Stocks> GetMiss() { return Miss_third; }

};


bool StockClassifier::CompareStocks(const pair<string, Stocks>& a, const pair<string, Stocks>& b) {
    return a.second.GetSurprisePercent() > b.second.GetSurprisePercent();
}


void StockClassifier::ClassifyStocks()
{
    map<string, Stocks> stockData;
    map<string, Stocks>::iterator itr;
    cout<<"Loading Earnings Data...."<<endl;
    LoadEarningsData(stockData);

    cout<<"Sorting based on Surprise Percent...."<<endl;
    // sort the map based on the surprise percent
    vector<pair<string, Stocks>> vec(stockData.begin(), stockData.end());
    sort(vec.begin(), vec.end(), CompareStocks);

    int size = vec.size();
    int third = size / 3;
    Beat_third.clear();
    Meet_third.clear();
    Miss_third.clear();

    // insert the top third, middle third and bottom third into map
    Beat_third.insert(vec.begin(), vec.begin() + third);
    Meet_third.insert(vec.begin() + third, vec.begin() + 2 * third);
    Miss_third.insert(vec.begin() + 2 * third, vec.end());
    
}
