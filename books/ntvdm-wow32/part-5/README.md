# Part 5: Extended Subsystems

This Part groups a fixed set of source-contract sections. The grouping prevents a long call chain from becoming one endless audit: each section owns one semantic boundary, and transfers neighboring behavior to an existing section.

## How to use this Part

Start with the smallest section that owns the proposed source change. Read its boundary, surviving semantics, modern-host discipline, and handoff paragraph before reading a convenient API or writing an adapter. If a finding crosses into another section, record the precise transfer rather than expanding the current section.

## Evidence rule

Each section is based on OpenNT source-contract evidence. It may guide implementation review, but it never substitutes for selected build/image proof or real runtime evidence.
