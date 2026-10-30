# SOL worker CPU sample

The current ten-image package passed three isolated Options interactions.
Wall-time open/close ms: 2406/2078, 1672/1891, 1828/1968. Worker process
kernel+user CPU-time deltas ms: 2234/2016, 1593/1782, 1766/1812. The
process-level CPU deltas are near wall time in this observation, which makes
pure timer waiting an insufficient explanation for the bulk of these samples.
The API aggregates all worker threads, has scheduler/time-accounting
resolution, and the timing phases include probe overhead, so this does not
identify a specific function or exclude all blocking. The probe uses
GetProcessTimes and does not change product timing or guest bytes. The stage
manifest and source hash identify the run; it is not a controlled before/after
benchmark or the Full suite.
Probe SHA-256: FA8863E2DE210B24B4EDFD795EAD979975F48C6EA2AE9DEF6DB745D66246D355
