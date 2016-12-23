


# Description of the directory structure

* mainProjectFolder
    - **build** - user created, build takes place here
        + **dist** - here builded application is copied (by deafult, see CMAKE_INSTALL_PREFIX in main CMakeLists.txt)
            * **bin**
            * **share**
                - **data** - here folder data is copied
                - **doc**
                    + **RunningDict**
                        * doxygen documentation lives here 
    - **data** - contains some test data
    - **doc** - contains documentation
        + CMakeLists.txt - doc building
        + **doxygen**
            * doxygen settings, and modern style config
    - **packaging**
        + CMakeLists.txt - package creation
        + exampleApp.desktop - linux shortcut (app is then visible in launchers)
        + exampleApp.png - icon for linux shortcut
        + exampleApp.icon.in.rc - icon for windows description file (used in source/CMakeLists.txt)
        + exampleApp.ico - icon for windows shortcut (linked via exampleApp.icon.in.rc into the executable)
    - **source** - contain source files
        + **gui** - source for gui part
        + CMakeLists.txt - source building
        + .ycm_extra_conf.py - for ycm smart autocompletion
    - **test** - testing
        + CMakeLists.txt - tests building
        + testmain.cpp - main tests function
    - **external**
        + CMakeLists.txt - external projects handling (eg. Download of Catch)
    - readme.md - main readme file
    - CMakeLists.md - Main CMake configuration
    - .clang-format - clang format config file
    - .clang-tidy - clang tidy config file
    - .travis.yml - continuous integration configuration


