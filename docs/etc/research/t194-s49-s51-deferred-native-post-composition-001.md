# M0 T194 S49--S51: Deferred Native-POST Metadata Composition

## Question

Can the paired `AX=4408h/4409h` metadata-provider source closure be
recomposed with native Bochs POST and deferred opaque handoff, without
expanding the Bochs boundary or executing the new image?

## Inputs

- The S39 paired metadata provider source replacement and its S42 focused
  MinGW C11 test.
- `tools/New-T98S1CurrentAdapterEngineDerivative.ps1` at the committed source
  revision, with `-DeferredStartupPlan`.
- Frozen S27 ROM root
  `artifacts/build/current/t194-s27-dpb-snapshot-bundle-r1/ROM`.
- MSVC x86 environment and the S49 generated root.

## Procedure

S49 generated
`artifacts/build/current/t194-s49-deferred-metadata-closure-r1` with
`-DeferredStartupPlan`. The generated manifest reports 68 adapter sources,
five CLI sources, and exactly two rebuilt Bochs objects: `main.o` and
`cpu/exception.o`. Its makefile sets
`BX_NTVDM_ENABLE_EXECUTION_PLAN=0` and
`BX_NTVDM_ENABLE_DEFERRED_STARTUP_PLAN=1`; it retains the established generic
listener and real-mode-vector diagnostic. It does not add a Bochs archive,
device, firmware, or a new Bochs source patch.

S50 invoked once from that root:

```text
call VsDevCmd.bat -arch=x86 -host_arch=x64 && nmake /f
ntdos64-t98-current-adapter.mak ntdos64-t98-current-adapter.exe
```

The command compiled the 68 adapter objects, five CLI objects, `main.o`, and
`cpu/exception.o`, then linked. The only compiler diagnostic was the retained
`exception.cc:1562` C4244 conversion warning. No archive or device target was
built. The executable was not run.

Before the S51 packet was formally entered in Status, the existing immutable
bundle tool copied that S50 executable and the frozen S27 ROM root to
`artifacts/build/current/t194-s51-deferred-metadata-bundle-r1`. This is a
chronology/governance correction: the assembly was a non-executing artifact
copy, and the record below makes its inputs and no-run limit explicit.

## Observations

| Item | SHA-256 |
| --- | --- |
| S50 engine / S51 `ntdos64-bochs.exe` | `79D404A9451F218009408D75AF2F2A36CFB8F984FFF64C0328DC34A8EBB0857E` |
| S50 `main.o` | `6732ADEE2202AC18FE03AD8F85757478D2EF92539CB0E39A5B35ED16A44DD648` |
| S50 `cpu/exception.o` | `9D8AD50E065A4444978D33117A6508F045264E651488E6379ABBBEF03D7555AF` |
| BIOS | `D974DE50C94D76445123FD1AC886612351BED7CC5E3C8D07C556814DA72BEBD6` |
| VGA BIOS | `CD9FDD6A789DCD22B8A6B3B152788D43238DE49CCE674CFF57BDEB94580246C6` |
| POST handoff option ROM | `E4CA4AE5190A15ED741D1DFED5A58533A2B6926A40302D839FB714F9FC673FFB` |

The S51 manifest has the same engine and ROM hashes. The bundle contains the
engine and copied `ROM` tree only. No CLI, engine process, CPU loop, guest, or
host capability was started.

## Interpretation and Follow-up

This is a reproducible composition candidate for one separately admitted
deferred native-POST observation. It corrects the known S48 direct-pre-POST
composition variable by selecting native POST before opaque handoff; it does
not establish continuous guest execution, metadata-provider reachability, or
any new BOP result. A next S must audit the runner inputs and admit at most one
watchdog-bounded observation before it can execute this bundle.
