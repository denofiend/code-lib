create procedure get_orders_messages  
( ) 
BEGIN
  DECLARE mesg_handle int;
  DECLARE mesg string;
  DECLARE username string;
  DECLARE isbn string;

  select join_mesg_group("orders") into mesg_handle ;

  REPEAT
    select recv_mesg(mesg_handle) into mesg;
    IF mesg is not null THEN
     set username = substring_index(mesg," ",1) ;
     set isbn = substring_index(mesg," ", -1) ;
     INSERT INTO book_order_log(timestamp,username,isbn)
      VALUES ( NULL, username, isbn) ;
    END IF;
 UNTIL mesg IS NULL
 END REPEAT;
END

