#!/usr/bin/python
import mysql.connector

def PythonApp(d) :
	conn = mysql.connector.connect(host='127.0.0.1', user='root', password='', database = 'TSC', use_unicode=True)
	cursor = conn.cursor()
	stmt = d['query']
	result = 'Query Result:\n'
	try:
		cursor.execute(stmt)
		if cursor.with_rows :
			for (x, y) in cursor :
				result += str(x) + ' ' + str(y) + '\n'
		else :
			result += '%d rows affected' % cursor.rowcount
	except mysql.connector.Error as err:
  		result = "Something went wrong: %s" % str(err)
	conn.commit()
	cursor.close()
	conn.close()
	return result
