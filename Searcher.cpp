#include "Searcher.h"
#include <stdlib.h>
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
MySQLConnector::aStringIntArray Searcher::FindAllStation(){
	MySQLConnector::aStringIntArray stationList;
	if (!connector->queryStationList(stationList))
	{
		cout << "Error: queryStationList" << endl;
	}
	#if 1
	for (auto iter = stationList.cbegin(); iter != stationList.cend(); iter++)
	{
		cout << "StationName: " << iter->first << endl;
	}
	#endif
	return stationList;
}

//�����豸���
MySQLConnector::aStringIntArray Searcher::FindCatalog(int catalogID){
	MySQLConnector::aStringIntArray catalogList;
	if (!connector->queryObjectCatalogList(catalogID, catalogList))
	{
		cout << "Error: queryObjectCatalog" << endl;
	}
	#if 1
	for (auto iter = catalogList.cbegin(); iter != catalogList.cend(); iter ++)
	{
		cout << "CatalogName: " << iter->first << endl;
	}
	#endif
	return catalogList;
}
//�����豸ǰ׺
string Searcher::FindCode(int LedgerID){
	string code;
	if (!connector->queryObjectCode(LedgerID, code))
	{
		cout << "Error: queryObjectCode" << endl;
	}
	#if 1
		cout << "Code: " << code << endl;
	#endif
	return code;
}

//������������
MySQLConnector::aStringArray Searcher::FindProducer(int stationID,int catalogID){
	MySQLConnector::aStringArray manufacturerNames;
	if (!connector->queryManufacturerList(manufacturerNames, stationID, catalogID))
	{
		cout << "Error: queryManufacturer" << endl;
	}
	#if 1
	for (auto iter = manufacturerNames.cbegin(); iter != manufacturerNames.cend(); iter++)
	{
		cout << "Manufacturer: " << iter->data() << endl;
	}
	#endif
	return manufacturerNames;
}

//����ģ���ͺ�
MySQLConnector::aStringIntArray Searcher::FindModelNum(int stationID,int catalogID,string producer){
	MySQLConnector::aStringIntArray modelNumList;
	if (!connector->queryEquipmentModelNumList(modelNumList, stationID, catalogID, &producer))
	{
		cout << "Error: queryEquipmentModelNum" << endl;
	}
	#if 1
	for (auto iter = modelNumList.cbegin(); iter != modelNumList.cend(); iter++)
	{
		cout << "ModelNum: " << iter->first << endl;
	}
	#endif
	return modelNumList;
}

//����վ������
MySQLConnector::aStringArray Searcher::FindRegion(int stationID){
	MySQLConnector::aStringArray regionNames;
	if (!connector->queryRegionLists(regionNames, stationID))
	{
		cout << "Error: queryRegion" << endl;
	}
	#if 1
	for (auto iter = regionNames.cbegin(); iter != regionNames.cend(); iter++)
	{
		cout << "Region: " << iter->data() << endl;
	}
	#endif
	return regionNames;
}

//��ѯ̨��
MySQLConnector::aLedgerInfoArray Searcher::FindLedger(int stationID,int catalogID,string keywords,int equipmentID,string* region){
	MySQLConnector::aLedgerInfoArray ledgerInfoArr;
	if (!connector->queryLedgerInfo(ledgerInfoArr, stationID, catalogID, keywords, equipmentID, region))
	{
		cout << "Error: queryLedgerInfo" << endl;
	}
	#if 1
	for (auto iter = ledgerInfoArr.cbegin(); iter != ledgerInfoArr.cend(); iter++)
	{
		cout << "Ledger EquimentName: " << iter->getValue(LedgerInfo::e_Finished) << endl;
		cout << "Ledger EquimentName: " << iter->getValue(LedgerInfo::e_Name) << endl;
		cout << "Ledger EquimentNum: " << iter->getValue(LedgerInfo::e_EquipmentNum) << endl;
		cout << "Ledger EquimentType: " << iter->getValue(LedgerInfo::e_EquipmentType) << endl;
		cout << "Ledger EquimentProducer: " << iter->getValue(LedgerInfo::e_EquipmentProducer) << endl;
		cout << "Ledger EquimentModelNum: " << iter->getValue(LedgerInfo::e_EquipmentModelNum) << endl;
		cout << "Ledger Region: " << iter->getValue(LedgerInfo::e_Region) << endl;
		cout << "Ledger OperateNum: " << iter->getValue(LedgerInfo::e_OperateNum) << endl;
		cout << "Ledger Remarks: " << iter->getValue(LedgerInfo::e_Remarks) << endl;
	}
	#endif
	return ledgerInfoArr;
}


//����̨��ƥ��
bool Searcher::attachLedgerLink(int ledgerId){
	return connector->updateLedgerSchedule(ledgerId,1);
}
//ȡ��̨��ƥ��
bool Searcher::canelLedgerLink(int ledgerId){
	return connector->updateLedgerSchedule(ledgerId,0);
}
//��ѯ̨��ƥ��
bool Searcher:: FindLedgerLink(int ledgerID){
	int finished;
	if (!connector->queryLedgerSchedule(finished,ledgerID))
	{
		cout << "Error: queryLedgerSchedule" << endl;
	}
	if(finished==1){
		return true;
	}else if(finished==0){
		return false;
	}else{
		return NULL;
	}

}

//��Combo Box�з���String IntԪ��
void Searcher::putElements(HWND hWnd,MySQLConnector::aStringIntArray stringIntArray){
	SendMessage(hWnd, CB_RESETCONTENT, 0, 0);
	for (auto iter = stringIntArray.cbegin(); iter != stringIntArray.cend(); iter++)
	{
		int idx =SendMessageA(hWnd, CB_ADDSTRING, 0, (LPARAM)(iter->first.c_str()));
		SendMessageA(hWnd, CB_SETITEMDATA, idx, (LPARAM)(iter->second));
	}
	if(SendMessageA(hWnd, CB_GETCOUNT, 0, 0)>0){
		SendMessageA(hWnd,CB_SETCURSEL,0,0);
	}
}
//��Combo Box�з���StringԪ��
void Searcher::putElements(HWND hWnd,MySQLConnector::aStringArray stringArray){
	SendMessage(hWnd, CB_RESETCONTENT, 0, 0);
	for (auto iter = stringArray.cbegin(); iter != stringArray.cend(); iter++)
	{
		SendMessageA(hWnd , CB_ADDSTRING, 0, (LPARAM)(iter->c_str()));
	}
	if(SendMessageA(hWnd, CB_GETCOUNT, 0, 0)>0){
		SendMessageA(hWnd,CB_SETCURSEL,0,0);
	}
}

