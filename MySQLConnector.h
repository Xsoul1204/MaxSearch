#pragma once

#include "mysql_driver.h"
#include "mysql_connection.h"
#include "cppconn/driver.h"

#include "LedgerInfo.h"

#include <string>
#include <vector>

class MySQLConnector
{
public:
	MySQLConnector();
	~MySQLConnector();
	bool connectServer(const std::string& url, const std::string& user, const std::string& password, const std::string& database);
	typedef std::vector<std::pair<std::string,int>> aStringIntArray;
	bool queryStationList(aStringIntArray& stationList) const;
	bool queryObjectCatalogList(int& catalogID, aStringIntArray& catalogList) const;
	typedef std::vector<std::string> aStringArray;
	bool queryManufacturerList(aStringArray& manufacturerNames, int stationID, int catalogID = -1) const;
	bool queryEquipmentModelNumList(aStringIntArray& modelNumList, int stationID, int catalogID = -1, std::string* producer = nullptr) const;
	bool queryRegionLists(aStringArray& regionList, int stationID) const;
	bool queryObjectCode(int& LedgerID, std::string& code) const;
	typedef std::vector<LedgerInfo> aLedgerInfoArray;
	bool queryLedgerInfo(
		aLedgerInfoArray& ledgerInfoArr,
		int stationID,
		int catalogID,
		const std::string& keywords,
		int equipmentID = -1,
		std::string* region = nullptr) const;

	bool updateLedgerSchedule(int ledgerID,int Finished) const;
	bool queryLedgerSchedule(int& finished,int ledgerID) const;
	bool queryEquipmentFromLedger(int& equipmentID, int ledgerID) const;
	bool updateModelEquipmentLink(int modelID, int equipmentID);

private:
	void GetDBMetaData(sql::Connection *dbcon) const;

private:
	sql::Driver*		m_driver;
	sql::Connection*	m_con;
};
