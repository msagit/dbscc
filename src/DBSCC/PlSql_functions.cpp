// PL/SQL Developer Plug-In DBSCC
// Copyright 2005 Art-Master 
// Developed by Matsak Sergei (matsak@wp.pl)

// Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "PlSql_functions.h"
#include "dbscc.h"
#include <string>
#include <sstream>


// Declaration of PL/SQL Developer callback functions
int   (*funcptr_SYS_Version)() = NULL;
char* (*funcptr_SYS_Registry)() = NULL;
char* (*funcptr_SYS_RootDir)() = NULL;
char* (*funcptr_SYS_OracleHome)() = NULL;
void  (*funcptr_IDE_MenuState)(int ID, int Index, BOOL Enabled) = NULL;
BOOL  (*funcptr_IDE_Connected)() = NULL;
void  (*funcptr_IDE_GetConnectionInfo)(char **Username, char **Password, char **Database) = NULL;
void  (*funcptr_IDE_GetBrowserInfo)(char **ObjectType, char **ObjectOwner, char **ObjectName) = NULL;
int   (*funcptr_IDE_GetWindowType)() = NULL;
int   (*funcptr_IDE_GetAppHandle)() = NULL;
int   (*funcptr_IDE_GetWindowHandle)() = NULL;
int   (*funcptr_IDE_GetClientHandle)() = NULL;
int   (*funcptr_IDE_GetChildHandle)() = NULL;
void  (*funcptr_IDE_CreateWindow)(int WindowType, char *Text, BOOL Execute) = NULL;
BOOL  (*funcptr_IDE_OpenFile)(int WindowType, char *Filename) = NULL;
BOOL  (*funcptr_IDE_SaveFile)() = NULL;
char* (*funcptr_IDE_Filename)() = NULL;
void  (*funcptr_IDE_CloseFile)() = NULL;
void  (*funcptr_IDE_SetReadOnly)(BOOL ReadOnly) = NULL;
char* (*funcptr_IDE_GetText)() = NULL;
char* (*funcptr_IDE_GetSelectedText)() = NULL;
char* (*funcptr_IDE_GetCursorWord)() = NULL;
int   (*funcptr_IDE_GetEditorHandle)() = NULL;
BOOL  (*funcptr_IDE_SetText)(char *Text) = NULL;//34*******************
BOOL  (*funcptr_IDE_SetStatusMessage)(char *Text) = NULL;//35*******************
BOOL  (*funcptr_IDE_SetErrorPosition)(int Line, int Col) = NULL;//36*******************
void  (*funcptr_IDE_ClearErrorPositions)() = NULL;//37*******************
int   (*funcptr_IDE_GetCursorWordPosition)() = NULL;//38*******************
BOOL  (*funcptr_IDE_Perform)(int Param) = NULL;//
int   (*funcptr_SQL_Execute)(char *SQL) = NULL;
int   (*funcptr_SQL_FieldCount)() = NULL;
BOOL  (*funcptr_SQL_Eof)() = NULL;
int   (*funcptr_SQL_Next)() = NULL;
char* (*funcptr_SQL_Field)(int Field) = NULL;
char* (*funcptr_SQL_FieldName)(int Field) = NULL;
int   (*funcptr_SQL_FieldIndex)(char *Name) = NULL;
int   (*funcptr_SQL_FieldType)(int Field) = NULL;

