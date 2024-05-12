#include "DataExtraction.h"

#include "curl/curl.h"
#include <map>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <stdio.h>
#include <string.h>
#include <string>
#include <sstream>
#include <fstream>
#include <locale>
#include <vector>
#include <mutex>


namespace fre {
    //Function to format the date in the form YYYY-MM-DD (2024-05-05)
	map<string, int> global_skippedSymbols;

    string formatDate(string Date)
    {
        map<string, string> months = { { "JAN", "01" },{ "FEB", "02" },{ "MAR", "03" },{ "APR", "04" }, { "MAY", "05" },{ "JUN", "06" },{ "JUL", "07" },{ "AUG", "08" },
		{ "SEP", "09" },{ "OCT", "10" },{ "NOV", "11" },{ "DEC", "12" } };
	
		string day = Date.substr(0, 2);
		string month = Date.substr(3, 3);
		for (int i = 0; i < 3; i++)
		{
			month[i] = toupper(month[i]);
		}
		month = months[month];
		string year = Date.substr(7, 2);
	
		string result = "20" + year + "-" + month + "-" + day;
		return result;
    }

	String SetDates(String datelist, map<string, Stocks>& stock_map, int N, string group)
	{
		String skipped_tickers;
		String tickers;
		Stocks *stock;
		std::string Start_date;
		std::string End_date;
		auto itr = stock_map.begin();
		for(;itr != stock_map.end(); itr++)
		{
			std::string ticker = (itr->first);
			stock = &(itr->second);
			auto itr2 = find(datelist.begin(), datelist.end(), stock->GetEarningsDate());
			int index = std::distance(datelist.begin(),itr2);
			if ((index + N) >= (int)datelist.size() || (index - N) < 0) {
				skipped_tickers.push_back(ticker);
				stock_map.erase(ticker); // skip the ticker
				continue;
			}
			else {
				tickers.push_back(ticker);
			}
			Start_date = datelist[index - N];
			End_date = datelist[index + N];
			stock->SetStartDate(Start_date);
			stock->SetEndDate(End_date);
		}
		if(skipped_tickers.size()) {       
			cout<<"\nWarning: No. of stocks in " << group << " whose window exceeds benchmark's window (needs to be extended): "<<skipped_tickers.size()<<endl;
			for (size_t i = 0; i < skipped_tickers.size(); i++) {
				cout << skipped_tickers[i] << "  ";
			}
			cout << endl;
		}
		return tickers;
	}
    
    //Loading the data from Russell Earnings csv file into a map containing ticker as key and Stock object as values
    void LoadEarningsData(map<string, Stocks> &stockData)
    {
        ifstream fin;
        
        //reading from csv file
		fin.open(CONFIG.SymbolsFile, ios::in);
		
		//declaring variables to store the data from csv file
		string line, ticker, earnings_date, period_ending, estimated_earnings, reported_earnings, surprise_earnings, surprise_percent;
		
		getline(fin, line);
		while (getline(fin, line))
		{
			stringstream sin(line);
			getline(sin, ticker, ',');
			getline(sin, earnings_date, ',');
			getline(sin, period_ending, ',');
			getline(sin, estimated_earnings, ',');
			getline(sin, reported_earnings, ',');
			getline(sin, surprise_earnings, ',');
			getline(sin, surprise_percent);
			
			//create a temporary stock object to store in the map
			Stocks stock;
			
			stock.SetTicker(ticker);
			stock.SetEarningsDate(formatDate(earnings_date));
			stock.SetEarningsPeriod(period_ending);
			stock.SetEstimatedEarnings(stod(estimated_earnings));
			stock.SetReportedEarnings(stod(reported_earnings));
			stock.SetSurpriseEarnings(stod(surprise_earnings));
			stock.SetSurprisePercent(stod(surprise_percent));
			
			//store the stock object in the map for Earnings Announcement Data
			stockData[ticker] = stock;
		}
    }
    
