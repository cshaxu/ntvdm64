# T148 S1 single-member CPU archive closure

## Result

T148 copies the retained T130 CPU archive and replaces exactly one member:
the registered BX-TRACE-056 `cpu.o`.  It does not rebuild the other CPU
translation units and does not link a direct override object.

## Evidence

- T130 base archive SHA-256:
  `E017EE968447D9F8DEDDF6F3277C1FEF9D213D04F097D57C99E45B09281BD4DB`.
- T148 archive SHA-256:
  `FE86D58DAAC61443336EE23CA55D3700FACB68AA268E6B24072F584BB4629B65`.
- A member-by-member extraction audit retained at
  `artifacts/analysis/t148-s1-single-member-hash-audit-001` finds 93 members:
  92 byte-identical, and only `cpu.o` differs.
- Base `cpu.o` SHA-256 is
  `437CB32ADAF691D2A0824D5727D1D15A7C6065CCDB44724732BE573A0F328A67`;
  replacement `cpu.o` SHA-256 is
  `3266FEE8DB2275BD674532F49E07AC6098B9B2B4C304A192A4894EF149FDC0A1`.
- The final map resolves CPU functions through `libcpu:cpu.o`, contains no
  `cpu_trace.obj`, and links executable SHA-256
  `8A5E67F9546E4BCE9825BC6E04E6322F4D03EDCA2BCB1458EB7A2182D03FD760`.

This closes static comparison provenance. It is not a runtime result; a later
single bounded observation must be separately admitted.