BOOL  (*funcptr_IDE_GetWindowObject)(char **ObjectType, char **ObjectOwner, char **ObjectName, char **SubObject) = NULL;
void  (*funcptr_IDE_CreateToolButton)(int ID, int Index, char *Name, char *BitmapFile, int BitmapHandle) = NULL;
void  (*funcptr_IDE_SetMenuCheck)(int ID, int Index, BOOL Enabled) = NULL;
char* (*funcptr_SQL_ErrorMessage)() = NULL;
BOOL  (*funcptr_IDE_GetReadOnly)() = NULL;
void  (*funcptr_IDE_SetMenuName)(int ID, int Index, char *Name) = NULL;
void* (*funcptr_IDE_CreatePopupItem)(int ID, int Index, char *Name, char *ObjectType) = NULL;
int   (*funcptr_IDE_GetPopupObject)(char **ObjectType, char **ObjectOwner, char **ObjectName, char **SubObject) = NULL;
BOOL  (*funcptr_IDE_ExecuteSQLReport)(char *SQL, char *Title, BOOL Updateable) = NULL;
void  (*funcptr_IDE_RefreshMenus)(int ID) = NULL;
int   (*funcptr_IDE_GetWindowCount)() = NULL;
void  (*funcptr_IDE_SplashCreate)(int ProgressMax) = NULL;
void  (*funcptr_IDE_SplashHide)() = NULL;
void  (*funcptr_IDE_SplashWrite)(char *s) = NULL;
void  (*funcptr_IDE_SplashWriteLn)(char *s) = NULL;
void  (*funcptr_IDE_SplashProgress)(int Progress) = NULL;
void  (*funcptr_IDE_SetMenuVisible)(int ID, int Index, BOOL Enabled) = NULL;
BOOL  (*funcptr_IDE_ObjectAction)(char *Action, char *ObjectType,char *ObjectOwner, char *ObjectName) = NULL;
BOOL  (*funcptr_IDE_BeautifyWindow)() = NULL;
BOOL  (*funcptr_IDE_WindowIsModified)() = NULL;
void  (*funcptr_IDE_KeyPress)(int Key, int Shift) = NULL;
BOOL  (*funcptr_IDE_FirstSelectedObject) (char *ObjectType,char *ObjectOwner, char *ObjectName, char *SubObject) = NULL;
BOOL  (*funcptr_IDE_NextSelectedObject) (char *ObjectType,char *ObjectOwner, char *ObjectName, char *SubObject) = NULL;
char* (*funcptr_IDE_TabInfo) (int Index) = NULL;
int   (*funcptr_IDE_TabIndex) (int Index) = NULL;

