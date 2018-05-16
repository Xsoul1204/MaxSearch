#include <iostream>
#include <map>
#include <memory>

#include "cppconn/statement.h"
#include "cppconn/prepared_statement.h"
#include "cppconn/metadata.h"
#include "cppconn/exception.h"


#include "MySQLConnector.h"
#include "3dsmaxport.h"

using namespace std;
using namespace sql;

class Searcher
{
public:
	Searcher();
	~Searcher();
	MySQLConnector* connector;
	int ConnectDataBase();
	MySQLConnector::aStringIntArray FindAllStation();
	MySQLConnector::aStringIntArray FindCatalog(int catalogID);
	string FindCode(int LedgerID);
	MySQLConnector::aStringArray FindProducer(int stationID,int catalogID);
	MySQLConnector::aStringIntArray FindModelNum(int stationID,int catalogID,string producer);
	MySQLConnector::aStringArray FindRegion(int stationID);
	MySQLConnector::aLedgerInfoArray FindLedger(int stationID,int catalogID,string keywords,int equipmentID,string* region);
	bool FindLedgerLink(int ledgerID);
	bool attachLedgerLink(int ledgerID);
	bool canelLedgerLink(int ledgerID);
	static void putElements(HWND hWnd,MySQLConnector::aStringIntArray stringIntArray);
	static void putElements(HWND hWnd,MySQLConnector::aStringArray stringArray);
	static Searcher* GetInstance() { 
		static Searcher theSearcher;
		return &theSearcher; 
	}
private:

};