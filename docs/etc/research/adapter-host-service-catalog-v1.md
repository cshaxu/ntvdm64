# Adapter Host-Service Catalogue v1

## Purpose

This is the common admission front-end for the declared NTIO/NTDOS/COMMAND
profile. It recognizes the copied four-byte form `C4 C4 selector service` and
returns only a family plus numeric service identifier. It does not dispatch a
handler or modify any guest/host state.

## Source-derived ranges

| Selector | Family | Accepted service range | Evidence |
| --- | --- | --- | --- |
| `50h` | DEM | `00h` through `48h` | `demdisp.c`, `SVC_DEMLASTSVC=49h` |
| `54h` | COMMAND | `00h` through `10h` | `cmddisp.c`, `SVC_CMDLASTSVC=17` |

The accompanying recovered selector map has exactly these two first-profile
entries. It still rejects `51h` (WOW), `52h` (XMS), `53h` (DPMI), and every
other historical selector until the declared profile needs them.

## Fail-closed rule

The catalogue rejects invalid windows, short windows, a wrong two-byte
prefix, an unknown selector, and either dispatcher's first out-of-range index.
It has no function pointers, host paths, CPU state edits, IP edit, guest-memory
access, or stop result.

Its purpose is to ensure every future host service enters the same plane
rather than adding another BOP-specific adapter hook.
