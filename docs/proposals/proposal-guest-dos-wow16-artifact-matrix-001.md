# Proposal: Guest DOS/WOW16 artifact matrix

## Purpose

Classify every selected OpenNT DOS and WOW16 file already held by the project
as a load-only guest input. The result is a reproducible executable-relative
media manifest, not a new guest build or a host-side linked component.

## Source boundary

The package starts with the project-owned `mvdm-guest` DOS, Win16/WOW16, font
and `bin86` mirrors plus their existing original/source-built outputs. It
compares the available OpenNT/OpenNT-4.5 provenance only to establish identity
and selection. Guest instructions are loaded into the SoftPC machine; no guest
translation unit is linked into the host executable.

## Required S sequence

### S1 — Media/provenance inventory

Create one path-level ledger covering DOS, WOW16, `bin86`, fonts, firmware
adjacency and existing staged outputs. Each row must have origin, hash, file
class, final product disposition and whether it is an original prebuilt,
source-built cross-check, required runtime media, build intermediate or
historical-only evidence.

### S2 — Executable-relative staging contract

Reconcile the selected runtime media with the existing staging script and
runtime manifest. Preserve original basename and guest-relative lookup shape;
place no guest system file on a host drive merely to start the product.

### S3 — Matrix verification and closure

Verify manifest hashes, missing/extra-file behavior and the selected x86/CPU40
product staging path. This package does not claim continuous guest execution.

## Exclusions

No guest source rewrite, BOP/provider implementation, SoftPC CPU/device
change, host system installation, registry mutation, virtual drive policy,
Bochs route or x64 runtime admission.

## Completion record

The closure names each asset's unique disposition and proves that only selected
runtime media enters an executable-relative stage. Source-built and original
artifact series remain distinct cross-check inputs rather than being silently
substituted for each other.
