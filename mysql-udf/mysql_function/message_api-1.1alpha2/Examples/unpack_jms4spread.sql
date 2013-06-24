-- Unpack the message text out of a jms4spread message.
--
-- This is a hack.  It does not really parse the serialized Java object,
-- but just extracts the final string from it (which seems to get the
-- job done).

CREATE FUNCTION unpack_jms4spread(message blob) 
RETURNS VARCHAR(1000) CHARACTER SET utf8
DETERMINISTIC NO SQL 
  BEGIN 
  DECLARE p, len, sz int;
  DECLARE c, c1, c2 char(1) ;

  -- Test to see if this is really a serialized Java object
  IF substring(message,1,4) != CHAR (0xAC, 0xED, 0x00, 0x05) 
    THEN RETURN NULL;
  END IF;   

  -- The final string of the serialized stream will consist of
  -- "t", then a two-byte length, then a utf8 string.   
  SET len = length(message);
  SET p = len - 2;
  REPEAT 
    SET c = substring(message,p,1);
    IF c = "t" 
    THEN
      SET c1 = substring(message,p+1,1);
      SET c2 = substring(message,p+2,1);
      SET sz = (ord(c1) * 256) + ord(c2);

      IF (p + 2 + sz) = len 
        THEN RETURN substring(message,p+3,sz);
      END IF;
    END IF;
    SET p = p - 1;
  UNTIL p < 5 OR p IS NULL
  END REPEAT;

  RETURN NULL;
  END;

