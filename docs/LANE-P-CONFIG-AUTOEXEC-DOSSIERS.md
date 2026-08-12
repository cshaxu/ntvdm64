# Lane P CONFIG.SYS And AUTOEXEC.BAT Dossiers

Status: M43 design dossiers, 2026-08-08. These are Lane P-only evidence
packets under M40. They neither implement a selector nor restore the
historical temporary-file policy.

## Shared Source Findings

| Fact | Evidence |
| --- | --- |
| Both command services write an OEM pathname to a caller-owned `DS:DX` buffer whose declared minimum is 64 bytes. | `base/mvdm/dos/command/cmdconf.c:51-120` |
| The historical body builds a temporary host file after PIF, Windows-directory, short-path, environment, and content-rewrite policy. | `cmdconf.c:193-315` |
| Historical preprocessing rejects a missing or zero-length source file and terminates the VDM; it does not return an empty pathname. | `cmdconf.c:218-235` |
| NTDOS immediately passes the CONFIG result to `doconf`. | `dos/v86/doskrnl/bios/sysinit1.asm:1153-1170` |
| The CONFIG receiver is exactly 64 zeroed bytes. | `dos/v86/doskrnl/bios/sysinit2.asm:1534-1536` |
| COMMAND immediately attempts DOS `OPEN` on the AUTOEXEC result, then takes either the batch or no-batch branch. | `dos/v86/cmd/command/init.asm:1402-1426` |

The historical host-created temporary files are product policy, not a neutral
machine contract. A Lane P service body may therefore replace that policy only
with a fixed, contained profile resource. It must retain the original
dispatcher, service identity, consumer ordering, pathname transport shape, and
fatal failure shape. There is no admission for an empty string, an ambient
`CONFIG.SYS`/`AUTOEXEC.BAT` search, a host temporary file, PIF interpretation,
or a synthetic DOS return path.

## LP-03: CONFIG.SYS Result

| Field | Dossier |
| --- | --- |
| Historical source/function | `base/mvdm/dos/command/cmdconf.c:60-82`, `cmdGetConfigSys` |
| Guest consumer | `dos/v86/doskrnl/bios/sysinit1.asm:1153-1170`, then original `doconf` |
| Lane | P only; raw `GetVDMAddr` and historical preprocessing are not preserved |
| Profile input | one named `config` resource: canonical OEM DOS absolute path plus immutable read-only file bytes in the contained namespace |
| Required backend | copied `DS:DX` context, one ordinary-RAM transaction, and the same session namespace that later services use for DOS `OPEN` |

### Profile Grammar

The path is a NUL-free OEM byte string of 1 through 63 bytes, followed by one
NUL in the guest receiver. It has the grammar:

```text
<drive> ":\\" <component> ("\\" <component>)*
<drive>     ::= "A" ... "Z"
<component> ::= one or more declared DOS 8.3-safe OEM bytes
```

It contains no `.` or `..` component, wildcard, device name, host path
prefix, separator other than backslash, or implicit case conversion. The exact
path maps to exactly one immutable contained resource. The profile
may choose its name, but it must declare the path and resource identity before
the session starts. The adapter neither creates nor rewrites the file.

The required fact is resource presence, not a nonzero postprocessed byte
count. Historical preprocessing rejects a missing or zero-length *raw* source,
but its filtering could theoretically produce an empty temporary output. NTDOS
then calls `doconf` without a success flag. A profile without a valid CONFIG
resource is therefore rejected before guest execution rather than represented
as an empty pathname or a missing file.

### Entry And Result Contract

1. Capture `DS:DX` only from copied transition state at the active epoch.
2. Encode the selected path plus its NUL as exactly one 2 through 64 byte
   ordinary-RAM transaction. The full receiver range must validate before any
   byte is written; a provider, ROM, hole, stale epoch, or translation failure
   commits nothing.
