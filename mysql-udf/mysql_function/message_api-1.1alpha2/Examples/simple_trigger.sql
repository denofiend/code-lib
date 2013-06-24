
CREATE TRIGGER send_notice
 AFTER INSERT ON my_table
 FOR EACH ROW
  DO send_mesg("my_mesg_group",new.id);
