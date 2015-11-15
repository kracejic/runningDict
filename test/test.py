


import ctypes
import platform

print(platform.system())
if platform.system() == "Linux":
    testlib = ctypes.CDLL("./libRunningDict.so")
else:
    testlib = ctypes.CDLL("./libRunningDict")
testlib.search.restype = ctypes.c_char_p

testlib.addDictionary.restype = ctypes.c_bool
print (testlib.addDictionary(b'test.dict', True))
print (testlib.addDictionary(b'freedict-deu-eng.dict', False))

testlib.setNumberOfThreads(ctypes.c_int(8))

import json

howMany = 40
sumOfTimes = 0
for x in range(0,howMany):
    rawBytes = testlib.search(b'CKeine_katze_CANTO_Tag_RSUUnit_KatzeUnd ihre')
    decodeData = rawBytes.decode("utf-8")
    jsonResult = json.loads(decodeData)
    sumOfTimes = sumOfTimes + jsonResult["speed"]
    print(str(jsonResult["speed"]) +", " + str(sumOfTimes))




# print (json.dumps(jsonResult, sort_keys=True, indent=4, separators=(',', ': ')))

print("avg time = " + str(sumOfTimes/howMany))

