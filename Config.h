#pragma once

#include <stdio.h>
#include <iostream>
using namespace std;
struct Config {
    const char* SymbolsFile = "Russell3000EarningsAnnouncements.csv"; 
    const string start_date = "2023-03-01";
    const string end_date = "2024-05-05";
    const string api_token = "6625812dd06ff6.56705439";
    const string url_common = "https://eodhistoricaldata.com/api/eod/";
    const string benchmark = "IWV";
};

static Config CONFIG;
