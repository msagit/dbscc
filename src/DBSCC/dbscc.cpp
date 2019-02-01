// PL/SQL Developer Plug-In DBScC
// Copyright 2005 Art-Master 
// Developed by Matsak Sergei (matsak@wp.pl)

// dbscc.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "dbscc.h"
#include "PlSql_functions.h"
#include <time.h>
#include <string>
#include <sstream>
#include <hash_set>
using namespace std;
using namespace stdext;


BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

char *const Desc = "PL/SQL DBScC - Team database source control system";
int PlugInID;

char *DBSCC_VERSION="1.5.2 RC";  // Текущая версия DBSCC PlugIn
char *DBSCC_ServerVersion="1."; // Поддерживаемая версия серверной части системы DBSCC

bool UseAutoSetCursorPosition = false;
bool DBSCC_Enabled = true;  // Изменяется пользователем
bool DBSCC_Active = true;   // Поддержка сервера
bool UseAutoLock  = true;   // Автоматическая блокировка при редактировании
bool UseAutoUnlock  = true;   // Автоматическая разблокировка при закртии окна
bool UseSmartLabels  = true;   // Автоматическа подстветка в контектсном меню информации о блокировках 
const int MaxSupportedObjects = 4;
char * SupportedObjects[MaxSupportedObjects] ={"FUNCTION","PROCEDURE","PACKAGE","PACKAGE BODY"};
const int MaxSpecalEquality = 1;
int SpecalEquality[MaxSpecalEquality][2]={{2,3}};  //Исключения для равентсва типов объектов. Задаются пары индексов из SupportedObjects которые равны.

bool DBSCC_DebugMode = false;   // Отладочный режим

bool WindowJustOpened = false;  // Флаг только что открытого окна
bool DontConfirmClose = false;  // Флаг только что закрытото окна

hash_set <int> LockedWindowsList;

const int c_errcode_unsuppored_type   = 20001;
const int c_errcode_object_not_exist  = 20002;
const int c_errcode_object_not_locked = 20003;
const int c_errcode_object_locked     = 20004;

//---------------------------------------------------------------------------
// Service functions
//---------------------------------------------------------------------------
const char* IntToStr(int n)
{
	static std::string s;
	std::stringstream ss;
	ss << n;
	s = ss.str();
	return s.c_str();
}
//---------------------------------------------------------------------------
void ShowMessage(const char* Data)
{
	MessageBox(NULL,Data, "DBScC Message",0);
}

//---------------------------------------------------------------------------
bool IsDBSCC_Server_Active()
{
	char s[240]="select dbscc.vesion()  as  res from dual";
	char *v=NULL;
	int res=SQL_Execute(s);
	if (res==0) 
	{
  		v=SQL_Field(SQL_FieldIndex("res"));
	}
	return ((v!=NULL)&&(strncmp(v,DBSCC_ServerVersion,strlen(DBSCC_ServerVersion))==0));
}
//---------------------------------------------------------------------------
bool IsDBSCCObjectType(char * Type)
{
	for (int i=0;i<MaxSupportedObjects;i++)
	{
		if (strcmp(Type,SupportedObjects[i])==0)
		{
			return true;
		}
	}
	return false;
}
//---------------------------------------------------------------------------
bool IsEqualObjectType(char * Type1,char * Type2)
{
	if (strcmp(Type1,Type2)==0)
	{
		return true;		
	}
	for (int i=0;i<MaxSpecalEquality;i++)
	{
		if ( ((strcmp(Type1,SupportedObjects[SpecalEquality[i][0]])==0)||(strcmp(Type1,SupportedObjects[SpecalEquality[i][1]])==0))&&
			((strcmp(Type2,SupportedObjects[SpecalEquality[i][0]])==0)||(strcmp(Type2,SupportedObjects[SpecalEquality[i][1]])==0)) )
		{
			return true;		
		}
	}
	return false;
}

