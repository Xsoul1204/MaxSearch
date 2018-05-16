#include "MySQLConnector.h"
#include "cppconn/metadata.h"
#include "cppconn/statement.h"
#include "cppconn/prepared_statement.h"

#include <windows.h>
#include <time.h>

using namespace std;
using namespace sql;

MySQLConnector::MySQLConnector()
	: m_driver(nullptr)
	, m_con(nullptr)
{
	//mysql::MySQL_Driver *driver = sql::mysql::get_mysql_driver_instance();
	m_driver = get_driver_instance();
}

MySQLConnector::~MySQLConnector()
{
	cout << "Cleaning up the resources .." << endl;

	/* Clean up */
	if (m_con)
	{
		m_con->close();
		delete m_con;
	}
}

void MySQLConnector::GetDBMetaData(Connection *dbcon) const
{
	if (dbcon->isClosed())
	{
		throw runtime_error("DatabaseMetaData FAILURE - database connection closed");
	}

	cout << "/nDatabase Metadata" << endl;
	cout << "-----------------" << endl;

	cout << boolalpha;

	/* The following commented statement won't work with Connector/C++ 1.0.5 and later */
	//auto_ptr < DatabaseMetaData > dbcon_meta (dbcon->getMetaData());  

	DatabaseMetaData *dbcon_meta = dbcon->getMetaData();

	cout << "Database Product Name: " << dbcon_meta->getDatabaseProductName() << endl;
	cout << "Database Product Version: " << dbcon_meta->getDatabaseProductVersion() << endl;
	cout << "Database User Name: " << dbcon_meta->getUserName() << endl << endl;

	cout << "Driver name: " << dbcon_meta->getDriverName() << endl;
	cout << "Driver version: " << dbcon_meta->getDriverVersion() << endl << endl;

	cout << "Database in Read-Only Mode?: " << dbcon_meta->isReadOnly() << endl;
	cout << "Supports Transactions?: " << dbcon_meta->supportsTransactions() << endl;
	cout << "Supports DML Transactions only?: " << dbcon_meta->supportsDataManipulationTransactionsOnly() << endl;
	cout << "Supports Batch Updates?: " << dbcon_meta->supportsBatchUpdates() << endl;
	cout << "Supports Outer Joins?: " << dbcon_meta->supportsOuterJoins() << endl;
	cout << "Supports Multiple Transactions?: " << dbcon_meta->supportsMultipleTransactions() << endl;
	cout << "Supports Named Parameters?: " << dbcon_meta->supportsNamedParameters() << endl;
	cout << "Supports Statement Pooling?: " << dbcon_meta->supportsStatementPooling() << endl;
	cout << "Supports Stored Procedures?: " << dbcon_meta->supportsStoredProcedures() << endl;
	cout << "Supports Union?: " << dbcon_meta->supportsUnion() << endl << endl;

	cout << "Maximum Connections: " << dbcon_meta->getMaxConnections() << endl;
	cout << "Maximum Columns per Table: " << dbcon_meta->getMaxColumnsInTable() << endl;
	cout << "Maximum Columns per Index: " << dbcon_meta->getMaxColumnsInIndex() << endl;
	cout << "Maximum Row Size per Table: " << dbcon_meta->getMaxRowSize() << " bytes" << endl;

	cout << "/nDatabase schemas: " << endl;

	auto_ptr < ResultSet > rs(dbcon_meta->getSchemas());

	cout << "/nTotal number of schemas = " << rs->rowsCount() << endl;
	cout << endl;

	int row = 1;

	while (rs->next()) {
		cout << "/t" << row << ". " << rs->getString("TABLE_SCHEM") << endl;
		++row;
	} // while  

	cout << endl << endl;
}

