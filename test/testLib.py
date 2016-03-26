import ctypes
import platform
import os
import sys
import json

print(sys.argv[1])

liblocation = sys.argv[1]


testlib = ctypes.CDLL(liblocation)

testlib.search.restype = ctypes.c_char_p

testlib.addDictionary.restype = ctypes.c_bool
testlib.addDictionaryForce.restype = ctypes.c_bool
# print (testlib.addDictionaryForce(b'test.dict', True))
print (testlib.addDictionaryForce(b'test.dict', False))
print (testlib.addDictionaryForce(b'test2.dict', False))
print("...done")

# testlib.setNumberOfThreads(ctypes.c_int(8))


rawBytes = testlib.search(b'CKeineKatz ihre')
decodeData = rawBytes.decode("utf-8")
jsonResult = json.loads(decodeData)

assert jsonResult["results"][0]["matches"] == []
assert jsonResult["results"][1]["word"] == "katz"
assert jsonResult["results"][1]["score"] == 1
assert len(jsonResult["results"][2]["matches"]) == 2
assert jsonResult["results"][2]["matches"][0][0] == "ihre"
assert jsonResult["results"][2]["matches"][0][1] == "your"
assert jsonResult["results"][2]["matches"][1][0] == "ihre"
assert jsonResult["results"][2]["matches"][1][1] == "youre"


# print (json.dumps(jsonResult, sort_keys=True, indent=4, separators=(',', ': ')))