//---------------------------------------------------------------------------
void ShowBrowserObject()
{
	char *Type, *Owner, *Name;
	char s[100];
	
	IDE_GetBrowserInfo(&Type, &Owner, &Name);
	s[0] = 0;
	strcat(s, Type);
	strcat(s, "\n");
	strcat(s, Owner);
	strcat(s, "\n");
	strcat(s, Name);
	ShowMessage(s);
}
//---------------------------------------------------------------------------
void ShowConnectionInfo()
{
	char *Username, *Password, *Database;
	char s[100];
	
	IDE_GetConnectionInfo(&Username, &Password, &Database);
	s[0] = 0;
	strcat(s, Username);
	strcat(s, "\n");
	strcat(s, Password);
	strcat(s, "\n");
	strcat(s, Database);
	ShowMessage(s);
}
//---------------------------------------------------------------------------
void SetSelection(char *s)
{
	int H = IDE_GetEditorHandle();
	if (H > 0)
	{
		SendMessage((HWND)H, EM_REPLACESEL, true, (int)s);
	}
}
//---------------------------------------------------------------------------
void MaximizeWindow()
{
	int H = IDE_GetChildHandle();
	int M = IDE_GetClientHandle();
	if (H > 0)
	{
		SendMessage((HWND)M, WM_MDIMAXIMIZE, H, (int)0);
	}
}

//---------------------------------------------------------------------------
DWORD GetCurrentCusorPosition(int &x, int &y)
{
	HWND H = (HWND)IDE_GetEditorHandle();
	DWORD StartingPos;
	SendMessage(H, EM_GETSEL, (WPARAM) (LPDWORD) &StartingPos, NULL);
	DWORD StartingPos_x = StartingPos;
	y = SendMessage(H, EM_LINEFROMCHAR, (WPARAM) StartingPos, NULL);
	x = StartingPos - SendMessage(H, EM_LINEINDEX, y, NULL);
	return StartingPos_x;
}
void DisplayCurrentCusorLineNumber(void)
{
	int x,y;
	DWORD StartingPos = GetCurrentCusorPosition(x,y);
	std::stringstream ss;
	ss << "Current line number = " << x << " " <<  y << " " << StartingPos ;
	ShowMessage(ss.str().c_str());
}

//---------------------------------------------------------------------------
// Functions to expert to PL/SQL Dev
//---------------------------------------------------------------------------
// Plug-In identification, a unique identifier is received and
// the description is returned
char* IdentifyPlugIn(int ID)
{
	PlugInID = ID;
	return Desc;
}
//---------------------------------------------------------------------------
//display an about dialog
char* About()
{
	std::stringstream ss;
	ss << Desc;
	ss <<"\n"<<"Version "<<DBSCC_VERSION;
	ss <<"\n"<<"Copyright 2005 Art-Master @ Matsak Sergei";
	ss <<"\n"<<"Compatible with DBScC Server version '"<<DBSCC_ServerVersion<<"x'";
    ss <<"\n"<<(IsDBSCC_Server_Active()?"Supported DBScC Server found":"Supported DBScC Server not found");
	
	ShowMessage(ss.str().c_str());
	return NULL;
}
//---------------------------------------------------------------------------
// Creating menus
char* CreateMenuItem(int Index)
{
	return MenuItemInterface(Index, true);
}

//---------------------------------------------------------------------------
// Show configuration dialog
void  Configure()
{
	ShowMessage("No configue menu. All settings is in Tools / Plug-In DBScC / Setup / ");
}

