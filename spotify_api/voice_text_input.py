import datetime
import json
import sqlite3

import spotipy
from spotipy.oauth2 import SpotifyClientCredentials, SpotifyOAuth


SPOTIFY_CLIENT_ID = ""
SPOTIFY_CLIENT_SECRET = ""
ACCESS_TOKEN = ""

server_user = 'team15'
ht_db = f'/var/jail/home/{server_user}/final/song_queue.db'
BASE_URL = 'https://api.spotify.com/v1/'

scope = "user-read-currently-playing user-top-read user-read-recently-played user-read-playback-state " \
        "user-modify-playback-state streaming app-remote-control user-library-read"
VALID_GROUPS = {'test1': "pass1", 'test2': "pass2"}


def request_handler(request):
    cache_handler = spotipy.cache_handler.CacheFileHandler(
        cache_path=f'/var/jail/home/{server_user}/final/spotify_cache')
    auth_manager = spotipy.oauth2.SpotifyOAuth(scope=scope,
                                               cache_handler=cache_handler,
                                               show_dialog=True, client_id=SPOTIFY_CLIENT_ID,
                                               client_secret=SPOTIFY_CLIENT_SECRET,
                                               redirect_uri="http://example.com")
    if request['method'] == "POST":
        if request["form"].get('code'):
            auth_manager.get_access_token(request["form"]["code"])
            return "Token added"
        else:
            if not auth_manager.validate_token(cache_handler.get_cached_token()):
                auth_url = auth_manager.get_authorize_url()
                return f'<h2><a href="{auth_url}">Sign in</a></h2>'
        sp = spotipy.Spotify(auth_manager=auth_manager)
        username = request["form"].get("user")
        group_name = request["form"].get("group")
        password = request["form"].get("password")
        voice_input = request["form"].get("voice")
        command, data = parse_voice_input(voice_input)
        response = None
        if group_name in VALID_GROUPS and VALID_GROUPS[group_name] == password:
            if command == "play" and data.get("song_name"):
                response = get_song_uri(sp, data.get("song_name"), data.get("artist_name"))
                add_song_to_db(sp, song_uri=response.get("track_uri"), song_name=response.get("song_name"),
                               group_name=group_name)
                play_song(sp, response['track_uri'])
                return f"Playing song: {data.get('song_name')}"
            elif command == "add" and data.get("song_name"):
                response = get_song_uri(sp, data.get("song_name"), data.get("artist_name"))
                add_song_to_db(sp, song_uri=response.get("track_uri"), song_name=response.get("song_name"),
                               group_name=group_name)
                add_song_to_queue(sp, response['track_uri'])
                return f"Song: {response.get('song_name')} added to the queue"
            elif command == "pause":
                pause(sp)
                return "Paused playback"
            elif command == "resume":
                resume(sp)
                return "Resumed playback"
            elif command == "clear":
                clear_queue()
                return "Cleared Queue"
            elif command == "skip":
                next_song = skip_song(group_name)
                sp.next_track()
                return f"Next song is {next_song}"
            elif command == "None":
                return "Invalid voice input, please try again"
            return response
    elif request["method"] == "GET":
        sp = spotipy.Spotify(auth_manager=auth_manager)
        group_name = request["values"]["group"]
        #Brandon and David added this field for returning more song info
        if group_name in VALID_GROUPS:
            if "requests" not in request["values"]:
                queue_manager(sp, group_name)
                with sqlite3.connect(ht_db) as c:
                    data = c.execute(
                        """SELECT song_name, tempo, danceability, segments FROM song_queue WHERE group_name = ? ORDER BY time_ ASC LIMIT 1;""",
                        (group_name,)).fetchone()
                    if data == None: return "No song currently queued"
                    if sp.currently_playing() == None: return "No song currently queued"
                    artist_uri = sp.currently_playing()['item']['artists'][0]['uri']
                    genres = sp.artist(artist_uri).get('genres')
                    data = list(data)
                    data.append(genres)
                    data2 = {"name": data[0], "tempo": data[1], "genres": genres}
                    return data2
            else:
                with sqlite3.connect(ht_db) as c:
                    data = c.execute(
                        """SELECT song_name, time_ FROM song_queue WHERE group_name = ? ORDER BY time_ ASC LIMIT 3;""",(group_name,)).fetchall()
                    if data == None: return "No song currently requested"
                    if len(data) == 0: return "No song currently requested"
                    return data

        else:
            return "invalid group"
    else:
        return "invalid HTTP method for this url."


