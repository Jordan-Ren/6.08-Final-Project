import unittest
from server import request_handler

'''
Unit tests for server and database: deliverable 1
'''
class TestServerDeliverable1(unittest.TestCase):

    # test post request (incorrect password)
    def test_post_invalid_pass(self):
        req = {
                'method': 'POST', 
                'args': [], 
                'values': {}, 
                'content-type': 'application/x-www-form-urlencoded', 
                'is_json': False,
                'data': b'group=test1&password=pass1&action=pause&song=None', 
                'form': {'group': 'test2', 'password': 'pass1', 'action':'pause', 'song': 'None'}
                }
        self.assertEqual(request_handler(req), "Incorrect group or password")

    # test pause post request
    def test_post_pause(self):
        req = {
                'method': 'POST', 
                'args': [], 
                'values': {}, 
                'content-type': 'application/x-www-form-urlencoded', 
                'is_json': False,
                'data': b'group=test1&password=pass1&action=pause&song=None', 
                'form': {'group': 'test1', 'password': 'pass1', 'action':'pause', 'song': 'None'}
                }
        self.assertEqual(request_handler(req), "Handle pause instructions here")

    # test play (no song) post request
    def test_post_play_ns(self):
        req = {
                'method': 'POST', 
                'args': [], 
                'values': {}, 
                'content-type': 'application/x-www-form-urlencoded', 
                'is_json': False,
                'data': b'group=test1&password=pass1&action=play&song=None', 
                'form': {'group': 'test1', 'password': 'pass1', 'action':'play', 'song': 'None'}
                }
        self.assertEqual(request_handler(req), "No song specified! Try again")
    
    # test play (song) post request
    def test_post_play_s(self):
        req = {
                'method': 'POST', 
                'args': [], 
                'values': {}, 
                'content-type': 'application/x-www-form-urlencoded', 
                'is_json': False,
                'data': b'group=test2&password=pass2&action=play&song=Despacito', 
                'form': {'group': 'test2', 'password': 'pass2', 'action':'play', 'song': 'Despacito'}
                }
        self.assertEqual(request_handler(req), "Song: Despacito added to the queue!")

    # test resume post request
    def test_post_resume(self):
        req = {
                'method': 'POST', 
                'args': [], 
                'values': {}, 
                'content-type': 'application/x-www-form-urlencoded', 
                'is_json': False,
                'data': b'group=test1&password=pass1&action=resume&song=None', 
                'form': {'group': 'test1', 'password': 'pass1', 'action':'resume', 'song': 'None'}
                }
        self.assertEqual(request_handler(req), "Handle resume instructions here")

    # test skip post request
    def test_post_skip(self):
        req = {
                'method': 'POST', 
                'args': [], 
                'values': {}, 
                'content-type': 'application/x-www-form-urlencoded', 
                'is_json': False,
                'data': b'group=test2&password=pass2&action=skip&song=None', 
                'form': {'group': 'test2', 'password': 'pass2', 'action':'skip', 'song': 'None'}
                }
        res = request_handler(req)
        self.assertEqual(res, "Handle skip instructions here")

    # test no action post request
    def test_post_no_action(self):
        req = {
                'method': 'POST', 
                'args': [], 
                'values': {}, 
                'content-type': 'application/x-www-form-urlencoded', 
                'is_json': False,
                'data': b'group=test1&password=pass1&action=&song=None', 
                'form': {'group': 'test1', 'password': 'pass1', 'action':'', 'song': 'None'}
                }
        self.assertEqual(request_handler(req), "No action specified")

    # test get request (no group specified)
    def test_get_no_group(self):
        req = {
                'method': 'GET', 
                'args': [], 
                'values': {}
                }
        self.assertEqual(request_handler(req), "Specify a group")

    # test get request (invalid group specified)
    def test_get_invalid_group(self):
        req = {
                'method': 'GET', 
                'args': ['group'], 
                'values': {'group': 'test3'}
                }
        self.assertEqual(request_handler(req), "Invalid group")

    # test get request (group with no songs)
    def test_get_no_song(self):
        req = {
                'method': 'GET', 
                'args': ['group'], 
                'values': {'group': 'test1'}
                }
        self.assertEqual(request_handler(req), list())
    
    # test get request (group with songs)
    def test_get_song(self):
        req = {
                'method': 'GET', 
                'args': ['group'], 
                'values': {'group': 'test2'}
                }
        self.assertEqual(request_handler(req)[0], "Despacito")

    def test_skip_with_two_songs(self):
        req = {
                'method': 'POST', 
                'args': [], 
                'values': {}, 
                'content-type': 'application/x-www-form-urlencoded', 
                'is_json': False,
                'data': b'group=test1&password=pass1&action=play&song=lovefool', 
                'form': {'group': 'test1', 'password': 'pass1', 'action':'play', 'song': 'lovefool'}
                }
        self.assertEqual(request_handler(req), "Song: lovefool added to the queue!")
        req3 = {
                'method': 'POST', 
                'args': [], 
                'values': {}, 
                'content-type': 'application/x-www-form-urlencoded', 
                'is_json': False,
                'data': b'group=test1&password=pass1&action=finished&song=None', 
                'form': {'group': 'test1', 'password': 'pass1', 'action':'finished', 'song': 'feverdream'}
                }
        print(request_handler(req3))

if __name__ == '__main__':
        #unittest.main()
        #test = TestServerDeliverable1()
        #test.test_skip_with_two_songs() 
        '''
        print("Finished")
        req3 = {
                'method': 'POST', 
                'args': [], 
                'values': {}, 
                'content-type': 'application/x-www-form-urlencoded', 
                'is_json': False,
                'data': b'group=test1&password=pass1&action=finished&song=None', 
                'form': {'group': 'test1', 'password': 'pass1', 'action':'finished', 'song': 'feverdream'}
                }
        print(request_handler(req3)
        
        print("\n\nAdd Song")
        req = {
                'method': 'POST', 
                'args': [], 
                'values': {}, 
                'content-type': 'application/x-www-form-urlencoded', 
                'is_json': False,
                'data': b'group=test1&password=pass1&action=play&song=lovefool', 
                'form': {'group': 'test1', 'password': 'pass1', 'action':'play', 'song': 'test6'}
                }
        print(request_handler(req))
        '''
        print("\n\nSkip Song")
        req3 = {
                'method': 'POST', 
                'args': [], 
                'values': {}, 
                'content-type': 'application/x-www-form-urlencoded', 
                'is_json': False,
                'data': b'group=test1&password=pass1&action=finished&song=None', 
                'form': {'group': 'test1', 'password': 'pass1', 'action':'finished', 'song': 'feverdream'}
                }
        print(request_handler(req3))
        
        


