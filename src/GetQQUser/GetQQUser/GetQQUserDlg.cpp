// GetQQUserDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "GetQQUser.h"
#include "GetQQUserDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CGetQQUserDlg 对话框




CGetQQUserDlg::CGetQQUserDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGetQQUserDlg::IDD, pParent)
	, m_strAddr(_T(""))
	, m_strGameType(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_hProcessQQGame=NULL;
	m_hMainQQGame=NULL;
	m_hGameListTree=NULL;
	m_pItemBuff=NULL;
	m_pOutBuff=NULL;
}

void CGetQQUserDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_ADDR, m_strAddr);
	DDX_Control(pDX, IDC_EDIT_MSG, m_edtMsg);
	DDX_CBString(pDX, IDC_COMBO_GAME_TYPE, m_strGameType);
	DDX_Control(pDX, IDC_COMBO_GAME_TYPE, m_cmbGameType);
}

void CGetQQUserDlg::InitQQGameInfo()
{
	/************************************************************************/

	FreeMemory();

	/************************************************************************/

	HMODULE hModule=LoadLibrary("Hook.dll");
	if (hModule!=NULL){
		StartHook=(TStartHook)GetProcAddress(hModule,"StartHook");
		StopHook=(TStopHook)GetProcAddress(hModule,"StopHook");

		m_hMapFile=OpenFileMapping(FILE_MAP_WRITE,FALSE,MappingFileName);
		if (m_hMapFile==NULL){
			m_hMapFile=CreateFileMapping(INVALID_HANDLE_VALUE,NULL,PAGE_READWRITE,0,sizeof(SHAREMEM),MappingFileName);
		}
		if (m_hMapFile==NULL){
			AppendMsg("不能建立共享内存!");
		}
		m_pShareMem=(PSHAREMEM)MapViewOfFile(m_hMapFile,FILE_MAP_WRITE|FILE_MAP_READ,0,0,0);
		if (m_pShareMem==NULL){
			CloseHandle(m_hMapFile);
			AppendMsg("不能映射共享内存!");
		}
	}

	/************************************************************************/

	DWORD dwPID=Star::Process::GetProcessID("QQGame");
	m_hProcessQQGame=OpenProcess( PROCESS_ALL_ACCESS, FALSE, dwPID );
	if(m_hProcessQQGame==NULL){
		AppendMsg("打开目标进程失败");
		return;
	}

	m_dwThreadId=Star::Process::GetThreadIdFromPID(dwPID);
	if (m_dwThreadId == 0){
		AppendMsg("获取进程的主线程ID失败");
	}
	else{
		StartHook(m_hWnd,NULL,m_dwThreadId);
	}

	m_hMainQQGame=::FindWindow(NULL,"QQ游戏");
	if (m_hMainQQGame==NULL){
		AppendMsg("获取“QQ游戏”窗口失败");
		return;
	}

	/************************************************************************/

	CString strText;
	HWND hwnd=::FindWindowEx(m_hMainQQGame,NULL,NULL,NULL);

	strText.Format("hwnd=%08X",hwnd);
	AppendMsg(strText);

	hwnd=::FindWindowEx(m_hMainQQGame,hwnd,NULL,NULL);
	strText.Format("hwnd=%08X",hwnd);
	AppendMsg(strText);
	hwnd=::FindWindowEx(m_hMainQQGame,hwnd,NULL,NULL);
	strText.Format("hwnd=%08X",hwnd);
	AppendMsg(strText);
	hwnd=::FindWindowEx(m_hMainQQGame,hwnd,NULL,NULL);
	strText.Format("hwnd=%08X",hwnd);
	AppendMsg(strText);

	hwnd=::FindWindowEx(hwnd,NULL,NULL,"SkinWnd");
	strText.Format("hwnd=%08X",hwnd);
	AppendMsg(strText);

	hwnd=::FindWindowEx(hwnd,NULL,NULL,"SkinTree");
	strText.Format("hwnd=%08X",hwnd);
	AppendMsg(strText);

/*
	HWND hParent=hwnd;
	hwnd=::FindWindowEx(hParent,NULL,NULL,NULL);
	strText.Format("hwnd=%08X",hwnd);
	AppendMsg(strText);
	hwnd=::FindWindowEx(hParent,hwnd,NULL,NULL);
	strText.Format("hwnd=%08X",hwnd);
	AppendMsg(strText);
	hwnd=::FindWindowEx(hParent,hwnd,NULL,NULL);
	strText.Format("hwnd=%08X",hwnd);
	AppendMsg(strText);
	m_hGameListTree=::FindWindowEx(hParent,hwnd,NULL,NULL);
	strText.Format("hwnd=%08X",m_hGameListTree);
	AppendMsg(strText);*/

	m_hGameListTree=::FindWindowEx(hwnd,NULL,"SysTreeView32",NULL);
	strText.Format("hwnd=%08X",m_hGameListTree);
	AppendMsg(strText);
	if (m_hGameListTree==NULL){
		AppendMsg("获取“QQ游戏”游戏列表窗口失败");
		return;
	}

	/************************************************************************/

	m_pItemBuff=(PBYTE)VirtualAllocEx(m_hProcessQQGame, NULL, sizeof(TVITEM), MEM_COMMIT, PAGE_READWRITE); 
	m_pOutBuff=(PBYTE)VirtualAllocEx(m_hProcessQQGame, NULL, 1024, MEM_COMMIT, PAGE_READWRITE);
	if (m_pItemBuff==NULL || m_pOutBuff==NULL){
		AppendMsg("分配内存失败");
		return;
	}
}

