---------------------------------------------------
PL/SQL DBScC - Team database source control system
---------------------------------------------------
* Copyright 2005 Matsak Sergei (matsaks@gmail.com)
* Version 1.5.1
Supported objects :PROCEDURE, FUNCTION, PACKAGE, PACKAGE BODY

Installation DBScC client (PL/SQL Developer PlugIn)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 To install DBScC just copy plugin files to "\Program Files\PLSQL Developer\PlugIns\"
  (dbscc.dll, list_all_history.bmp, list_all_locks.bmp, unlock_all_objects.bmp)

Installation DBScC server 
~~~~~~~~~~~~~~~~~~~~~~~~~
1. Run tables.sql to create tables;
2. Compile package dbscc.pck
3. Compile trigger TR_DBSCC_CHANGE.trg


---------------------------------------------------------
Description
---------------------------------------------------------
DBScC PlugIn
~~~~~~~~~~~~
All setup options is in main memu Tools / Plug-In DBScC / Setup.
(This menu is active when supported DBScC server is found.)

Description of DBScC PlugIn options:
* Enable/Disable DBScC 
   Main switch to turn on/off plugin
* Enable/Disable AutoLock
   Locks object when you open edit window
* Enable/Disable AutoUnlock
   Unlocks object when you close edit window 
   (works only for automaticly locked windows)
* Enable/Disable SmartLabels
   The popup labels will contain lock information.
