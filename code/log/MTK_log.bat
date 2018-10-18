@echo off
adb wait-for-device
adb root
adb remount
set name=mtklog
set year=%DATE:~0,4%
set mounth=%DATE:~5,2%
set data=%DATE:~8,2%
set minute=%time:~3,2%
set sec=%time:~6,2%
adb pull storage/emulated/0/mtklog C:/Users/Administrator/Desktop/log/log/%name%-%year%-%mounth%-%data%-%minute%-%sec%
pause|echo pull mtklog sucessful !


