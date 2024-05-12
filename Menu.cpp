#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <map>
#include <string>
#include <chrono>
#include <cmath>
#include <sstream>
#include <thread>
#include <algorithm>

#include "Group.h"
#include "Bootstrap.h"
#include "Gnuplot.h"

using namespace std;
using namespace std::chrono;
using namespace fre;

template <typename T>
void displayStatistics(const string& title, T data) {
    cout << title << endl;
    for (int i = 0; i < (int)data.size(); i++) {
        cout << setw(11) << setfill(' ') << fixed << setprecision(4) << data[i] << " ";
        if ((i + 1) % 10 == 0)
            cout << endl;
    }
    cout << endl << endl;
}

void Display(Stocks s, const map<string, Stocks>& IWV_map, string group)
{
    std::cout << "Ticker: " << s.GetTicker()<<endl;
	std::cout << "Earnings Announcement Date: " << s.GetEarningsDate()<<endl;
	std::cout << "Earning Estimate: $" << s.GetEstimatedEarnings()<<endl;
	std::cout << "Reported Earnings per share: $" << s.GetReportedEarnings()<<endl;
	std::cout << "Earnings Surprise: $" << s.GetSurpriseEarnings()<<endl;
	std::cout << "Earnings surprise percent: " << s.GetSurprisePercent() << "%"<<endl;
    std::cout << "Group: " << group << endl;
    std::cout << "Number of dates: " << s.GetDate().size() << endl;
    cout << endl;

    String dates = s.GetDate();
    Vector prices = s.GetACP();
    Vector Daily_return = cal_DailyRet(prices);
    Vector Cumm_return = cal_CumulativeRet(prices);

    displayStatistics<String> ("Stock Daily Prices Dates:", dates);
    displayStatistics<Vector> ("Stock Daily Prices:", prices);
    displayStatistics<Vector> ("Stock Daily Returns:", Daily_return);
    displayStatistics<Vector> ("Stock Cumulative Returns:", Cumm_return);
    
    map<string, double> stock_prices = s.GetPrices();
    map<string, double> IWV_prices = IWV_map.at("IWV").GetPrices();

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

    displayStatistics<Vector> ("Stock Abnormal Returns:", abnormal_returns);
}

void task2 (map<string, Stocks> x_third, map<string, Stocks> IWVData, string group, string t) {
    Display(x_third[t], IWVData, group);
    std::cout << std::endl;
}

void task3 (map<string, Stocks> groupData, map<string, Stocks> & IWVData, int N, int num_samples, string groupName){
    cout << groupName << " Group Bootstrap construct" << endl;
    Bootstrap bootstrap(groupData, IWVData, N, num_samples);
    cout << groupName << " Group Bootstrap start" << endl;
    bootstrap.PerformBootstrap();

    vector<double> avg_aar = bootstrap.GetAvgAAR();
    vector<double> avg_caar = bootstrap.GetAvgCAAR();
    vector<double> std_aar = bootstrap.GetStdAAR();
    vector<double> std_caar = bootstrap.GetStdCAAR();

    cout << "\nSummary: " << groupName << " Group:" << endl;
    displayStatistics<Vector> ("Average AAR:", avg_aar);
    displayStatistics<Vector> ("Standard Deviation AAR:", std_aar);
    displayStatistics<Vector> ("Average CAAR:", avg_caar);
    displayStatistics<Vector> ("Standard Deviation CAAR:", std_caar);
}

Matrix task4 (map<string, Stocks> & x_third, map<string, Stocks> & IWVData, int N, int num_samples){
    Bootstrap bootstrap_x(x_third, IWVData, N, num_samples);
    bootstrap_x.PerformBootstrap();
    Vector avg_caar_x = bootstrap_x.GetAvgCAAR();
    Vector avg_aar_x = bootstrap_x.GetAvgAAR();
    Vector std_caar_x = bootstrap_x.GetStdCAAR();
    Vector std_aar_x = bootstrap_x.GetStdAAR();
    vector<Vector> res; 
    res.push_back(avg_aar_x);
    res.push_back(std_aar_x);
    res.push_back(avg_caar_x);
    res.push_back(std_caar_x);
    return res; 
}

