#pragma once
#include <map>
#include <vector>
#include <string>
#include "Stocks.h"
#include "Config.h"
namespace fre {
    
    struct MemoryStruct 
    {
    	char* memory;
    	size_t size;
    };
    
    string formatDate(string Date);
    
    void LoadEarningsData(map<string, Stocks> &stockData);
    
    int write_data(void* ptr, size_t size, size_t nmemb, void* data);
        
    void FetchStockData(map<string, Stocks> &stockData, String tickers, int N, string groupName);
    
    void FetchIWVData(map<string, Stocks> &IWVData, String &datesList); 

    // String SetDates(String datelist, map<string, Stocks>& stock_map, int N, string group);
    String SetDates(String datelist, map<string, Stocks>& stock_map, int N, string group);

    extern map<string, int> global_skippedSymbols;
}
