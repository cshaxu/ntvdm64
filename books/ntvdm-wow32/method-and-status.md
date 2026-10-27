# Method, evidence, and status

## The audit boundary

The audited chain is:

```text
Win16 application
  -> immutable Guest DLLs and thunks
  -> NTVDM / WOW entry, dispatch, conversion, and services
  -> modern public Win32 APIs or a named compatibility adapter
```

Guest code is evidence for call shape and observable semantics. It is not a
repair surface. SoftPC instruction execution, V86 mechanics, DOS/BIOS, and
device internals are read only when a direct Win16/WOW contract needs them.

## Evidence classes

Keep these categories separate:

| Evidence | It can establish | It cannot establish alone |
| --- | --- | --- |
| OpenNT source and build recipe | original owner, ABI shape, condition, cleanup order | selected product image or live modern behavior |
| Official modern API material | public API contract and documented limits | preservation of a private NT4 behavior |
| Product build/image receipt | selected source reached an image | runtime semantic equivalence |
| Runtime receipt | one exercised route and its stated cases | all callers, variants, or unseen failure paths |

## Minimum-diff rule

Choose the smallest valid implementation form, in order:

1. reuse the selected original body unchanged;
2. add a small registered binding at the original owner;
3. put a material mechanism in a named specialist adapter/ABI owner;
4. retain an explicit unavailable result when no source-shaped modern binding
   exists.

Do not create a second USER queue, scheduler, task table, window/class
authority, callback-lifetime manager, or generic Guest-memory mapping service.

## Completion vocabulary

A section can be statically audited while product implementation or runtime
validation remains open. Report source contract, modern disposition, build/image
receipt, runtime receipt, and remaining limitation separately. Counts of
reports, lexical hits, or symbol names are never completion percentages.
