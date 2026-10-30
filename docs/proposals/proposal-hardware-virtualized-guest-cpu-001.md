# Proposal — hardware-virtualized guest CPU acceleration

## Status and objective

On 2026-10-29 the owner requested a new candidate to accelerate 16/32-bit
guest execution using modern hardware virtualization instead of a software-
emulated CPU. The owner subsequently moved it before the Windows 1.01 launch
candidate. This proposal is not an active task and does not allocate a T number.

## Scope and acceptance

1. At admission, examine the then-current CPU, memory, interrupt, device and
   WOW execution boundaries and the available host virtualization interfaces.
   Determine which guest execution modes and operations can run directly and
   which require exits or emulation; record hardware, OS and coexistence
   requirements and incompatibilities with evidence.
2. Design and implement a coherent hardware-virtualized backend where
   feasible. Preserve the existing guest-visible instruction, exception,
   memory, interrupt, device and lifecycle contracts. Do not modify or
   rebuild immutable guest components. Do not claim that virtualization
   eliminates every software-handled operation.
3. Compare representative DOS and Win16/Win3.x workloads against the existing
   software backend for correctness, startup, operation, teardown and
   performance. Include protected-mode transitions, faults, timing, input and
   graphics, and document supported configurations and uncovered boundaries.
4. Preserve a usable software backend for hosts or workloads where the new
   backend is unavailable or cannot faithfully execute the required behavior.
   Record measurable costs and any observable semantic difference before
   adopting it; obtain the owner's approval for new semantic concessions.

Feasibility and the eventual interface are research questions, not established
results. This candidate does not authorize changes to the current active task.