bool MySQLConnector::connectServer(const string& url, const string& user, const string& password, const string& database)
{
	if (!m_driver)
		return false;

	try
	{
		/* create a database connection using the Driver */
		m_con = m_driver->connect(url, user, password);

		/* alternate syntax using auto_ptr to create the db connection */
		//auto_ptr  con (driver -> connect(url, user, password));  

		/* turn off the autocommit */
		m_con->setAutoCommit(0);

		cout << "/nDatabase connection/'s autocommit mode = " << m_con->getAutoCommit() << endl;

		/* select appropriate database schema */
		m_con->setSchema(database);

		/* retrieve and display the database metadata */
		GetDBMetaData(m_con);
	}
	catch (SQLException &e) {
		cout << "ERROR: " << e.what();
		cout << " (MySQL error code: " << e.getErrorCode();
		cout << ", SQLState: " << e.getSQLState() << ")" << endl;

		if (e.getErrorCode() == 1047) {
			/*
			Error: 1047 SQLSTATE: 08S01 (ER_UNKNOWN_COM_ERROR)
			Message: Unknown command
			*/
			cout << "/nYour server does not seem to support Prepared Statements at all. ";
			cout << "Perhaps MYSQL < 4.1?" << endl;
		}

		return false;
	}
	catch (std::runtime_error &e) {
		cout << "ERROR: " << e.what() << endl;

		return false;
	}
	return true;
}

string UTF8_To_string(const string & str)
{
	int nwLen = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);

	wchar_t * pwBuf = new wchar_t[nwLen + 1];
	memset(pwBuf, 0, nwLen * 2 + 2);

	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.length(), pwBuf, nwLen);

	int nLen = WideCharToMultiByte(CP_ACP, 0, pwBuf, -1, NULL, NULL, NULL, NULL);

	char * pBuf = new char[nLen + 1];
	memset(pBuf, 0, nLen + 1);

	WideCharToMultiByte(CP_ACP, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

	string retStr = pBuf;

	delete[]pBuf;
	delete[]pwBuf;

	pBuf = NULL;
	pwBuf = NULL;

	return retStr;
}

string string_To_UTF8(const string & str)
{
	int nwLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);

	wchar_t * pwBuf = new wchar_t[nwLen + 1];
	ZeroMemory(pwBuf, nwLen * 2 + 2);

	::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), pwBuf, nwLen);

	int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);

	char * pBuf = new char[nLen + 1];
	ZeroMemory(pBuf, nLen + 1);

	::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

	string retStr(pBuf);

	delete[]pwBuf;
	delete[]pBuf;

	pwBuf = NULL;
	pBuf = NULL;

	return retStr;
}

bool MySQLConnector::queryStationList(aStringIntArray& stationList) const
{
	if (!m_con || !m_con->isValid())
		return false;

	try
	{
		boost::scoped_ptr< Statement > stmt(m_con->createStatement());
		if (!stmt)
			return false;

		boost::scoped_ptr<ResultSet> res(stmt->executeQuery("SELECT * FROM StationTable"));
		if (!res)
			return false;

		stationList.clear();
		while (res->next())
		{
			const string& resultName = UTF8_To_string(res->getString("Name"));
			int resultID = res->getInt("StationID");
			stationList.push_back(std::make_pair(resultName, resultID));
		}
		res->close();
	}
	catch (SQLException &e) {
		cout << "ERROR: " << e.what();
		cout << " (MySQL error code: " << e.getErrorCode();
		cout << ", SQLState: " << e.getSQLState() << ")" << endl;

		if (e.getErrorCode() == 1047) {
			/*
			Error: 1047 SQLSTATE: 08S01 (ER_UNKNOWN_COM_ERROR)
			Message: Unknown command
			*/
			cout << "/nYour server does not seem to support Prepared Statements at all. ";
			cout << "Perhaps MYSQL < 4.1?" << endl;
		}

		return false;
	}
	catch (sql::InvalidArgumentException &) {
		cout << "#\t\t fetch error." << endl;

		return false;
	}
	catch (runtime_error &e) {
		cout << "ERROR: " << e.what() << endl;

		return false;
	}

	return true;
}

