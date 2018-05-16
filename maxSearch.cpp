//**************************************************************************/
// Copyright (c) 1998-2007 Autodesk, Inc.
// All rights reserved.
// 
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information written by Autodesk, Inc., and are
// protected by Federal copyright law. They may not be disclosed to third
// parties or copied or duplicated in any form, in whole or in part, without
// the prior written consent of Autodesk, Inc.
//**************************************************************************/
// DESCRIPTION: Appwizard generated plugin
// AUTHOR: 
//***************************************************************************/

#include "maxSearch.h"
#include "3dsmaxport.h"
#include "Searcher.h"
#include "MySQLConnector.h"
#include <sstream>
#include <windows.h>
#include <string>
#include <thread>
using namespace std;
#define maxSearch_CLASS_ID	Class_ID(0x3eaf7f07, 0x927ccfe8)

class SearchDialog
{
public:
	HWND hDialog;
	// The core interface instance.
	Interface&  mInterface;	
	SearchDialog();
	~SearchDialog();

	// Main dialog resize functions
	void ResizeWindow(int x, int y);
	void SetMinDialogSize(int w, int h)	{ minDlgWidth = w; minDlgHeight = h; }
	int	 GetMinDialogWidth()			{ return minDlgWidth; }
	int  GetMinDialogHeight()			{ return minDlgHeight; }

	void SearchDialog::DoDialog();
	void setStationID(int stationID);
	void setHWND();
	void updateCatalog();
	void updateProducer();
	void putCatalog();
	void putCombo();
	void search();
	void match();
	void cancel();
	void changeSelect();
	void showMessage(string meaasge);
	void updateCombo();
	int getCatalogID();
	void putEmpty(HWND hWnd);
	void initResult();
	static wchar_t* char2wchar(const char* char_temp)
	{
		int nLen = MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, char_temp, -1, NULL, 0 );
		if (nLen == 0)
		{
			return NULL;
		}
		wchar_t* pResult = new wchar_t[nLen];
		MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, char_temp, -1, pResult, nLen );
		return pResult;
	}
	static void push(int rInt){
		push(to_string(rInt));
	}
	static void push(string rString){
		push(rString.c_str());
	}
	static void push(char* rChar){
		GetCOREInterface()->PopPrompt();
		GetCOREInterface()->PushPrompt(char2wchar(rChar));
	}
private:
	static INT_PTR CALLBACK searchDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static INT_PTR CALLBACK messageDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	int	minDlgWidth;
	int	minDlgHeight;
	int	stationID;
	HWND catalogCombo1;
	HWND catalogCombo2;
	HWND catalogCombo3;
	HWND producerCombo;
	HWND modelNumCombo;
	HWND regionCombo;
	HWND searchEdit;
	HWND resultList;
	HWND selected;
	HWND prefixEdit;
	HWND progress;
	MySQLConnector::aStringIntArray catalogList1;
	MySQLConnector::aStringIntArray catalogList2;
	MySQLConnector::aStringIntArray catalogList3;
	MySQLConnector::aStringArray manufacturerNames;
	MySQLConnector::aStringIntArray modelNumList;
	MySQLConnector::aStringArray regionNames;
	MySQLConnector::aLedgerInfoArray ledgerInfoArr;
	int catalogParent1;
	int catalogParent2;
	int catalogID;
	string producer;
	int itemCount;
	vector<int> selectList;
	bool startSearch;
	bool noChange;
};

class maxSearch : public UtilityObj 
{
public:

	//Constructor/Destructor
	maxSearch();
	virtual ~maxSearch();

	virtual void DeleteThis() { }
	
	virtual void BeginEditParams(Interface *ip,IUtil *iu);
	virtual void EndEditParams(Interface *ip,IUtil *iu);

	virtual void Init(HWND hWnd);
	virtual void Destroy(HWND hWnd);
	virtual void DoDialog(HWND hWnd);

	// Singleton access
	static maxSearch* GetInstance() { 
		static maxSearch themaxSearch;
		return &themaxSearch; 
	}
private:

	static INT_PTR CALLBACK DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	HWND   hPanel;
	IUtil* iu;
};


