# M0 T400 S4 — passive WRITE scratch-file transition observation

## Run

An external .NET `FileSystemWatcher` was enabled recursively on the selected
seven-character x86 stage root before the unchanged Console-owner observation
launched the product:

```text
ObserveSoftpcStartup.mjs
  --launcher build/tools/console-startup-observer.exe
  --product build/M0-T398/S2/error-interaction-x86/original-softpc-process.exe
  --stage O:\t398
  --product-command system32\WRITE.EXE
  --without-diagnostics --observation-timeout-ms 30000
```

The resulting window report is retained at
`O:\t398\t400-s4-write-scratch-window-002.txt`; the watcher record is
`O:\t398\t400-s4-filesystem-events-002.txt`. The watcher was external to the
launched process, did not write under the observed guest tree, supplied no
input, environment variable, message, hook, debugger, or CCPU transport.

## Result

The ordinary product result was unchanged: `WOW32.DLL` loaded, `MSWRITE_MENU`
had a native menu, WRITE showed its original `IDPMTCantRunM` dialog, and the
observer timed out after roughly 30 seconds.

The watcher recorded exactly three created paths, all written by the observer
*after* its product run: the `.console.txt`, text report and JSON report named
`t400-s4-write-scratch-window-002`. It recorded no candidate `~*.TMP` file,
no other guest-system-root file creation, deletion, or rename during the live
ordinary run. Those known observer writes also prove the external watcher was
active and writing its separate report correctly.

## Bounded interpretation

This is a negative visibility result, not a causal branch result. It does not
prove `FInitFiles` was never entered: an earlier failure inside it, a failure
before KERNEL.97 creates its file, a guest path outside the selected physical
root, or an unobserved virtual path remains possible. Conversely, a future
scratch-file event would prove only that the chain passed the create operation,
not that a later BTE or emergency-save allocation succeeded.

S4 therefore selects no file, memory, KERNEL, WOW, CPU, DPMI, x64, or UI
repair. The next admissible question is the source-and-linkage ownership of
the active KERNEL.97 local API declaration/table that lacks an active provider
body, before treating that absence as a runtime defect.
