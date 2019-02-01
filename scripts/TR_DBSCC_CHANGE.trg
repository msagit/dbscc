create or replace trigger TR_DBSCC_CHANGE
before create on schema
DECLARE
    vmachine   VARCHAR2(4000);
    vlockedby  dbscc_locks.lockedby%TYPE;
    vlockinfo  dbscc_locks.lockinfo%TYPE;
    vopdate    dbscc_locks.opdate%TYPE;
    vstate     dbscc_locks.state%TYPE;
    vmsg       VARCHAR2(2000);
    vobjexists INTEGER;
BEGIN
    -- Only locking PROCEDURES, PACKAGE and PACKAGE BODY
    IF ora_dict_obj_type IN
       ('PROCEDURE', 'FUNCTION', 'PACKAGE', 'PACKAGE BODY') THEN
        -- Get the machine name that is executing the current session
        SELECT rtrim(sys_context('USERENV', 'HOST'), chr(0))
          INTO vmachine
          FROM dual;
    
        -- Check if the object already exists 
        SELECT COUNT(*)
          INTO vobjexists
          FROM user_objects o
         WHERE o.object_type = ora_dict_obj_type
           AND o.object_name = ora_dict_obj_name;
    
        -- If the object does not exist then we allow the
        IF vobjexists = 0 THEN
            RETURN;
        END IF;
    
        -- Query the DBSCC_LOCKS table to see if and who
        -- has the object locked
        BEGIN
            SELECT l.lockedby,
                   l.lockinfo,
                   l.opdate,
                   l.state
              INTO vlockedby,
                   vlockinfo,
                   vopdate,
                   vstate
              FROM dbscc_locks l
             WHERE l.objtype = ora_dict_obj_type
               AND l.objname = ora_dict_obj_name;
        EXCEPTION
            WHEN no_data_found THEN
                vstate := NULL;
        END;
    
        -- Test the lock state of the object
        IF (vstate IS NULL) OR (vstate = 'U') THEN
            -- Not locked
            IF (vstate = 'U') AND (trunc(vopdate) = trunc(SYSDATE)) THEN
                vmsg := 'last time edited by ' || vlockedby || ' (' ||
                        vlockinfo || ') at ' ||
                        to_char(vopdate, 'HH24:MI DD.MM.YY');
            END IF;
        
            raise_application_error(-20003,
                                    ora_dict_obj_type || '.' ||
                                    ora_dict_obj_name ||
                                    ' is NOT locked, please use LOCK before updating' ||
                                    chr(13) || vmsg);
        ELSIF vlockedby <> vmachine THEN
            -- Locked from another workstation
            raise_application_error(-20004,
                                    ora_dict_obj_type || '.' ||
                                    ora_dict_obj_name ||
                                    ' cannot be created/updated because it is currently locked' ||
                                    chr(13) || ' by ' || vlockedby || ' (' ||
                                    vlockinfo || ') at ' ||
                                    to_char(vopdate, 'HH24:MI DD.MM.YY'));
        END IF;
        -- If we get here then the workstation owning the session
        -- has the lock and the creation can proceed.
    END IF;
END;
/