class maxSearchClassDesc : public ClassDesc2 
{
public:
	virtual int IsPublic() 							{ return TRUE; }
	virtual void* Create(BOOL /*loading = FALSE*/) 	{ return maxSearch::GetInstance(); }
	virtual const TCHAR *	ClassName() 			{ return GetString(IDS_CLASS_NAME); }
	virtual SClass_ID SuperClassID() 				{ return UTILITY_CLASS_ID; }
	virtual Class_ID ClassID() 						{ return maxSearch_CLASS_ID; }
	virtual const TCHAR* Category() 				{ return GetString(IDS_CATEGORY); }

	virtual const TCHAR* InternalName() 			{ return _T("maxSearch"); }	// returns fixed parsable name (scripter-visible name)
	virtual HINSTANCE HInstance() 					{ return hInstance; }					// returns owning module handle
	

};


ClassDesc2* GetmaxSearchDesc() { 
	static maxSearchClassDesc maxSearchDesc;
	return &maxSearchDesc; 
}




//--- maxSearch -------------------------------------------------------
maxSearch::maxSearch()
	: hPanel(nullptr)
	, iu(nullptr)
{
	
}

maxSearch::~maxSearch()
{

}

void maxSearch::BeginEditParams(Interface* ip,IUtil* iu) 
{
	this->iu = iu;
	hPanel = ip->AddRollupPage(
		hInstance,
		MAKEINTRESOURCE(IDD_PANEL),
		DlgProc,
		GetString(IDS_PARAMS),
		0);
}
	
void maxSearch::EndEditParams(Interface* ip,IUtil*)
{
	this->iu = nullptr;
	ip->DeleteRollupPage(hPanel);
	hPanel = nullptr;
}

void maxSearch::Init(HWND hWnd)
{
	SetWindowTextA(GetDlgItem(hWnd,IDC_STATION_STATIC),"站所：");
	SetWindowTextA(GetDlgItem(hWnd,IDC_START),"打开！");
	SetWindowTextA(GetDlgItem(hWnd,IDC_CLOSE),"关闭");
	MySQLConnector::aStringIntArray stationList=Searcher::GetInstance()->FindAllStation();
	Searcher::putElements(GetDlgItem(hWnd,IDC_STATION_COMBO),stationList);
}

void maxSearch::Destroy(HWND /*handle*/)
{

}

INT_PTR CALLBACK maxSearch::DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

	switch (msg) 
	{
		case WM_INITDIALOG:
			maxSearch::GetInstance()->Init(hWnd);
			break;

		case WM_DESTROY:
			maxSearch::GetInstance()->Destroy(hWnd);
			break;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
			case IDC_START:
				maxSearch::GetInstance()->DoDialog(hWnd);
				break;
			case IDC_CLOSE:
				maxSearch::GetInstance()->iu->CloseUtility();
				break;
			default:
				break;
			}
			break;

		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MOUSEMOVE:
			GetCOREInterface()->RollupMouseMessage(hWnd,msg,wParam,lParam);
			break;

		default:
			return 0;
	}
	return 1;
}

void maxSearch::DoDialog(HWND hWnd){
	int idx = SendMessageA(GetDlgItem(hWnd, IDC_STATION_COMBO), CB_GETCURSEL, 0,0);
	int stationId = SendMessageA(GetDlgItem(hWnd, IDC_STATION_COMBO), CB_GETITEMDATA, idx, 0);
	SearchDialog* searchDialog =new SearchDialog;
	searchDialog->setStationID(stationId);
	searchDialog->DoDialog();
}

SearchDialog::SearchDialog():
	mInterface(*GetCOREInterface()),
	hDialog(NULL)
	{
}