void RegisterCallback(int Index, void *Addr)
{
	switch (Index)
	{
    case 1 :
		memcpy(&funcptr_SYS_Version, &Addr, sizeof(Addr));
		break;
    case 2 :
		memcpy(&funcptr_SYS_Registry, &Addr, sizeof(Addr));
		break;
    case 3 :
		memcpy(&funcptr_SYS_RootDir, &Addr, sizeof(Addr));
		break;
    case 4 :
		memcpy(&funcptr_SYS_OracleHome, &Addr, sizeof(Addr));
		break;
    case 10 :
		memcpy(&funcptr_IDE_MenuState, &Addr, sizeof(Addr));
		break;
    case 11 :
		memcpy(&funcptr_IDE_Connected, &Addr, sizeof(Addr));
		break;
    case 12 :
		memcpy(&funcptr_IDE_GetConnectionInfo, &Addr, sizeof(Addr));
		break;
    case 13 :
		memcpy(&funcptr_IDE_GetBrowserInfo, &Addr, sizeof(Addr));
		break;
    case 14 :
		memcpy(&funcptr_IDE_GetWindowType, &Addr, sizeof(Addr));
		break;
    case 15 :
		memcpy(&funcptr_IDE_GetAppHandle, &Addr, sizeof(Addr));
		break;
    case 16 :
		memcpy(&funcptr_IDE_GetWindowHandle, &Addr, sizeof(Addr));
		break;
    case 17 :
		memcpy(&funcptr_IDE_GetClientHandle, &Addr, sizeof(Addr));
		break;
    case 18 :
		memcpy(&funcptr_IDE_GetChildHandle, &Addr, sizeof(Addr));
		break;
    case 20 :
		memcpy(&funcptr_IDE_CreateWindow, &Addr, sizeof(Addr));
		break;
    case 21 :
		memcpy(&funcptr_IDE_OpenFile, &Addr, sizeof(Addr));
		break;
    case 22 :
		memcpy(&funcptr_IDE_SaveFile, &Addr, sizeof(Addr));
		break;
    case 23 :
		memcpy(&funcptr_IDE_Filename, &Addr, sizeof(Addr));
		break;
    case 24 :
		memcpy(&funcptr_IDE_CloseFile, &Addr, sizeof(Addr));
		break;
    case 25 :
		memcpy(&funcptr_IDE_SetReadOnly, &Addr, sizeof(Addr));
		break;
	case 26 :
		memcpy(&funcptr_IDE_GetReadOnly, &Addr, sizeof(Addr));
		break;
	case 27 :
		memcpy(&funcptr_IDE_ExecuteSQLReport, &Addr, sizeof(Addr));
		break;
    case 30 :
		memcpy(&funcptr_IDE_GetText, &Addr, sizeof(Addr));
		break;
    case 31 :
		memcpy(&funcptr_IDE_GetSelectedText, &Addr, sizeof(Addr));
		break;
    case 32 :
		memcpy(&funcptr_IDE_GetCursorWord, &Addr, sizeof(Addr));
		break;
    case 33 :
		memcpy(&funcptr_IDE_GetEditorHandle, &Addr, sizeof(Addr));
		break;
    case 34 :
		memcpy(&funcptr_IDE_SetText, &Addr, sizeof(Addr));
		break;
    case 35 :
		memcpy(&funcptr_IDE_SetStatusMessage, &Addr, sizeof(Addr));
		break;
    case 36 :
		memcpy(&funcptr_IDE_SetErrorPosition, &Addr, sizeof(Addr));
		break;
    case 37 :
		memcpy(&funcptr_IDE_ClearErrorPositions, &Addr, sizeof(Addr));
		break;
    case 38 :
		memcpy(&funcptr_IDE_GetCursorWordPosition, &Addr, sizeof(Addr));
		break;
    case 39 : //????
		memcpy(&funcptr_IDE_Perform, &Addr, sizeof(Addr));
		break;
    case 40 :
		memcpy(&funcptr_SQL_Execute, &Addr, sizeof(Addr));
		break;
    case 41 :
		memcpy(&funcptr_SQL_FieldCount, &Addr, sizeof(Addr));
		break;
    case 42 :
		memcpy(&funcptr_SQL_Eof, &Addr, sizeof(Addr));
		break;
    case 43 :
		memcpy(&funcptr_SQL_Next, &Addr, sizeof(Addr));
		break;
    case 44 :
		memcpy(&funcptr_SQL_Field, &Addr, sizeof(Addr));
		break;
    case 45 :
		memcpy(&funcptr_SQL_FieldName, &Addr, sizeof(Addr));
		break;
    case 46 :
		memcpy(&funcptr_SQL_FieldIndex, &Addr, sizeof(Addr));
		break;
    case 47 :
		memcpy(&funcptr_SQL_FieldType, &Addr, sizeof(Addr));
		break;
    case 48 :
		memcpy(&funcptr_SQL_ErrorMessage, &Addr, sizeof(Addr));
		break;
    case 64 :
		memcpy(&funcptr_IDE_RefreshMenus, &Addr, sizeof(Addr));
		break;
	case 65 :
		memcpy(&funcptr_IDE_SetMenuName, &Addr, sizeof(Addr));
		break;
    case 66 :		
		memcpy(&funcptr_IDE_SetMenuCheck, &Addr, sizeof(Addr));
		break;
    case 67 :		
		memcpy(&funcptr_IDE_SetMenuVisible, &Addr, sizeof(Addr));
		break;
    case 69 :		
		memcpy(&funcptr_IDE_CreatePopupItem, &Addr, sizeof(Addr));
		break;
	case 74 :		
		memcpy(&funcptr_IDE_GetPopupObject, &Addr, sizeof(Addr));
		break;		
	case 77 :		
		memcpy(&funcptr_IDE_FirstSelectedObject, &Addr, sizeof(Addr));
		break;		
	case 78 :		
		memcpy(&funcptr_IDE_NextSelectedObject, &Addr, sizeof(Addr));
		break;		
	case 80 :		
		memcpy(&funcptr_IDE_GetWindowCount, &Addr, sizeof(Addr));
		break;		
	case 83 :		
		memcpy(&funcptr_IDE_WindowIsModified, &Addr, sizeof(Addr));
		break;		
    case 90 :
		memcpy(&funcptr_IDE_SplashCreate, &Addr, sizeof(Addr));
		break;
    case 91 :
		memcpy(&funcptr_IDE_SplashHide, &Addr, sizeof(Addr));
		break;
    case 92 :
		memcpy(&funcptr_IDE_SplashWrite, &Addr, sizeof(Addr));
		break;
    case 93 :
		memcpy(&funcptr_IDE_SplashWriteLn, &Addr, sizeof(Addr));
		break;
    case 94 :
		memcpy(&funcptr_IDE_SplashProgress, &Addr, sizeof(Addr));
		break;
    case 110 :
		memcpy(&funcptr_IDE_GetWindowObject, &Addr, sizeof(Addr));
		break;
    case 120 :
		memcpy(&funcptr_IDE_KeyPress, &Addr, sizeof(Addr));
		break;

    case 148 :
		memcpy(&funcptr_IDE_TabInfo, &Addr, sizeof(Addr));
		break;
    case 149 :
		memcpy(&funcptr_IDE_TabIndex, &Addr, sizeof(Addr));
		break;
    
    case 150 :		
		memcpy(&funcptr_IDE_CreateToolButton, &Addr, sizeof(Addr));
		break;
    case 161 :		
		memcpy(&funcptr_IDE_BeautifyWindow, &Addr, sizeof(Addr));
		break;		
    case 165 :		
		memcpy(&funcptr_IDE_ObjectAction, &Addr, sizeof(Addr));
		break;		
	}
}