bool MySQLConnector::queryObjectCatalogList(int& catalogID, aStringIntArray& catalogList) const
{
	if (!m_con || !m_con->isValid())
		return false;
	if (catalogID < 0)
		return false;

	try
	{
		boost::scoped_ptr< PreparedStatement > prep_stmt(m_con->prepareStatement("SELECT * FROM ObjectCatalogMenu WHERE Parent=?"));
		if (!prep_stmt)
			return false;
		prep_stmt->setInt(1, catalogID);

		boost::scoped_ptr<ResultSet> res(prep_stmt->executeQuery());
		if (!res)
			return false;

		/*cout << "/nRetrieved " << res->rowsCount() << " row(s)." << endl;
		ResultSetMetaData *res_meta = res->getMetaData();
		int numcols = res_meta->getColumnCount();
		cout << "/nNumber of columns in the result set = " << numcols << endl << endl;*/

		catalogList.clear();
		while (res->next())
		{
			const string& resultName = UTF8_To_string(res->getString("Name"));
			int resultID = res->getInt("CatalogID");
			catalogList.push_back(std::make_pair(resultName, resultID));
		}
		res->close();
	}
	catch (SQLException &e) {
		cout << "ERROR: " << e.what();
		cout << " (MySQL error code: " << e.getErrorCode();
		cout << ", SQLState: " << e.getSQLState() << ")" << endl;

		if (e.getErrorCode() == 1047) {
			/*
			Error: 1047 SQLSTATE: 08S01 (ER_UNKNOWN_COM_ERROR)
			Message: Unknown command
			*/
			cout << "/nYour server does not seem to support Prepared Statements at all. ";
			cout << "Perhaps MYSQL < 4.1?" << endl;
		}

		return false;
	}
	catch (sql::InvalidArgumentException &) {
		cout << "#\t\t fetch error." << endl;

		return false;
	}
	catch (runtime_error &e) {
		cout << "ERROR: " << e.what() << endl;

		return false;
	}

	return true;
}

bool MySQLConnector::queryManufacturerList(aStringArray& manufacturerNames, int stationID, int catalogID /*= -1*/) const
{
	if (!m_con || !m_con->isValid())
		return false;
	if (stationID <= 0)
		return false;

	try
	{
		boost::scoped_ptr< PreparedStatement > prep_stmt;
		if (catalogID > 0)
		{
			prep_stmt.reset(m_con->prepareStatement(
				"SELECT DISTINCT Producer FROM EquipmentTable WHERE EquipmentID IN (SELECT DISTINCT EquipmentInfo FROM LedgerTable WHERE Station=?) AND (Catalog=? OR Catalog IN (SELECT CatalogID FROM ObjectCatalogMenu WHERE Parent=?))"));
			prep_stmt->setInt(2, catalogID);
			prep_stmt->setInt(3, catalogID);
		}
		else
		{
			prep_stmt.reset(m_con->prepareStatement(
				"SELECT DISTINCT Producer FROM EquipmentTable WHERE EquipmentID IN (SELECT DISTINCT EquipmentInfo FROM LedgerTable WHERE Station=?)"));
		}
		prep_stmt->setInt(1, stationID);

		boost::scoped_ptr<ResultSet> res(prep_stmt->executeQuery());
		if (!res)
			return false;

		manufacturerNames.clear();
		while (res->next())
		{
			const string& resultName = UTF8_To_string(res->getString("Producer"));
			manufacturerNames.push_back(resultName);
		}
		res->close();
	}
	catch (SQLException &e) {
		cout << "ERROR: " << e.what();
		cout << " (MySQL error code: " << e.getErrorCode();
		cout << ", SQLState: " << e.getSQLState() << ")" << endl;

		if (e.getErrorCode() == 1047) {
			/*
			Error: 1047 SQLSTATE: 08S01 (ER_UNKNOWN_COM_ERROR)
			Message: Unknown command
			*/
			cout << "/nYour server does not seem to support Prepared Statements at all. ";
			cout << "Perhaps MYSQL < 4.1?" << endl;
		}

		return false;
	}
	catch (sql::InvalidArgumentException &) {
		cout << "#\t\t fetch error." << endl;

		return false;
	}
	catch (runtime_error &e) {
		cout << "ERROR: " << e.what() << endl;

		return false;
	}
	return true;
}

