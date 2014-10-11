#!/bin/bash

g++ -c main.cpp
g++ -c rtspServer.cpp
g++ -c tools.cpp -lX11
g++ -o appServer main.o rtspServer.o tools.o -lX11
