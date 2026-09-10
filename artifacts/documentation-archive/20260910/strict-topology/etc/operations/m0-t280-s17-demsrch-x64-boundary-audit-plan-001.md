# M0 T280 S17 — `demsrch.c` remaining x64 boundary audit

Inventory every remaining x64 width-sensitive warning and source call in
original `dos/dem/demsrch.c` after S16. Classify each by original owner,
value class (guest numeric, host-local pointer, host handle or internal
buffer), required adapter/mapping route, failure contract and dependency.

This is a whole-source audit. It does not implement a warning-selected patch,
enable a provider, create a mapper, or modify Bochs. It must group related
directory-buffer, device-handle and pathname-helper paths into bounded future
worksets, and distinguish values requiring the existing `guest_memory` mapper
from host-local values that must never be mapped.
