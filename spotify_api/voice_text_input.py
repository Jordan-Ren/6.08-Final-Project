import os
import json
import spotipy
from spotipy.oauth2 import SpotifyOAuth
import sqlite3
import datetime

server_user = 'team15'
ht_db = f'/var/jail/home/{server_user}/final/songs.db'

HEADERS = {
    'Authorization': 'Bearer {token}'.format(token=os.environ.get("ACCESS_TOKEN"))
}
BASE_URL = 'https://api.spotify.com/v1/'

scope = "user-read-currently-playing user-top-read user-read-recently-played user-read-playback-state " \
            "user-modify-playback-state streaming app-remote-control user-library-read"

sp = spotipy.Spotify(auth_manager=SpotifyOAuth(scope=scope, client_id=os.environ.get('SPOTIFY_CLIENT_ID'),
                                                client_secret=os.environ.get('SPOTIFY_CLIENT_SECRET'),
                                               redirect_uri="http://example.com"))

def request_handler(request):
    if request['method'] =="GET":
        username = request["values"]["user"]
        group_name = request["values"]["group"]
        voice_input = request["values"]["voice"]
        command, data = parse_voice_input(voice_input)
        response = None
        if command == "play" and data.get("song_name"):
            response = get_song_uri(data.get("song_name"))
            add_song_to_db(song_uri=response.get("track_uri"), song_name=data.get("song_name"), group_name=group_name)
            # play_song(response['track_uri'])
        elif command == "add" and data.get("song_name"):
            response = get_song_uri(data.get("song_name"))
            add_song_to_db(song_uri=response.get("track_uri"), song_name=data.get("song_name"), group_name=group_name)
            # add_song_to_queue(response['track_uri'])
        elif command == "pause":
            pause()
        return response
    else:
        return "invalid HTTP method for this url."


def create_db():
    with sqlite3.connect(ht_db) as c:
        c.execute("""CREATE TABLE IF NOT EXISTS queue (time_ timestamp, group_name text, song_name text, song_uri text, 
        tempo real, energy real, time_signature integer, danceability real, segments text);""")


def add_song_to_db(song_uri, song_name, group_name):
    tempo, energy, time_signature, danceability, segments = get_audio_features(song_uri)
    now = datetime.datetime.now()
    with sqlite3.connect(ht_db) as c:
        c.execute("""INSERT into queue VALUES (?,?,?,?,?,?,?,?,?)""", (now, group_name, song_name, tempo, energy,
                                                                             time_signature, danceability,
                                                                             json.dumps(segments)))

def clean_input(voice_input):
    voice_input = voice_input.replace("to the queue", "")
    # Add in any other input cleaning to this function ex. if people keep saying play me ______, remove the "me"
    return voice_input


def parse_voice_input(voice_input):
    try:
        voice_input = clean_input(voice_input)
        input_list = voice_input.lower().split()
        data = {}
        if "play" in input_list:
            command = "play"
            data["song_name"] = " ".join(input_list[(input_list.index("play")+1):])
            print("Command: ", command, "  Data: ", data)
            return command, data
        elif "pause" in input_list:
            command = "pause"
            print("Command: ", command, "  Data: ", data)
            return command, data
        elif "add" in input_list:
            command = "add"
            data["song_name"] = " ".join(input_list[(input_list.index("add") + 1):])
            print("Command: ", command, "  Data: ", data)
            return command, data
    except Exception as e:
        print(e)
        raise e


def get_song_uri(song):
    res = sp.search(song, limit=1, type="track")
    response_data = {}
    if len(res["tracks"]["items"]) > 0:
        response_data['track_uri'] = res["tracks"]["items"][0]["uri"]
        response_data['url'] = res["tracks"]["items"][0]["external_urls"]["spotify"]
        return response_data
    else:
        return "Song not found"


def play_song(song_uri):
    sp.start_playback(uris=[song_uri], position_ms=0)


def add_song_to_queue(song_uri):
    sp.add_to_queue(uri=song_uri)

def pause():
    sp.pause_playback()

def get_audio_features(song_uri):
    res = sp.audio_features(tracks=[song_uri])
    tempo, danceability = res[0].get('tempo'), res[0].get('danceability')
    energy, time_signature = res[0].get('energy'), res[0].get('time_signature')
    res = sp.audio_analysis(song_uri)
    segments = [{'start': x.get('start'), 'duration': x.get('duration'), 'loudness': x.get('loudness')} for x in res.get('sections')]
    return tempo, energy, time_signature, danceability, segments


if __name__ == "__main__":
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
    get_audio_features('spotify:track:6habFhsOp2NvshLv26DqMb')