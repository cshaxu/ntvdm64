# M0 T194 S45: Metadata Provider Derivative Build

The documented MSVC x86 `nmake /f ntdos64-t98-current-adapter.mak
ntdos64-t98-current-adapter.exe` completed once in the S44 root. It compiled
68 adapter objects, five CLI objects and the pre-existing generic-listener
`cpu\exception.o`, then linked the engine. The only warning was the retained
`exception.cc:1562` C4244 conversion warning.

Engine SHA-256:
`92A1C09EAA96FCC8D2EEFED473FB54240A2F5224551D122C793D197511CE388C`.
The rebuilt exception object SHA-256 is
`FD4BD854D037C9D9094F6ED92516945449222EAED24C4B0EE2F0E6903610C9FD`.

The final link contains the paired metadata provider object and no legacy
IOCTL object. It does not rebuild a Bochs archive, device, firmware or
`main.o`. The executable was not run.
