# T484 S5 P3 Keyboard PIF consolidation

CMD keyboard preparation now copies one authored 681-byte profile template
and uses the released PIF editor to set the program, CONFIG.NT and AUTOEXEC.NT
paths. The native PIF writer, its build target and 78 lines of duplicate C
code are removed. The original keyboard reader remains a native fixture and
uses the same production keyboard and registry objects.

Reapplying the three fields to the template reproduced the old writer's
output byte for byte. The integration test also reads back the actual staged
profile, including its 386 flags and memory limits, before starting it.

The initial test run timed out before MEM output. The standard Windows logon
environment rerun produced real MEM output and passed the configured,
malformed, absent and layout-unavailable/no-install reader checks. Receipts
are under build/t484-s5-keyboard-pif.

PID 16500 completed the cached full build, matched all eleven host build
images to both published sets, and verified all fourteen addon release
hashes. Neither published host set required a replacement image.