int main() 
{
    string val; //Menu input value to select 1 among 5 options
    int N = 0; //Number of days
    int NLowerBound = 60, NUpperBound = 90;
    int integer_val = 0;
    int integer_grp = 0;
    string t, group;
    int g = 0;
    int num_samples = 40;
    String Dates;
    map<string, Stocks> IWVData;
    map<string, Stocks> Beat;
    map<string, Stocks> Meet;
    map<string, Stocks> Miss;

    while (true) 
    {
        cout << endl;
        cout << "\nMenu:" << endl;
        cout << "1. Enter N to retrieve 2N+1 historical price data for all stocks. " << endl;
        cout << "2. Pull information for one stock from one group. " << endl;
        cout << "3. Show AAR, AAR-STD, CAAR and CAAR-STD for one group. " << endl;
        cout << "4. Show gnuplot plot for CAAR of all 3 groups. " << endl;
        cout << "5. Exit program." << endl;
        cout << "Please choose one: ";
        cin >> val;

        // Enter N to retrieve 2N+1 days of historical price data for all stocks
        if(val=="1")
        {
            while(true)
            {
                cout << "Enter N value between " << NLowerBound << " and " << NUpperBound << ": " ;
                cin >> N;
                integer_val = (int) N;
                if(integer_val!= N) 
                {
                    cout << "Error, only integer values are allowed, please enter an integer.";
                }
                else 
                {
                    if (N >= NLowerBound && N <= NUpperBound && !cin.fail())
                    {
                        cout << "Data is loading, please allow around 2 mins to load!" << endl;
    
                        FetchIWVData(IWVData, Dates);
                        // StockGroup(Beat, Meet, Miss, global_skipped_tickers);
                        StockClassifier classifier;
                        classifier.ClassifyStocks();
                        Beat = classifier.GetBeat();
                        Meet = classifier.GetMeet();
                        Miss = classifier.GetMiss();
                        string group_beat = "Beat Group";
                        String beat_tickers = SetDates(Dates, Beat, N, group_beat); //Set end and start dates & return group tickers
                        string group_meet = "Meet Group";
                        String meet_tickers = SetDates(Dates, Meet, N, group_meet); //Set end and start dates & return group tickers
                        string group_miss = "Miss Group";
                        String miss_tickers = SetDates(Dates, Miss, N, group_miss); //Set end and start dates & return group tickers
                        
                        // Fetch data
                        auto start = high_resolution_clock::now();
                        thread t1(FetchStockData,std::ref(Beat), beat_tickers, N, group_beat);
                        thread t2(FetchStockData,std::ref(Meet), meet_tickers, N, group_meet);
                        thread t3(FetchStockData,std::ref(Miss), miss_tickers, N, group_miss);
                        t1.join();
                        t2.join();
                        t3.join();
                        auto stop = high_resolution_clock::now();
                        auto duration = duration_cast<seconds>(stop - start);
                        cout << endl;
                        cout << "\nFetched data in seconds: " << duration.count()<<endl;

                        if (fre::global_skippedSymbols.size()) {
                            cout << "Skipped " << fre::global_skippedSymbols.size() << " stocks which don't have 2N+1 data: " << endl;
                            for (map<string, int>::iterator itr = global_skippedSymbols.begin(); itr != global_skippedSymbols.end(); itr++) {
                                cout << itr->first << ": " << itr->second << " days data" << endl;
                            }
                        }
                        cout << "The size of Beat group that fetched: " << Beat.size() << endl;
                        cout << "The size of Meet group that fetched: " << Meet.size() << endl;
                        cout << "The size of Miss group that fetched: " << Miss.size() << endl;
                        
                        break;
                    }
                    else
                    {
                        cout << "Error, please enter an integer value between 60 and 100 " << endl;
                        N = 0;
                        cin.clear();
                        break;
                    }
                } 
            }

        }

        // Pull information for one stock from one group
        else if(val == "2")
        {
            if(N >= NLowerBound && N <= NUpperBound)
            {
                while(true)
                {
                    cout << "Please provide ticker of a stock: ";
                    cin >> t;
                    cout << endl;
                    if(Beat.find(t) != Beat.end())
                    {
                        task2(Beat, IWVData, "Beat", t);
                        break;
                    }
                    else if (Meet.find(t) != Meet.end())
                    {
                        task2(Meet, IWVData, "Meet", t);
                        break;
                    }
                    else if (Miss.find(t) != Miss.end())
                    {
                        task2(Miss, IWVData, "Miss", t);
                        break;
                    }
                    else
                        cout <<"Error, The ticker is not in our stock pool. Please enter other tickers." << endl;
                }
            }
            else
            {
                cout << "Data is not retrived yet, Retrieve data from Option 1." << endl;
            }
        }

        // Show AAR, AAR-STD, CAAR and CAAR-STD for one group
        else if(val == "3")
        {
            if(N >= NLowerBound && N <= NUpperBound)
            {
                while(true)
                {
                    cout <<endl <<  "1. Beat \t 2. Meet \t 3. Miss \t 4. Menu"<< endl;
                    cout << "Enter appropriate group No.: ";
                    cin >> g;
                    integer_grp = (int) g;
                    if(integer_grp != g || g<1 || g>4)
                    {
                        cout<< "Error, Please select valid output, 1 2 3 4" << endl;
                        cin.clear();
                    }
                    else
                    {
                        if (g == 1)
                            task3(Beat, IWVData, N, num_samples, "Beat");
                        else if (g == 2)
                            task3(Meet, IWVData, N, num_samples, "Meet");
                        else if (g == 3)
                            task3(Miss, IWVData, N, num_samples, "Miss");
                        else if (g == 4)
                            break;
                    }
                }
            }
            else
                cout << "Data is not retrived yet, Retrieve data from Option 1." << endl;
        }

        // Show the gnuplot graph with CAAR for all 3 groups
        else if(val == "4")
        {
            if(N >= NLowerBound && N <= NUpperBound)
            {
                Matrix Beat_martix = task4(Beat, IWVData, N, num_samples);
                Matrix Meet_martix = task4(Meet, IWVData, N, num_samples);
                Matrix Miss_martix = task4(Miss, IWVData, N, num_samples);
                // The expected AAR, AAR STD, and expected CAAR and CAAR STD for 3 groups are present d in a matrix. The row of the matrix is the group#, and matrix columns are for AAR, AAR-STD, CAAR, CAAR-STD
                MatrixList BootstrapResult = {Beat_martix, Meet_martix, Miss_martix};
                plot Plotter(Beat_martix[2], Meet_martix[2], Miss_martix[2], "CAAR");
                Plotter.plotResult();
            }   
            else
                cout << "Data is not retrived yet, Retrieve data from Option 1." << endl;
        }

        // Exit your program
        else if(val == "5")
            exit(0);
        else
        {
            cout << "Invalid menu input, Select 1-5. " << endl;
            cin.clear();
        }
    }
    return 0;
}
