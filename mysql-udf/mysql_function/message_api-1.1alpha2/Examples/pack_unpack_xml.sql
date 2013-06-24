
-- Sample SQL code to illustrate how you could pack multiple values into an
-- XML message before sending, and unpack them after receiving.
--
-- The "unpack" function requires MySQL 5.1.5 or later.
--
-- These examples use a simple one-tag message format, e.g.:
--
--        <LinkCount user="15" count="8" />

delimiter //

CREATE FUNCTION pack_link_count_XML(user_id INT, link_count INT) 
RETURNS VARCHAR(100)
DETERMINISTIC NO SQL 
 BEGIN
  RETURN concat(
    "&lt;LinkCount ",
     'user="' , user_id , '" ',
     'count="', link_count , '"', 
    " /&gt;" );
 END
//


CREATE PROCEDURE unpack_link_count_XML(
   IN message VARCHAR(100),
   OUT user_id INT,
   OUT link_count INT
)
BEGIN 
  SET user_id = ExtractValue(message,"/LinkCount/@user");
  SET link_count = ExtractValue(message,"/LinkCount/@count");
END
//