void CGetQQUserDlg::FreeMemory()
{
	if (m_hProcessQQGame!=NULL){
		if (m_pItemBuff!=NULL){
			VirtualFreeEx(m_hProcessQQGame, m_pItemBuff, 0, MEM_RELEASE); 
			m_pItemBuff=NULL;
		}

		if (m_pOutBuff!=NULL){
			VirtualFreeEx(m_hProcessQQGame, m_pOutBuff, 0, MEM_RELEASE); 
			m_pOutBuff=NULL;
		}

		CloseHandle(m_hProcessQQGame);
		m_hProcessQQGame=NULL;
	}
}

HWND CGetQQUserDlg::FindExitRoomButton()
{
	CString strText;
	HWND hwnd=::FindWindowEx(m_hMainQQGame,NULL,NULL,"SkinWnd");

	strText.Format("hwnd=%08X",hwnd);
	AppendMsg(strText);

	hwnd=::FindWindowEx(hwnd,NULL,NULL,"SkinTab");
	strText.Format("hwnd=%08X",hwnd);
	AppendMsg(strText);
	hwnd=::FindWindowEx(hwnd,NULL,NULL,"SkinWnd");
	strText.Format("hwnd=%08X",hwnd);
	AppendMsg(strText);
	hwnd=::FindWindowEx(hwnd,NULL,NULL,"SkinWnd");
	strText.Format("hwnd=%08X",hwnd);
	AppendMsg(strText);
	hwnd=::FindWindowEx(hwnd,NULL,NULL,"SkinWnd");
	strText.Format("hwnd=%08X",hwnd);
	AppendMsg(strText);

	hwnd=::FindWindowEx(hwnd,NULL,NULL,"SkinWnd");
	strText.Format("hwnd=%08X",hwnd);
	AppendMsg(strText);

	hwnd=::FindWindowEx(hwnd,NULL,NULL,"SkinButton");
	strText.Format("hwnd=%08X",hwnd);
	AppendMsg(strText);

	return hwnd;
}