void ErrorMessage(const char* ErrMsg)
{
	std::stringstream ss;
	ss << "Error:  (" << ErrMsg << ") Function not initialized!!!" ;
	MessageBox(NULL, ss.str().c_str(), NULL, 0);
}
int   SYS_Version(){if (!funcptr_SYS_Version) {ErrorMessage("SYS_Version");return NULL;}return funcptr_SYS_Version();}
char* SYS_Registry(){if (!funcptr_SYS_Registry) {ErrorMessage("SYS_Registry");return NULL;}return funcptr_SYS_Registry();}
char* SYS_RootDir(){if (!funcptr_SYS_RootDir) {ErrorMessage("SYS_RootDir");return NULL;}return funcptr_SYS_RootDir();}
char* SYS_OracleHome(){if (!funcptr_SYS_OracleHome) {ErrorMessage("SYS_OracleHome");return NULL;}return funcptr_SYS_OracleHome();}
int   IDE_GetWindowType(){if (!funcptr_IDE_GetWindowType) {ErrorMessage("IDE_GetWindowType");return NULL;}return funcptr_IDE_GetWindowType();}
int   IDE_GetAppHandle(){if (!funcptr_IDE_GetAppHandle) {ErrorMessage("IDE_GetAppHandle");return NULL;}return funcptr_IDE_GetAppHandle();}
int   IDE_GetWindowHandle(){if (!funcptr_IDE_GetWindowHandle) {ErrorMessage("IDE_GetWindowHandle");return NULL;}return funcptr_IDE_GetWindowHandle();}
int   IDE_GetClientHandle(){if (!funcptr_IDE_GetClientHandle) {ErrorMessage("IDE_GetClientHandle");return NULL;}return funcptr_IDE_GetClientHandle();}
int   IDE_GetChildHandle(){if (!funcptr_IDE_GetChildHandle) {ErrorMessage("IDE_GetChildHandle");return NULL;}return funcptr_IDE_GetChildHandle();}
BOOL  IDE_Connected(){if (!funcptr_IDE_Connected) {ErrorMessage("IDE_Connected");return NULL;}return funcptr_IDE_Connected();}
BOOL  IDE_SaveFile(){if (!funcptr_IDE_SaveFile) {ErrorMessage("IDE_SaveFile");return NULL;}return funcptr_IDE_SaveFile();}
char* IDE_Filename(){if (!funcptr_IDE_Filename) {ErrorMessage("IDE_Filename");return NULL;}return funcptr_IDE_Filename();}
void  IDE_CloseFile(){if (!funcptr_IDE_CloseFile) {ErrorMessage("IDE_CloseFile");return;}funcptr_IDE_CloseFile();}
char* IDE_GetText(){if (!funcptr_IDE_GetText) {ErrorMessage("IDE_GetText");return NULL;}return funcptr_IDE_GetText();}
char* IDE_GetSelectedText(){if (!funcptr_IDE_GetSelectedText) {ErrorMessage("IDE_GetSelectedText");return NULL;}return funcptr_IDE_GetSelectedText();}
char* IDE_GetCursorWord(){if (!funcptr_IDE_GetCursorWord) {ErrorMessage("IDE_GetCursorWord");return NULL;}return funcptr_IDE_GetCursorWord();}
int   IDE_GetEditorHandle(){if (!funcptr_IDE_GetEditorHandle) {ErrorMessage("IDE_GetEditorHandle");return NULL;}return funcptr_IDE_GetEditorHandle();}
int   SQL_FieldCount(){if (!funcptr_SQL_FieldCount) {ErrorMessage("SQL_FieldCount");return NULL;}return funcptr_SQL_FieldCount();}
BOOL  SQL_Eof(){if (!funcptr_SQL_Eof) {ErrorMessage("SQL_Eof");return NULL;}return funcptr_SQL_Eof();}
int   SQL_Next(){if (!funcptr_SQL_Next) {ErrorMessage("SQL_Next");return NULL;}return funcptr_SQL_Next();}
void  IDE_ClearErrorPositions(){if (!funcptr_IDE_ClearErrorPositions) {ErrorMessage("IDE_ClearErrorPositions");return;} funcptr_IDE_ClearErrorPositions();}
int   IDE_GetCursorWordPosition(){if (!funcptr_IDE_GetCursorWordPosition) {ErrorMessage("IDE_GetCursorWordPosition");return NULL;}return funcptr_IDE_GetCursorWordPosition();}

