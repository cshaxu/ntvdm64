# Part 2: DOS redirector declarations and shared callback boundary

Four owned `dos/v86/redir` entries have independently recorded identities in `dos-tools-dispatch-observations.json`. Original `redir/makefile:112-120,167-202` selects resident, named-pipe, mailslot and message objects with `localmac.inc` dependencies. `localmac.inc::DosCallBack` either calls the recorded DOS multiplex handler or emits INT 2F/AH=12 based on CALL_DOS. This is guest callback plumbing, not proof both routes or all nested/reentrant cases run in the packaged guest.

`redir.asm:46,111` includes `rdrmisc.inc` and uses its LM20_CNLEN=15 for the guest computer-name buffer. `fudgedef.inc` separately defines the same value plus CR/LF/version, but the independent shard-2 filename search did not establish an actual consumer; do not infer all constants or alternate configurations are redundant. Original service declarations connect guest redirector calls to VDMREDIR host services, which own translated handle/async state; Windows owns native pipe/network resources. Exact original guest error and task-retirement semantics must be retained.

| Owned path | Evidence-supported role and disposition |
| --- | --- |
| `dos/v86/redir/localmac.inc` | Original guest callback macro included by multiple historical compiled redirector source inputs; sufficient build/interface role evidence, runtime profile/behavior pending. |
| `dos/v86/redir/rdrmisc.inc` | Original guest declarations directly included by redir.asm and others; specific LM20_CNLEN use shown, not all consumers. |
| `dos/v86/redir/fudgedef.inc` | Original alternate constants with one overlapping value; active consumer/all profiles unknown. Keep pending, no overall exclusion. |
| `dos/v86/redir/obj/redir.exe` | Current-only supplemental binary F4BE4E... differs from installed/ZIP B6E9FA... per shard-2 observer. Exact source provenance, selection and why it exists remain pending; do not substitute for the packaged guest. |

T420 S20 operation matrix records specific formal-x86-013 real DOS named-pipe and NetBIOS witnesses, including async completion and subsequent MEM/COMMAND return. Those are candidate-specific and do not prove every current provider, guest caller or pending-cancel/task-loss interleaving. Original commented initialization SVC in redir source must not be counted as an active call (see `dos-redir-guest-review.md`).

S12/S17 should bind current actual guest REDIR and host VDMREDIR hashes, test real named-pipe/mailslot/network positive and absent-provider errors, nested DOS callback, asynchronous cancellation and termination cleanup. S11 covers execution/reentry, S19 exact release. Preserve alternate declaration and guest behavior; no source, guest, product or ledger modified, and no runtime test was run.

Replay `rg -n "localmac.inc|rdrmisc.inc|fudgedef.inc|LM20_CNLEN" src/mvdm/dos/v86/redir`, read `localmac.inc`, makefile, shard-2 reports and the part-2 observer.
