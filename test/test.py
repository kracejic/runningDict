


import ctypes
import platform
import os

print(platform.system())
if platform.system() == "Linux":
    testlib = ctypes.CDLL("./libRunningDict.so")
else:
    if platform.architecture()[0] == '64bit':
        dllpath = "libRunningDict64.dll"
    else:
        dllpath = "libRunningDict32.dll"
    print("Loading: " + dllpath)
    testlib = ctypes.CDLL(dllpath)
testlib.search.restype = ctypes.c_char_p

testlib.addDictionary.restype = ctypes.c_bool
testlib.addDictionaryForce.restype = ctypes.c_bool
print("Loading dicts...")
# print (testlib.addDictionaryForce(b'test.dict', True))
print (testlib.addDictionaryForce(b'freedict-deu-eng.dict', False))
# print (testlib.addDictionaryForce(b'test.dict', True))
print("...done")

testlib.setNumberOfThreads(ctypes.c_int(8))

import json

howMany = 40
sumOfTimes = 0
print("Testing:")
for x in range(0,howMany):
    rawBytes = testlib.search(b'CKeine_katze_CANTO_Tag_RSUUnit_KatzeUnd ihre')
    decodeData = rawBytes.decode("utf-8")
    jsonResult = json.loads(decodeData)
    sumOfTimes = sumOfTimes + jsonResult["speed"]
    print(" " + str(jsonResult["speed"]) + "s") # +", " + str(sumOfTimes))




# print (json.dumps(jsonResult, sort_keys=True, indent=4, separators=(',', ': ')))

print("\navg time = " + str(sumOfTimes/howMany) + "s")

