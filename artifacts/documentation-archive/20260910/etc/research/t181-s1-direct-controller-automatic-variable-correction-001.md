# T181 S1 direct-controller automatic-variable correction 001

T180 exposed a prelaunch defect in the T179 controller: the local variable $pid
collided case-insensitively with PowerShell's read-only $PID automatic variable.
The controller now uses directProcessId throughout its result record.

The focused static test now parses the script and rejects assignments to PID,
PSHOME, PSVersionTable, Host, Error, Input, Matches and Args, in addition to
its pre-existing direct-PID and forbidden-wrapper checks. The test passed. No
Bochs, guest or fixture was launched or changed.

T182 may create a fresh source-identical evidence root and make one direct-PID
observation. Neither T180 nor T181 is a guest outcome.

