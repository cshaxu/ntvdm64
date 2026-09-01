# M0 T337 S4 — Fixed MVDM system-root observation

## Question

Does the freshly linked x86 product start from the fixed package whose only
media root is `mvdm/`, without changing guest bytes or switching the observer?

## Procedure

1. Build the existing console-owning, non-debug x86 observer under
   `build/M0-T337/S4/observer`.
2. Use `ObserveSoftpcStartup.mjs` with the freshly linked x86 product and the
   byte-identified `build/M0-T337/S3/package-contract-r3` stage.
3. Keep the observer, timeout, media manifest and working directory fixed.

## Observation

The observer completed normally and recorded its bounded timeout after 8000
milliseconds. Its JSON record binds the staged product SHA-256
`0b65b082b45c597f5e8230046083f63888a7da0bf4231a2c2c72eb15e2598aa0`,
stage manifest SHA-256
`4ba66c24580eaf95b5d0be753760fc27e382fb80dbcd2c54ace3f076725fb88c`,
and fixed media manifest SHA-256
`15b6a215e9bf3cede6650c0990602b0f421181f18ea2ff431031fd237cd82858`.
The report is a bounded timeout; it is not an execution-continuity claim.

## Interpretation

The packaged root is accepted by the unchanged observer and no immediate
missing-media exit was observed. The timeout does not identify a new owner and
does not prove a guest path, COMMAND or Win16 execution result. No further
runtime retry is selected by T337.