//---------------------------------------------------------------------------
void OnMenuClick(int Index)
{
	MenuItemInterface(Index, false);
}
//---------------------------------------------------------------------------
// Called when someone logs on or off
void OnConnectionChange()
{
	if (IsDBSCC_Server_Active())
	{
		DBSCC_Active=true; 
	}
	else
	{
		DBSCC_Active=false;
	}
	IDE_RefreshMenus(PlugInID);
	IDE_MenuState(PlugInID,3,DBSCC_Active&&DBSCC_Enabled);
	IDE_MenuState(PlugInID,4,DBSCC_Active&&DBSCC_Enabled);
	IDE_MenuState(PlugInID,5,DBSCC_Active&&DBSCC_Enabled);
	IDE_MenuState(PlugInID,6,DBSCC_Active&&DBSCC_Enabled);
	IDE_MenuState(PlugInID,7,DBSCC_Active&&DBSCC_Enabled);
	IDE_MenuState(PlugInID,8,DBSCC_Active&&DBSCC_Enabled);
	LockedWindowsList.clear();
}
//---------------------------------------------------------------------------
// Called when another item in the browser gets selected
void OnBrowserChange()
{
 //char *Type, *Owner, *Name;

//  IDE_GetBrowserInfo(&Type, &Owner, &Name);
}
//---------------------------------------------------------------------------
// Called when the Plug-In is created
void OnCreate()
{ 
	char Buff[32]="";
	GetProfileString("CursorPosOption","UseAutoSetCursorPosition","0",Buff,sizeof(Buff));
	UseAutoSetCursorPosition = (atoi(Buff))?true:false;
	GetProfileString("DBSCC","Enabled","1",Buff,sizeof(Buff));
	DBSCC_Enabled = (atoi(Buff))?true:false;
	GetProfileString("DBSCC","AutoLock","1",Buff,sizeof(Buff));
	UseAutoLock = (atoi(Buff))?true:false;
	GetProfileString("DBSCC","AutoUnlock","1",Buff,sizeof(Buff));
	UseAutoUnlock = (atoi(Buff))?true:false;
	GetProfileString("DBSCC","SmartLabels","1",Buff,sizeof(Buff));
	UseSmartLabels = (atoi(Buff))?true:false;
	
}
//---------------------------------------------------------------------------
// Called when the Plug-In is activated
void OnActivate()
{
    char s[100];
	IDE_SplashWrite("\nStarting DBScC");
	IDE_CreateToolButton(PlugInID,6,"List all locks","PlugIns\\list_all_locks.bmp",0);
	IDE_CreateToolButton(PlugInID,7,"List all history","PlugIns\\list_all_history.bmp",0);
	IDE_CreateToolButton(PlugInID,8,"Unlock all my objects","PlugIns\\unlock_all_objects.bmp",0);
	IDE_CreateToolButton(PlugInID,11,"Recompile all invalid objects","PlugIns\\recompile_all_invalid.bmp",0);
	for (int i=0;i<MaxSupportedObjects;i++)
	{
		IDE_CreatePopupItem(PlugInID,3,"Lock",SupportedObjects[i]);
		IDE_CreatePopupItem(PlugInID,4,"Unlock",SupportedObjects[i]);
		IDE_CreatePopupItem(PlugInID,5,"Force unlock",SupportedObjects[i]);
		IDE_CreatePopupItem(PlugInID,6,"List all locks",SupportedObjects[i]);
		IDE_CreatePopupItem(PlugInID,7,"List all history",SupportedObjects[i]);
		//IDE_CreatePopupItem(PlugInID,12,"Beautify",SupportedObjects[i]);
   	    s[0] = 0;
      	strcat(s, SupportedObjects[i]);
      	strcat(s, "+");
		IDE_CreatePopupItem(PlugInID,12,"Beautify",s);
		IDE_SplashWrite(".");
	}
  OnBrowserChange();
  OnConnectionChange();
  OnWindowChange();
  IDE_SplashWriteLn("ok");
}
//---------------------------------------------------------------------------
// Called when the Plug-In is destroyed
void OnDestroy()
{
	WriteProfileString("CursorPosOption","UseAutoSetCursorPosition",IntToStr(UseAutoSetCursorPosition));
	WriteProfileString("DBSCC","Enabled",IntToStr(DBSCC_Enabled));
	WriteProfileString("DBSCC","AutoLock",IntToStr(UseAutoLock));
	WriteProfileString("DBSCC","AutoUnlock",IntToStr(UseAutoUnlock));
	WriteProfileString("DBSCC","SmartLabels",IntToStr(UseSmartLabels));
}

