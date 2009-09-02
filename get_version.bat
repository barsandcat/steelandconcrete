@ECHO off
DEL version.nsh
FOR /F "usebackq" %%s IN (`bin\steelandconcrete_d_server.exe --version`) DO @SET REV=%%s
ECHO !define VERSION "%REV%" >> version.nsh

