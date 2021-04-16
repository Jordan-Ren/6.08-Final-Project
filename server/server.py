import sqlite3
import datetime
ht_db = '/var/jail/home/jordanr1/final/songs.db' #assumes you have a lab06b dir in your home dir
now = datetime.datetime.now()
valid_groups = {'test1': "pass", 'test2': "pass2"}

def request_handler(request):
    if request["method"] == "POST":
        try:
            group = request['form']['group']
            password = request['form']['password']
            action = request['form']['action']
            song = request['form']['song']

            if group in valid_groups and valid_groups[group] == password:
                with sqlite3.connect(ht_db) as c:
                    c.execute("""CREATE TABLE IF NOT EXISTS queue (time_ timestamp, group_name text, song text);""")
                    if action == "pause":
                        return "Handle pause instructions here"
                    elif action == "play":
                        if song != "None":
                            c.execute("""INSERT into queue VALUES (?,?,?)""", (now, group, song))
                            return f'Song: {song} added to the queue!'
                        else:
                            return "No song specified! Try again"
                    elif action == "resume":
                        return "Handle resume instructions here"
                    elif action == "skip":
                        return "Handle skip instructions here"
                    else:
                        return "No action specified"
            else:
                return "Incorrect group or password"
        except:
            return "Incorrect content type or form format"
    elif request["method"] == "GET":
        group = request['values']['group']
        if group in valid_groups:
            with sqlite3.connect(ht_db) as c:
                data = c.execute("""SELECT song FROM queue WHERE group_name = ? ORDER BY time_ ASC LIMIT 4;""", (group,)).fetchall()
                data = [x[0] for x in data]
                return data
        else:
            return "Incorrect group or password"


'''
req = {'method': 'POST', 'args': [], 'values': {}, 'content-type': 'application/x-www-form-urlencoded', 'is_json': False,
'data': b'group=test1&password=pass&action=resume&song=None', 'form': {'group': 'test1', 'password': 'pass', 'action':
'resume', 'song': 'None'}}
print(request_handler(req))
'''
