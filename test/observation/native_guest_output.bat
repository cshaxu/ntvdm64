@echo off
echo DOS-INTERVAL
echo ready>@PACKAGE@tests\CGREADY
:wait
if not exist @PACKAGE@tests\CGDONE goto wait
echo DOS-RESUMED
