
#!/usr/local/bin/python3.3

import pymysql
#conn = pymysql.connect(host='127.0.0.1', unix_socket='/tmp/mysql.sock', user='root', passwd=None, db='mysql')
conn = pymysql.connect(host='10.100.15.7', port=3306, user='mx_u_loc_cn', passwd='mx_u_loc_cn', db='mx_u_loc_cn')
cur = conn.cursor()
cur.execute("SELECT * from base_user_info")
for r in cur:
	print(r)
cur.close()
conn.close()

