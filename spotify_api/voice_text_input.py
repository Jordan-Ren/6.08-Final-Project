import requests
import os
from dotenv import load_dotenv

load_dotenv()

def request_handler(request):
    if request['method'] =="GET":
        username = request["values"]["user"]
        voice_input = request["values"]["voice"]
        command, data = parse_voice_input(voice_input)
        response = None
        if command == "play" and data.get("song_name"):
            response = get_song_uri(data.get("song_name"))
            print(response)
        elif command == "add" and data.get("song_name"):
            # TODO: add song to the queue instead of just playing
            pass
        elif command == "pause":
            # Todo: pause playback
            pass
        return response

    else:
        return "invalid HTTP method for this url."

def get_spotify_token():
    CLIENT_ID = os.environ.get('SPOTIFY_CLIENT_ID')
    CLIENT_SECRET = os.environ.get('SPOTIFY_CLIENT_SECRET')
    if CLIENT_ID == None: CLIENT_ID = os.environ.get('CLIENT_ID')
    if CLIENT_SECRET == None: CLIENT_SECRET = os.environ.get('CLIENT_SECRET')
    AUTH_URL = 'https://accounts.spotify.com/api/token'

    # POST
    auth_response = requests.post(AUTH_URL, {
        'grant_type': 'client_credentials',
        'client_id': CLIENT_ID,
        'client_secret': CLIENT_SECRET,
    })

    # convert the response to JSON
    auth_response_data = auth_response.json()
    # save the access token
    access_token = auth_response_data['access_token']

    return access_token


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
    # parsed_voice = voice_input
    # if "play" in voice_input:
    #     parsed_voice = parsed_voice.replace('play', '')
    # print(parsed_voice)
    # return parsed_voice.strip()


def get_song_uri(song):
    # access_token = get_spotify_token()
    access_token = os.environ.get("ACCESS_TOKEN")
    headers = {
        'Authorization': 'Bearer {token}'.format(token=access_token)
    }
    
    BASE_URL = 'https://api.spotify.com/v1/'
    song_search = f"q={song}" # query
    type_of = "type=track" # album , artist, playlist, track, show, episode
    limit = "&limit=1"
    r = requests.get(BASE_URL + 'search?' + song_search + "&" + type_of + "&" + limit, headers=headers)

    # track_id = '6y0igZArWVi6Iz0rj35c1Y'
    # r = requests.get(BASE_URL + 'audio-features/' + track_id, headers=headers)

    r = r.json()
    response_data = {}
    if len(r["tracks"]["items"]) > 0:
        response_data['track_uri'] = r["tracks"]["items"][0]["uri"]
        response_data['url'] = r["tracks"]["items"][0]["external_urls"]["spotify"]
        return response_data
    else:
        return "Song not found"


if __name__ == "__main__":
    # print(get_spotify_token())
    req = {
        "method": "GET",
        "values": {
            "user": "acelli",
            "voice": "Play despacito"
        }
    }
    request_handler(req)
    req2 = {
        "method": "GET",
        "values": {
            "user": "acelli",
            "voice": "can you play despacito by Luis Fonzi"
        }
    }
    request_handler(req2)
# print(send_song_name(parse_voice_input("play Despacito")))