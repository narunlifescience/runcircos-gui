[![alt tag](runcircos-gui/data/icons/runcircos-gui2.png)
# runcircos-gui
[![Build Status](https://travis-ci.org/narunlifescience/runcircos-gui.svg?branch=master)](https://travis-ci.org/narunlifescience/runcircos-gui)  [![Build status](https://ci.appveyor.com/api/projects/status/jq6q44uggvokxmgt?svg=true)](https://ci.appveyor.com/project/narunlifescience/runcircos-gui)

runCircos-gui is a simple yet comprehensive crossplatform software devoloped in QT(C++) to execute Circos http://circos.ca/ from a graphical user interface. The software eliminates the need to use command line for running Circos without compremising the power of complete commandline parameters and options.

runCircos-gui optimize running parameters (both toggle options & options with arguments) and install Perl packages without using command

Compiling from source
---------------------

Get the code (if you haven't already):

    git clone https://github.com/narunlifescience/runcircos-gui.git

Compile and install:

    cd src
    qmake
    make 
    sudo make install
