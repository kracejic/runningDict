# RunningDict

## Command line arguments

* -d [file]   dictionary file
* -D [file]   dictionary with higher priority, gets also partial words
* -j[num] - number of threads
* -h --help - prints help


## Speed reference table

Measured on i7-4770 (4cores + HT) @ Linux 3.16.0

| # of threads | time | speedUp |
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


## Building


### Build on linux

```
mkdir build ; cd build
cmake ..
make -j8
```

```
mkdir build ; cd build
cmake -GNinja -DCMAKE_CXX_COMPILER="clang++-3.6" ..
ninja
```


### Build on windows using MSYS2 + ninja

* With gcc:

```
mkdir build ; cd build
cmake -G "Ninja" ..
ninja install
```

* With clang++

```
mkdir build ; cd build
cmake -G "Ninja" -DCMAKE_CXX_COMPILER="clang++" ..
ninja install
```

* Note for sublime text: You want to have *c:/runLinux32.bat* with this:
```
set MSYSTEM=MINGW32
C:\msys64\usr\bin\bash.exe --login -c "cd - ; %*"
```
    * and edit makebuildrun.sh and ninjabuildrun.sh to tailor arguments

### CMAKE variables

* -DCMAKE_INSTALL_PREFIX= - location for instalation
* -DVERSION_HOST= - build machine name