//---------------------------------------------------------------------------
// Called before a window is closed
//return values:
//0 = Default behavior
//1 = Ask the user for confirmation (like the contents was changed)
//2 = Don’t ask, allow to close without confirmation
int OnWindowClose(int WindowType, BOOL Changed)
{
/*	int x,y;
	DWORD StartingPos = GetCurrentCusorPosition(x,y);
	std::string FileName = IDE_Filename();
	WriteProfileString("CursorPos",FileName.c_str(),IntToStr(y)); */
	if (DBSCC_Active&&DBSCC_Enabled&&UseAutoUnlock&&WindowType==3)
	{
		/*if (DBSCC_DebugMode)
		{
			char s[500];
			sprintf(s,"Close ClientHandle = %i, ChildHandle = %i",IDE_GetClientHandle(), IDE_GetChildHandle() );
			IDE_SplashWriteLn(s);
		}*/
		int n=LockedWindowsList.erase(IDE_GetChildHandle());
		if (n>0)
		{
			char *Type, *Owner, *Name, *SubObject;
			IDE_GetWindowObject(&Type,&Owner,&Name,&SubObject);
			UnlockObject(Type,Name);
			char *TabInfo=NULL;
			TabInfo=IDE_TabInfo(1); 
			if (strlen(TabInfo)>0)
			{
					if (strcmp(Type,"PACKAGE")==0)
					{
  					   UnlockObject("PACKAGE BODY",Name);
					}
					else if(strcmp(Type,"PACKAGE BODY")==0)
					{
  					   UnlockObject("PACKAGE",Name);
					}
			}
		if (DBSCC_DebugMode)
		{
			char s[500];
			sprintf(s,"Close Type=%s,Owner=%s,Name%s,Sub=%s",Type,Owner,Name,SubObject);
			IDE_SplashWriteLn(s);
		}
		}
	}
	if (DontConfirmClose)
	{
		DontConfirmClose=false;
        return 2;
	}
	else
	{
		return 0;
	}
}

//---------------------------------------------------------------------------
//Called directly after a new window is created.
void OnWindowCreate(int WindowType)
{
	WindowJustOpened = true;
}
void OnWindowCreated(int WindowType)
{
	if (DBSCC_Enabled&&DBSCC_Active)
	{
		if ((WindowType==3)&&(!IDE_GetReadOnly()))
		{
			
			char *Type, *Owner, *Name, *SubObject;
			IDE_GetWindowObject(&Type, &Owner, &Name, &SubObject);
			if (DBSCC_DebugMode)
			{
				//char s[2000];
				//sprintf(s,"Created Type=%s,Owner=%s,Name=%s,Sub=%s",Type,Owner,Name,SubObject);
				//IDE_SplashWriteLn(s);
				//sprintf(s,"IDE_TabInfo 0=%s,1=%s",IDE_TabInfo(0), IDE_TabInfo(1));
				//IDE_SplashWriteLn(s);
			}
			Type=_strupr(Type);
			Name=_strupr(Name);
               //ShowMessage(Type);
			if (IsDBSCCObjectType(Type))
			{
               //ShowMessage(Type);

				if (UseAutoLock) 
				{   
					//if ((strcmp(Type,"PACKAGE")==0)&&(!strcmp(IDE_TabInfo(1),"")==0))
					/*if (strlen(IDE_TabInfo(1))>0)&&((strcmp(Type,"PACKAGE")==0)||(strcmp(Type,"PACKAGE BODY")==0))
					{
  					  int res=LockObject("PACKAGE",Name);
  					  res=LockObject("PACKAGE BODY",Name);
					}
					else
					{
   					  int res=LockObject(Type,Name);
					}*/
					int res=LockObject(Type,Name);
                    int res1=0;
					char *TabInfo=NULL;
					TabInfo=IDE_TabInfo(1); 
					if (strlen(TabInfo)>0)
					{
						if (strcmp(Type,"PACKAGE")==0)
						{
  							res1=LockObject("PACKAGE BODY",Name);
						}
						else if (strcmp(Type,"PACKAGE BODY")==0)
						{
  							res1=LockObject("PACKAGE",Name);
						}
					}
			if (DBSCC_DebugMode)
			{
				char s[2000];
				sprintf(s,"res=%i res1=%i",res,res1);
				IDE_SplashWriteLn(s);
			}
					
					if ((res!=0)||(res1!=0))
					{
						if ((abs(res)==abs(c_errcode_object_locked))||(abs(res1)==abs(c_errcode_object_locked)))
						{
							IDE_SetReadOnly(true);
							std::stringstream ss;
							ss << "Can't lock:" << SQL_ErrorMessage();
							IDE_SetStatusMessage((char *)ss.str().c_str());
						}
						else if ((abs(res)==abs(c_errcode_object_not_exist))||(abs(res1)==abs(c_errcode_object_not_exist)))
						{
							IDE_SetStatusMessage("This is new object. Lock it after compile.");
						}
						else
						{
							std::stringstream ss;
							ss << "Can't lock:" << SQL_ErrorMessage();
							IDE_SetStatusMessage((char *)ss.str().c_str());
						}
					}
					else
					{
						IDE_SetStatusMessage("Successfully locked.");
						if (UseAutoUnlock)
						{
							LockedWindowsList.insert(IDE_GetChildHandle());
		//char s[500];
		//sprintf(s,"Open ClientHandle = %i, ChildHandle = %i",IDE_GetClientHandle(), IDE_GetChildHandle() );
		//IDE_SplashWriteLn(s);
		//sprintf(s,"Open Type=%s,Owner=%s,Name%s,Sub=%s",Type,Owner,Name,SubObject);
		//IDE_SplashWriteLn(s);
						}
					}
				}
				else
				{
					switch (abs(CanEdit(Type, Name)))
					{
					case 1:
						IDE_SetStatusMessage(LockInfo(Type, Name));
						break;
					case c_errcode_object_not_locked:
						IDE_SetReadOnly(true);
						IDE_SetStatusMessage("Object is read-only. Lock it before open editor.");
						break;
					case c_errcode_object_locked:
						IDE_SetReadOnly(true);
						std::stringstream ss;
						ss << "Object is read-only. Object is locked:" << LockInfo(Type, Name);
						IDE_SetStatusMessage((char *)ss.str().c_str());
						break;
					}
				}
			}	
		}
	}

}


