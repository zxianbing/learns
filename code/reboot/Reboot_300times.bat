@echo off
rem �ر��������
set minute=%time:~3,2%
set sec=%time:~6,2%
set /a count=0

@echo      ******����300��******  >  ./Reboot_log_%minute%_%sec%.txt
@echo      ******����300��****** 
	
setlocal enabledelayedexpansion
rem ���ñ���Ϊ�ӳ���չ �ӳٱ���

:start
	rem ��1����1��300����������
	set /a count+=1
	
	@echo.  >>  ./Reboot_log_%minute%_%sec%.txt
	@echo.
	
	echo �� %count% �������� >>  ./Reboot_log_%minute%_%sec%.txt
	echo �� %count% ��������
	call time_get.bat 

	
	for /f "delims=" %%i in ('adb  reboot 2^<^&1') do (
		echo.%%i >> ./Reboot_log_%minute%_%sec%.txt
		echo.%%i
	)
	set /a trytimes=0
	
	for /l %%i in (1,1,100) do (
		set /a trytimes+=1
		adb shell getprop sys.boot_completed 1>nul 2>nul&& goto success 
		ping localhost -n 3 >nul
	)
	rem echo %trytimes%
	if "%trytimes%"=="100" goto fail
	
:success
	ping localhost -n 10 >nul
	rem ��������֮��ȴ�10s
	for /f "tokens=1 delims=:" %%i in ('adb devices^| findstr/n "[0-9]"') do (
		echo �����ɹ���   >>  ./Reboot_log_%minute%_%sec%.txt
		echo �����ɹ���
		call time_get.bat
		goto end
	)
	echo ����ʧ�ܣ� >>./Reboot_log_%minute%_%sec%.txt
	echo ����ʧ�ܣ�
	call time_get.bat
	goto end
	
:fail
	echo ����ʧ�ܣ� >>./Reboot_log_%minute%_%sec%.txt
	echo ����ʧ�ܣ�
	call time_get.bat
	goto end
	
:end
	@echo.  >>  ./Reboot_log_%minute%_%sec%.txt
	@echo.  >>  ./Reboot_log_%minute%_%sec%.txt
	@echo.  >>  ./Reboot_log_%minute%_%sec%.txt
	@echo.
	@echo.
	@echo.

	if "%count%"=="300" goto continue
	rem Ĭ������300��
	
goto start

:continue

adb shell input swipe 240 719 396 720  
adb shell input keyevent 26
exit

:delay
echo WScript.Sleep %1>delay.vbs
rem ��wscript.sleep %1д�뵽temp.vbs��
CScript //B delay.vbs
rem ����delay.vbs
del delay.vbs
rem ɾ��delay.vbs
goto :eof
ren ����eof��ǩ

pause
