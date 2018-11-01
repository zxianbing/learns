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
set Pan=.\log 
 if exist %Pan% (         
        rem 目录d:\<span style="font-family: Arial, Helvetica, sans-serif;">MIS</span>已存在，无需创建  
    ) else (  
        rem 创建d:\MIS   
        mkdir %Pan%
    )  
adb pull storage/emulated/0/mtklog .\log\%name%-%year%-%mounth%-%data%-%minute%-%sec%
pause|echo pull mtklog sucessful !