SearchDialog::~SearchDialog()
{
	if(hDialog){
		DestroyWindow(hDialog);
	}
}
void SearchDialog::showMessage(string message){
	DialogBoxParam(
		hInstance,
		MAKEINTRESOURCE(IDD_MESSAGE_DIALOG),
		hDialog,
		messageDlgProc,LPARAM(message.c_str()));
}
void SearchDialog::setStationID(int stationID){
	this->stationID=stationID;
}
void SearchDialog::putEmpty(HWND hWnd){
	int idx =SendMessageA(hWnd, CB_ADDSTRING, 0, (LPARAM)(""));
	SendMessageA(hWnd, CB_SETITEMDATA, idx, (LPARAM)(-1));
	if(SendMessageA(hWnd, CB_GETCOUNT, 0, 0)>0){
		SendMessageA(hWnd,CB_SETCURSEL,0,0);
	}
}
int SearchDialog::getCatalogID(){
	if(SendMessageA(catalogCombo3, CB_GETCOUNT, 0, 0)>0){
		int idx3 = SendMessageA(catalogCombo3,CB_GETCURSEL,0,0);
		int cID=SendMessageA(catalogCombo3, CB_GETITEMDATA, idx3, 0);
		if (cID==-1)
		{
			int idx2 = SendMessageA(catalogCombo2,CB_GETCURSEL,0,0);
			return SendMessageA(catalogCombo2, CB_GETITEMDATA, idx2, 0);
		}else{
			return cID;
		}
	}else {
		return -1;
	}
}
void SearchDialog::updateCatalog()
{
		int idx1 = SendMessageA(catalogCombo1,CB_GETCURSEL,0,0);
		int data1 = SendMessageA(catalogCombo1, CB_GETITEMDATA, idx1, 0);
		if(data1!=catalogParent1){
			catalogList2=Searcher::GetInstance()->FindCatalog(data1);
			Searcher::GetInstance()->putElements(catalogCombo2,catalogList2);
			putEmpty(catalogCombo2);
			if(SendMessageA(catalogCombo2, CB_GETCOUNT, 0, 0)>0){
				int idx2 = SendMessageA(catalogCombo2, CB_GETCURSEL, 0, 0);
				catalogParent2 = SendMessageA(catalogCombo2, CB_GETITEMDATA, idx2, 0);
				catalogList3=Searcher::GetInstance()->FindCatalog(catalogParent2);
				Searcher::GetInstance()->putElements(catalogCombo3,catalogList3);
				putEmpty(catalogCombo3);
			}else if(SendMessageA(catalogCombo2, CB_GETCOUNT, 0, 0)==0){
				catalogParent2=-1;
			}
			catalogParent1=data1;
		}else{
			int idx2 = SendMessageA(catalogCombo2,CB_GETCURSEL,0,0);
			int data2 = SendMessageA(catalogCombo2, CB_GETITEMDATA, idx2, 0);
			if(data2!=catalogParent2){
				catalogList3=Searcher::GetInstance()->FindCatalog(data2);
				Searcher::GetInstance()->putElements(catalogCombo3,catalogList3);
				putEmpty(catalogCombo3);
				catalogParent2=data2;
			}
		}
}
void SearchDialog::updateProducer(){
	manufacturerNames=Searcher::GetInstance()->FindProducer(stationID,catalogID);
	Searcher::GetInstance()->putElements(producerCombo,manufacturerNames);
	int idx = SendMessageA(producerCombo,CB_GETCURSEL,0,0);
	char szMessage[1024];
	SendMessageA(producerCombo, CB_GETLBTEXT, idx, (LPARAM)szMessage);
	producer = szMessage;
	modelNumList=Searcher::GetInstance()->FindModelNum(stationID,catalogID,producer);
	Searcher::GetInstance()->putElements(modelNumCombo,modelNumList);
}
void SearchDialog::updateCombo(){
	updateCatalog();
	int nowCataLogID=getCatalogID();
	if(nowCataLogID!=catalogID){
		catalogID=nowCataLogID;
		updateProducer();
	}else{
		int idx = SendMessageA(producerCombo,CB_GETCURSEL,0,0);
		char szMessage[1024];
		SendMessageA(producerCombo, CB_GETLBTEXT, idx, (LPARAM)szMessage);
		string nowProducer(szMessage);
		if(nowProducer!=producer){
			producer=nowProducer;
			modelNumList=Searcher::GetInstance()->FindModelNum(stationID,catalogID,producer);
			Searcher::GetInstance()->putElements(modelNumCombo,modelNumList);
		}
	}

}
void SearchDialog::putCatalog(){
	catalogList1=Searcher::GetInstance()->FindCatalog(0);
	Searcher::GetInstance()->putElements(catalogCombo1,catalogList1);
	int idx1 = SendMessageA(catalogCombo1,CB_GETCURSEL,0,0);
	putEmpty(catalogCombo1);
	catalogParent1 = SendMessageA(catalogCombo1, CB_GETITEMDATA, idx1, 0);
	catalogList2=Searcher::GetInstance()->FindCatalog(catalogParent1);
	Searcher::GetInstance()->putElements(catalogCombo2,catalogList2);
	putEmpty(catalogCombo2);
	int idx2 = SendMessageA(catalogCombo2,CB_GETCURSEL,0,0);
	catalogParent2 = SendMessageA(catalogCombo2, CB_GETITEMDATA, idx2, 0);
	catalogList3=Searcher::GetInstance()->FindCatalog(catalogParent2);
	Searcher::GetInstance()->putElements(catalogCombo3,catalogList3);
	putEmpty(catalogCombo3);
	catalogID = getCatalogID();
}
void SearchDialog::putCombo(){
	putCatalog();
	manufacturerNames=Searcher::GetInstance()->FindProducer(stationID,catalogID);
	Searcher::GetInstance()->putElements(producerCombo,manufacturerNames);
	int idx = SendMessageA(producerCombo,CB_GETCURSEL,0,0);
	char szMessage[1024];
	SendMessageA(producerCombo, CB_GETLBTEXT, idx, (LPARAM)szMessage);
	producer = szMessage;
	modelNumList=Searcher::GetInstance()->FindModelNum(stationID,catalogID,producer);
	Searcher::GetInstance()->putElements(modelNumCombo,modelNumList);
	regionNames=Searcher::GetInstance()->FindRegion(stationID);
	Searcher::GetInstance()->putElements(regionCombo,regionNames);
	putEmpty(regionCombo);
}

