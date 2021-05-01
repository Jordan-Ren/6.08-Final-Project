import sqlite3
from sqlite3 import Error
import datetime
server_user = 'team15'
ht_db = f'/var/jail/home/{server_user}/final/songs.db' #assumes you have a final dir on our server dir
#ht_db = 'songs.db'
now = datetime.datetime.now()
valid_groups = {'test1': "pass1", 'test2': "pass2"}

def request_handler(request) -> str:
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
                        data = c.execute("""SELECT time_ FROM queue WHERE group_name = ? ORDER BY time_ ASC LIMIT 1;""", (group,)).fetchone()
                        c.execute("""DELETE FROM queue WHERE time_ = ?""", (data[0][0],))
                        data = c.execute("""SELECT song FROM queue WHERE group_name = ? ORDER BY time_ ASC LIMIT 1;""", (group,)).fetchone()
                        next_song = data[0]
                        return f'Next song is {next_song}' 
                    elif action == "finished":
                        data = c.execute("""SELECT song, time_ FROM queue WHERE group_name = ? ORDER BY time_ ASC LIMIT 2;""", (group,)).fetchall()                  
                        if len(data) == 0:
                            return "No more songs on the queue"
                        else:
                            c.execute("""DELETE FROM queue WHERE time_ = ?""", (data[0][1],))
                        if len(data) == 1:
                            return "Last song played"
                        else:
                            next_song = data[1][0]
                            return f'Next song is {next_song}'      
                    else:
                        return "No action specified"
        except:
            return "Incorrect content type or form format"
    elif request["method"] == "GET":
        try:
            group = request['values']['group']
            if group in valid_groups:
                with sqlite3.connect(ht_db) as c:
                    data = c.execute("""SELECT song FROM queue WHERE group_name = ? ORDER BY time_ ASC LIMIT 4;""", (group,)).fetchall()
                    data = [x[0] for x in data]
                    return data
            else:
                return "Invalid group"
        except: 
            return "Specify a group"
    