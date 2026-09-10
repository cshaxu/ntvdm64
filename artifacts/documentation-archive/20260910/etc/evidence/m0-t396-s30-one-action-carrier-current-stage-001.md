# M0 T396 S30 — one-action carrier and current-stage result 001

## Scope

S30 copied the checked-in Console-owning observer only into
`build/M0-T396/S30` and changed that copy's modal predicate to a
process-lifetime action latch.  It retains the original visible-dialog
predicate: same child process, visible `#32770`, control ID `102`, and exact
button text `&Ignore`.  The copy does not modify the product, guest memory,
CPU, BOP behavior, media, or the original dialog.

Its x86 executable SHA-256 is
`d06c157f3fd80d1c303c85584793e03a7b906d223d1f0a1bc999e477e9a6d521`.

## Fixed run and result

The fixed `O:\\t396` command was
`ntvdm32.exe -f -o --command system32\\WOWEXEC.EXE`, with a 120,000-ms
observation budget.  Its report records:

```text
result=exited
exit=0xc0000409
elapsed-ms=7203
observer-modal-continue-selections=0
observer-modal-continue-action-taken=no
loaded-module-count=23
loaded-wow32=yes
```

The existing S27 observer, run without the modal option against the same
current stage, independently reproduced `0xc0000409`, 23 modules, and
`loaded-wow32=yes` after 6547 ms.  Therefore S30 made neither a dialog action
nor a causal product change.

## Stage identity correction

The parent and provider identities retain S27's hashes:

- `ntvdm32.exe`: `ef60d6a993041df34635c7c867e53d560f624190de2136cc857aa94399decf44`;
- `WOW32.DLL`: `cec8951b5ce20c64cf53393c22587e26acc7441306c65e34fc6b6e0fd5bd9c3a`.

However, the current `O:\\t396\\runtime-manifest.json` and staged
`system32\\KRNL386.EXE` were written after S27's recorded observation.  S27
does not pin their identity, so its pre-WOW32 timeout is not a control for
this current media snapshot.  S30 supersedes no source conclusion from S27;
it records a distinct current-stage runtime result only.

## Disposition

S30 closes the invalid per-HWND test-carrier issue: the local carrier enforces
at most one action, and this run made zero.  The current frontier is an
ordinary post-WOW32 `0xc0000409` terminal.  Its native/source provenance is a
separate task; no dialog, CPU, BOP, or WOW32 semantic recovery is claimed.