BEGIN_MESSAGE_MAP(CGetQQUserDlg, CDialog)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_START, &CGetQQUserDlg::OnBnClickedButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_SAVETODB, &CGetQQUserDlg::OnBnClickedButtonSavetodb)
	ON_BN_CLICKED(IDC_BUTTON1, &CGetQQUserDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CGetQQUserDlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// CGetQQUserDlg 消息处理程序

BOOL CGetQQUserDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	/************************************************************************/
	InitQQGameInfo();
	/************************************************************************/
	m_edtMsg.SetLimitText(-1);

	for (int nItem=0;nItem<sizeof(gamelist)/sizeof(GAME_INFO);nItem++)
	{
		m_cmbGameType.InsertString(nItem,gamelist[nItem].strGameName);
		m_cmbGameType.SetItemData(nItem,gamelist[nItem].dwGameID);
	}
	/************************************************************************/
	bool bFirstCreate=GetFileAttributes(Star::Global::GetStartPath()+"player.db")==-1;
	m_sqlitedb.Connect( Star::Global::GetStartPath()+"player.db" );
	if(bFirstCreate==TRUE)
	{
		m_sqlitedb.ExecuteSQL(
			"CREATE TABLE qqplayer ("
			"	id INTEGER PRIMARY KEY,"
			"	qq CHAR(20),"
			"	nicky VARCHAR(100),"
			"	played INTEGER,"
			"	blacklv INTEGER DEFAULT 0,"
			"	mailtm	TIMESTAMP DEFAULT '2000-01-01 00:00:00' "
			");"
			"create index indexqq on qqplayer(qq);;"
			);
	}
	/************************************************************************/
	
	//MODULEENTRY32 module;
	//Star::Process::GetModuleEntry(GetCurrentProcessId(),"kernel32.dll",&module);
/*
	CString strText;
	strText="普通场(345人)";
	int nPos1=strText.ReverseFind('(');
	int nPos2=strText.Find("人)",nPos1);
	if (nPos1!=-1 && nPos2!=-1){
		strText=strText.Mid(nPos1+1,nPos2-nPos1-1);
		DWORD dwA=atoi(strText);
		dwA=0;
	}*/

		
	/************************************************************************/

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CGetQQUserDlg::PostNcDestroy()
{
	FreeMemory();
	m_sqlitedb.Disconnect();
}

void CGetQQUserDlg::OnPaint()
{
	CDialog::OnPaint();
}

void CGetQQUserDlg::AppendMsg(CString strMsg)
{
	LONG nLen=(LONG)m_edtMsg.SendMessage(WM_GETTEXTLENGTH);
	m_edtMsg.SetSel(nLen,-1);
	m_edtMsg.ReplaceSel(strMsg+"\r\n");
}

void CGetQQUserDlg::OnBnClickedButtonStart()
{
	m_edtMsg.SetWindowText(NULL);
	UpdateData();
	DWORD dwAddr=m_pShareMem->dwSkinListThis;
	if ( dwAddr==0 ){
		AppendMsg("g_pShareMem->dwSkinListThis==0");
		return;
	}

	DWORD dwAddrBegin,dwAddrEnd,dwReadWrite;
	ReadProcessMemory(m_hProcessQQGame,LPCVOID(dwAddr+0x74),&dwAddrBegin,sizeof(DWORD),&dwReadWrite);
	ReadProcessMemory(m_hProcessQQGame,LPCVOID(dwAddr+0x78),&dwAddrEnd,sizeof(DWORD),&dwReadWrite);
	if( dwAddrEnd-dwAddrBegin<sizeof(DWORD) )
	{
		AppendMsg("没有信息");
		return;
	}
	
	DWORD dwReadAddr=dwAddrBegin;
	char buff[0x100];
	CString strText;
	PLAYER_INFO stPlayerInfo;
	int nCount=0;
	for ( DWORD dwReadAddr=dwAddrBegin+sizeof(DWORD); 
		dwReadAddr<=dwAddrEnd; dwReadAddr+=sizeof(DWORD) )
	{
		DWORD dwPlayer=0;
		ReadProcessMemory(m_hProcessQQGame,LPCVOID(dwReadAddr),&dwPlayer,sizeof(DWORD),&dwReadWrite);
		if(dwPlayer==0)
			break;
		ReadProcessMemory(m_hProcessQQGame,LPCVOID(dwPlayer),buff,sizeof(buff),&dwReadWrite);
		
		stPlayerInfo.strQQ=buff+0xEC;
		stPlayerInfo.strNicky=buff+0x66;

		if ( stPlayerInfo.strQQ.Find('-')!=-1 )
			continue;

		strText.Format("QQ：%s\t昵称：%s",stPlayerInfo.strQQ,stPlayerInfo.strNicky);
		AppendMsg(strText);

		stPlayerInfo.strQQ.Replace("\'","\'\'");
		stPlayerInfo.strQQ.Replace("\"","\"\"");
		m_vtPlayer.push_back(stPlayerInfo);
		nCount++;
	}
	strText.Format("本次收集 %d 个用户信息，总共有 %d 个用户信息。",
		nCount,m_vtPlayer.size());
	AppendMsg(strText);
}

void CGetQQUserDlg::OnBnClickedButtonSavetodb()
{
	UpdateData();
	int nItem=m_cmbGameType.GetCurSel();
	if(nItem<0){
		AppendMsg("请选择一种游戏类型！！！");
		return;
	}

	CString strTemp;
	DWORD dwGameType=m_cmbGameType.GetItemData(nItem);
	CString strSQL;

	DWORD dwTimeCost=GetTickCount();

	m_sqlitedb.BeginTransaction();

	for (vector<PLAYER_INFO>::iterator iter=m_vtPlayer.begin();
		iter!=m_vtPlayer.end(); ++iter)
	{
		int result;
		char*szErrorMsg = NULL;
		char**dbResult;
		int nRow, nColumn;

		strSQL.Format("SELECT played FROM qqplayer WHERE qq='%s';",iter->strQQ);
		result = sqlite3_get_table( m_sqlitedb.m_pDB, strSQL, &dbResult, &nRow, &nColumn, &szErrorMsg );
		if(result!=SQLITE_OK)
		{
			TRACE(szErrorMsg);
			sqlite3_free(szErrorMsg);
			continue;
		}

		if ( dbResult==NULL )
			continue;

		if( nRow > 0 ){
			//记录存在，更新信息
			DWORD dwPlayedGame=atoi(dbResult[1]);

			if ( (dwPlayedGame&dwGameType) == 0 ){//不包含该游戏类型
				dwPlayedGame|=dwGameType;
				strSQL.Format("UPDATE qqplayer SET nicky='%s', played=%d WHERE qq='%s';",
					iter->strNicky,dwPlayedGame,iter->strQQ);
				m_sqlitedb.ExecuteSQL(strSQL);
			}
			
		}else{
			//记录不存在，直接插入
			strSQL.Format("INSERT INTO qqplayer (qq,nicky,played) VALUES('%s','%s',%d);",
				iter->strQQ,iter->strNicky,dwGameType);
			m_sqlitedb.ExecuteSQL(strSQL);
		}

		sqlite3_free_table(dbResult);

	}

	m_sqlitedb.CommitTransaction();

	m_vtPlayer.clear();

	dwTimeCost = GetTickCount() - dwTimeCost;
	strTemp.Format("入库完毕，共用时间：%d ms", dwTimeCost);
	AppendMsg(strTemp);
}

BOOL CGetQQUserDlg::ItemHasChildren(HWND hWnd, HTREEITEM hItem)
{
	TVITEM item;
	item.hItem = hItem;
	item.mask = TVIF_CHILDREN;
	item.cChildren=0;
	BOOL bOK=WriteProcessMemory(m_hProcessQQGame,m_pItemBuff,&item,sizeof(TVITEM),NULL);
	if (bOK==TRUE){
		//AppendMsg("ItemHasChildren WriteProcessMemory OK");
		::SendMessage(hWnd, TVM_GETITEM, 0, (LPARAM)m_pItemBuff);
	}
	
	return item.cChildren;
}

CString CGetQQUserDlg::GetItemText(HWND hWnd,HTREEITEM hItem)
{
	TVITEM item;
	item.hItem = hItem;
	item.mask = TVIF_TEXT;
	item.pszText = (LPSTR)m_pOutBuff;
	item.cchTextMax = 512;
	char buff[512]={0};

	BOOL bOK=WriteProcessMemory(m_hProcessQQGame,m_pItemBuff,&item,sizeof(TVITEM),NULL);
	if (bOK==TRUE){
		//AppendMsg("GetItemText WriteProcessMemory OK");
		::SendMessage(hWnd, TVM_GETITEM, 0, (LPARAM)m_pItemBuff);
		ReadProcessMemory(m_hProcessQQGame,m_pOutBuff,buff,512,NULL);
	}
	
	return buff;
}

void CGetQQUserDlg::GetItemParam(HWND hWnd,HTREEITEM hItem)
{
	TVITEM item;
	item.hItem = hItem;
	item.mask = TVIF_PARAM;
	item.lParam=NULL;
	char buff1[MAX_PATH]={0};
	char buff2[MAX_PATH]={0};
	CString strText;

	TREEITEMPARAM itemdata;
	BOOL bOK=WriteProcessMemory(m_hProcessQQGame,m_pItemBuff,&item,sizeof(TVITEM),NULL);
	if (bOK==TRUE){
		//AppendMsg("GetItemParam WriteProcessMemory OK");
		::SendMessage(hWnd, TVM_GETITEM, 0, (LPARAM)m_pItemBuff);
		ReadProcessMemory(m_hProcessQQGame,m_pItemBuff,&item,sizeof(TVITEM),NULL);
		if (item.lParam!=NULL){
			//AppendMsg("GetItemParam OK");
			ReadProcessMemory(m_hProcessQQGame,(LPCVOID)item.lParam,&itemdata,sizeof(TREEITEMPARAM),NULL);
			ReadProcessMemory(m_hProcessQQGame,(LPCVOID)itemdata.lpszMsg1,buff1,sizeof(buff1),NULL);
			//AppendMsg(buff1);
			ReadProcessMemory(m_hProcessQQGame,(LPCVOID)itemdata.lpszMsg2,buff2,sizeof(buff2),NULL);
			//AppendMsg(buff2);
			strText.Format("lPram：%s%s",buff1,buff2);
			AppendMsg(strText);
		}
	}
}

void CGetQQUserDlg::GetItemRect(HWND hWnd,HTREEITEM hItem,LPRECT lpRect)
{
	BOOL bOK=WriteProcessMemory(m_hProcessQQGame,m_pItemBuff,&hItem,sizeof(HTREEITEM*),NULL);
	if (bOK==TRUE){
		//AppendMsg("GetItemRect WriteProcessMemory OK");
		::SendMessage(hWnd, TVM_GETITEMRECT, (WPARAM)FALSE,(LPARAM)m_pItemBuff);
		ReadProcessMemory(m_hProcessQQGame,m_pItemBuff,lpRect,sizeof(RECT),NULL);
	}
}

void CGetQQUserDlg::VisitTree(CTreeCtrl&ctrlTree,HTREEITEM hItem)
{
	//CString strText;

	ctrlTree.Expand(hItem,TVE_EXPAND);
	HTREEITEM hChildItem=ctrlTree.GetChildItem(hItem);

	if (hChildItem==NULL){
		//没有孩子节点，说明是房间可以双击进去
		m_vtTreeItem.push_back(hItem);
	}

	while(hChildItem!=NULL){
		//strText=GetItemText(ctrlTree.m_hWnd,hChildItem);
		//AppendMsg(strText);
		//GetItemParam(ctrlTree.m_hWnd,hChildItem);
		VisitTree( ctrlTree,hChildItem );//递归遍历孩子节点
		hChildItem=ctrlTree.GetNextItem(hChildItem,TVGN_NEXT);
	}
}

HTREEITEM CGetQQUserDlg::GetSpecifiedItem(CTreeCtrl&ctrlTree,HTREEITEM hItem,LPCTSTR lpszDstText,BOOL bJustContain)
{
	HTREEITEM hItemFound=NULL;
	CString strText;

	while(hItem!=NULL){
		strText=GetItemText(ctrlTree.m_hWnd,hItem);
		if (bJustContain==TRUE){
			if (strText.Find(lpszDstText)!=-1){
				hItemFound=hItem;
				break;
			}
		}else{
			if (strText.Compare(lpszDstText)==0){
				hItemFound=hItem;
				break;
			}
		}
		GetSpecifiedItem( ctrlTree,ctrlTree.GetChildItem(hItem),lpszDstText,bJustContain );           //递归遍历孩子节点
		hItem=ctrlTree.GetNextItem(hItem,TVGN_NEXT);  
	}

	return hItemFound;
}

void CGetQQUserDlg::OnBnClickedButton1()
{
	CTreeCtrl ctrlTree;

	ctrlTree.m_hWnd=m_hGameListTree;
	HTREEITEM hRoot=ctrlTree.GetRootItem();
	m_nTreeItemCount=0;
	//VisitTree(ctrlTree,hRoot);
	HTREEITEM hItemCollection=GetSpecifiedItem(ctrlTree,hRoot,"我收藏的游戏",TRUE);
	if (hItemCollection!=NULL){
		m_vtTreeItem.clear();
		VisitTree(ctrlTree,hItemCollection);
		m_nSelectedItem=0;
	}
	ctrlTree.m_hWnd=NULL;

	CString strText;
	strText.Format( "共遍历 %d 个节点", m_vtTreeItem.size() );
	AppendMsg(strText);
}

/*
if((GetItemState(hItem,   TVIS_EXPANDED   )&TVIS_EXPANDED)!=TVIS_EXPANDED)  
AfxMessageBox("Closed");  
		  else  
			  AfxMessageBox("Expanded");   */

void CGetQQUserDlg::OnBnClickedButton2()
{
	m_nSelectedItem=1;
	while(m_nSelectedItem!=0)
	{
		ExitRoom();

		while (m_nSelectedItem!=0)
		{
			if (EnterRoom()==TRUE){
				break;
			}
		}
		

		int nTryTimes=5;
		while(TRUE){
			Sleep(1000);
			nTryTimes--;
			HWND hBtnExit=FindExitRoomButton();
			if (hBtnExit!=NULL || nTryTimes==0){
				break;
			}

			/**************************************************/
			HWND hTipWnd=::FindWindow("#32770","提示信息");
			::SendMessage(hTipWnd,WM_CLOSE,0,0);
			/**************************************************/
		}
		OnBnClickedButtonStart();
		OnBnClickedButtonSavetodb();
	}

}

BOOL CGetQQUserDlg::EnterRoom()
{
	BOOL bEntered=FALSE;

	if( m_nSelectedItem>=0 && m_nSelectedItem<m_vtTreeItem.size() ){
		CString strText;
		strText.Format("要求进入房间：%08X",m_vtTreeItem[m_nSelectedItem]);
		AppendMsg(strText);
		
		HTREEITEM hItem=m_vtTreeItem[m_nSelectedItem];

		strText=GetItemText(m_hGameListTree,hItem);
		int nPos1=strText.ReverseFind('(');
		int nPos2=strText.Find("人)",nPos1);
		if (nPos1!=-1 && nPos2!=-1){
			strText=strText.Mid(nPos1+1,nPos2-nPos1-1);
			if( atoi(strText) > 90 ){
				AppendMsg(strText);

				//设置该节点为选中状态：CTreeCtrl::SelectItem
				::SendMessage(m_hGameListTree, TVM_SELECTITEM, TVGN_CARET, (LPARAM)m_vtTreeItem[m_nSelectedItem]);
				//发送双击消息
				::SendMessage(m_hGameListTree, WM_LBUTTONDBLCLK, 0, 0);

				bEntered=TRUE;
			}
		}

		m_nSelectedItem++;
	}else{
		m_nSelectedItem=0;
	}

	return bEntered;
}

void CGetQQUserDlg::ExitRoom()
{
	HWND hBtnExit=FindExitRoomButton();
	if (hBtnExit!=NULL){
		::SendMessage(hBtnExit,WM_LBUTTONDOWN,MK_LBUTTON,MAKELPARAM(9,5));
		::SendMessage(hBtnExit,WM_LBUTTONUP,0,MAKELPARAM(9,5));
	}
}