void SearchDialog::setHWND(){
	GetCOREInterface()->RegisterDlgWnd(hDialog);
	this->catalogCombo1=GetDlgItem(hDialog,IDC_CATALOG_COMBO1);
	this->catalogCombo2=GetDlgItem(hDialog,IDC_CATALOG_COMBO2);
	this->catalogCombo3=GetDlgItem(hDialog,IDC_CATALOG_COMBO3);
	this->producerCombo=GetDlgItem(hDialog,IDC_PRODUCER_COMBO);
	this->modelNumCombo=GetDlgItem(hDialog,IDC_MODELNUM_COMBO);
	this->regionCombo=GetDlgItem(hDialog,IDC_REGION_COMBO);
	this->searchEdit=GetDlgItem(hDialog,IDC_SEARCH_EDIT);
	this->resultList=GetDlgItem(hDialog,IDC_RESULTS_LIST);
	this->selected=GetDlgItem(hDialog,IDC_SELECTED);
	this->prefixEdit=GetDlgItem(hDialog,IDC_PREFIX_EDIT);
	this->progress=GetDlgItem(hDialog,IDC_PROGRESS);

	SetWindowTextA(hDialog,"台帐查询匹配工具V1.0");
	SetWindowTextA(GetDlgItem(hDialog,IDC_KEYWORD_STATIC),"关键词：");
	SetWindowTextA(GetDlgItem(hDialog,IDC_SEARCH),"查询");
	SetWindowTextA(GetDlgItem(hDialog,IDC_CATALOG_STATIC),"模型类型：");
	SetWindowTextA(GetDlgItem(hDialog,IDC_PRODUCE_STATIC),"生产厂家：");
	SetWindowTextA(GetDlgItem(hDialog,IDC_MODELNUM_STATIC),"型号：");
	SetWindowTextA(GetDlgItem(hDialog,IDC_REGION_STATIC),"所属区域：");
	SetWindowTextA(GetDlgItem(hDialog,IDC_PREFIX_STATIC),"前缀：");
	SetWindowTextA(GetDlgItem(hDialog,IDC_SELECTED_STATIC),"已选择:");
	SetWindowTextA(GetDlgItem(hDialog,IDC_MATCH),"开始匹配");
	SetWindowTextA(GetDlgItem(hDialog,IDC_CANCEL),"取消匹配");
}
void SearchDialog::initResult(){
	SendMessageA(resultList, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_CHECKBOXES);
	LV_COLUMNA lvc;
	lvc.mask = LVCF_TEXT |LVCF_FMT|LVCF_WIDTH;
	char* finished="是否匹配";//Finished
	lvc.fmt = LVCFMT_CENTER; 
	lvc.pszText =finished;
	//lvc.cx = strlen(finished)*8;
	SendMessageA(resultList , LVM_INSERTCOLUMNA, 0, (LPARAM)(&lvc));
	char* name="设备名称";//Name
	lvc.pszText =name;
	//lvc.cx = strlen(name)*8;
	SendMessageA(resultList , LVM_INSERTCOLUMNA, 1, (LPARAM)(&lvc));
	char* equipmentNum="PM编码";//EquipmentNum
	lvc.pszText =equipmentNum;
	//lvc.cx = strlen(equipmentNum)*8;
	SendMessageA(resultList , LVM_INSERTCOLUMNA, 2, (LPARAM)(&lvc));
	char* equipmentType="设备类型";//EquipmentType
	lvc.pszText =equipmentType;
	//lvc.cx = strlen(equipmentType)*8;
	SendMessageA(resultList , LVM_INSERTCOLUMNA, 3, (LPARAM)(&lvc));
	char* equipmentModelNum ="型号";//EquipmentModelNum
	lvc.pszText =equipmentModelNum;
	//lvc.cx = strlen(equipmentModelNum)*8;
	SendMessageA(resultList , LVM_INSERTCOLUMNA, 4, (LPARAM)(&lvc));
	char* equipmentProducer = "生产厂家";//Producer
	lvc.pszText = equipmentProducer;
	//lvc.cx = strlen(equipmentProducer)*8;
	SendMessageA(resultList , LVM_INSERTCOLUMNA, 5, (LPARAM)(&lvc));
	char* region ="电压等级";//Region
	lvc.pszText =region;
	//lvc.cx = strlen(region)*8;
	SendMessageA(resultList , LVM_INSERTCOLUMNA, 6, (LPARAM)(&lvc));
	char* operateNum ="运行编号";//OperateNum
	lvc.pszText = operateNum;
	//lvc.cx = strlen(operateNum)*8;
	SendMessageA(resultList , LVM_INSERTCOLUMNA, 7, (LPARAM)(&lvc));
	char* remarks = "备注";//Remarks
	lvc.pszText = remarks;
	//lvc.cx = strlen(remarks)*8;
	SendMessageA(resultList , LVM_INSERTCOLUMNA, 8, (LPARAM)(&lvc));
	for(int i = 0; i < 9; i++)  
	{
		SendMessageA(resultList , LVM_SETCOLUMNWIDTH , i,LVSCW_AUTOSIZE);
		int nColumnWidth = SendMessageA(resultList , LVM_GETCOLUMNWIDTH , i,0);
		SendMessageA(resultList , LVM_SETCOLUMNWIDTH , i,LVSCW_AUTOSIZE_USEHEADER);
		int nHeaderWidth = SendMessageA(resultList , LVM_GETCOLUMNWIDTH , i,0);
		SendMessageA(resultList , LVM_SETCOLUMNWIDTH , i,max(nColumnWidth,nHeaderWidth));
	}
}
void SearchDialog::DoDialog(){
	CreateDialogParam(
		hInstance,
		MAKEINTRESOURCE(IDD_MAINDIALOG),
		mInterface.GetMAXHWnd(),
		searchDlgProc,
		(LPARAM)this);

}
void SearchDialog::search(){
	startSearch=TRUE;
	noChange=FALSE;
	int len1 =SendMessageA(searchEdit, WM_GETTEXTLENGTH, 0, 0);
	char* szMessage1=new char[len1+1];
	SendMessageA(searchEdit,WM_GETTEXT,len1+1,(LPARAM)szMessage1);
	string keywords = szMessage1;
	int pIdx = SendMessageA(modelNumCombo,CB_GETCURSEL,0,0);
	int equipmentID = SendMessageA(modelNumCombo, CB_GETITEMDATA, pIdx, 0);
	int vIdx = SendMessageA(regionCombo, CB_GETCURSEL,0,0);
	int regionID = SendMessageA(regionCombo, CB_GETITEMDATA, vIdx, 0);
	if(regionID==-1){
		ledgerInfoArr=Searcher::GetInstance()->FindLedger(stationID,catalogID,keywords,equipmentID,nullptr);
	}else{
		int len2 = SendMessageA(regionCombo, CB_GETLBTEXTLEN, vIdx,0);
		char* szMessage2=new char[len2+1];
		SendMessageA(regionCombo, CB_GETLBTEXT, vIdx, (LPARAM)szMessage2);
		string region =szMessage2;
		ledgerInfoArr=Searcher::GetInstance()->FindLedger(stationID,catalogID,keywords,equipmentID,&region);
	}
	itemCount=ledgerInfoArr.size();
	int row=0;
	SendMessageA(resultList,LVM_DELETEALLITEMS,0,0);
	SendMessageA(progress, PBM_SETRANGE, 0, MAKELPARAM(0,itemCount));
	PBRANGE high;
	SendMessageA(progress, PBM_GETRANGE, FALSE, (LPARAM)&high);
	PBRANGE low;
	SendMessageA(progress, PBM_GETRANGE, TRUE, (LPARAM)&low);
	SendMessageA(progress, PBM_SETSTEP, (WPARAM)1, 0);
	SendMessage(progress, PBM_SETPOS, (WPARAM)0, 0);
	for (auto iter = ledgerInfoArr.cbegin(); iter != ledgerInfoArr.cend(); iter++)
	{
		LV_ITEMA item ={0} ;
		item.iItem=row;
		item.mask=LVCF_TEXT;
		SendMessageA(resultList , LVM_INSERTITEMA, 0, (LPARAM)(&item));
		LV_ITEMA lvi;
		lvi.iSubItem=0;
		lvi.pszText=(char*)(iter->getValue(LedgerInfo::e_Finished)).c_str();
		SendMessageA(resultList , LVM_SETITEMTEXTA, row, (LPARAM)(&lvi));
		lvi.iSubItem=1;
		lvi.pszText=(char*)(iter->getValue(LedgerInfo::e_Name)).c_str();
		SendMessageA(resultList , LVM_SETITEMTEXTA, row, (LPARAM)(&lvi));
		lvi.iSubItem=2;
		lvi.pszText=(char*)(iter->getValue(LedgerInfo::e_EquipmentNum)).c_str();
		SendMessageA(resultList , LVM_SETITEMTEXTA, row, (LPARAM)(&lvi));
		lvi.iSubItem=3;
		lvi.pszText=(char*)(iter->getValue(LedgerInfo::e_EquipmentType)).c_str();
		SendMessageA(resultList , LVM_SETITEMTEXTA, row, (LPARAM)(&lvi));
		lvi.iSubItem=4;
		lvi.pszText=(char*)(iter->getValue(LedgerInfo::e_EquipmentModelNum)).c_str();
		SendMessageA(resultList , LVM_SETITEMTEXTA, row, (LPARAM)(&lvi));
		lvi.iSubItem=5;
		lvi.pszText=(char*)(iter->getValue(LedgerInfo::e_EquipmentProducer)).c_str();
		SendMessageA(resultList , LVM_SETITEMTEXTA, row, (LPARAM)(&lvi));
		lvi.iSubItem=6;
		lvi.pszText=(char*)(iter->getValue(LedgerInfo::e_Region)).c_str();
		SendMessageA(resultList , LVM_SETITEMTEXTA, row, (LPARAM)(&lvi));
		lvi.iSubItem=7;
		lvi.pszText=(char*)(iter->getValue(LedgerInfo::e_OperateNum)).c_str();
		SendMessageA(resultList , LVM_SETITEMTEXTA, row, (LPARAM)(&lvi));
		lvi.iSubItem=8;
		lvi.pszText=(char*)(iter->getValue(LedgerInfo::e_Remarks)).c_str();
		SendMessageA(resultList , LVM_SETITEMTEXTA, row, (LPARAM)(&lvi));
		row++;
		SendMessage(progress, PBM_STEPIT, 0, 0);
	}
	for(int i = 0; i < 9; i++)  
	{
		SendMessageA(resultList , LVM_SETCOLUMNWIDTH , i,LVSCW_AUTOSIZE);
		int nColumnWidth = SendMessageA(resultList , LVM_GETCOLUMNWIDTH , i,0);
		SendMessageA(resultList , LVM_SETCOLUMNWIDTH , i,LVSCW_AUTOSIZE_USEHEADER);
		int nHeaderWidth = SendMessageA(resultList , LVM_GETCOLUMNWIDTH , i,0);
		SendMessageA(resultList , LVM_SETCOLUMNWIDTH , i,max(nColumnWidth,nHeaderWidth));
	}
	startSearch=FALSE;
	noChange=FALSE;
}
void SearchDialog::match(){
	int nodeCount = mInterface.GetSelNodeCount();
	if(nodeCount>0){
		if(selectList.size()>0){
			bool success=TRUE;
			EnableWindow(hDialog,FALSE);
			for (int i=0;i<selectList.size();i++)
			{
				if(i+1>nodeCount){
					break;
				}
				LedgerInfo ledgerInfo =ledgerInfoArr[selectList[i]];
				string equipmentNum = ledgerInfo.getValue(LedgerInfo::e_EquipmentNum);
				std::stringstream ss;
				int len =SendMessageA(prefixEdit, WM_GETTEXTLENGTH, 0, 0);
				char* prefix=new char[len+1];
				SendMessageA(prefixEdit,WM_GETTEXT,len+1,(LPARAM)prefix);
				string prefixStr (prefix);
				if(prefixStr.size()>0){
					ss << prefix << "-"<<equipmentNum;
				}else{
					string code=Searcher::GetInstance()->FindCode(ledgerInfo.getID());
					if(code==""){
						ss <<"QT-"<<equipmentNum;
					}else{
						ss << code <<"-"<<equipmentNum;
					}
				}
				mInterface.GetSelNode(i)->SetName(char2wchar(ss.str().c_str()));
				if(!Searcher::GetInstance()->attachLedgerLink(ledgerInfo.getID())){
					success=FALSE;
					if(i==0){
						showMessage("匹配失败！");
					}else{
						showMessage("部分匹配失败！");
					}
					break;
				}
			}
			if(success){
				SetWindowTextA(prefixEdit,"");
				if(selectList.size()==1||nodeCount==1){
					showMessage("匹配成功！");
				}else{
					showMessage("全部匹配成功！");
				}
			}
			EnableWindow(hDialog,TRUE);
			search();
		}else{
			showMessage("请先选中台帐条目！");
		}
	}else{
		showMessage("请先选中场景内物体！");
	}
}

