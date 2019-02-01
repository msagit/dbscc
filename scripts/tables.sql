prompt Created on 23 Август 2005 г. by s_matsak
set feedback off
set define off
prompt Creating DBSCC_LOCKS...
create table DBSCC_LOCKS
(
  OBJTYPE  VARCHAR2(19) not null,
  OBJNAME  VARCHAR2(30) not null,
  LOCKEDBY VARCHAR2(64) not null,
  LOCKINFO VARCHAR2(240),
  OPDATE   DATE,
  STATE    VARCHAR2(1)
)
;
comment on column DBSCC_LOCKS.STATE
  is 'State - L- locked U- unlocked';
alter table DBSCC_LOCKS
  add constraint PK_DBSCC_LOCKS primary key (OBJTYPE, OBJNAME);
create index DDSCC_LOCKS_STATE on DBSCC_LOCKS (STATE);

prompt Creating DBSCC_HISTORY...
-- Create table
create table DBSCC_HISTORY
(
  OBJTYPE  VARCHAR2(19) not null,
  OBJNAME  VARCHAR2(30) not null,
  EDITBY   VARCHAR2(64) not null,
  EDITINFO VARCHAR2(240),
  OPDATE   DATE not null,
  OBJDATA  CLOB
);
-- Create/Recreate primary, unique and foreign key constraints 
alter table DBSCC_HISTORY
  add constraint PK_DBSCC_HISTORY primary key (OPDATE, OBJTYPE, OBJNAME);

set feedback on
set define on
prompt Done.
