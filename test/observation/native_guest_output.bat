@echo off
echo DOS-INTERVAL
echo ready>tests\CGREADY
:wait
if not exist tests\CGDONE goto wait
echo DOS-RESUMED
