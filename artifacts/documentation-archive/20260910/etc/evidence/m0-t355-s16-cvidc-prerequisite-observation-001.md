# M0 T355 S16 — fixed-container C-VID prerequisite observation

## Immutable container

One console-owning, non-debug observation used the existing `O:\\ntvdm`
package.  The media manifest remained fixed; `ObserveSoftpcStartup.mjs`
replaced only `original-softpc-process.exe` with the formally linked CPU40/x86
product.

The observer record is retained at:

```text
O:\ntvdm\m0-t355-s16-cvidc-vector.txt
```

Its JSON record identifies the fixed media-manifest hash as
`15b6a215e9bf3cede6650c0990602b0f421181f18ea2ff431031fd237cd82858` and the
staged product hash as
`e2585305b5e0a2de6fb09243dcf694a5d540193dd62f64184d7e450961505584`.

## Result

The prior null indirect-call at `Sas.Sas_overwrite_memory` did not recur.  The
same bounded run reached `50:42` repeatedly, as recorded in the existing BOP
return report.  It then ended with an access violation at:

```text
runtime address: 0x007b9651
image base:      0x007b0000
image offset:    0x00009651
link address:    0x00409651
```

The formal map places that offset between original CPU40
`ccpusas4.c::_phyR` (`0x00409520`) and its next local label
`$LN19` (`0x00409678`).  The next owner is therefore the original CCPU40
physical-read path, not the repaired C-VID vector and not `50:42`.

## Disposition

S16 closes as a single discriminating observation.  It establishes that the
S15 binding removed the exact earlier terminal and transfers only the newly
reached CPU40 physical-read owner for a separately admitted source/contract
audit.  It does not authorize a retry, a FastRead claim, or a repair of
`_phyR`.
