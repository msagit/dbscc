CREATE OR REPLACE PACKAGE dbscc IS

  -- Author  : S_MATSAK
  -- Created : 27.07.2005 15:36:59
  -- Purpose : Контроль кода

  -- Public function and procedure declarations
  c_version     CONSTANT VARCHAR2(240) := '1.5.1';
  c_locked      CONSTANT VARCHAR2(1) := 'L';
  c_unlocked    CONSTANT VARCHAR2(1) := 'U';
  c_savehistory CONSTANT BOOLEAN := TRUE;

  c_errcode_unsuppored_type   CONSTANT NUMBER := -20001;
  c_errcode_object_not_exist  CONSTANT NUMBER := -20002;
  c_errcode_object_not_locked CONSTANT NUMBER := -20003;
  c_errcode_object_locked     CONSTANT NUMBER := -20004;

  FUNCTION vesion RETURN VARCHAR2;

  FUNCTION last_unlock(p_objtype IN dbscc_locks.objtype%TYPE,
                       p_objname IN dbscc_locks.objname%TYPE) RETURN VARCHAR2;

  FUNCTION islocked(p_objtype IN dbscc_locks.objtype%TYPE,
                    p_objname IN dbscc_locks.objname%TYPE) RETURN VARCHAR2;

  PROCEDURE lock_object(p_objtype IN dbscc_locks.objtype%TYPE,
                        p_objname IN dbscc_locks.objname%TYPE);

  PROCEDURE unlock_object(p_objtype IN dbscc_locks.objtype%TYPE,
                          p_objname IN dbscc_locks.objname%TYPE,
                          p_force   IN INTEGER := 0);

  FUNCTION get_object(p_type IN VARCHAR2, p_name IN VARCHAR2) RETURN CLOB;

  FUNCTION save2history(p_type     IN VARCHAR2,
                        p_name     IN VARCHAR2,
                        p_editby   IN VARCHAR2,
                        p_editinfo IN VARCHAR2,
                        p_opdate   IN DATE) RETURN NUMBER;

  PROCEDURE unlock_all_objects(p_force IN INTEGER := 0);

  FUNCTION object_exist(p_objtype IN VARCHAR2, p_objname IN VARCHAR2)
    RETURN NUMBER;

  FUNCTION can_edit(p_objtype IN dbscc_locks.objtype%TYPE,
                    p_objname IN dbscc_locks.objname%TYPE) RETURN NUMBER;

