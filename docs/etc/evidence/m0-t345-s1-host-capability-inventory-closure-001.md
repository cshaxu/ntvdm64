# M0 T345 S1 — Host capability inventory closure

## Result

S1 closes the source/ABI/caller/failure inventory without introducing a new
host-capability provider. Its sole selected recovery cohort is the existing
original DEM package for ordinary host files, directories, drives, volumes and
directory search. The cohort is source-shaped and already bound through the
single session mapping-manager and the existing public Win32 façade.

## Source findings

- `dos/dem/demdir.c`, `demdisp.c` and `demerror.c` are byte-identical to the
  OpenNT baseline. The remaining cohort bodies retain the OpenNT body and have
  only README-registered bindings for the selected product root, session
  identity mapping, guest DTA/PDB/SFT leases or a native-width private host
  buffer.
- DEM remains the owner of DOS result translation. Ordinary host access denial,
  directory and search failure, broken-pipe and disk-full outcomes retain their
  original caller-visible carry/AX or byte-count paths. No readonly policy,
  virtual drive or parallel filesystem provider is selected.
- The `COMMAND` configuration path is not a missing-file fallback. Original
  `cmdGetConfigSys -> ExpandConfigFiles(TRUE) -> GetPIFConfigFiles` selects a
  PIF-specified or default `CONFIG.NT` input; `cmdconf.c` expands that selected
  input and handles its `SHELL=` record. If the selected input cannot be
  opened, the original error path is retained; it does not silently select a
  different configuration file.
- Raw media, serial/printer/pipe endpoints, fullscreen presentation,
  Redirector and child-process lifecycle remain explicitly transferred to
  their named owner packages. They are not hidden in the selected ordinary
  filesystem cohort.

## Admission to S2

S2 may now prove the selected DEM package as one whole source-shaped cohort:
filesystem/handle operations, directory and drive operations, and search/FCB
operations together. It may use only the existing adapter-mvdm-host-out Win32
façade and the session mapping manager, and must not add a generic capability
dispatcher or virtual filesystem.

## Evidence

- `docs/etc/operations/m0-t345-s1-host-capability-owner-ledger.tsv`
- `docs/etc/operations/m0-t345-s1-dem-host-capability-mirror-binding-ledger.tsv`
- `src/mvdm-host/dos/command/cmdconf.c`
- `src/mvdm-host/dos/dem/dem{dir,disp,error,file,gset,hndl,srch}.c`
- `src/mvdm-host/README.md`
