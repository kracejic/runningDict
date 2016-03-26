# RunningDict

This project is fast translator for programmers. It is capable of dissasembling camelCases, underscore_case and CThisIsClass names. It internaly uses dictd-like dictionaries.


There is extra support for these:
* .ycm_extra_conf.py for ycmd support 
    - Smart autocompletion, goto def, etc.
    - Works in Vim, Sublime Text 3, Atom
* Sublime text 3 project file
    - Build system defined for *make* or *ninja*
* Feel free to delete these files.


## Binary files

There are two options how to use this. Executable from command line, or library with C interface.

## Executable

Executable has the name runningDict(.exe). For implementation see main.cpp.

*Command line arguments reference:*

* -d [file]   dictionary file
* -D [file]   dictionary with higher priority, gets also partial words
* -j[num] - number of threads, defaults to number of cores
* -h --help - prints help

*Usage exaple:*

`./runMe -j8 -d freedict-deu-eng.dict -D test.dict Ihre deutsch ist CStrukturKeine`


## Library

libRunningDict.so or libRunningDict64.dll or libRunningDict32.dll. For interface see LibInterface.h file. Library has advantage of needing to load dictionary only once and then only processing new searches. Command line application has to load dictionaries every time it is executed.

For example response to "CKeineKatz ihre" can match following JSON object:
~~~
{
    "results": [
        {
            "word": "keine"
            "score": 0,
            "matches": [
                ["keine", "neither; no; none"]
            ],
        },
        {
            "word": "katz"
            "score": 1,
            "matches": [
                ["latz", "bib"],
                ["katze", "cat; tabby"],
                ["satz", "clause; dart; jerk; phrase; record; sentence; set"],
                ["kauz", "codger; fogey"]
            ],
        },
        {
            "word": "ihre"
            "score": 0,
            "matches": [
                ["ihre", "her; hers; their; theirs"],
                ["ihre", "your"]
            ],
        }
    ],
    "speed": 0.018015
}
~~~

Note the type in Katze word. Library will return four possible translations and they can be presented to user and he can then better understand which one is correct.


## Build

### Prerequisites for build

* CMake 3.2 and newer
* Doxygen for docs (*Graphviz for more graphs in docs*)


### Build on linux

Standard Makefiles:
~~~
mkdir build ; cd build
cmake ..
make -j8 install
~~~


Ninja build witch clang, build all+doc and install it to dist folder:
~~~
mkdir build ; cd build
cmake -GNinja -DCMAKE_CXX_COMPILER="clang++-3.7" ..
ninja all doc install
~~~

There are basic tests which can be exuted by building **check** target.

~~~
mkdir build ; cd build
cmake ..
make check
~~~


### Build on windows

* note on call graphs and rest: Since there is no package in MSYS2, getting more graphs to work could be challenging.


#### Build on windows using MSYS2 + ninja

On windows prefer Ninja since it is **MUCH** faster than make.

With gcc build and install (default is build/dist):
~~~
mkdir build ; cd build
cmake -GNinja ..
ninja install
~~~

With clang++, build executable, doxygen documentation and install:
~~~
mkdir build ; cd build
cmake -GNinja -DCMAKE_CXX_COMPILER="clang++" ..
ninja all doc install
~~~

Notes for sublime text: You want to have *c:/runLinux32.bat* with this:
~~~
set MSYSTEM=MINGW32
C:\msys64\usr\bin\bash.exe --login -c "cd - ; %*"
~~~
    * and edit makebuildrun.sh and ninjabuildrun.sh to execute final executable


#### Build on Windows using Microsoft Visual Studio 14

First you can edit `CMakeLists.txt` and set new project name. Just go to the folder with the project and create new directory and create project files with this:

~~~
mkdir buildmsvc
cd buildmsvc
cmake -G "Visual Studio 14 2015" ..
~~~

and you can now open a `.sln` file with Visual Studio. You need to RMB click on executable target and *Set as StartUp project*. To really see the console window you want to do *Start without debugging*, when run in debug mode, console widow is closed too fast.


## Speed reference table

Measured on i7-4770 (4cores + HT) @ Linux 3.16.0

| # of threads | time [ms] | speedUp |
|-------|-------|------|
|  12   | 84    | 2.6  |
|  10   | 87    | 2.5  |
|  9    | 84    | 2.6  |
|  8    | 76    | 2.93 |
|  7    | 81    | 2.7  |
|  6    | 89    | 2.5  |
|  5    | 96    | 2.3  |
|  4    | 101   | 2.2  |
|  3    | 91    | 2.4  |
|  2    | 121   | 1.84 |
|  1    | 223   | 1    |




## Developing

* [Basic directory structure](./md_doc_directoryStructure.html)


# License

> Copyright (c) 2016 Jiri Ohnheiser
> 
> Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
> 
> The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
> 
> THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