void SearchDialog::cancel(){
	if(selectList.size()>0){
		bool success=TRUE;
		EnableWindow(hDialog,FALSE);
		for (int i=0;i<selectList.size();i++)
		{
			LedgerInfo ledgerInfo =ledgerInfoArr[selectList[i]];
			if(!Searcher::GetInstance()->canelLedgerLink(ledgerInfo.getID())){
				success=FALSE;
				if(i==0){
					showMessage("取消失败！");
				}else{
					showMessage("部分取消失败！");
				}
				break;
			}
		}
		search();
		EnableWindow(hDialog,TRUE);
		if(success){
			if(selectList.size()==1){
				showMessage("取消成功！");
			}else{
				showMessage("全部取消成功！");
			}
		}
	}else{
		showMessage("请先选中台帐条目！");
	}
}

void ScreenToClient(HWND hWnd, RECT* rect)
{
	POINT pt;
	pt.x = rect->left;
	pt.y = rect->top;
	ScreenToClient(hWnd, &pt);
	rect->left = pt.x;
	rect->top = pt.y;
	pt.x = rect->right;
	pt.y = rect->bottom;
	ScreenToClient(hWnd, &pt);
	rect->right = pt.x;
	rect->bottom = pt.y;
}
void SearchDialog::ResizeWindow(int x, int y)
{
	HWND	pCtrl;
	RECT	lbRect;
	RECT	prevCtrlRect;
	int		offset;
	BOOL	bRepaint = FALSE;

	pCtrl = GetDlgItem(hDialog, IDC_SEARCH);
	GetWindowRect(pCtrl, &lbRect);
	ScreenToClient(hDialog, &lbRect);
	offset = lbRect.right - lbRect.left;
	lbRect.left = x-offset - 10;
	lbRect.right = lbRect.left + offset;
	MoveWindow(pCtrl, lbRect.left, lbRect.top, lbRect.right-lbRect.left, lbRect.bottom-lbRect.top, bRepaint);
	prevCtrlRect = lbRect;

	pCtrl = GetDlgItem(hDialog, IDC_SEARCH_EDIT);
	GetWindowRect(pCtrl, &lbRect);
	ScreenToClient(hDialog, &lbRect);
	offset = lbRect.bottom - lbRect.top;
	lbRect.top = y - offset - 6;
	lbRect.bottom = lbRect.top + offset;
	lbRect.right = prevCtrlRect.left - 35;
	MoveWindow(pCtrl, lbRect.left, lbRect.top, lbRect.right-lbRect.left, lbRect.bottom-lbRect.top, bRepaint);
	prevCtrlRect = lbRect;

	pCtrl = GetDlgItem(hDialog, IDC_RESULTS_LIST);
	GetWindowRect(pCtrl, &lbRect);
	ScreenToClient(hDialog, &lbRect);
	lbRect.bottom = prevCtrlRect.top - 3;
	lbRect.right = prevCtrlRect.right+30;
	MoveWindow(pCtrl, lbRect.left, lbRect.top, lbRect.right-lbRect.left, lbRect.bottom-lbRect.top, bRepaint);
	
	InvalidateRect(hDialog, NULL, TRUE);
}
INT_PTR CALLBACK SearchDialog::messageDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_INITDIALOG:
		{
			SetWindowTextA(hWnd,"消息");
			SetWindowTextA(GetDlgItem(hWnd,IDC_MESSAGE),(LPCSTR)(char*)lParam);
			CenterWindow(hWnd, GetParent(hWnd));
		}
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			EndDialog(hWnd,IDOK);
			break;
		default:
			break;
		}
		break;
	case WM_CLOSE:
		EndDialog(hWnd,WM_CLOSE);
		break;
	default:
		return 0;
	}
	return 1;
}
void  SearchDialog::changeSelect(){
	if(!noChange){
		stringstream selects;
		if(startSearch){
			noChange=TRUE;
			selects<<"0/"<<itemCount;
		}else{
			selectList.clear();
			for (int i=0;i<itemCount;i++)
			{
				if(ListView_GetCheckState(resultList,i)){
					selectList.push_back(i);
				}
			}
			selects<<selectList.size()<<"/"<<itemCount;
		}
		SetWindowTextA(selected,(LPCSTR)selects.str().c_str());
	}
}
INT_PTR CALLBACK SearchDialog::searchDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	SearchDialog* util = DLGetWindowLongPtr<SearchDialog*>(hWnd);
	switch (msg) {
	case WM_INITDIALOG:
		{
			RECT rect;
			util = (SearchDialog*)lParam;
			util->hDialog = hWnd;
			DLSetWindowLongPtr(hWnd, lParam);
			CenterWindow(hWnd, util->mInterface.GetMAXHWnd());
			GetWindowRect(hWnd, &rect);
			util->SetMinDialogSize((rect.right - rect.left)*2/3, rect.bottom - rect.top);
			SendMessage(hWnd, WM_SETICON, ICON_SMALL, DLGetClassLongPtr<LPARAM>(util->mInterface.GetMAXHWnd(), GCLP_HICONSM));
			util->setHWND();
			util->putCombo();
			util->initResult();
			ShowWindow(hWnd,1);
		}
		break;
	case WM_COMMAND:
		if(HIWORD(wParam) == CBN_SELCHANGE)
		{
			util->updateCombo();
		}
		switch (LOWORD(wParam))
		{
		case IDC_SEARCH_EDIT:
			// If we get a setfocus message, disable accelerators
			if(HIWORD(wParam) == EN_SETFOCUS)
				DisableAccelerators();
			// Otherwise enable them again
			else if(HIWORD(wParam) == EN_KILLFOCUS) {
				EnableAccelerators();
			}
			break;
		case IDC_PREFIX_EDIT:
			// If we get a setfocus message, disable accelerators
			if(HIWORD(wParam) == EN_SETFOCUS)
				DisableAccelerators();
			// Otherwise enable them again
			else if(HIWORD(wParam) == EN_KILLFOCUS) {
				EnableAccelerators();
			}
			break;
		case IDC_SEARCH:
			util->search();
			break;
		case IDC_MATCH:
			util->match();
			break;
		case IDC_CANCEL:
			util->cancel();
			break;
		default:
			break;
		}
		break;
	case WM_NOTIFY:
		switch (((LPNMHDR)lParam)->code)
		{
			case LVN_ITEMCHANGED:
				if (((LPNMHDR)lParam)->idFrom == IDC_RESULTS_LIST)
				{
					/*LPNMLISTVIEW pnmv = (LPNMLISTVIEW) lParam;
					pnmv->iItem;*/
					util->changeSelect();
				}
				break;
		}
		break;
	case WM_CLOSE:
		DestroyWindow(hWnd);
		//EndDialog(hWnd, 1);
		break;
	case WM_DESTROY:
		util->hDialog = NULL;
		break;
	case WM_WINDOWPOSCHANGING:
		{
			if(IsIconic(hWnd)) {
				return FALSE;
			}

			// prevent the window from stretching horizontally
			WINDOWPOS *wp = (WINDOWPOS*)lParam;
			if (wp->cx < util->GetMinDialogWidth()) {
				wp->cx = util->GetMinDialogWidth();
			}
			if (wp->cy < util->GetMinDialogHeight()) {
				wp->cy = util->GetMinDialogHeight();
			}

			break;
		}
	case WM_SIZE:
		util->ResizeWindow(LOWORD(lParam), HIWORD(lParam));
		break;
	default:
		return 0;
	}
	return 1;
}




