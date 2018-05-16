#include <iostream>
#include <map>
#include <memory>

#include "cppconn/statement.h"
#include "cppconn/prepared_statement.h"
#include "cppconn/metadata.h"
#include "cppconn/exception.h"


#include "MySQLConnector.h"

using namespace std;
using namespace sql;

class Searcher
{
public:
	Searcher();
	~Searcher();
	MySQLConnector* connector;
	int ConnectDataBase();
	int FindAllStation(MySQLConnector::aStringIntArray stationList);
	int FindCatalog(MySQLConnector::aStringIntArray catalogList,int catalogID);
	int FindProducer(MySQLConnector::aStringArray manufacturerNames,int stationID,int catalogID);
	int FindModelNum(MySQLConnector::aStringIntArray modelNumList,int stationID,int catalogID,string producer);
	int FindVoltageClass(MySQLConnector::aStringArray voltageClassNames,int stationID);
	int FindLedger(MySQLConnector::aLedgerInfoArray ledgerInfoArr,int stationID,int catalogID,string keywords,int equipmentID,string voltageClass);
private:

};