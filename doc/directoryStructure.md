


# Description of the directory structure

* mainFolder
    - **build** - user created, build takes place here
        + **dist** - here builded application is copied
            * **bin**
            * **share**
                - **data** - here folder data is copied
                - **doc**
                    + **RunningDict**
                        * doxygen documentation lives here 
    - **data** - contains some test data
    - **test** - contains some tests and test data
        + CMakeLists.txt - tests building
    - **doc** - contains documentation
        + CMakeLists.txt - doc building
        + **doxygen**
            * doxygen settings, and modern style config
    - **source** - contain source files
        + **gui** - source for gui part
        + .ycm_extra_con.py - for ycm smart automcompnetion
    - **external**
        + CMakeLists.txt - external projects handling (eg. Download of Catch)
    - readme.md - main readme file
    - CMakeLists.md - CMake configuration
    - makebuildrun.sh - compile and run custom command for make
    - ninjabuildrun.sh - compile and run custom command for ninja
    - runningDict.sublime-project - project file fopr sublime text
    - .clang-format - clang format config file


