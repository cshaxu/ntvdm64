# T178 S3 CLI-result admission decision 001

## Decision

Do **not** admit an engine-to-CLI DOS-result transport package now.

## Evidence basis

T178 S1 and S2 establish the complete currently provable order:

```text
target launch -> 50:36 -> SHARE INT 21h/4Ch -> 50:3C accepted
-> original DOS teardown/parent restoration -> [unproven] -> COMMAND 54:11
```

The desired `54:11` boundary is not observed. Its historical handler also
mixes guest DX with Win32 event, redirection, directory and command-queue
composition, so a process exit code, watchdog result or historical AL value
cannot serve as a substitute result ABI.

## Disposition

The next required predecessor is a separate guest teardown/control-state
attribution package: it must source-correlate the post-`50:3C` teardown and
the first recorded stack/control failure to identify whether native Bochs
mechanics, a guest-state precondition or an unavailable original owner blocks
return to COMMAND. That package begins read-only and cannot add a BOP handler,
device, result transport or synthetic guest state.

The queued engine-to-CLI result transport remains a later candidate only after
a real `54:11` call and its CF-clear guest continuation are both observed.
