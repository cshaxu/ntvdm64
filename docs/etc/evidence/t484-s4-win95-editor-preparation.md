# T484 S4 P41 Win95 editor preparation

The editor runner delegates launcher installation and interactive selection
to select-guest-launch.cmd. It invokes the existing WIN.CMD on a marked
installation copy and supplies the requested selections through stdin.
The test-local Run16 stub suppresses launch during selection. The observer
subsequently launches the actual generated PIF with the selected package.
The original guest installation is read-only.

Both editor runners use prepare-editor-inputs.cmd to copy COMMAND, create
the two prompt PIFs with released PIF.EXE, and prepare save inputs. The
Win95 caller uses the same entry for controller placement and WIN.INI keys.
The existing guest-ini and input-copy tools own those formats and copies.
The Win31 caller continues to use the shared matrix launch configuration.
The PowerShell runner still checks output hashes and coordinates runtime
input and evidence. No new native executable was introduced.

Evidence is under build/t484-s4-win95-editor-preparation. PID24336 ran
the Win95 DOSONLY=0 multitask read/write witness against the coherent
production package. It reached the controller boot and ready markers, but
timed out before editor completion. PID22928 ran the HEAD script with its
source-relative path resolved to the checked-in directory, the same
package, installation, temporary guest path and arguments. It likewise
timed out and retained both original five-byte seeds. Neither run passes
editor interaction or normal shutdown. The first attempt to start the
baseline had a transient wrapper parse error and produced no runtime
evidence.

PID16704 independently prepared the new profile on the same temporary guest
path. All eleven compared files matched the old preparation, covering
SYSTEM.INI, pre-launch WIN.INI, WIN.PIF and both prompt PIFs, CONFIG.NT,
AUTOEXEC.NT, COMMAND.COM, the controller and both save seeds. The
pre-launch WIN.INI hash was observed before the guest updated the file.
The original installation retained its hashes. Owned copies were removed.

PID24612 passed the cached full build, the shared Win31 preparation
contract, and the 22 published host hash checks. The Win95 editor
configuration producer is migrated. The matched runtime timeout remains
open as a runtime limitation, and the full regression gate remains
pending. Census: 352/355 disposed, 3 pending, plus 5 overlapping
supplemental chains.