3. Preserve all registers and FLAGS. The historical body has no defined
   register result.
4. The namespace must make the exact path readable by the immediately
   following original DOS `OPEN`; the result transaction alone is not success
   evidence.

### Failure And Fixtures

Invalid profile grammar, missing/nonempty-resource mismatch, unavailable
transaction capability, inaccessible destination, cancellation, or failed
commit is the Lane P declared fault/stop outcome. It performs no partial write
and no register mutation. It does not show a dialog.

- a valid path writes its exact OEM bytes and NUL once, then an original DOS
  open fixture resolves the same resource;
- a 63-byte path is accepted and a 64-byte-or-longer path is rejected before
  guest mutation;
- mapped, stale, or crossing receiver ranges retain their original bytes;
- a missing CONFIG resource is rejected before execution; a zero-byte declared
  result remains a namespace/open fixture case rather than an empty pathname;
- trace records only service identity, selected role identity, encoded length,
  namespace result, and disposition, all tagged `lane_p`.

## LP-04: AUTOEXEC.BAT Result

| Field | Dossier |
| --- | --- |
| Historical source/function | `base/mvdm/dos/command/cmdconf.c:97-120`, `cmdGetAutoexecBat` |
| Guest consumer | `dos/v86/cmd/command/init.asm:1402-1426`, immediately followed by DOS `OPEN` |
| Lane | P only; raw `GetVDMAddr`, temp-file creation, PIF lookup, environment export, and content rewriting are not preserved |
| Profile input | one named `autoexec` resource: canonical OEM DOS absolute path plus immutable read-only file bytes in the contained namespace |
| Required backend | copied `DS:DX` context, one ordinary-RAM transaction, and contained DOS `OPEN`/read/close behavior |

### Profile And Transport Contract

`autoexec` uses the LP-03 path grammar, length bound, character restrictions,
and exact namespace-identity requirement. It is a separate named role and may
not alias `config` unless the fixed profile explicitly permits identical
resource identity and its byte content is valid for both consumers.

The service performs the same single transaction shape: exact OEM path bytes
plus NUL to `DS:DX`, with the complete interval preflighted as ordinary RAM.
It preserves all registers and FLAGS. A successful transaction is followed by
the original COMMAND DOS `OPEN`; normal `OPEN` failure still follows the
historical `NoAbat` branch, but profile admission requires the declared
resource to resolve, so it is a fixture failure rather than a supported host
fallback.

This differs intentionally from historical host behavior only at the admitted
policy boundary: the original implementation creates a nonempty temporary
file and returns its short path. Lane P instead exposes immutable profile bytes
at a declared DOS path. It does not merge the host environment, inject
`PROMPT`/`PATH`, expand `%SystemRoot%`, create/delete a file, or read PIF data.

### Failure And Fixtures

The failure result matches LP-03: declared Lane P fault/stop, no guest-memory
or register mutation, no dialog, and no fallback to an ambient AUTOEXEC file.

- an accepted AUTOEXEC path is exactly the path opened by the original COMMAND
  sequence and resolves to the selected immutable bytes;
- a valid transaction followed by namespace-open failure fails the fixture;
- malformed, overlong, non-ordinary, stale, or cancelled transactions retain
  receiver bytes and registers;
- profile absence is rejected before execution; a zero-byte declared result
  remains a namespace/open fixture case rather than an empty pathname;
- trace records only service/role identity, path length, namespace result, and
  disposition, tagged `lane_p`.

## Deferred Historical Policy

The following remain unavailable rather than receiving guessed replacements:
PIF-selected config files, `%SystemRoot%` expansion, `SHELL=` normalization,
`COUNTRY=` filtering, third-party shell support, autoexec environment capture,
host temporary-file naming/cleanup, dialog presentation, and all ambient
Windows directory or environment lookup. A future Lane H restoration may
study them at their original host-operation boundaries; it does not alter this
contained Lane P contract.
