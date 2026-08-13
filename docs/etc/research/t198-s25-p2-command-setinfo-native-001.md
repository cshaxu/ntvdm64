# T198 S25 P2 COMMAND SetInfo Native 001

`54:05` is original `SVC_CMDSETINFO`.  Its source-derived adapter service
validates DS:DX, DS:DX+206, DS:BX and DS:CX as one-megabyte physical locators,
stores them in the COMMAND launch-state plane, and resumes at RIP+4.  It has
no host action or raw guest pointer.

The MSVC x64 `/MT` focused composition fixture passes its four locator and
resume assertions.  A native exact NTIO/NTDOS relink reaches `54:05`, resumes
it, and stops at the next unhandled original DEM `50:1B`.  The observer's
controlled stop is expected; no DTA provider change is included here.