void  IDE_MenuState(int ID, int Index, BOOL Enabled)
{
	if (!funcptr_IDE_MenuState) 
	{
		ErrorMessage("IDE_MenuState");
		return;
	}
	funcptr_IDE_MenuState(ID, Index, Enabled);
}
void  IDE_GetConnectionInfo(char **Username, char **Password, char **Database)
{
	if (!funcptr_IDE_GetConnectionInfo) {ErrorMessage("IDE_GetConnectionInfo");return;}
	funcptr_IDE_GetConnectionInfo(Username, Password, Database);
}
void  IDE_GetBrowserInfo(char **ObjectType, char **ObjectOwner, char **ObjectName)
{
	if (!funcptr_IDE_GetBrowserInfo) {ErrorMessage("IDE_GetBrowserInfo");return;}
	funcptr_IDE_GetBrowserInfo(ObjectType, ObjectOwner, ObjectName);
}
void  IDE_CreateWindow(WINTYPE WindowType, char *Text, BOOL Execute)
{
	if (!funcptr_IDE_CreateWindow) {ErrorMessage("IDE_CreateWindow");return;}
	funcptr_IDE_CreateWindow(WindowType, Text, Execute);
}
BOOL  IDE_OpenFile(WINTYPE WindowType, char *Filename)
{
	if (!funcptr_IDE_OpenFile) {ErrorMessage("IDE_OpenFile");return NULL;}
	return funcptr_IDE_OpenFile(WindowType, Filename);
}
void  IDE_SetReadOnly(BOOL ReadOnly)
{
	if (!funcptr_IDE_SetReadOnly) {ErrorMessage("IDE_SetReadOnly");return;}
	funcptr_IDE_SetReadOnly(ReadOnly);
}
int   SQL_Execute(char *SQL)
{
	if (!funcptr_SQL_Execute) {ErrorMessage("SQL_Execute");return NULL;}
	return funcptr_SQL_Execute(SQL);
}
char* SQL_Field(int Field)
{
	if (!funcptr_SQL_Field) {ErrorMessage("SQL_Field");return NULL;}
	return funcptr_SQL_Field(Field);
}
char* SQL_FieldName(int Field)
{
	if (!funcptr_SQL_FieldName) {ErrorMessage("SQL_FieldName");return NULL;}
	return funcptr_SQL_FieldName(Field);
}
int   SQL_FieldIndex(char *Name)
{
	if (!funcptr_SQL_FieldIndex) {ErrorMessage("SQL_FieldIndex");return NULL;}
	return funcptr_SQL_FieldIndex(Name);
}
int   SQL_FieldType(int Field)
{
	if (!funcptr_SQL_FieldType) {ErrorMessage("SQL_FieldType");return NULL;}
	return funcptr_SQL_FieldType(Field);
}
//Newly added functions
BOOL IDE_Perform(PCMD Param)
{
	if (!funcptr_IDE_Perform) {ErrorMessage("IDE_Perform");return NULL;}
	return funcptr_IDE_Perform(Param);
/*This function allows you to perform a specific action as if the menu item as specified in Param was selected. The following values are supported:
  1: Execute
  2: Break
  3: Kill
  4: Commit
  5: Rollback
  6: Print
*/
}
BOOL  IDE_SetText(char *Text)
{
	if (!funcptr_IDE_SetText) {ErrorMessage("IDE_SetText");return NULL;}
	return funcptr_IDE_SetText(Text);
}
BOOL  IDE_SetStatusMessage(char *Text)
{
	if (!funcptr_IDE_SetStatusMessage) {ErrorMessage("IDE_SetStatusMessage");return NULL;}
	return funcptr_IDE_SetStatusMessage(Text);
}
BOOL  IDE_SetErrorPosition(int Line, int Col)
{
	if (!funcptr_IDE_SetErrorPosition) {ErrorMessage("IDE_SetErrorPosition");return NULL;}
	return funcptr_IDE_SetErrorPosition(Line, Col);
}

