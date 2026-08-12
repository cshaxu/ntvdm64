# OpenNT Service Boundary Matrix

## Evidence

The OpenNT snapshot's `inc/dossvc.h`, `dos/dem/demdisp.c`, `inc/xmssvc.h`, and
`inc/rdrsvc.h` show that the historical guest did not access Windows resources
directly. It issued categorized guest-to-host service requests, and a host-side
dispatcher selected handlers. This matrix records responsibilities only; it
deliberately omits historical byte encodings, selector values, and private ABI
layouts.

## Owned Adapter Categories

| Historical responsibility family | Bootstrap relevance | Owned engine contract | Initial status |
| --- | --- | --- | --- |
| Kernel placement / boot drive / DOS state | required | validated runtime image plan, boot-drive policy, guest-memory writes | research only |
| Path, directory and current-drive operations | required | rooted DOS namespace capability | parsing/rooting and read-only metadata probe |
| Open/create/read/write/seek/close/commit | required | handle-table and stream capability | read-only open/read/close pilot |
| Find-first/find-next and FCB operations | likely required | enumeration capability and explicit FCB adapter | unimplemented |
| File date/time, system date/time | required | clock capability and DOS timestamp conversion | unimplemented |
| Disk/media/DPB/IOCTL/absolute I/O | profile dependent | removable media and block capability | unimplemented |
| Console input/output and termination | required | stream, input, lifecycle capabilities | probe-only output/exit |
| Critical error / retry | required for robust I/O | typed DOS error result plus guest interrupt policy | unimplemented |
| XMS/A20/UMB/move/alloc | required if the selected runtime loads HIMEM | memory-manager capability; CPU owns A20 semantics | unimplemented |
| DPMI / protected-mode callbacks | optional bootstrap extension | CPU mode/descriptor/interrupt subsystem | unimplemented |
| Redirector, pipes, mailslots, network | optional | opt-in namespace/network adapter | deliberately absent |
| WOW, debugger, symbols, host process handoff | not required for DOS bootstrap | separate product feature or host dispatcher | deliberately absent |

## Dispatcher Rule

The future `nvtdm` adapter must dispatch by a **profile-local request
descriptor**, not by generic instruction interception. A descriptor declares:

- the recognized request form for one version-locked runtime profile;
- checked register and guest-memory inputs;
- one capability category from this matrix;
- a finite result: resume, guest interrupt, wait, terminate, or guest fault;
- validated output register/memory patches committed atomically.

Neither an adapter nor a handler may change CPU execution mode, register an
arbitrary host callback, alter memory routing, or reach a host API directly.
Those remain machine/platform responsibilities.

## Bootstrap Cut

The smallest credible NTIO/NTDOS bootstrap experiment needs more than the
current probe: real-mode CPU coverage, IVT/BDA policy, image-placement evidence,
the first kernel-placement request, DOS namespace and stream capabilities,
clock, and enough XMS/A20 behavior for the selected `HIMEM.SYS`. Redirector,
WOW, networking, debugger and Win16 services are intentionally outside that
cut.

This is why direct reuse of a historical host dispatcher is not an acceptable
shortcut: it would import precisely the private host integration that the
non-invasive design excludes.
