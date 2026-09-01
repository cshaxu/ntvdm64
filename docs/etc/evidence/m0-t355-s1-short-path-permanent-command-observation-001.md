# M0 T355 S1 — short-path permanent COMMAND observation

Date: 2026-09-01

## Fixed container

The owner approved a newly created disposable output package at `O:\ntvdm`.
The standard `Stage-OriginalSoftpcRuntime.mjs` script refused any overwrite,
copied the formally linked CPU40/x86 product and the selected immutable media,
and wrote a format-3 manifest.

Its MVDM root is `O:\ntvdm\mvdm`. Public `GetShortPathNameA` returned that
same 13-character spelling. The unchanged original `cmdconf.c` format has
`13 + 25 + 13 + 9 = 60` visible value bytes, within the NTDOS 63-byte limit.

The staged product hash was:

```text
700bee42ab8747533453013cff904d265088a91fe23b60e098e15fa267565ecd
```

The immutable media-manifest hash was:

```text
15b6a215e9bf3cede6650c0990602b0f421181f18ea2ff431031fd237cd82858
```

## One admitted observation

One non-debug console-owning observer run produced:

```text
result=exited
exit=0xc0000409
fixed-system-root=O:\ntvdm\mvdm
fixed-system-root-short-chars=13
```

The retained default-off DEM observation proves that original `cmdconf.c`
created and NTDOS opened the transformed temporary configuration file. The
file contains the expected original line:

```text
shell=O:\ntvdm\mvdm\System32\command.com /p O:\ntvdm\mvdm\system32
```

Nevertheless, the next recorded original permanent open was:

```text
MVDM-DEM-OPEN phase=0 ... path=C:\COMMAND.COM
MVDM-DEM-OPEN phase=2 ... status=00000002 ax=0002 cf=1 path=C:\COMMAND.COM
```

The BOP sequence again reached `54:05`, `54:0C`, then `50:12` and original
`50:3D`. Therefore neither the package path contract nor the generated host
configuration text explains the default command pathname. The exception is
not interpreted as a repair target.

## Disposition

S1 makes no source change and permits no rerun. It selects the original
NTDOS configuration multi-pass and `commnd` persistence cohort for S2.
