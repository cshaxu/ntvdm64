# DPMI buffer and saved-state source owners

The original `dpmi32/buffer.c` and `dpmi32/savestat.c` match both original
editions, are selected into the DPMI worker library and bind five actual
object bodies to the released NTVDM image. Original INT 21 translation uses
these owners to copy buffers and save/restore segments and stack. The linked
body evidence does not establish a complete real guest execution chain.

The buffer code chooses low-memory passthrough or allocates and copies, and
copyback and release belong to that same owner. The state code links caller-
supplied context records into `StateStack` and pops them on restore. Verify
guest pointer/span and memory lifetime, failed or short allocations, low/high
memory boundaries, FCB/string bounds, nested translations, exception unwind,
stack ownership and task/session exit before treating either file as complete.
In particular, an apparent unchecked allocation or a null stack from source
alone is not a proved guest-reachable defect. No product or guest was run or
modified by this audit.
