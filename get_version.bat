@ECHO off
DEL version.nsh
FOR /F "usebackq tokens=3" %%s IN (`bin\steelandconcrete_server.exe --version`) DO @SET REV=%%s
ECHO !define VERSION "%REV%" >> version.nsh

