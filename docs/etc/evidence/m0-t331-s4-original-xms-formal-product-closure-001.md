# M0 T331 S4 — Original XMS formal product closure

## Result

S4 is closed. The selected original SoftPC/XMS product links on both target
graphs without `/FORCE` and without suppressing a duplicate CRT definition.

## Source-shaped CRT recovery

The original `softpc.new/obj.vdm/ntvdm.c`, `host/src/fprt.c`, and
`host/src/stubs.c` deliberately interpose a small CRT surface. On Win64 that
surface collides with static UCRT exports. `mvdm_crt_redirect.h` is force
included only in those original host translation units:

- both x86 and x64 privately bind the original `getenv`, `fprintf` and
  `_assert` bodies;
- only x64 privately binds the original `printf`/`fgets`/`gets`/`puts`/
  `fwrite` bodies, because x86 retains the original process-wide `fprt.c`
  `printf` provider required by the remaining original CCPU and device
  objects.

This is a build binding, not a source-body rewrite: the original names,
control flow and host-local behavior remain available at every original call
site. App and unrelated modern code continue to bind normal UCRT entrypoints.

## Product links

The regenerated `New-T310OriginalSoftpcNinja.ps1` graphs were used for the
selected incremental source/product relinks:

- `build/M0-T319/S3/termination-x86/original-softpc-process.exe`: pass;
- `build/M0-T319/S3/termination-x64/original-softpc-process.exe`: pass.

The x86 link proves the original process-wide `printf` contract remains intact.
The x64 link proves the complete source-local CRT interposition set coexists
with modern UCRT. Neither result uses `/FORCE`, a duplicate-symbol ignore, a
replacement XMS provider or a Bochs route.

## Fixed-container observation

Exactly one x86 observation used the existing immutable, non-debug,
console-owning `runtime-t319-s3-cpu40` container. The product executable was
the only replaced input and ran with the existing `-f -o --ordinary-child`
arguments. The observer result was:

```text
container=console-owning-nondebug
result=exited
exit=0xc0000005
timeout-ms=8000
```

The retained console and original exception reports show the unchanged startup
sequence `50:11`, `50:3B`, `50:0F`, `50:1B`, `54:05`, followed by original
`0xc0000005` reporting at a null access. Thus the result is not an XMS
dispatcher failure and does not select a new BOP repair.

The immutable media manifest remained
`b46ad6c47e692ba3b1213dd11b9c1a9b3c686410028c3d70e172086b26b62ce2`.
This unchanged terminal observation does not identify an XMS defect and does
not authorize a trace-selected repair. It remains with the pre-existing
CPU40/worker execution owner.

## T331 closure

T331 establishes the original XMS package as code- and product-link complete
through the selected CPU40/session boundary. Continuous guest XMS execution,
controller delivery and the observed CPU40 terminal failure remain separately
owned integration work; they are not silently converted into XMS policy or a
new service shim.