def clear_queue():
    with sqlite3.connect(ht_db) as c:
        res = c.execute("""DELETE from song_queue""")
        return res


def skip_song(group_name):
    with sqlite3.connect(ht_db) as c:
        res = c.execute("""SELECT time_ FROM song_queue WHERE group_name = ? ORDER BY time_ ASC LIMIT 1;""",
                        (group_name,)).fetchone()
        c.execute("""DELETE FROM song_queue WHERE time_ = ?""", (res[0],))
        res = c.execute("""SELECT song_name FROM song_queue WHERE group_name = ? ORDER BY time_ ASC LIMIT 1;""",
                        (group_name,)).fetchone()
        if res:
            next_song = res[0]
        else:
            next_song = None
        return next_song


def get_queue():
    with sqlite3.connect(ht_db) as c:
        res = c.execute("""SELECT * from song_queue""").fetchall()
        return res


def create_db():
    with sqlite3.connect(ht_db) as c:
        c.execute("""CREATE TABLE IF NOT EXISTS song_queue (time_ timestamp, group_name text, song_name text, song_uri text, 
        tempo real, energy real, time_signature integer, danceability real, segments text);""")


def add_song_to_db(sp, song_uri, song_name, group_name):
    create_db()
    if len(get_queue()) == 0:
        play_song(sp, song_uri)
    tempo, energy, time_signature, danceability, segments= get_audio_features(sp, song_uri)
    now = datetime.datetime.now()
    with sqlite3.connect(ht_db) as c:
        c.execute("""INSERT into song_queue VALUES (?,?,?,?,?,?,?,?,?)""",
                  (now, group_name, song_name, song_uri, tempo, energy,
                   time_signature, danceability,
                   json.dumps(segments)))


def clean_input(voice_input):
    voice_input = voice_input.lower()
    voice_input = voice_input.replace('"', "")
    if voice_input[-1] == '.':
        voice_input = voice_input[:-1]
    voice_input = voice_input.replace("to the queue", "")
    voice_input = voice_input.replace("to the q", "")
    voice_input = voice_input.replace("can you please", "")
    # voice_input = voice_input.replace(voice_input.split("play")[0], "") # remove everything before "play [song]"
    inp_list = voice_input.split(' ')
    if "next song" not in voice_input and "next" == inp_list[-1]:
        voice_input = voice_input.replace("next", "")
    if "now" == inp_list[-1]:
        voice_input = voice_input.replace("now", "")
    return voice_input


def parse_artist(song_desc):
    data = {}
    if "by" in song_desc[:-1]:
        song = " ".join(song_desc[:song_desc.index("by")])
        artist = " ".join(song_desc[(song_desc.index("by") + 1):])
    elif "bye" in song_desc[:-1]:
        song = " ".join(song_desc[:song_desc.index("bye")])
        artist = " ".join(song_desc[(song_desc.index("bye") + 1):])
    else:
        song = " ".join(song_desc)
        artist = "None"
    data["song_name"] = song
    data["artist_name"] = artist
    return data


def parse_voice_input(voice_input):
    '''
    Possible Commands:
    # Skipping
        * skip *
        * next song *
    # Add to queue (Only if the skipping phrases are missing)
        * play ___ ("" | next | now)
        * add ___ (to the queue | "" | next)
        * queue up ___ (next | now | "")
        # With a specific artist request within ___
            "SONG" by "ARTIST"
    # Pausing (Only if the add/play/queue-up key words are missing)
        * pause *
    # Resuming (Only if the add/play/queue-up/pause key words are missing)
        * resume *
    '''
    try:
        voice_input = clean_input(voice_input)
        input_list = voice_input.split()
        data = {}
        if "skip" in input_list or "next song" in voice_input:
            command = "skip"
        elif "play" in input_list:
            command = "play"
            data = parse_artist(input_list[(input_list.index("play") + 1):])
        elif "add" in input_list:
            command = "add"
            data = parse_artist(input_list[(input_list.index("add") + 1):])
        elif "queue up" in voice_input or "q up" in voice_input:
            command = "add"
            data = parse_artist(input_list[(input_list.index("up") + 1):])
        elif "pause" in input_list:
            command = "pause"
        elif "resume" in input_list:
            command = "resume"
        elif "clear" in input_list:
            return "clear", None
        else:
            command = "No Command"
        return command, data
    except Exception as e:
        raise e


