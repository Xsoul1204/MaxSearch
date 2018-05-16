#include <iostream>
#include <map>
#include <memory>

#include "cppconn/statement.h"
#include "cppconn/prepared_statement.h"
#include "cppconn/metadata.h"
#include "cppconn/exception.h"

#include "MySQLConnector.h"

#define DBHOST "tcp://192.168.8.189:3306"
#define USER "vkingDev3"
#define PASSWORD "pass"
#define DATABASE "vking_smartmodeltoolsDB"

using namespace std;
using namespace sql;

int main(int argc, char *argv[])
{
	std::unique_ptr<MySQLConnector> connector(new MySQLConnector());
	if (!connector || !connector->connectServer(DBHOST, USER, PASSWORD, DATABASE))
		return -1;
	
	MySQLConnector::aStringIntArray stationList;
	if (!connector->queryStationList(stationList))
	{
		cout << "Error: queryStationList" << endl;
		return -1;
	}
	for (auto iter = stationList.cbegin(); iter != stationList.cend(); iter++)
	{
		cout << "StationName: " << iter->first << endl;
	}
	int stationID = stationList.at(1).second;

	// Catalog
	int catalogLevel = 0;
	int catalogID = 0;
	vector<int> catalogHierachy;
	MySQLConnector::aStringIntArray catalogList;
	do 
	{
		if (!connector->queryObjectCatalogList(catalogID, catalogList))
		{
			cout << "Error: queryObjectCatalog" << endl;
			return -1;
		}
		for (auto iter = catalogList.cbegin(); iter != catalogList.cend(); iter ++)
		{
			cout << "CatalogName: " << iter->first << endl;
		}
		cout << "CatalogID: " << catalogID << endl;
		
		// Set Catalog String
		if(catalogList.empty())
			break;
		catalogID = catalogList.at(0).second;
		catalogHierachy.push_back(catalogID);
	} while (!catalogList.empty() && (catalogLevel++ < 2));

	catalogID = 8;

	// Producer
	MySQLConnector::aStringArray manufacturerNames;
	if (!connector->queryManufacturerList(manufacturerNames, stationID, catalogID))
	{
		cout << "Error: queryManufacturer" << endl;
		return -1;
	}
	for (auto iter = manufacturerNames.cbegin(); iter != manufacturerNames.cend(); iter++)
	{
		cout << "Manufacturer: " << iter->data() << endl;
	}

	// ModelNum
	if (manufacturerNames.empty())
		return 0;
	string producer = manufacturerNames.at(0);
	MySQLConnector::aStringIntArray modelNumList;
	if (!connector->queryEquipmentModelNumList(modelNumList, stationID, catalogID, &producer))
	{
		cout << "Error: queryEquipmentModelNum" << endl;
		return -1;
	}
	for (auto iter = modelNumList.cbegin(); iter != modelNumList.cend(); iter++)
	{
		cout << "ModelNum: " << iter->first << endl;
	}

	// Region
	MySQLConnector::aStringArray regionNames;
	if (!connector->queryRegionLists(regionNames, stationID))
	{
		cout << "Error: queryRegion" << endl;
		return -1;
	}
	for (auto iter = regionNames.cbegin(); iter != regionNames.cend(); iter++)
	{
		cout << "Region: " << iter->data() << endl;
	}

	// Ledger
	string keywords("");
	if (modelNumList.empty())
		return 0;
	int equipmentID = modelNumList.at(0).second;
	if (regionNames.empty())
		return 0;
	string region = regionNames.at(2);
	MySQLConnector::aLedgerInfoArray ledgerInfoArr;
	if (!connector->queryLedgerInfo(ledgerInfoArr, stationID, catalogID, keywords, equipmentID, &region))
	{
		cout << "Error: queryLedgerInfo" << endl;
		return -1;
	}
	for (auto iter = ledgerInfoArr.cbegin(); iter != ledgerInfoArr.cend(); iter++)
	{
		cout << "Ledger EquimentName: " << iter->getValue(LedgerInfo::e_Name) << endl;
		cout << "Ledger EquimentNum: " << iter->getValue(LedgerInfo::e_EquipmentNum) << endl;
		cout << "Ledger EquimentType: " << iter->getValue(LedgerInfo::e_EquipmentType) << endl;
		cout << "Ledger EquimentProducer: " << iter->getValue(LedgerInfo::e_EquipmentProducer) << endl;
		cout << "Ledger EquimentModelNum: " << iter->getValue(LedgerInfo::e_EquipmentModelNum) << endl;
		cout << "Ledger Region: " << iter->getValue(LedgerInfo::e_Region) << endl;
		cout << "Ledger OperateNum: " << iter->getValue(LedgerInfo::e_OperateNum) << endl;
		cout << "Ledger Remarks: " << iter->getValue(LedgerInfo::e_Remarks) << endl;
	}

	// Update ledger schedule
	if (ledgerInfoArr.empty())
		return 0;
	const LedgerInfo& ledgerInfo = ledgerInfoArr.at(0);
	int ledgerID = ledgerInfo.getID();
	if (!connector->updateLedgerSchedule(ledgerID))
	{
		cout << "Error: updateLedgerSchedule" << endl;
		return -1;
	}

	equipmentID = -1;
	if (!connector->queryEquipmentFromLedger(equipmentID, ledgerID))
	{
		cout << "Error: queryEquipmentFromLedger" << endl;
		return -1;
	}

	// update ModelEquipmentLink
	int modelID = 1;
	if (!connector->updateModelEquipmentLink(modelID, equipmentID))
	{
		cout << "Error: updateModelEquipmentLink" << endl;
		return -1;
	}

	return 0;
}