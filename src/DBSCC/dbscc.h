// PL/SQL Developer Plug-In DBSCC
// Copyright 2005 Art-Master 
// Developed by Matsak Sergei (matsak@wp.pl)

#if !defined(DBSCC_H_INCLUDED_)
#define DBSCC_H_INCLUDED_

//Functions to expert to PL/SQL Dev
extern "C"
{
  __declspec(dllexport) char* IdentifyPlugIn(int);
  __declspec(dllexport) char* About();
  __declspec(dllexport) char* CreateMenuItem(int);
  __declspec(dllexport) void  RegisterCallback(int, void *);
  __declspec(dllexport) void  OnMenuClick(int);
  __declspec(dllexport) void  OnCreate();
  __declspec(dllexport) void  OnActivate();
  __declspec(dllexport) void  OnDestroy();
  __declspec(dllexport) void  OnBrowserChange();
  __declspec(dllexport) void  OnWindowChange();
  __declspec(dllexport) void  OnConnectionChange();
  __declspec(dllexport) int	  OnWindowClose(int WindowType, BOOL Changed);
  __declspec(dllexport) void  OnWindowCreate(int WindowType);
  __declspec(dllexport) void  OnWindowCreated(int WindowType);
  __declspec(dllexport) void  Configure();
  __declspec(dllexport) void  OnPopup(char *ObjectType, char *ObjectName);
}


//---------------------------------------------------------------
//  DBSCC basic actions
//---------------------------------------------------------------
int   LockObject(char *Type, char *Name);
int   UnlockObject(char *Type, char *Name, bool force=false);
int   CanEdit(char *Type, char *Name);
char* LockInfo(char *Type, char *Name);
char* LastUnlock(char *Type, char *Name);
bool  ObjectExist(char *Type, char *Name);
//---------------------------------------------------------------

char* MenuItemInterface(int Index, bool CreateMenuItem_);

#endif // !defined(DBSCC_H_INCLUDED_)