//---------------------------------------------------------------------------
//Called if PL/SQL Developer child windows change focus.
void OnWindowChange()
{
/*	if (UseAutoSetCursorPosition && WindowJustOpened)
	{
//		WindowJustOpened = false;
		CString FileName = IDE_Filename();
		DWORD LastPos = theApp.GetProfileInt("CursorPos",FileName,1);
		HWND H = (HWND)IDE_GetEditorHandle();
		DWORD p = SendMessage(H, EM_LINEINDEX, LastPos, NULL);
		SendMessage(H, EM_SETSEL, p, p);
	}*/
	if (DBSCC_DebugMode)
	{
		char *Type, *Owner, *Name, *SubObject;
		IDE_GetWindowObject(&Type, &Owner, &Name, &SubObject);
		char s[4000];
		sprintf(s,"OnWindowChange Type=%s, Name=%s, SubObject=%s Owner=%s,", Type, Name, SubObject, Owner);
		IDE_SplashWriteLn(s);
	}
	WindowJustOpened = false;
}
//---------------------------------------------------------------------------
//Called when a context sensitive popup is about to be displayed
void  OnPopup(char *ObjectType, char *ObjectName)
{
	IDE_MenuState(PlugInID,3,DBSCC_Active&&DBSCC_Enabled);
	IDE_MenuState(PlugInID,4,DBSCC_Active&&DBSCC_Enabled);
	IDE_MenuState(PlugInID,5,DBSCC_Active&&DBSCC_Enabled);
	IDE_MenuState(PlugInID,6,DBSCC_Active&&DBSCC_Enabled);
	IDE_MenuState(PlugInID,7,DBSCC_Active&&DBSCC_Enabled);
	IDE_MenuState(PlugInID,8,DBSCC_Active&&DBSCC_Enabled);
	IDE_SetMenuName(PlugInID,3, "Lock");
	IDE_SetMenuName(PlugInID,4, "Unlock");
	if (!(DBSCC_Active&&DBSCC_Enabled))
	{
		return;
	}
	// Выполняем какие надо действия
	// нужно здесь менять значения надписям
	std::stringstream ss;
	char* lastunlock;
	switch (abs(CanEdit(ObjectType, ObjectName)))
	{
	case 1:
//		IDE_SetMenuName(PlugInID,3, );
		IDE_MenuState(PlugInID,3, false);
		if (UseSmartLabels)
		{
			IDE_SetMenuName(PlugInID,3, "Locked by you");
		}
   		break;
	case c_errcode_object_not_locked:
		IDE_MenuState(PlugInID,4, false);
		IDE_MenuState(PlugInID,5, false);
		if (UseSmartLabels)
		{
			lastunlock=LastUnlock(ObjectType, ObjectName);
			if (lastunlock!=NULL)
			{
				ss << "Unlocked " << lastunlock;
				IDE_SetMenuName(PlugInID,4, (char *)ss.str().c_str());
			}
		}
		break;
	case c_errcode_object_locked:
		IDE_MenuState(PlugInID,3, false);
		IDE_MenuState(PlugInID,4, false);
		if (UseSmartLabels)
		{
			ss << "Locked by " << LockInfo(ObjectType, ObjectName);
			IDE_SetMenuName(PlugInID,3, (char *)ss.str().c_str());
		}
		break;
	}
	

}
//---------------------------------------------------------------------------
// DBSCC basic actions
//---------------------------------------------------------------------------
int  CanEdit(char *Type, char *Name)
{
	char s[500];
	sprintf(s,"select DBSCC.can_edit('%s', '%s') res from dual",Type,Name);
	if (SQL_Execute(s)==0)
	{
		int idx=SQL_FieldIndex("res");
		char *r=SQL_Field(idx);
		if (!((r==NULL)||(strcmp(r,"")==0)))
		{
			return atoi(r);
		}
	}
	return 0;
}
char* LockInfo(char *Type, char *Name)
{
	char s[500];
	sprintf( s,"select DBSCC.ISLOCKED('%s', '%s') res from dual",Type,Name);
	SQL_Execute(s);
	if (SQL_Execute(s)==0)
	{
		int idx=SQL_FieldIndex("res");
		char *lockinfo=SQL_Field(idx);
		if (!((lockinfo==NULL)||(strcmp(lockinfo,"")==0)))
		{
			return lockinfo;
		}
	}
	return NULL;
}