bool MySQLConnector::queryEquipmentModelNumList(aStringIntArray& modelNumList, int stationID, int catalogID /*= -1*/, std::string* producer /*= nullptr*/) const
{
	if (!m_con || !m_con->isValid())
		return false;
	if (stationID <= 0)
		return false;

	try
	{
		boost::scoped_ptr< PreparedStatement > prep_stmt;
		if (catalogID > 0)
		{
			if (producer)
			{
				prep_stmt.reset(m_con->prepareStatement(
					"SELECT * FROM EquipmentTable WHERE EquipmentID IN (SELECT DISTINCT EquipmentInfo FROM LedgerTable WHERE Station=?) AND (Catalog=? OR Catalog IN (SELECT CatalogID FROM ObjectCatalogMenu WHERE Parent=?)) AND Producer=?"));
				prep_stmt->setString(4, string_To_UTF8(*producer));
			}
			else
			{
				prep_stmt.reset(m_con->prepareStatement(
					"SELECT * FROM EquipmentTable WHERE EquipmentID IN (SELECT DISTINCT EquipmentInfo FROM LedgerTable WHERE Station=?) AND (Catalog=? OR Catalog IN (SELECT CatalogID FROM ObjectCatalogMenu WHERE Parent=?))"));
			}
			prep_stmt->setInt(2, catalogID);
			prep_stmt->setInt(3, catalogID);
		}
		else
		{
			if (producer)
			{
				prep_stmt.reset(m_con->prepareStatement(
					"SELECT * FROM EquipmentTable WHERE EquipmentID IN (SELECT DISTINCT EquipmentInfo FROM LedgerTable WHERE Station=?) AND Producer=?"));
				prep_stmt->setString(2, string_To_UTF8(*producer));
			}
			else
			{
				prep_stmt.reset(m_con->prepareStatement(
					"SELECT * FROM EquipmentTable WHERE EquipmentID IN (SELECT DISTINCT EquipmentInfo FROM LedgerTable WHERE Station=?)"));
			}
		}
		prep_stmt->setInt(1, stationID);

		/* run a query which returns exactly one result set */
		boost::scoped_ptr<ResultSet> res(prep_stmt->executeQuery());
		if (!res)
			return false;

		modelNumList.clear();
		while (res->next())
		{
			const string& resultName = UTF8_To_string(res->getString("ModelNum"));
			int resultID = res->getInt("EquipmentID");
			modelNumList.push_back(std::make_pair(resultName, resultID));
		}
		res->close();
	}
	catch (SQLException &e) {
		cout << "ERROR: " << e.what();
		cout << " (MySQL error code: " << e.getErrorCode();
		cout << ", SQLState: " << e.getSQLState() << ")" << endl;

		if (e.getErrorCode() == 1047) {
			/*
			Error: 1047 SQLSTATE: 08S01 (ER_UNKNOWN_COM_ERROR)
			Message: Unknown command
			*/
			cout << "/nYour server does not seem to support Prepared Statements at all. ";
			cout << "Perhaps MYSQL < 4.1?" << endl;
		}

		return false;
	}
	catch (sql::InvalidArgumentException &) {
		cout << "#\t\t fetch error." << endl;
		return false;
	}
	catch (runtime_error &e) {
		cout << "ERROR: " << e.what() << endl;

		return false;
	}
	return true;
}

bool MySQLConnector::queryRegionLists(aStringArray& regionList, int stationID) const
{
	if (!m_con || !m_con->isValid())
		return false;
	if (stationID <= 0)
		return false;

	try
	{
		boost::scoped_ptr< PreparedStatement > prep_stmt(m_con->prepareStatement(
			"SELECT DISTINCT Region FROM LedgerTable WHERE Station=?"));
		prep_stmt->setInt(1, stationID);

		boost::scoped_ptr<ResultSet> res(prep_stmt->executeQuery());
		if (!res)
			return false;

		regionList.clear();
		while (res->next())
		{
			const string& resultName = UTF8_To_string(res->getString("VoltageClass"));
			regionList.push_back(resultName);
		}
		res->close();
	}
	catch (SQLException &e) {
		cout << "ERROR: " << e.what();
		cout << " (MySQL error code: " << e.getErrorCode();
		cout << ", SQLState: " << e.getSQLState() << ")" << endl;

		if (e.getErrorCode() == 1047) {
			/*
			Error: 1047 SQLSTATE: 08S01 (ER_UNKNOWN_COM_ERROR)
			Message: Unknown command
			*/
			cout << "/nYour server does not seem to support Prepared Statements at all. ";
			cout << "Perhaps MYSQL < 4.1?" << endl;
		}

		return false;
	}
	catch (sql::InvalidArgumentException &) {
		cout << "#\t\t fetch error." << endl;

		return false;
	}
	catch (runtime_error &e) {
		cout << "ERROR: " << e.what() << endl;

		return false;
	}
	return true;
}

