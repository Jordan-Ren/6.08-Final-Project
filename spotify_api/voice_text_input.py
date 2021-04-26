import requests
import os

def request_handler(request):
    if request['method'] =="GET":
        username = request["values"]["user"]
        voice_input = request["values"]["voice"]

        song = parse_voice_input(voice_input)

        response = send_song_name(song)

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


def parse_voice_input(voice_input):
    parsed_voice = voice_input
    if "play" in voice_input:
        parsed_voice = parsed_voice.replace('play', '')
    print(parsed_voice)
    return parsed_voice.strip()


def send_song_name(song):
    # access_token = get_spotify_token()
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

    if len(r["tracks"]["items"]) > 0:
        track_uri = r["tracks"]["items"][0]["uri"]
        url = r["tracks"]["items"][0]["external_urls"]["spotify"]
        return track_uri, url
    else:
        return "Song not found"



print(get_spotify_token())
print(send_song_name(parse_voice_input("play Despacito")))