	//function to update the Stocks object with the data from EOD
    void updateStock(stringstream &sData, Stocks* stock)
	{
		String Date;
		Vector Open_price;
		Vector High_price;
		Vector Low_price;
		Vector Close_price;
		Vector Adjusted_close;
		Vector volume;
		
		string line, sDate, sOpen, sHigh, sLow, sClose, sAdjClose, sVolume;
		
		while (getline(sData, line)) {
			size_t found = line.find('-');
			if (found != std::string::npos)
			{
				//Date, Open, High, Low, Close, Adjusted_close, Volume
				stringstream sin(line);
				getline(sin, sDate, ',');
				getline(sin, sOpen, ',');
				getline(sin, sHigh, ',');
				getline(sin, sLow, ',');
				getline(sin, sClose, ',');
				getline(sin, sAdjClose, ',');
				getline(sin, sVolume);
				
				Date.push_back(sDate);
				Open_price.push_back(stod(sOpen));
				High_price.push_back(stod(sHigh));
				Low_price.push_back(stod(sLow));
				Close_price.push_back(stod(sClose));
				Adjusted_close.push_back(stod(sAdjClose));
				volume.push_back(stod(sVolume));
			}
		}

		stock->SetDate(Date);
		stock->SetOP(Open_price);
		stock->SetHP(High_price);
		stock->SetLP(Low_price);
		stock->SetCP(Close_price);
		stock->SetACP(Adjusted_close);
		stock->SetVol(volume);
		stock->CalculateReturns();
		stock->CalculateCumulativeReturns();
	
	}
	
	
	void* myrealloc(void* ptr, size_t size)
	{
		if (ptr)
			return realloc(ptr, size);
		else
			return malloc(size);
	}
	
	int write_data(void* ptr, size_t size, size_t nmemb, void* data)
	{
		size_t realsize = size * nmemb;
		struct MemoryStruct* mem = (struct MemoryStruct*)data;
		mem->memory = (char*)myrealloc(mem->memory, mem->size + realsize + 1);
		if (mem->memory) {
			memcpy(&(mem->memory[mem->size]), ptr, realsize);
			mem->size += realsize;
			mem->memory[mem->size] = 0;
		}
		return realsize;
	}

	// progress bar
    void printProgress(int cur_index, int last_index, map<string, Stocks>& stocks_in_group) {
		static mutex m;
		lock_guard<mutex> lock(m);

		// Calculate progress
		double progress = 100.0 * cur_index / last_index;

		// Create progress bar string
		int progress_int = static_cast<int>(progress);
		string progress_bar(progress_int, '*');
		progress_bar += string(100 - progress_int, ' ');

		// Print progress
		cout << fixed << showpoint << setprecision(3);
		cout << "\r\033[F" // Move cursor to the beginning of the line and up one line
		<< "progress: [" << progress << "%] [" << progress_bar << "]" << endl;
	}

