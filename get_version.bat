@ECHO off
DEL version.nsh
FOR /F "usebackq" %%s IN (`bin\steelandconcrete_server.exe --short_version`) DO @SET REV=%%s
ECHO !define VERSION "%REV%" >> version.nsh