bool MySQLConnector::queryLedgerInfo(
	aLedgerInfoArray& ledgerInfoArr,
	int stationID,
	int catalogID,
	const string& keywords,
	int equipmentID /*= -1*/,
	string* region /*= nullptr*/) const
{
	if (!m_con || !m_con->isValid())
		return false;
	if (stationID <= 0 || catalogID <= 0)
		return false;

	try
	{
		int paramCnt = 3;
		boost::scoped_ptr< PreparedStatement > prep_stmt;
		if (region && equipmentID > 0)
		{
			prep_stmt.reset(m_con->prepareStatement(
				"SELECT * FROM LedgerTable WHERE Station=? AND EquipmentInfo IN (SELECT EquipmentID FROM EquipmentTable WHERE EquipmentID=?) AND Region=? AND (EquipmentType LIKE ? OR Name LIKE ? OR Remarks LIKE ? OR Unit LIKE ?)"));
			prep_stmt->setInt(2, equipmentID);
			prep_stmt->setString(3, string_To_UTF8(*region));
			paramCnt = 4;
		}
		else if (equipmentID > 0)
		{
			prep_stmt.reset(m_con->prepareStatement(
				"SELECT * FROM LedgerTable WHERE Station=? AND EquipmentInfo IN (SELECT EquipmentID FROM EquipmentTable WHERE EquipmentID=?) AND (EquipmentType LIKE ? OR Name LIKE ? OR Remarks LIKE ? OR Unit LIKE ?)"));
			prep_stmt->setInt(2, equipmentID);
			paramCnt = 3;
		}
		else if (region)
		{
			prep_stmt.reset(m_con->prepareStatement(
				"SELECT * FROM LedgerTable WHERE Station=? AND EquipmentInfo IN (SELECT EquipmentID FROM EquipmentTable WHERE (Catalog=? OR Catalog IN (SELECT CatalogID FROM ObjectCatalogMenu WHERE Parent=?))) AND Region=? AND (EquipmentType LIKE ? OR Name LIKE ? OR Remarks LIKE ? OR Unit LIKE ?)"));
			prep_stmt->setInt(2, catalogID);
			prep_stmt->setInt(3, catalogID);
			prep_stmt->setString(4, string_To_UTF8(*region));
			paramCnt = 5;
		}
		else
		{
			prep_stmt.reset(m_con->prepareStatement(
				"SELECT * FROM LedgerTable WHERE Station=? AND EquipmentInfo IN (SELECT EquipmentID FROM EquipmentTable WHERE (Catalog=? OR Catalog IN (SELECT CatalogID FROM ObjectCatalogMenu WHERE Parent=?))) AND (EquipmentType LIKE ? OR Name LIKE ? OR Remarks LIKE ? OR Unit LIKE ?)"));
			prep_stmt->setInt(2, catalogID);
			prep_stmt->setInt(3, catalogID);
			paramCnt = 4;
		}
		prep_stmt->setInt(1, stationID);
		string remarks = "%" + string_To_UTF8(keywords) + "%";
		prep_stmt->setString(paramCnt ++, remarks);
		prep_stmt->setString(paramCnt ++, remarks);
		prep_stmt->setString(paramCnt ++, remarks);
		prep_stmt->setString(paramCnt ++, remarks);

		boost::scoped_ptr<ResultSet> res(prep_stmt->executeQuery());
		if (!res)
			return false;

		ledgerInfoArr.clear();
		while (res->next())
		{
			LedgerInfo ledgerInfo;
			ledgerInfo.setID(res->getInt("LedgerID"));
			ledgerInfo.setValue(LedgerInfo::e_Name, UTF8_To_string(res->getString("Name")));
			ledgerInfo.setValue(LedgerInfo::e_EquipmentNum, UTF8_To_string(res->getString("EquipmentNum")));
			ledgerInfo.setValue(LedgerInfo::e_EquipmentType, UTF8_To_string(res->getString("EquipmentType")));
			ledgerInfo.setValue(LedgerInfo::e_VoltageClass, UTF8_To_string(res->getString("VoltageClass")));
			ledgerInfo.setValue(LedgerInfo::e_OperateNum, UTF8_To_string(res->getString("OperateNum")));
			ledgerInfo.setValue(LedgerInfo::e_Remarks, UTF8_To_string(res->getString("Remarks")));
			int equipmentID = res->getInt("EquipmentInfo");
			if (equipmentID > 0)
			{
				prep_stmt->clearParameters();
				prep_stmt.reset(m_con->prepareStatement(
					"SELECT * FROM EquipmentTable WHERE EquipmentID=?"));
				prep_stmt->setInt(1, equipmentID);

				boost::scoped_ptr<ResultSet> res_equipment(prep_stmt->executeQuery());
				if (res_equipment)
				{
					res_equipment->next();
					if (res_equipment->rowsCount() > 0)
					{
						ledgerInfo.setValue(LedgerInfo::e_EquipmentModelNum, UTF8_To_string(res_equipment->getString("ModelNum")));
						ledgerInfo.setValue(LedgerInfo::e_EquipmentProducer, UTF8_To_string(res_equipment->getString("Producer")));
					}
					res_equipment->close();
				}
			}
			ledgerInfoArr.push_back(ledgerInfo);
		}
		res->close();
	}
	catch (sql::InvalidArgumentException &) {
		cout << "#\t\t fetch error." << endl;
	}
	catch (SQLException &e) {
		cout << "ERROR: " << e.what();
		cout << " (MySQL error code: " << e.getErrorCode();
		cout << ", SQLState: " << e.getSQLState() << ")" << endl;

		if (e.getErrorCode() == 1047) {
			/*
			Error: 1047 SQLSTATE: 08S01 (ER_UNKNOWN_COM_ERROR)
			Message: Unknown command
			*/
			cout << "/nYour server does not seem to support Prepared Statements at all. ";
			cout << "Perhaps MYSQL < 4.1?" << endl;
		}

		return false;
	}
	catch (runtime_error &e) {
		cout << "ERROR: " << e.what() << endl;

		return false;
	}

	return true;
}

