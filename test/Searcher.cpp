#include "Searcher.h"

#define DBHOST "tcp://192.168.8.189:3306"
#define USER "vkingDev3"
#define PASSWORD "pass"
#define DATABASE "vking_smartmodeltoolsDB"


Searcher::Searcher()
{
	ConnectDataBase();
}

Searcher::~Searcher()
{
}
//�������ݿ�
int Searcher::ConnectDataBase(){
	connector=new MySQLConnector();
	if (!connector || !connector->connectServer(DBHOST, USER, PASSWORD, DATABASE))
		return -1;
	return 0;
}

//�ҵ�����վ��
int Searcher::FindAllStation(MySQLConnector::aStringIntArray stationList){
	if (!connector->queryStationList(stationList))
	{
		cout << "Error: queryStationList" << endl;
		return -1;
	}
	#if 1
	for (auto iter = stationList.cbegin(); iter != stationList.cend(); iter++)
	{
		cout << "StationName: " << iter->first << endl;
	}
	#endif
	return stationList.size();
}

//�����豸���
int Searcher::FindCatalog(MySQLConnector::aStringIntArray catalogList,int catalogID){
	if (!connector->queryObjectCatalogList(catalogID, catalogList))
	{
		cout << "Error: queryObjectCatalog" << endl;
		return -1;
	}
	#if 1
	for (auto iter = catalogList.cbegin(); iter != catalogList.cend(); iter ++)
	{
		cout << "CatalogName: " << iter->first << endl;
	}
	#endif
	return catalogList.size();
}

//������������
int Searcher::FindProducer(MySQLConnector::aStringArray manufacturerNames,int stationID,int catalogID){
	if (!connector->queryManufacturerList(manufacturerNames, stationID, catalogID))
	{
		cout << "Error: queryManufacturer" << endl;
		return -1;
	}
	#if 1
	for (auto iter = manufacturerNames.cbegin(); iter != manufacturerNames.cend(); iter++)
	{
		cout << "Manufacturer: " << iter->data() << endl;
	}
	#endif
	return manufacturerNames.size();
}

//����ģ���ͺ�
int Searcher::FindModelNum(MySQLConnector::aStringIntArray modelNumList,int stationID,int catalogID,string producer){
	if (!connector->queryEquipmentModelNumList(modelNumList, stationID, catalogID, &producer))
	{
		cout << "Error: queryEquipmentModelNum" << endl;
		return -1;
	}
	#if 1
	for (auto iter = modelNumList.cbegin(); iter != modelNumList.cend(); iter++)
	{
		cout << "ModelNum: " << iter->first << endl;
	}
	#endif
	return modelNumList.size();
}

//����վ������
int Searcher::FindVoltageClass(MySQLConnector::aStringArray voltageClassNames,int stationID){
	if (!connector->queryRegionLists(voltageClassNames, stationID))
	{
		cout << "Error: queryVoltageClass" << endl;
		return -1;
	}
	#if 1
	for (auto iter = voltageClassNames.cbegin(); iter != voltageClassNames.cend(); iter++)
	{
		cout << "VoltageClass: " << iter->data() << endl;
	}
	#endif
	return voltageClassNames.size();
}

//��ѯ̨��
int Searcher::FindLedger(MySQLConnector::aLedgerInfoArray ledgerInfoArr,int stationID,int catalogID,string keywords,int equipmentID,string voltageClass){
	if (!connector->queryLedgerInfo(ledgerInfoArr, stationID, catalogID, keywords, equipmentID, &voltageClass))
	{
		cout << "Error: queryLedgerInfo" << endl;
		return -1;
	}
	#if 1
	for (auto iter = ledgerInfoArr.cbegin(); iter != ledgerInfoArr.cend(); iter++)
	{
		cout << "Ledger EquimentName: " << iter->getValue(LedgerInfo::e_Name) << endl;
		cout << "Ledger EquimentNum: " << iter->getValue(LedgerInfo::e_EquipmentNum) << endl;
		cout << "Ledger EquimentType: " << iter->getValue(LedgerInfo::e_EquipmentType) << endl;
		cout << "Ledger EquimentProducer: " << iter->getValue(LedgerInfo::e_EquipmentProducer) << endl;
		cout << "Ledger EquimentModelNum: " << iter->getValue(LedgerInfo::e_EquipmentModelNum) << endl;
		cout << "Ledger VoltageClass: " << iter->getValue(LedgerInfo::e_VoltageClass) << endl;
		cout << "Ledger OperateNum: " << iter->getValue(LedgerInfo::e_OperateNum) << endl;
		cout << "Ledger Remarks: " << iter->getValue(LedgerInfo::e_Remarks) << endl;
	}
	#endif
	return ledgerInfoArr.size();
}



