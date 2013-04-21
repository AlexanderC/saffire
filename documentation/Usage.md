Using Saffire
-------------
Saffire is a single binary that holds many other smaller files: the executer, the fastcgi server, the lint checker etc etc.
To fetch help on which commands Saffire can use, look them up in the help:

    saffire help

or optionally: 'saffire', 'saffire -h' or 'saffire --help'.

In order to figure out help about a specific command, use:

    saffire help <command>

For instance:

    saffire help exec

which shows you information and arguments that can be used to execute saffire code.



Running scripts directly from Saffire
-------------------------------------
Running a Saffire script is easy:

    saffire <scriptname>.sf

This actually is short for:

    saffire exec <scriptname.sf>

To run the standard hello world example:

    saffire hello.sf

or

    saffire exec hello.sf

This should output "hello world" on your screen.



Using Saffire as a FastCGI module
---------------------------------
Saffire can be run as a FastCGI process. More information about setting this up will be added later.



Configuration files
-------------------
There can be many 4 places where configuration files resides. They are checked in this order:

    * If the --configpath parameter has been given, this path is checked.
    * Check for saffire.ini in the path where the Saffire binary resides.
    * Check for saffire.ini in the homedir of the user that runs the Saffire binary
    * At the specific global location: /etc/saffire/saffire.ini

When a ini file has been found, it will not try to find and load other configuration files.



Generating a configuration file
-------------------------------
It's easy to get a default configuration:

    saffire config generate > saffire.ini

For most users, this configuration should be pretty ok. If you like to modify this configuration, either manually edit
the file, or use 'saffire config':

    saffire config set global.timezone "Europe/Amsterdam"

This will automatically save a "timezone = Europe/Amsterdam" line under the "global" section of your ini file. 'saffire
config' has got more options to set and get your settings easily.



Advanced Saffire usage
----------------------
For debugging and contributing to Saffire, it sometimes is necessary to use some advanced Saffire settings.
First of all, you can create a debug version of saffire by using

    ./configure --enable-debug
    make clean
    make

If you run saffire, it will output lots of debug information, and you can use a debugger like GDB to step through the
code easily.

Sometimes, you need to figure out what kind of bytecode Saffire is generating from your scripts. This can be done with
the 'bytecode' command:

    saffire bytecode compile hello.sf --text --dot

This compiles hello.sf into a bytecode file (hello.sfc) without executing it. The --text will generate an assembly file
(hello.sfa), which gives you an overview of the generated bytecode. The --dot will generate a 'hello.dot' file, which
can be used generate a visual AST tree:

    dot -T png -o hello.png hello.dot