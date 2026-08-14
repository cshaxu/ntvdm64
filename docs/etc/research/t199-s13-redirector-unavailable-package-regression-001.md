# M0 T199 S13 Redirector Unavailable Package Regression

## Question

Can the CLI composition provide the original, unavailable Redirector result
for every defined `BOP_REDIR` member without implementing a Redirector
capability or admitting the historical host composition?

## Inputs And Implementation

`bx_ntvdm_redir_package_facade_v1` accepts only common ingress and provider
selection that the system plane has classified as Redirector explicit
unavailability. It has no selector-specific switch and no per-service branch.
It follows the retained `MS_bop_7` failure branch: `CF=1` and
`AX=ERROR_INVALID_FUNCTION` (`1`). Since current ingress copies the whole
`C4 C4 57 service` form from fault RIP, its continuation is `fault_rip + 4`.

This replaces the runtime role of the old disconnected three-byte provider;
that older file is retained as historical evidence only and is not wired into
the composition.

## Procedure And Observations

1. MSVC x64 `/MT` compiled `bx_ntvdm_redir_package_facade_v1_test.c` with the
   current ingress, registry and system plane. Its `00h..31h` loop exited zero;
   it checked `RIP+4`, `CF=1`, `AX=1`, and rejected `32h`.
2. The existing MSVC x64 `/MT` composition recipe rebuilt and ran
   `artifacts/build/t199-s13-redir-composition-r2`. The composition fixture
   invokes all 50 members through the bound generic-UD bridge and checks the
   same typed outcome; its `32h` negative path also exits zero.
3. `Test-BopInventoryProfileRegression.ps1`, documentation governance, and
   `git diff --check` passed. The source-derived inventory is 195 definitions,
   two non-callable sentinels, and 193 callable host-family services.

## Interpretation And Limit

The Redirector package is closed at the selected CLI profile disposition:
uniform, explicit unavailability. It does not claim named-pipe, mailslot,
network, NetBIOS, DLC, VDM window, or Redirector lifecycle support. No guest
trace was run and no BOP trace may create a service-specific implementation
task.

## Follow-up

Proceed with the next complete owner package, Debugger. Native traces remain
integration observations after package work; trace-led incremental repair is
not admitted before the global BOP package closure is complete.