END dbscc;
/
CREATE OR REPLACE PACKAGE BODY dbscc IS

  FUNCTION vesion RETURN VARCHAR2 IS
  BEGIN
    RETURN c_version;
  END;

  FUNCTION is_supported_type(p_objtype IN dbscc_locks.objtype%TYPE)
    RETURN NUMBER IS
  BEGIN
    IF p_objtype IN ('PROCEDURE', 'FUNCTION', 'PACKAGE', 'PACKAGE BODY') THEN
      RETURN 1;
    END IF;
    RETURN 0;
  END;

  FUNCTION get_hostname RETURN VARCHAR2 IS
    vres VARCHAR2(240);
  BEGIN
    SELECT rtrim(sys_context('USERENV', 'HOST'), chr(0)) --MSA  Были случаи когда sys_context возвращял в конце строки код нуля
      INTO vres
      FROM dual;
    RETURN vres;
  END;

  FUNCTION get_osuser RETURN VARCHAR2 IS
    vres VARCHAR2(240);
  BEGIN
    SELECT rtrim(sys_context('USERENV', 'OS_USER'), chr(0))
      INTO vres
      FROM dual;
    RETURN vres;
  END;

  FUNCTION object_exist(p_objtype IN VARCHAR2, p_objname IN VARCHAR2)
    RETURN NUMBER IS
    vres NUMBER;
  BEGIN
    SELECT o.object_id
      INTO vres
      FROM user_objects o
     WHERE o.object_type = p_objtype
       AND o.object_name = p_objname;
    RETURN 1;
  EXCEPTION
    WHEN no_data_found THEN
      RETURN 0;
  END;

  FUNCTION last_unlock(p_objtype IN dbscc_locks.objtype%TYPE,
                       p_objname IN dbscc_locks.objname%TYPE) RETURN VARCHAR2 IS
    vunlocks  INTEGER;
    vlockedby dbscc_locks.lockedby%TYPE;
    vobjtype  dbscc_locks.objtype%TYPE;
    vobjname  dbscc_locks.objname%TYPE;
    vlockinfo dbscc_locks.lockinfo%TYPE;
    vopdate   dbscc_locks.opdate%TYPE;
  BEGIN
    vobjtype := upper(TRIM(p_objtype));
    vobjname := upper(TRIM(p_objname));
  
    -- Validate object type
    IF is_supported_type(vobjtype) = 0 THEN
      RETURN '---';
    END IF;
    -- Validate object that the object exists
    -- Не небудем лишняя трата времени
  
    -- Assume the object is already locked
    vunlocks := 1;
  
    -- Query the locks table to determine if and who
    -- has the object locked.
    BEGIN
      SELECT scc.lockedby, scc.lockinfo, scc.opdate
        INTO vlockedby, vlockinfo, vopdate
        FROM dbscc_locks scc
       WHERE scc.objtype = vobjtype
         AND scc.objname = vobjname
         AND scc.state = c_unlocked;
    EXCEPTION
      -- NO_DATA_FOUND implies that the object is not locked
      WHEN no_data_found THEN
        vunlocks := 0;
    END;
    -- already unlocked
    IF (vunlocks > 0) THEN
      IF trunc(vopdate) <> trunc(SYSDATE) THEN
        --vlockinfo := vlockinfo || ' ' ||
        --             to_char(vopdate, 'DD.MM HH24:MI');
        RETURN('');
      ELSE
        vlockinfo := vlockinfo || ' ' || to_char(vopdate, 'HH24:MI');
      END IF;
      RETURN('(' || vlockinfo || ' ' || vlockedby || ')');
    ELSE
      RETURN('');
    END IF;
  END last_unlock;

  FUNCTION can_edit(p_objtype IN dbscc_locks.objtype%TYPE,
                    p_objname IN dbscc_locks.objname%TYPE) RETURN NUMBER IS
    /*
    Return values
    '1'    - yes can edit (object is locked by caller)
    '-20004' - object is locked (but not by caller)
    '-20003' - object is not locked
    '-20002  - object does not exit
    '-20001' - unsupported object type
    */
    vlocks    INTEGER;
    vlockedby dbscc_locks.lockedby%TYPE;
    vobjtype  dbscc_locks.objtype%TYPE;
    vobjname  dbscc_locks.objname%TYPE;
    vlockinfo dbscc_locks.lockinfo%TYPE;
    vopdate   dbscc_locks.opdate%TYPE;
    vmachine  VARCHAR2(4000);
  BEGIN
    vobjtype := upper(TRIM(p_objtype));
    vobjname := upper(TRIM(p_objname));
    IF is_supported_type(vobjtype) = 0 THEN
      RETURN(c_errcode_unsuppored_type);
    END IF;
    IF object_exist(vobjtype, vobjname) = 0 THEN
      RETURN(c_errcode_object_not_exist);
    END IF;
    -- Assume the object is already locked
    vlocks := 1;
    -- Query the locks table to determine if and who
    -- has the object locked.
    BEGIN
      SELECT scc.lockedby, scc.lockinfo, scc.opdate
        INTO vlockedby, vlockinfo, vopdate
        FROM dbscc_locks scc
       WHERE scc.objtype = vobjtype
         AND scc.objname = vobjname
         AND scc.state = c_locked;
    EXCEPTION
      -- NO_DATA_FOUND implies that the object is not locked
      WHEN no_data_found THEN
        vlocks := 0;
    END;
    IF (vlocks > 0) THEN
      vmachine := get_hostname;
      IF vmachine <> vlockedby THEN
        RETURN(c_errcode_object_locked);
      ELSE
        RETURN(1); -- YES can edit!!!
      END IF;
    END IF;
    RETURN(c_errcode_object_not_locked);
  END can_edit;

  FUNCTION islocked(p_objtype IN dbscc_locks.objtype%TYPE,
                    p_objname IN dbscc_locks.objname%TYPE) RETURN VARCHAR2 IS
    vlocks    INTEGER;
    vlockedby dbscc_locks.lockedby%TYPE;
    vobjtype  dbscc_locks.objtype%TYPE;
    vobjname  dbscc_locks.objname%TYPE;
    vlockinfo dbscc_locks.lockinfo%TYPE;
    vopdate   dbscc_locks.opdate%TYPE;
  BEGIN
    vobjtype := upper(TRIM(p_objtype));
    vobjname := upper(TRIM(p_objname));
  
    -- Validate object type
    IF is_supported_type(vobjtype) = 0 THEN
      RETURN '---';
    END IF;
  
    -- Validate object that the object exists
    -- Не небудем лишняя трата времени
  
    -- Assume the object is already locked
    vlocks := 1;
  
    -- Query the locks table to determine if and who
    -- has the object locked.
    BEGIN
      SELECT scc.lockedby, scc.lockinfo, scc.opdate
        INTO vlockedby, vlockinfo, vopdate
        FROM dbscc_locks scc
       WHERE scc.objtype = vobjtype
         AND scc.objname = vobjname
         AND scc.state = c_locked;
    EXCEPTION
      -- NO_DATA_FOUND implies that the object is not locked
      WHEN no_data_found THEN
        vlocks := 0;
    END;
  
    -- already locked
    IF (vlocks > 0) THEN
      IF trunc(vopdate) <> trunc(SYSDATE) THEN
        vlockinfo := vlockinfo || ' ' ||
                     to_char(vopdate, 'DD.MM.YY HH24:MI');
      ELSE
        vlockinfo := vlockinfo || ' ' || to_char(vopdate, 'HH24:MI');
      END IF;
      RETURN('(' || vlockinfo || ' ' || vlockedby || ')');
    ELSE
      RETURN('');
    END IF;
  END islocked;

  PROCEDURE lockit(p_objtype IN dbscc_locks.objtype%TYPE,
                   p_objname IN dbscc_locks.objname%TYPE) IS
    vmachine  VARCHAR2(4000);
    vlockinfo dbscc_locks.lockinfo%TYPE;
    vnodata   NUMBER;
  BEGIN
    -- Get the name of the workstation requesting the lock
    vmachine  := get_hostname;
    vlockinfo := get_osuser;
  
    SELECT COUNT(*)
      INTO vnodata
      FROM dbscc_locks scc
     WHERE scc.objtype = p_objtype
       AND scc.objname = p_objname;
  
    IF vnodata = 0 THEN
      INSERT INTO dbscc_locks
        (objtype, objname, lockedby, lockinfo, opdate, state)
      VALUES
        (p_objtype, p_objname, vmachine, vlockinfo, SYSDATE, c_locked);
    ELSE
      UPDATE dbscc_locks scc
         SET scc.state    = c_locked,
             scc.opdate   = SYSDATE,
             scc.lockedby = vmachine,
             scc.lockinfo = vlockinfo
       WHERE scc.objtype = p_objtype
         AND scc.objname = p_objname;
    END IF;
  END;

  PROCEDURE lock_object(p_objtype IN dbscc_locks.objtype%TYPE,
                        p_objname IN dbscc_locks.objname%TYPE) IS
    PRAGMA AUTONOMOUS_TRANSACTION;
    vlocks    INTEGER;
    vlockedby dbscc_locks.lockedby%TYPE;
    vobjtype  dbscc_locks.objtype%TYPE;
    vobjname  dbscc_locks.objname%TYPE;
    vlockinfo dbscc_locks.lockinfo%TYPE;
    vopdate   dbscc_locks.opdate%TYPE;
    vmachine  VARCHAR2(4000);
  BEGIN
    vobjtype := upper(TRIM(p_objtype));
    vobjname := upper(TRIM(p_objname));
  
    -- Validate object type
    IF is_supported_type(vobjtype) = 0 THEN
      raise_application_error(c_errcode_unsuppored_type,
                              'Only objects of type PROCEDURE, FUNCTION and PACKAGE can be locked');
    END IF;
  
    -- Validate object that the object exists
    IF object_exist(vobjtype, vobjname) = 0 THEN
      raise_application_error(c_errcode_object_not_exist,
                              vobjtype || '.' || vobjname ||
                              ' does not exist');
    END IF;
  
    -- Assume the object is already locked
    vlocks := 1;
  
    -- Query the locks table to determine if and who
    -- has the object locked.
    BEGIN
      SELECT scc.lockedby, scc.lockinfo, scc.opdate
        INTO vlockedby, vlockinfo, vopdate
        FROM dbscc_locks scc
       WHERE scc.objtype = vobjtype
         AND scc.objname = vobjname
         AND scc.state = c_locked;
    EXCEPTION
      -- NO_DATA_FOUND implies that the object is not locked
      WHEN no_data_found THEN
        vlocks := 0;
    END;
  
    -- If the object is locked inform the caller that the object is
    -- already locked
    IF (vlocks > 0) THEN
      vmachine := get_hostname;
      IF vmachine <> vlockedby THEN
        raise_application_error(c_errcode_object_locked,
                                vobjtype || '.' || vobjname ||
                                ' is locked by ' || vlockedby || ' (' ||
                                vlockinfo || ') at ' ||
                                to_char(vopdate, 'HH24:MI DD.MM.YY'));
      END IF;
    ELSE
    
      -- Special case PACKAGE objects so that both the
      -- PACKAGE and PACKAGE BODY are locked
      /*IF vobjtype = 'PACKAGE' OR (vobjtype = 'PACKAGE BODY') THEN
          lockit('PACKAGE', vobjname);
          lockit('PACKAGE BODY', vobjname);
      ELSE*/
      lockit(vobjtype, vobjname);
      --END IF;
      COMMIT;
    END IF;
  END lock_object;

  PROCEDURE unlock_object(p_objtype IN dbscc_locks.objtype%TYPE,
                          p_objname IN dbscc_locks.objname%TYPE,
                          p_force   IN INTEGER := 0) IS
    PRAGMA AUTONOMOUS_TRANSACTION;
    vmachine  VARCHAR2(4000);
    vobjtype  dbscc_locks.objtype%TYPE;
    vobjname  dbscc_locks.objname%TYPE;
    vopdate   dbscc_locks.opdate%TYPE;
    vlockinfo dbscc_locks.lockinfo%TYPE;
    vlocks    INTEGER;
  BEGIN
    vobjtype := upper(TRIM(p_objtype));
    vobjname := upper(TRIM(p_objname));
  
    -- Validate object type
    IF is_supported_type(vobjtype) = 0 THEN
      raise_application_error(c_errcode_unsuppored_type,
                              'Only objects of type PROCEDURE, FUNCTION and PACKAGE are supported');
    END IF;
  
    -- Get the name of the workstation executing the request
    vmachine := get_hostname;
  
    -- Determine if the object is locked and if it is locked
    -- by the caller, if p_force is non-zero then the machine
    -- is ignored
    SELECT COUNT(*), MAX(lockinfo)
      INTO vlocks, vlockinfo
      FROM dbscc_locks scc
     WHERE scc.objtype = vobjtype
       AND scc.objname = vobjname
       AND scc.state = c_locked
       AND (scc.lockedby = vmachine OR p_force <> 0);
  
    -- If locked then remove the entry from the DBSCC_LOCKS
    -- table. PACKAGE and PACKAGE BODY are special cased
    IF vlocks > 0 THEN
      vopdate := SYSDATE;
      /*            IF (vobjtype = 'PACKAGE') OR (vobjtype = 'PACKAGE BODY') THEN
                      UPDATE dbscc_locks
                         SET state    = c_unlocked,
                             opdate   = vopdate,
                             lockedby = vmachine
                       WHERE objtype IN ('PACKAGE', 'PACKAGE BODY')
                         AND objname = vobjname;
                      vlocks := save2history('PACKAGE',
                                             vobjname,
                                             vmachine,
                                             vlockinfo,
                                             vopdate);
                      vlocks := save2history('PACKAGE BODY',
                                             vobjname,
                                             vmachine,
                                             vlockinfo,
                                             vopdate);
                  ELSE
      */
      UPDATE dbscc_locks
         SET state = c_unlocked, opdate = vopdate, lockedby = vmachine
       WHERE objtype = vobjtype
         AND objname = vobjname;
      vlocks := save2history(vobjtype,
                             vobjname,
                             vmachine,
                             vlockinfo,
                             vopdate);
      -- END IF;
      COMMIT;
    ELSE
      raise_application_error(c_errcode_object_not_locked,
                              'You do not have ' || vobjtype || '.' ||
                              vobjname || ' locked');
    END IF;
  END unlock_object;

  FUNCTION get_object(p_type IN VARCHAR2, p_name IN VARCHAR2) RETURN CLOB IS
    res CLOB;
  BEGIN
    dbms_lob.createtemporary(res, TRUE);
    FOR s IN (SELECT s.text
                FROM user_source s
               WHERE s.type = p_type
                 AND s.name = p_name
               ORDER BY s.line) LOOP
      IF s.text IS NOT NULL THEN
        dbms_lob.writeappend(res, length(s.text), s.text);
      END IF;
    END LOOP;
    RETURN(res);
  END get_object;

  FUNCTION save2history(p_type     IN VARCHAR2,
                        p_name     IN VARCHAR2,
                        p_editby   IN VARCHAR2,
                        p_editinfo IN VARCHAR2,
                        p_opdate   IN DATE) RETURN NUMBER IS
    PRAGMA AUTONOMOUS_TRANSACTION;
    vdata    CLOB;
    volddata CLOB;
    vres     NUMBER := 0;
  BEGIN
    IF c_savehistory THEN
      BEGIN
        SELECT h.objdata
          INTO volddata
          FROM dbscc_history h
         WHERE h.objtype = p_type
           AND h.objname = p_name
           AND rownum <= 1
         ORDER BY h.opdate DESC;
      EXCEPTION
        WHEN no_data_found THEN
          volddata := NULL;
      END;
      vdata := get_object(p_type, p_name);
      IF (vdata IS NOT NULL) AND
         ((volddata IS NULL) OR (dbms_lob.compare(vdata, volddata) <> 0)) THEN
        INSERT INTO dbscc_history
          (objtype, objname, editby, editinfo, opdate, objdata)
        VALUES
          (p_type, p_name, p_editby, p_editinfo, p_opdate, vdata);
        COMMIT;
        vres := 1;
      END IF;
    END IF;
    RETURN vres;
  END save2history;

  PROCEDURE unlock_all_objects(p_force IN INTEGER := 0) IS
    PRAGMA AUTONOMOUS_TRANSACTION;
    vmachine VARCHAR2(4000);
    vopdate  dbscc_locks.opdate%TYPE;
    vres     NUMBER;
  BEGIN
    vopdate  := SYSDATE;
    vmachine := get_hostname;
    SELECT SUM(save2history(objtype, objname, vmachine, lockinfo, vopdate))
      INTO vres
      FROM dbscc_locks
     WHERE (lockedby = vmachine OR p_force <> 0)
       AND state = c_locked;
    UPDATE dbscc_locks
       SET state = c_unlocked, opdate = vopdate, lockedby = vmachine
     WHERE (lockedby = vmachine OR p_force <> 0)
       AND state = c_locked;
    COMMIT;
  END unlock_all_objects;

END dbscc;
/
