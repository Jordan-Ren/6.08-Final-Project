import sqlite3
import datetime
ht_db = '/var/jail/home/{kerb}/{dir}/{dbname}.db' #assumes you have a lab06b dir in your home dir
now = datetime.datetime.now()

def request_handler(request):
    if request["method"]=="POST":
        user = request['form']['user']
        temp = float(request['form']['temperature'])
        pressure = float(request['form']['pressure'])
       
        with sqlite3.connect(ht_db) as c:
            c.execute("""CREATE TABLE IF NOT EXISTS sensor_data (time_ timestamp, user text, temperature real, pressure real);""")
            # some other query(ies) about inserting data
            c.execute("""INSERT into sensor_data VALUES (?,?,?,?)""", (now, user, temp, pressure))

        return "Data POSTED successfully"
    else:
        return "invalid HTTP method for this url."