string GetCurrentTimeStamp()
{
	time_t now;
	struct tm timeinfo;
	time(&now);
	localtime_s(&timeinfo, &now);
	char year[5], mon[3], mday[3], hour[3], minute[3], sec[3];
	sprintf_s(year, 5, "%d", timeinfo.tm_year + 1900);
	sprintf_s(mon, 3, "%d", timeinfo.tm_mon + 1);
	sprintf_s(mday, 3, "%d", timeinfo.tm_mday);
	sprintf_s(hour, 3, "%d", timeinfo.tm_hour);
	sprintf_s(minute, 3, "%d", timeinfo.tm_min);
	sprintf_s(sec, 3, "%d", timeinfo.tm_sec);

	std::string yearStr = std::string(year);
	std::string monStr = std::string(mon);
	if (monStr.size() == 1) monStr = "0" + monStr;
	std::string mdayStr = std::string(mday);
	if (mdayStr.size() == 1) mdayStr = "0" + mdayStr;
	std::string hourStr = std::string(hour);
	if (hourStr.size() == 1) hourStr = "0" + hourStr;
	std::string minuteStr = std::string(minute);
	if (minuteStr.size() == 1) minuteStr = "0" + minuteStr;
	std::string secStr = std::string(sec);
	if (secStr.size() == 1) secStr = "0" + secStr;
	return yearStr + "-" + monStr + "-" + mdayStr + " " + \
		hourStr + ":" + minuteStr + ":" + secStr;
}