    //Getting stock data from EODHistoricalData
	void FetchStockData(map<string, Stocks> &stockData, const String group_tickers, int N, string groupName)
	{
		//Declaring vector containing all the tickers
		String symbolList;
		
		int group_total_tickers = group_tickers.size();
		
		// declaration of an object CURL 
		CURL* handle;
		CURLcode result;
	
		// set up the program environment that libcurl needs 
		curl_global_init(CURL_GLOBAL_ALL);
		
		// curl_easy_init() returns a CURL easy handle 
		handle = curl_easy_init();
		
		// if everything's all right with the easy handle... 
		if (handle)
		{
			string url_common = CONFIG.url_common;
			string api_token = CONFIG.api_token;  // repalce API token
			
			for(int i = 0; i < group_total_tickers; i++) 
			{	
				if(group_tickers[i].length()==0)                          
				{
					continue;
				}
				
				printProgress(i, group_total_tickers, stockData);
				string start_date = stockData[group_tickers[i]].GetStartDate();
				string end_date = stockData[group_tickers[i]].GetEndDate();
				struct MemoryStruct data;
				data.memory = NULL;
				data.size = 0;
				
				string symbol = group_tickers[i];
				
				string url_request = url_common + symbol + ".US?" + "from=" + start_date + "&to=" + end_date + "&api_token=" + api_token + "&period=d";
				
				//set the url to call the data from
				curl_easy_setopt(handle, CURLOPT_URL, url_request.c_str());
		
				//adding a user agent
				curl_easy_setopt(handle, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:74.0) Gecko/20100101 Firefox/74.0");
				curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0);
				curl_easy_setopt(handle, CURLOPT_SSL_VERIFYHOST, 0);
				
				//create a temporary pointer used to access the stock object values corresponding to the ticker
				Stocks *ticker = &stockData[symbol];
				
				//store the data in the Stock class using write_data function
				curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data);
				curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void*)&data);
		
				result = curl_easy_perform(handle);	
				if (result != CURLE_OK)
				{
				    // Error Handling
					fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
					cout<<"\n\n\nError in reading data!\n\n\n";
				}
				
				stringstream sData;
				sData.str(data.memory);
				// Updating the stock object for the corresponding ticker
				updateStock(sData, ticker);
				// Remove the stock from map if it doesn't have 2N+1 days
				if (ticker->GetACP().size() != (unsigned)(2*N+1)) {
					global_skippedSymbols[ticker->GetTicker()] = ticker->GetACP().size();
					stockData.erase(symbol);  
            	}
				
				free(data.memory);
			}
			// cout << "The size of " << groupName << " that fetched: " << stockData.size() << endl;
			
		}	
		else
		{
			fprintf(stderr, "Curl init failed!\n");
			cout << "\n\n\n Curl initialization failed! \n\n\n";
		}
	
		// cleanup since you've used curl_easy_init  
		curl_easy_cleanup(handle);
	
		// release resources acquired by curl_global_init() 
		curl_global_cleanup();
	}
	
	void FetchIWVData(map<string, Stocks> &IWVData, String &datesList)
	{
		
		CURL* handle;
		CURLcode result;

		curl_global_init(CURL_GLOBAL_ALL);

		handle = curl_easy_init();

		// if everything's all right with the easy handle...
		if (handle)
		{
			string url_common = CONFIG.url_common;
			string start_date = CONFIG.start_date;
			string end_date = CONFIG.end_date;
			string api_token = CONFIG.api_token; 

			struct MemoryStruct data;
			data.memory = NULL;
			data.size = 0;

			string SYMBOL = CONFIG.benchmark;

			if (SYMBOL.back() == '\r') {
				SYMBOL.pop_back();
			}

			string url_request = url_common + SYMBOL + ".US?" + "from=" + start_date + "&to=" + end_date + "&api_token=" + api_token + "&period=d";
			curl_easy_setopt(handle, CURLOPT_URL, url_request.c_str());

			//adding a user agent
			curl_easy_setopt(handle, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:74.0) Gecko/20100101 Firefox/74.0");
			curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0);
			curl_easy_setopt(handle, CURLOPT_SSL_VERIFYHOST, 0);

			curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data);
			curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void*)&data);

			result = curl_easy_perform(handle);

			// check for errors
			if (result != CURLE_OK) {
				fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
				//return -1; Need to STOP EXECUTION HERE;
				return;
			}

			stringstream sData;
			sData.str(data.memory);
			string sValue, sDate;
			double dValue = 0;
			Vector Adjusted_close;
			string line;
			while (getline(sData, line))
			{
				size_t found = line.find('-');
				if (found != std::string::npos)
				{
					sDate = line.substr(0, line.find_first_of(','));
					line.erase(line.find_last_of(','));
					sValue = line.substr(line.find_last_of(',') + 1);
					dValue = strtod(sValue.c_str(), NULL);
					Adjusted_close.push_back(dValue);
					datesList.push_back(sDate);
				}
			}
			
			Stocks temp;
			
			temp.SetACP(Adjusted_close);
			temp.SetDate(datesList); 
			
			IWVData[SYMBOL] = temp;
			
			free(data.memory);
			data.size = 0;
		}
		else
		{
			fprintf(stderr, "Curl init failed!\n");
			return;
		}
		// cleanup since you've used curl_easy_init
		curl_easy_cleanup(handle);

		// release resources acquired by curl_global_init()
		curl_global_cleanup();

	}

}