char* LastUnlock(char *Type, char *Name)
{
	char s[500];
	sprintf( s,"select DBSCC.LAST_UNLOCK('%s', '%s') res from dual",Type,Name);
	SQL_Execute(s);
	if (SQL_Execute(s)==0)
	{
		int idx=SQL_FieldIndex("res");
		char *lockinfo=SQL_Field(idx);
		if (!((lockinfo==NULL)||(strcmp(lockinfo,"")==0)))
		{
			return lockinfo;
		}
	}
	return NULL;
}

bool ObjectExist(char *Type, char *Name)
{
	char s[500];
	sprintf(s,"select DBSCC.object_exist('%s', '%s') res from dual",Type,Name);
	if (SQL_Execute(s)==0)
	{
		int idx=SQL_FieldIndex("res");
		char *r=SQL_Field(idx);
		if (!((r==NULL)||(strcmp(r,"0")==0)))
		{
			return true;
		}
	}
	return false;
}
int LockObject(char *Type, char *Name)
{
	char s[500];
	sprintf(s,"begin DBSCC.LOCK_OBJECT('%s', '%s'); end;",Type,Name);
	return SQL_Execute(s);
}
//---------------------------------------------------------------------------
int UnlockObject(char *Type, char *Name, bool force)
{
	char s[500];
	sprintf(s,"begin DBSCC.UNLOCK_OBJECT('%s', '%s', '%s'); end;",Type,Name,force?"1":"0");
	return SQL_Execute(s);
}
//---------------------------------------------------------------------------
bool BeautifyObject(char *Type, char *Owner, char *Name)
{
	if (IDE_ObjectAction("EDITSPECANDBODY", Type, Owner, Name)) {
		IDE_BeautifyWindow();
		if (IDE_WindowIsModified()){
			if (DBSCC_Active&&DBSCC_Enabled) {
				LockObject(Type,Name);
			}
			IDE_Perform(pcmd_Execute);
			if (DBSCC_Active&&DBSCC_Enabled) {
				UnlockObject(Type,Name);
			}
		}
		DontConfirmClose=true;
		IDE_KeyPress(VK_F4, 4);
		DontConfirmClose=false;
		//int OnWindowClose(int WindowType, BOOL Changed)
	}
	return true;
}
//---------------------------------------------------------------------------
// DBSCC events
//---------------------------------------------------------------------------
int OnListAllLocks()
{
	return IDE_ExecuteSQLReport("select l.*,l.rowid from dbscc_locks l where l.state='L'","DBSCC Locks",true);
}
//---------------------------------------------------------------------------
int OnListAllHistory()
{
	return IDE_ExecuteSQLReport("select h.*,h.rowid from dbscc_history h order by h.opdate desc","DBSCC History",true);
}
//---------------------------------------------------------------------------
void OnPopUpLockObject()
{
	char *Type, *Owner, *Name, *SubObject;
	IDE_GetPopupObject(&Type,&Owner,&Name,&SubObject);
	int res=LockObject(Type,Name);
	if (res!=0)
	{
		std::stringstream ss;
		ss << "Can't lock:" << SQL_ErrorMessage();
		ShowMessage(ss.str().c_str());
	}
}
//---------------------------------------------------------------------------
void OnPopUpUnlockObject(bool force=false)
{
	char *Type, *Owner, *Name, *SubObject;
	IDE_GetPopupObject(&Type,&Owner,&Name,&SubObject);
	int res=UnlockObject(Type,Name,force);
	if (res!=0)
	{
		std::stringstream ss;
		ss << "Can't unlock:" << SQL_ErrorMessage();
		ShowMessage(ss.str().c_str());
	}
}
//---------------------------------------------------------------------------
void OnUnlockAllObjects()
{
	if (LockedWindowsList.size()>0) 
	{
		MessageBeep(-1);
		if (MessageBox(NULL,"Not all edit windows closed. Don't forget to reopen windows before you will lock it next time. \nContinue unlock all objects?","DBScC",MB_YESNO)!=IDYES)
		{
			return;
		}
		LockedWindowsList.clear();
	}
	char *Type, *Owner, *Name, *SubObject;
	IDE_GetPopupObject(&Type,&Owner,&Name,&SubObject);
	int res=SQL_Execute("begin DBSCC.UNLOCK_ALL_OBJECTS; end;");
	if (res!=0)
	{
		std::stringstream ss;
		ss << "Can't unlock:" << SQL_ErrorMessage();
		ShowMessage(ss.str().c_str());
	}
}
//---------------------------------------------------------------------------
void OnRecompileInvalid()
{
	//IDE_ExecuteSQLReport("select DBSCC.recompile_all_invalid_objects from dual","Recompile all invalid objects",false);
	IDE_CreateWindow(wtReportWindow,"select DBSCC.recompile_all_invalid_objects from dual",true);
}
//---------------------------------------------------------------------------
void OnPopUpBeautifyObjects()
{
	char *Type, *Owner, *Name, *SubObject;
	IDE_GetPopupObject(&Type,&Owner,&Name,&SubObject);
	BeautifyObject(Type,Owner,Name);
	/*if (IDE_FirstSelectedObject(Type,Owner,Name,SubObject)) {
		do 
		{
			BeautifyObject(Type,Owner,Name);
		}
		while (IDE_NextSelectedObject(Type,Owner,Name,SubObject));
	}*/
}
//---------------------------------------------------------------------------
char* MenuItemInterface(int Index, bool CreateMenuItem_)
{
	if (!(DBSCC_Active))
	{
		return NULL;
	}
	switch (Index)
	{
	case 1 :		
		if (CreateMenuItem_) return (!DBSCC_Enabled?"Tools / &Plug-In DBScC / Setup / &Enable DBSCC":"Tools / &Plug-In DBScC / Setup / &Disable DBSCC");
		DBSCC_Enabled=!DBSCC_Enabled;
		IDE_SetMenuName(PlugInID,1, (!DBSCC_Enabled?"&Enable DBScC":"&Disable DBScC"));
		OnConnectionChange();
		return NULL;
	case 2 :
		if (CreateMenuItem_) return (!UseAutoLock?"Tools / &Plug-In DBScC / Setup / Enable AutoLock":"Tools / &Plug-In DBScC / Setup / Disable AutoLock");
		UseAutoLock=!UseAutoLock;
		IDE_SetMenuName(PlugInID,2, (!UseAutoLock?"Enable AutoLock":"Disable AutoLock"));
		return NULL;
	case 3 :
		if (CreateMenuItem_) return NULL;
		OnPopUpLockObject();
		return NULL;
	case 4 :
		if (CreateMenuItem_) return NULL;
		OnPopUpUnlockObject();
		return NULL;
	case 5 :
		if (CreateMenuItem_) return NULL;
		OnPopUpUnlockObject(true);
		return NULL;
	case 6 :
		if (CreateMenuItem_) return "Tools / &Plug-In DBScC / List all locks";
		OnListAllLocks();
		return NULL;
	case 7 :
		if (CreateMenuItem_) return "Tools / &Plug-In DBScC / List all history";
		OnListAllHistory();
		return NULL;
	case 8 :
		if (CreateMenuItem_) return "Tools / &Plug-In DBScC / Unlock all my objects";
		OnUnlockAllObjects();
		return NULL;
	case 9 :
		if (CreateMenuItem_) return (!UseAutoUnlock?"Tools / &Plug-In DBScC / Setup / Enable AutoUnlock":"Tools / &Plug-In DBScC / Setup /Disable AutoUnlock");
		UseAutoUnlock=!UseAutoUnlock;
		IDE_SetMenuName(PlugInID,9, (!UseAutoUnlock?"Enable AutoUnlock":"Disable AutoUnlock"));
		return NULL;
	case 10 :
		if (CreateMenuItem_) return (!UseSmartLabels?"Tools / &Plug-In DBScC / Setup / Enable SmartLabels":"Tools / &Plug-In DBScC / Setup /Disable SmartLabels");
		UseSmartLabels=!UseSmartLabels;
		IDE_SetMenuName(PlugInID,10, (!UseSmartLabels?"Enable SmartLabels":"Disable SmartLabels"));
		return NULL;
	case 11 :
		if (CreateMenuItem_) return "Tools / &Plug-In DBScC / Recompile all invalid objects";
		OnRecompileInvalid();
		return NULL;
	case 12 :
		if (CreateMenuItem_) return NULL;
		OnPopUpBeautifyObjects();
		return NULL;
	case 13 :
		if (CreateMenuItem_) return "Tools / &Plug-In DBScC / Debug OFF";
		DBSCC_DebugMode=!DBSCC_DebugMode;
		IDE_SetMenuName(PlugInID,13, (DBSCC_DebugMode?"Debug ON":"Debug OFF"));
		if (DBSCC_DebugMode)
		{
			IDE_SplashHide();
			IDE_SplashCreate(0);
			char s[500];
			sprintf(s,"Count = %i",LockedWindowsList.size() );
			IDE_SplashWriteLn(s);
		}
		else
		{
			IDE_SplashHide();
		}
		return NULL;

/*
	case 4 :
		if (CreateMenuItem_) return "Tools / &Plug-In 2 Demo / &ReadOnly / &On";
		IDE_SetReadOnly(true);
		return NULL;
    case 5 :
		if (CreateMenuItem_) return "Tools / &Plug-In 2 Demo / &ReadOnly / O&ff";
		IDE_SetReadOnly(false);
		return NULL;
*/
/*
	case 8 :
		if (CreateMenuItem_) return "Tools / &Plug-In DBScC / &Auto Cursor / &On";
		UseAutoSetCursorPosition = true;
		return NULL;
    case 9 :
		if (CreateMenuItem_) return "Tools / &Plug-In 2 DBSCC / &Auto Cursor / O&ff";
		UseAutoSetCursorPosition = false;
		return NULL;
*/
	}
	return "";
}
