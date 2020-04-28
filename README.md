# Chat Server

A simple chat server that uses sockets to communicate between clients, utilising concurrency
to handle several users at once. The client program runs on the terminal and uses the ncurses library
to display a minimalistic GUI that serves it's purpose. This project is created for educational purposes,
to get some practice on concurrent events and utilising external libraries in C.

## Features

* Allows multiple users to connect
* A minimalistic GUI using ncurses
* Concurrency

## Dependencies

* Any Linux distribution
* CMake version 3.10.2 or greater
* Ncurses (package names are libncurses5, libncurses5-dev)

## Usage

1. Get the listed dependencies. To install CMake and Ncurses run these commands:
```
sudo apt install build-essential cmake
sudo apt install libncurses5, libncurses5-dev
```
2. After getting the listed dependencies, make a directory in the repository called "build". Afterwards,
change into that directory and run these commands:
```
cmake ..
make
```
3. If there are no errors, on one terminal run this command to start the server:
```
./chat_server
```
4. Run this command on another terminal on the same directory to connect to the server using a client:
```
./chat_client
```
5. Type in a username that is less than or equal to 16 characters long, and you should be able to
connect to the server! The terminal running the server will print a message saying you connected.

6. Type /quit in the client to quit and Ctrl-C on the server to stop it.

## Roadmap

Currently, there is no plans to continue work on this chat program. However, if I ever work on it again,
these are the features I plan to implement or bugs I intend to fix:

* Fix flickering
* Prevent junky text when typing too fast
* Chat scrolling
* Add more commands (e.g. /help, /onlineusers)
* Multiple chat channels
* Customistation (e.g. colour, different GUI layout etc.)
* Server annoucements for users leaving/joining
* Make servers send a magic string to tell the client program they have connected to the right server

## Known Issues

* Typing too fast (spamming one character and enter really, really fast) will print some junk from ncurses' refresh
* There is some flickering when holding any typable character keys for more than 3 or more seconds.
Although they may be flickering on every key press depending on what type of terminal you use. GNOME terminal works fine for the most part.

## Support

* Any Linux distribution
* Any terminal for the client program (preferably GNOME, where there is less flickering)
* No support for Windows
* macOS may work
