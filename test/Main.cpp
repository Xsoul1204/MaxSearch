#include <iostream>
#include "Searcher.h"
#include "MySQLConnector.h"
using namespace std;

int main(int argc, char *argv[])
{
	Searcher* searcher=new Searcher;
	MySQLConnector::aStringIntArray stationList;
	searcher->FindAllStation(stationList);
	for (auto iter = stationList.cbegin(); iter != stationList.cend(); iter++)
	{
		cout << "StationName: " << iter->first << endl;
	}
}