def get_song_uri(sp, song, artist):
    response_data = {}
    lim = 1 if artist == "None" else 50
    res = sp.search(song, limit=lim, type="track")
    found = False
    if len(res["tracks"]["items"]) > 0:
        if artist == "None":
            response_data['song_name'] = res["tracks"]["items"][0]["name"]
            response_data['track_uri'] = res["tracks"]["items"][0]["uri"]
            response_data['url'] = res["tracks"]["items"][0]["external_urls"]["spotify"]
            found = True
        else:
            for song in res["tracks"]["items"]:
                artists = {a['name'].lower() for a in song['artists']}
                if artist in artists:
                    response_data['song_name'] = song["name"]
                    response_data['track_uri'] = song["uri"]
                    response_data['url'] = song["external_urls"]["spotify"]
                    found = True
    if found:
        return response_data
    else:
        return "Song not found"


def play_song(sp, song_uri):
    sp.start_playback(uris=[song_uri], position_ms=0)


def add_song_to_queue(sp, song_uri):
    sp.add_to_queue(uri=song_uri)


def pause(sp):
    sp.pause_playback()


def resume(sp):
    sp.start_playback()


def get_audio_features(sp, song_uri):
    res = sp.audio_features(tracks=[song_uri])
    tempo, danceability = res[0].get('tempo'), res[0].get('danceability')
    energy, time_signature = res[0].get('energy'), res[0].get('time_signature')
    res = sp.audio_analysis(song_uri)
    segments = [{'start': x.get('start'), 'duration': x.get('duration'), 'loudness': x.get('loudness')} for x in
                res.get('sections')]
    return tempo, energy, time_signature, danceability, segments

def queue_manager(sp, group_name):
    currently_playing = sp.currently_playing().get('item')
    if currently_playing:
        song_uri = currently_playing.get('uri')
        with sqlite3.connect(ht_db) as c:
            res = c.execute("""SELECT song_uri FROM song_queue WHERE group_name = ? ORDER BY time_ ASC LIMIT 1;""",
                            (group_name,)).fetchone()
        if res:
            res = res[0]
        if res and song_uri != res:
            skip_song(group_name)
            queue_manager(sp, group_name)

if __name__ == "__main__":
    # auth_manager = spotipy.oauth2.SpotifyOAuth(scope=scope,
    #                                            show_dialog=True, client_id=SPOTIFY_CLIENT_ID,
    #                                            client_secret=SPOTIFY_CLIENT_SECRET, redirect_uri="http://example.com")
    # sp = spotipy.Spotify(auth_manager=auth_manager)
    # u = sp.currently_playing()['item']['artists'][0]['uri']
    # print(sp.artist(u)['genres'])
    # print(sp.album(u).get('genres'))
    # print(sp.audio_features(['spotify:track:2r6OAV3WsYtXuXjvJ1lIDi']))
    # print(get_song_uri(sp, "fun", "None"))
    # queue_manager(sp)
    # req = {
    #     "method": "GET",
    #     "values": {
    #         "user": "acelli",
    #         "group": "group15",
    #         "voice": "Play despacito"
    #     }
    # }
    # request_handler(req)
    # req2 = {
    #     "method": "GET",
    #     "values": {
    #         "user": "acelli",
    #         "group": "group15",
    #         "voice": "add sunburn to the queue"
    #     }
    # }
    # request_handler(req2)
    # import time
    # time.sleep(10)
    # req3 = {
    #     "method": "GET",
    #     "values": {
    #         "user": "acelli",
    #         "voice": "pause"
    #     }
    # }
    # request_handler(req3)
    req = {
        "method": "POST",
        "form": {
            "user": "acelli",
            "group": "test1",
            "password": "pass1",
            "voice": "play sunburn"
        }
    }
    # print(request_handler(req))
    # print(get_audio_features('spotify:track:6habFhsOp2NvshLv26DqMb'))