//My Functions
BOOL  IDE_GetWindowObject(char **ObjectType, char **ObjectOwner, char **ObjectName, char **SubObject)
{
	if (!funcptr_IDE_GetWindowObject) {ErrorMessage("IDE_GetWindowObject");return NULL;}
	return funcptr_IDE_GetWindowObject(ObjectType, ObjectOwner, ObjectName, SubObject);
}

void  IDE_CreateToolButton(int ID, int Index, char *Name, char *BitmapFile, int BitmapHandle)
{
	if (!funcptr_IDE_CreateToolButton) {ErrorMessage("IDE_CreateToolButton");return;}
	funcptr_IDE_CreateToolButton(ID, Index, Name, BitmapFile, BitmapHandle);
}
void  IDE_SetMenuCheck(int ID, int Index, BOOL Enabled)
{
	if (!funcptr_IDE_SetMenuCheck) {ErrorMessage("IDE_SetMenuCheck");return;}
	funcptr_IDE_SetMenuCheck(ID, Index, Enabled);
}
char* SQL_ErrorMessage()
{
	if (!funcptr_SQL_ErrorMessage) {ErrorMessage("SQL_ ErrorMessage");return NULL;}
	return funcptr_SQL_ErrorMessage();
}
BOOL  IDE_GetReadOnly()
{
	if (!funcptr_IDE_GetReadOnly) {ErrorMessage("IDE_GetReadOnly");return NULL;}
	return funcptr_IDE_GetReadOnly();
}
void  IDE_SetMenuName(int ID, int Index, char *Name)
{
	if (!funcptr_IDE_SetMenuName) {ErrorMessage("IDE_SetMenuName");return;}
	funcptr_IDE_SetMenuName(ID, Index, Name);
}
void* IDE_CreatePopupItem(int ID, int Index, char *Name, char *ObjectType)
{
	if (!funcptr_IDE_CreatePopupItem) {ErrorMessage("IDE_CreatePopupItem");return NULL;}
	return funcptr_IDE_CreatePopupItem(ID, Index, Name, ObjectType);
};
int   IDE_GetPopupObject(char **ObjectType, char **ObjectOwner, char **ObjectName, char **SubObject)
{
	if (!funcptr_IDE_GetPopupObject) {ErrorMessage("IDE_GetPopupObject");return NULL;}
	return funcptr_IDE_GetPopupObject(ObjectType, ObjectOwner, ObjectName, SubObject);
};
BOOL  IDE_ExecuteSQLReport(char *SQL, char *Title, BOOL Updateable)
{
	if (!funcptr_IDE_ExecuteSQLReport) {ErrorMessage("IDE_ExecuteSQLReport");return NULL;}
	return funcptr_IDE_ExecuteSQLReport(SQL,Title,Updateable);
}
void  IDE_RefreshMenus(int ID)
{
	if (!funcptr_IDE_RefreshMenus) {ErrorMessage("IDE_RefreshMenus");return;}
	funcptr_IDE_RefreshMenus(ID);
}
int   IDE_GetWindowCount()
{
	if (!funcptr_IDE_GetWindowCount) {ErrorMessage("IDE_GetWindowCount");return NULL;}
	return funcptr_IDE_GetWindowCount();
}
void  IDE_SplashCreate(int ProgressMax)
{
	if (!funcptr_IDE_SplashCreate) {ErrorMessage("IDE_SplashCreate");return;}
	funcptr_IDE_SplashCreate(ProgressMax);
}
void  IDE_SplashHide()
{
	if (!funcptr_IDE_SplashHide) {ErrorMessage("IDE_SplashHide");return;}
	funcptr_IDE_SplashHide();
}
void  IDE_SplashWrite(char *s)
{
	if (!funcptr_IDE_SplashWrite) {ErrorMessage("IDE_SplashWrite");return;}
	funcptr_IDE_SplashWrite(s);
}
void  IDE_SplashWriteLn(char *s)
{
	if (!funcptr_IDE_SplashWriteLn) {ErrorMessage("IDE_SplashWriteLn");return;}
	funcptr_IDE_SplashWriteLn(s);
}
void  IDE_SplashProgress(int Progress)
{
	if (!funcptr_IDE_SplashProgress) {ErrorMessage("IDE_SplashProgress");return;}
	funcptr_IDE_SplashProgress(Progress);
}
void  IDE_SetMenuVisible(int ID, int Index, BOOL Enabled)
{
	if (!funcptr_IDE_SetMenuVisible) {ErrorMessage("IDE_SetMenuVisible");return;}
	funcptr_IDE_SetMenuVisible(ID, Index, Enabled);
}
BOOL  IDE_ObjectAction(char *Action, char *ObjectType,char *ObjectOwner, char *ObjectName)
{
	if (!funcptr_IDE_ObjectAction) {ErrorMessage("IDE_ObjectAction");return NULL;}
	return funcptr_IDE_ObjectAction(Action,ObjectType,ObjectOwner,ObjectName);
}

