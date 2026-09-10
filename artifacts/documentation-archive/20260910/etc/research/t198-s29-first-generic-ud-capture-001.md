# T198 S29 First Generic UD Capture 001

## Question

What is the first non-BOP `#UD` reached by the unrestricted exact CPU5 guest
path, and did the path first complete the S26 drive-result BOP?

## Method

The external fixture bridge consumes the existing fixed-width generic event
only.  It classifies a BOP solely for the already-composed normal path.  For
the first event whose copied instruction window is not `C4 C4`, it copies
CS:EIP, execution mode, vector, and the first four already-copied window bytes
to test-local storage, returns the existing typed STOP outcome, and performs
no guest-memory access.  It also reports whether a `50:0F` typed resume had
already been observed.

## Exact Run

The existing MSVC x64 `/MT` compile/link recipe and CPU5 source-built
NTIO/NTDOS fixture exited zero:

```
t198-s23 status=4 observed-5011=1 observed-503b-resume=1
observed-dta-resume=1 observed-hard-error-resume=0
observed-system-symbol-resume=1 observed-drive-resume=1
observed-first-generic-ud=1 fault=0000:0a84 mode=1 vector=6
window=15:ffffffff observed-stop=1 ... terminal=1:0000:0ad2
```

The native log also retains the earlier `EIP [00010000] > CS.limit
[0000ffff]` warning and `HLT` with IF clear, but it never reaches the former
LSL/VGA panic because the bridge accepts the copied STOP first.

## Interpretation

The first generic `#UD` is an all-`FF` instruction window at real-mode
`0000:0A84`, after a recorded drive-result resume.  It is not a BOP, an LSL
instruction, or a VGA memory execution.  The associated terminal CS:EIP
`0000:0AD2` confirms that the current failure frontier is low conventional
RAM control flow.  The current evidence therefore rejects device enablement
and instead requires a source/trace audit of the transfer into the
`0000:0A84..0AD2` region and of the required low-memory/IVT/BIOS exchange
state.

## Limitations

The bridge proves the copied fault event and local prior-drive marker only. It
does not establish the predecessor instruction, stack, or exact transfer that
reached `0000:0A84`; those require a distinct trace/audit task.

## Follow-Up

The next S must map the original NTDOS/NTIO continuation and the minimal
machine's initialized low-memory ranges around `0000:0A84..0AD2`.  It must
first identify the missing or wrong owner state before changing a BIOS, IVT,
RAM initializer, BOP provider, or device.
