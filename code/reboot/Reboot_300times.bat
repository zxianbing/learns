@echo off
rem 关闭命令回显
set minute=%time:~3,2%
set sec=%time:~6,2%
set /a count=0

@echo      ******重启300次******  >  ./Reboot_log_%minute%_%sec%.txt
@echo      ******重启300次****** 
	
setlocal enabledelayedexpansion
rem 设置本地为延迟扩展 延迟变量

:start
	rem 从1步进1到300（计数器）
	set /a count+=1
	
	@echo.  >>  ./Reboot_log_%minute%_%sec%.txt
	@echo.
	
	echo 第 %count% 次重启： >>  ./Reboot_log_%minute%_%sec%.txt
	echo 第 %count% 次重启：
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
	rem 到了桌面之后等待10s
	for /f "tokens=1 delims=:" %%i in ('adb devices^| findstr/n "[0-9]"') do (
		echo 重启成功：   >>  ./Reboot_log_%minute%_%sec%.txt
		echo 重启成功：
		call time_get.bat
		goto end
	)
	echo 重启失败： >>./Reboot_log_%minute%_%sec%.txt
	echo 重启失败：
	call time_get.bat
	goto end
	
:fail
	echo 重启失败： >>./Reboot_log_%minute%_%sec%.txt
	echo 重启失败：
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
	rem 默认重启300次
	
goto start

:continue

adb shell input swipe 240 719 396 720  
adb shell input keyevent 26
exit

:delay
echo WScript.Sleep %1>delay.vbs
rem 将wscript.sleep %1写入到temp.vbs中
CScript //B delay.vbs
rem 运行delay.vbs
del delay.vbs
rem 删除delay.vbs
goto :eof
ren 跳到eof标签

pause