BOOL IDE_BeautifyWindow()
{
	if (!funcptr_IDE_BeautifyWindow) {ErrorMessage("IDE_BeautifyWindow");return NULL;}
	return funcptr_IDE_BeautifyWindow();
}
BOOL  IDE_WindowIsModified()
{
	if (!funcptr_IDE_WindowIsModified) {ErrorMessage("IDE_WindowIsModified");return NULL;}
	return funcptr_IDE_WindowIsModified();
}
void  IDE_KeyPress(int Key, int Shift)
{
	if (!funcptr_IDE_KeyPress) {ErrorMessage("IDE_KeyPress");return;}
	funcptr_IDE_KeyPress(Key, Shift);
}
BOOL  IDE_FirstSelectedObject (char *ObjectType,char *ObjectOwner, char *ObjectName, char *SubObject)
{
	if (!funcptr_IDE_FirstSelectedObject) {ErrorMessage("IDE_FirstSelectedObject");return NULL;}
	return funcptr_IDE_FirstSelectedObject(ObjectType,ObjectOwner,ObjectName,SubObject);
}
BOOL  IDE_NextSelectedObject (char *ObjectType,char *ObjectOwner, char *ObjectName, char *SubObject)
{
	if (!funcptr_IDE_NextSelectedObject) {ErrorMessage("IDE_NextSelectedObject");return NULL;}
	return funcptr_IDE_NextSelectedObject(ObjectType,ObjectOwner,ObjectName,SubObject);
}

char* IDE_TabInfo (int Index)
{
	if (!funcptr_IDE_TabInfo) {ErrorMessage("IDE_TabInfo");return NULL;}
	return funcptr_IDE_TabInfo(Index);
}

int IDE_TabIndex (int Index)
{
	if (!funcptr_IDE_TabIndex) {ErrorMessage("IDE_TabIndex");return NULL;}
	return funcptr_IDE_TabIndex(Index);
}

