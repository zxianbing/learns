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
        rem Ŀ¼d:\<span style="font-family: Arial, Helvetica, sans-serif;">MIS</span>�Ѵ��ڣ����贴��  
    ) else (  
        rem ����d:\MIS   
        mkdir %Pan%
    )  
adb pull storage/emulated/0/mtklog .\log\%name%-%year%-%mounth%-%data%-%minute%-%sec%
pause|echo pull mtklog sucessful !


