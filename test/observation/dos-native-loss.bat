@echo off
cmd.exe /d /k echo S2-NATIVE-WAIT
if errorlevel 255 echo S2-NATIVE-EXIT-255
echo S2-DOS-RECOVERED
mem
echo S2-DOS-END
