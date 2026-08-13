# T198 S25 P1 Source-Built NTDOS Next Boundary 001

The S25 generator verifies and embeds only the locked source-built NTDOS
input: 27,858 bytes, SHA-256
`957662320654AD5251C3A8B228A5DADEC28AA65DDDBCBA38C3658A6E7F93BC84`.
It is passed unchanged to the existing checked DEM `50:11` publication.

The MSVC x64 `/MT` CPU5 native run now reports:

```
t198-s23 status=4 observed-5011=1 observed-503b-resume=1 observed-stop=1 next=54:05 terminal=1:0000:0ad2
```

The prior mock NTDOS HLT is gone.  Exact NTIO reaches `50:11`, resumes the
global DEM-MISC `50:3B` result, enters source-built NTDOS, and then reaches
the next unhandled BOP `C4 C4 54 05`; the existing test observer returns a
typed controlled stop.  The CPU prefetch diagnostic is retained observation,
not a cause attribution.  `54:05` must next be mapped through the existing
COMMAND plane before any provider is enabled.
