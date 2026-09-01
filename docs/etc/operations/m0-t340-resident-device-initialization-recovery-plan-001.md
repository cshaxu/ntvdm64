# M0 T340 — Resident DOS-BIOS device-initialization recovery

## Purpose

Recover the original owner map of the resident DOS-BIOS device initialization
that follows COMMAND `SVC_CMDSETINFO` registration. This is a package-first
source audit, not a device-by-device trace repair.

## Source-defined entry and boundary

`msinit.asm::DosInit` invokes `SVC_CMDSETINFO` and returns from the host
`cmdSetInfo` body. It then establishes `initiret`, calls `charinit` for the
BIOS-provided resident device chain, and iterates the remaining headers before
the next BOP, `SVC_DEMGETDPBLIST`. `charinit` forms an original `devinit`
request and invokes `DEVIOCALL2`, which performs the header's strategy then
interrupt calls. The initial `res_dev_list` is original `msbio1.asm` data.

This places the pending boundary in guest resident devices and firmware/machine
contracts, not in the normal `config.nt` producer, COMMAND BOP dispatch, DEM
file provider, or a generic host console rehost.

## S plan

### S1 — Source/ABI/failure inventory

Inventory every initial resident header (`CON`, `AUX`, `PRN`, `CLOCK$`, COM,
LPT and any configuration-linked successor), its strategy/interrupt target,
request packet fields, state/failure returns, and all SoftPC firmware/machine
calls. Classify each dependency as direct original selected body,
same-shaped existing adapter, pending original owner package, hard boundary,
or non-reached. Include the first post-chain `SVC_DEMGETDPBLIST` transfer.

**Closed.** The complete initial resident chain is source-proven to be an
original function-00 no-op-success cohort. Its first selected non-trivial
successor is the configuration-loaded original `HIMEM.SYS` package; see the
[S1 evidence](../evidence/m0-t340-s1-resident-device-initialization-contract-001.md)
and [ledger](m0-t340-s1-resident-device-initialization-contract-ledger.tsv).

### S2 — Complete original cohort recovery

After S1 identifies one complete smallest cohort, recover it as a whole using
original source and same-shaped binding first. Preserve device order, packet
layout, strategy/interrupt convention, status flags and failure path. Any
machine behavior change must remain owned by original SoftPC/firmware or a
named adapter and be separately admitted if it crosses S1's boundary.

**Selected cohort.** S2 is limited to the original guest `HIMEM.SYS`
initialization and its direct original SoftPC XMS, A20, INT 15, HMA and UMB
interfaces. It does not reopen resident console/serial/parallel devices or
select a trace-derived BOP leaf.

### S3 — Local and fixed-container verification

Build the affected formal CPU40 graph, run focused source-contract tests, then
perform at most one unchanged fixed-container observation. The observation
may verify the completed package and transfer the next owner, but cannot
select a new device leaf repair.

## Exit

Close only when the whole reached device-initialization cohort has one original
owner disposition and its selected recovery has source, ABI and failure
evidence. Reaching a later BOP alone is not device-package closure.
