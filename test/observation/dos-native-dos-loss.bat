@echo off
cmd.exe /d /c run16.exe COMMAND.COM
if errorlevel 255 echo S2-NATIVE-EXIT-255
echo S2-DOS-RECOVERED
mem
echo S2-DOS-END
