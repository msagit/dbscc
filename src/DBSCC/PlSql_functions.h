// PL/SQL Developer Plug-In DBSCC
// Copyright 2005 Art-Master 
// Developed by Matsak Sergei (matsak@wp.pl)

#if !defined(HELPER_H_INCLUDED_)
#define HELPER_H_INCLUDED_

extern char *const Desc;
extern int PlugInID;

// Window types
enum WINTYPE{wtSQL = 1, wtTest, wtProcEdit, wtCommand, wtPlan, wtReportWindow};
enum PCMD{pcmd_Execute = 1, pcmd_Break, pcmd_Kill, pcmd_Commit, pcmd_Rollback, pcmd_Print};

// Declaration of PL/SQL Developer callback functions
int   SYS_Version();
char* SYS_Registry();
char* SYS_RootDir();
char* SYS_OracleHome();
void  IDE_MenuState(int ID, int Index, BOOL Enabled);
BOOL  IDE_Connected();
void  IDE_GetConnectionInfo(char **Username, char **Password, char **Database);
void  IDE_GetBrowserInfo(char **ObjectType, char **ObjectOwner, char **ObjectName);
int   IDE_GetWindowType();
int   IDE_GetAppHandle();
int   IDE_GetWindowHandle();
int   IDE_GetClientHandle();
int   IDE_GetChildHandle();
void  IDE_CreateWindow(WINTYPE WindowType, char *Text, BOOL Execute);
BOOL  IDE_OpenFile(WINTYPE WindowType, char *Filename);
BOOL  IDE_SaveFile();
char* IDE_Filename();
void  IDE_CloseFile();
void  IDE_SetReadOnly(BOOL ReadOnly);
char* IDE_GetText();
char* IDE_GetSelectedText();
char* IDE_GetCursorWord();
int   IDE_GetEditorHandle();
BOOL  IDE_SetText(char *Text);//34*******************
BOOL  IDE_SetStatusMessage(char *Text);//35*******************
BOOL  IDE_SetErrorPosition(int Line, int Col);//36*******************
void  IDE_ClearErrorPositions();//37*******************
int   IDE_GetCursorWordPosition();//38*******************
int   SQL_Execute(char *SQL);
int   SQL_FieldCount();
BOOL  SQL_Eof();
int   SQL_Next();
char* SQL_Field(int Field);
char* SQL_FieldName(int Field);
int   SQL_FieldIndex(char *Name);
int   SQL_FieldType(int Field);
BOOL IDE_Perform(PCMD Param);
bool FileExist(const char* FileName);
bool FileExistViaAccess(const char* FileName);
bool IsDirectory(const char* FileName);

//My Functions
BOOL  IDE_GetWindowObject(char **ObjectType, char **ObjectOwner, char **ObjectName, char **SubObject);
void  IDE_CreateToolButton(int ID, int Index, char *Name, char *BitmapFile, int BitmapHandle);
void  IDE_SetMenuCheck(int ID, int Index, BOOL Enabled);
char* SQL_ErrorMessage();
BOOL  IDE_GetReadOnly();
void  IDE_SetMenuName(int ID, int Index, char *Name);
void* IDE_CreatePopupItem(int ID, int Index, char *Name, char *ObjectType);
int   IDE_GetPopupObject(char **ObjectType, char **ObjectOwner, char **ObjectName, char **SubObject);
BOOL  IDE_ExecuteSQLReport(char *SQL, char *Title, BOOL Updateable);
void  IDE_RefreshMenus(int ID);
int   IDE_GetWindowCount();
void  IDE_SplashCreate(int ProgressMax);
void  IDE_SplashHide();
void  IDE_SplashWrite(char *s);
void  IDE_SplashWriteLn(char *s);
void  IDE_SplashProgress(int Progress);
void  IDE_SetMenuVisible(int ID, int Index, BOOL Enabled);
BOOL  IDE_ObjectAction(char *Action, char *ObjectType,char *ObjectOwner, char *ObjectName);
BOOL  IDE_BeautifyWindow();
BOOL  IDE_WindowIsModified();
void  IDE_KeyPress(int Key, int Shift);
BOOL  IDE_FirstSelectedObject (char *ObjectType,char *ObjectOwner, char *ObjectName, char *SubObject);
BOOL  IDE_NextSelectedObject (char *ObjectType,char *ObjectOwner, char *ObjectName, char *SubObject);
char* IDE_TabInfo(int Index);
int   IDE_TabIndex(int Index);

#endif // !defined(HELPER_H_INCLUDED_)
