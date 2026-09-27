# T423 S5 Hidden Backend Acceptance

## Question and baseline

Prove the owner-approved expanded hidden Console contract on the independent
frontend architecture, not merely that the transport links. S4 P1
9c27b5fd2d21bdf71a730c8d8fcfae10c613bf29 is pushed and its verified seven-file
package is at O:/winnt. The [S3/S4 ledger](m0-t423-s3-hidden-console-ledger.md)
pins binaries, configuration, runtime evidence and retained uncertainty.
Review existing owners and tests before authoring more implementation.

## Checklist

| Contract | Existing test to reuse | S5 evidence state |
| --- | --- | --- |
| Native output, cooked input, reuse, streams, EOF, Unicode environment, ordered input return | tests/app/native_console_host_test.c default | Pending current-build run and assertion review |
| Raw/processed Ctrl+C, Ctrl+Alt+C, Ctrl+Break | native_console_host_test.c --control-input | Pending |
| Visible frontend controls to hidden target, default exit, suspended DOS I/O | native_console_frontend_test.c --controls | Pending; private desktop |
| Completed result versus lost presentation | native_console_host_test.c --completion; native_console_frontend_test.c default | Pending |
| Cancel/unresponsive helper and bounded close | native_console_host_test.c --close-timeout; native_console_frontend_test.c default | Pending |
| Unicode, palette/cursor, scrolling, viewport and resize | native_console_capture_test.c; native_console_host_test.c default | Pending |
| Actual users outlive direct parent, helper failure is not empty | native_console_members_test.c | Pending |
| Four-level DOS/native chains and I/O restoration | tools/audit/Verify-CommandExitStatus.ps1 frontend-chain-a/frontend-chain-b with OrdinaryFrontend | Pending integration |
| Mixed launcher/helper/frontend/worker faults and unrelated-session survival | verify-frontend-lifetime.ps1 and request/scope tests; extend uncovered combinations only | Pending coverage review and live tests |
| Regression and release | DOS17, separate WOW frontiers, graph ownership, seven-file hashes, governance | Mandatory for production changes; tests/docs alone do not redeploy unchanged binaries |

## Boundaries and procedure

Native component fixtures use real Windows Console/helper resources, but do
not alone prove guest or broker integration. DOS tests select x86 CCPU40 and
unchanged original media. Intermediates stay under build/M0-T423/S5; runtime
reports stay under O:/winnt/logs. Use fresh identifiers, actual output
assertions and bounded observation. Never infer completion from helper death
or call forced cleanup natural retirement. Preserve failed evidence.

The historical unattributed startup timeout retains its S4 disposition/TODO.
Recurrence requires live worker/window capture before cleanup and exact
artifact/configuration identity, not a retry or fabricated success. Window,
display and guest modification are not admitted by S5.

## Results

Admission only. Tests and run evidence follow after governance.