bool MySQLConnector::updateLedgerSchedule(int ledgerID)
{
	if (!m_con || !m_con->isValid())
		return false;
	if (ledgerID <= 0)
		return false;

	int updatecount = 0;
	boost::scoped_ptr< Savepoint > savept(m_con->setSavepoint("SAVEPT1"));

	try
	{
		boost::scoped_ptr< PreparedStatement > prep_stmt(m_con->prepareStatement(
			"REPLACE INTO LedgerSchedule (Ledger,Finished,FinishTime) VALUES (?,1,?)"));
		prep_stmt->setInt(1, ledgerID);
		prep_stmt->setDateTime(2, GetCurrentTimeStamp());
		int updatecount = prep_stmt->executeUpdate();
		if (updatecount > 0)
		{
			m_con->commit();
			return true;
		}
	}
	catch (SQLException &e) {
		cout << "ERROR: " << e.what();
		cout << " (MySQL error code: " << e.getErrorCode();
		cout << ", SQLState: " << e.getSQLState() << ")" << endl;

		cout << "/tRolling back until the last save point \"SAVEPT1 \" .." << endl;
		m_con->rollback(savept.get());
		m_con->releaseSavepoint(savept.get());

		return false;
	}
	catch (runtime_error &e) {
		cout << "ERROR: " << e.what() << endl;

		return false;
	}
	return false;
}

bool MySQLConnector::queryEquipmentFromLedger(int& equipmentID, int ledgerID) const
{
	if (!m_con || !m_con->isValid())
		return false;
	if (ledgerID <= 0)
		return false;

	try	{
		boost::scoped_ptr< PreparedStatement > prep_stmt(m_con->prepareStatement(
			"SELECT EquipmentInfo FROM LedgerTable WHERE LedgerID=?"));
		prep_stmt->setInt(1, ledgerID);

		boost::scoped_ptr<ResultSet> res( prep_stmt->executeQuery() );
		if (!res)
			return false;
		res->next();
		if (res->rowsCount() > 0)
			equipmentID = res->getInt("EquipmentInfo");
		res->close();
	}
	catch (SQLException &e) {
		cout << "ERROR: " << e.what();
		cout << " (MySQL error code: " << e.getErrorCode();
		cout << ", SQLState: " << e.getSQLState() << ")" << endl;

		if (e.getErrorCode() == 1047) {
			/*
			Error: 1047 SQLSTATE: 08S01 (ER_UNKNOWN_COM_ERROR)
			Message: Unknown command
			*/
			cout << "/nYour server does not seem to support Prepared Statements at all. ";
			cout << "Perhaps MYSQL < 4.1?" << endl;
		}

		return false;
	}
	catch (sql::InvalidArgumentException &) {
		cout << "#\t\t fetch error." << endl;

		return false;
	}
	catch (runtime_error &e) {
		cout << "ERROR: " << e.what() << endl;

		return false;
	}
	
	return true;
}

bool MySQLConnector::updateModelEquipmentLink(int modelID, int equipmentID)
{
	if (!m_con || !m_con->isValid())
		return false;
	if (equipmentID <= 0 || modelID <= 0)
		return false;

	try
	{
		boost::scoped_ptr< PreparedStatement > prep_stmt(m_con->prepareStatement(
			"INSERT INTO ModelEquipmentLink (Equipment,ModelObject) SELECT ?,? FROM DUAL WHERE NOT EXISTS (SELECT * FROM ModelEquipmentLink WHERE Equipment=? AND ModelObject=?)"));
		prep_stmt->setInt(1, equipmentID);
		prep_stmt->setInt(2, modelID);
		prep_stmt->setInt(3, equipmentID);
		prep_stmt->setInt(4, modelID);
		int updatecount = prep_stmt->executeUpdate();
		if (updatecount > 0)
		{
			m_con->commit();
			return true;
		}
	}
	catch (SQLException &e) {
		cout << "ERROR: " << e.what();
		cout << " (MySQL error code: " << e.getErrorCode();
		cout << ", SQLState: " << e.getSQLState() << ")" << endl;

		return false;
	}
	catch (runtime_error &e) {
		cout << "ERROR: " << e.what() << endl;

		return false;
	}
	return false;
}