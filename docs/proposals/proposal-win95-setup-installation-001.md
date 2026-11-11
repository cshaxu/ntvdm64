# Proposal — Windows 95 Setup installation package

## Status and owner objective

The owner requests a new queued T package for a `win95-setup` installation
package. This is an unnumbered candidate at the end of the queue and does not
change the active task. It should provide a usable Windows 95 installation
workflow on the project's NTVDM, from a user-selected copy of installation
media through Setup to a launchable installed system.

## Baseline and boundaries

`tools/win31-setup` is the existing media-copy Setup workflow and
`tools/win95-launch` prepares an already installed Windows 95 tree; neither
alone establishes a Windows 95 Setup workflow. Reuse their applicable
contracts and released utilities, including existing Win95 adaptations and
mouse and guest-driver assets, after verifying actual format, source and
lifecycle requirements. Do not duplicate a previously delivered operation
or claim that preparing an existing installation proves Setup works.

The supplied original installation media remain immutable. Work with explicit
user-selected, recoverable media and installation copies; preserve original
file identities and provide a truthful recovery path. Existing guest-binary
exceptions are bounded to their recorded scopes; identify and document any
additional proposed media or guest modifications and their authorization
before implementing them. No hard-coded machine paths, silent mutation of an
existing installation or unsupported-version success claims.

## Scope and suggested stages

1. Establish a baseline using identified Windows 95 media, an isolated target,
   and the current release. Run original Setup far enough to identify actual
   failures and prerequisites, including text-to-graphical transitions,
   required guest devices, mouse, storage, and restart behavior. Record media
   identities and distinguish genuine defects from installation choices.
2. Design and implement `tools/win95-setup` with CMD entry points and existing
   project utilities. Prepare a media copy with only justified, version-bound
   adaptations, preserving originals and providing APPLY/UNAPPLY. Reuse assets
   from `assets/release`; do not require PowerShell as part of the user-facing
   workflow. Support user-selected source and destination paths with explicit
   failures for unsupported or ambiguous media.
3. Run the original Setup against the prepared media copy. Handle installation
   directory selection, file copying, graphical Setup, required restarts and
   first desktop using the normal guest lifecycle. Integrate the installed
   launch workflow through existing Win95 Launch functionality rather than
   maintaining independent competing preparations. Define ownership and
   rollback at each boundary, including failed and repeated installations.
4. Validate fresh custom installation in an isolated tree, desktop interaction,
   video and mouse, clean shutdown/restart, and installed launcher behavior
   across applicable DOSONLY and guest multitasking profiles. Test missing,
   malformed or unsupported media, existing backups, user files, interrupted
   setup, recovery and reapplication. Verify source media hashes and report
   exact supported version identities, limitations and non-passing cases.

Completion requires the actual end-to-end installation and interaction on the
selected product package, not only script fixtures or a preinstalled Win95
image. Preserve the current task and existing queue order.
