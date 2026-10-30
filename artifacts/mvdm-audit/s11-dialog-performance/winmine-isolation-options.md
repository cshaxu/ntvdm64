# Winmine profile isolation: available routes and current evidence

The earlier denial applies to running the Winmine probe while it might write
the current user's VirtualStore profile. Do not retry that action unchanged.
Changing only guest profile API arguments has not established containment of
all file access. A post-run hash is detection, not prevention.

Microsoft describes Windows Sandbox as a disposable separate virtual machine
whose guest state is discarded when the sandbox closes; host paths are only
accessible when explicitly shared. Its documented mapped-folder configuration
supports read-only input sharing. See:
https://learn.microsoft.com/windows/security/threat-protection/windows-sandbox/windows-sandbox-overview
https://learn.microsoft.com/windows/security/application-security/application-isolation/windows-sandbox/windows-sandbox-configure-using-wsb-file
A purpose-owned output location still needs a design that cannot expose the
user's Windows/VirtualStore profile; do not map that profile. A sandbox result
would need its own complete candidate, guest manifest, hash and test receipts,
and cannot silently substitute for a host-specific acceptance case.

On the current host, a read-only `Get-Command WindowsSandbox.exe` found no
command and `Test-Path %SystemRoot%\System32\WindowsSandbox.exe` returned
false. No `vmrun`, `VBoxManage`, `virsh` or `qemu-system-x86_64` command was
found by that search. An unelevated DISM feature query returned error 740;
therefore this does not establish Windows Sandbox is unsupported or disabled,
or that no other VM exists. No Windows features or host settings were changed.

AppContainer is another documented isolation mechanism, but its different
filesystem, window and IPC access can affect the broker/worker product path.
It is not accepted as equivalent without a separate full-chain proof; do not
assume normal desktop, RPC or user-profile behavior from documentation alone.
No Winmine process was launched during this review.

A subsequent sandbox-permission escalation for the same read-only DISM query
still returned error 740: the process was not Windows-UAC elevated. This is a
query failure, not evidence that the feature is absent or disabled. No system
feature or setting was changed.

## Test-owned file-lock and profile-cache controls

Using only disposable files under `build/t473-s11`, an open read handle with
`FILE_SHARE_READ` rejected a separate process's direct write, and rejected
file deletion and rename. An already-open writer prevented acquiring that
protection. These observations are limited to these explicit file operations.

The initial API flush experiment was invalid: PowerShell converted the
three null string arguments into empty strings, and the purported flush wrote
an empty section/key (`[]` and `=`). Therefore the observed post-unlock hash
change did not prove that the rejected value was cached or later committed.
A corrected test used an `IntPtr.Zero` signature for actual null pointers:
the child process's profile write returned FALSE with sharing violation 32,
the protected file stayed byte-identical while locked and after the true-null
flush attempt, and its contents retained `value=before` rather than the
attempted `value=after`. The true-null flush returned FALSE, so it does not
prove a successful cache flush, and does not prove all system or guest paths
are contained. The first exploratory run also used PowerShell's reserved
`$Error` variable and was invalid. All files in these controls were temporary
and test-owned; the real Winmine INI was not opened.

The lock is promising against direct writes, deletes and renames, but remains
unverified against the complete real process chain, alternative file paths,
pre-existing writable mappings and all profile/registry behavior. It cannot
by itself be treated as proven isolation or a passing product test.
A separate test-owned file was opened for read/write and mapped with a writable
view; the original FileStream was then closed while the mapping/view remained
open. Acquiring the proposed read/share-read guard failed with a sharing
violation, and the file's first byte stayed unchanged. This shows that this
particular pre-existing writable mapping prevents guard acquisition; it is not
proof for every section/mapping, alias, alternate file, or process-chain case.
Two preliminary test-harness attempts failed (PowerShell turned a null map
name into an empty name, then an unrelated read conflicted with the mapping);
only the corrected control is reported here. The real Winmine file was not
opened.
