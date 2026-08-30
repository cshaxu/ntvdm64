# M0 T310 S19 P2 — input-controller x86 formal assembly

## Command and result

The admitted S16 formal Win32/x86 CCPU40 graph was used as the selected
source graph.  Its seven input objects were cleaned and rebuilt directly:

```text
ninja -C build/M0-T310/S16/original-softpc-ccpu40-s16p5 -t clean \
  obj/keymouse/keyba.obj obj/keymouse/keybd_io.obj obj/keymouse/mouse.obj \
  obj/keymouse/ppi.obj obj/keymouse/mouse_io.obj obj/host/nt_keycd.obj \
  obj/host/nt_mouse.obj
ninja -C build/M0-T310/S16/original-softpc-ccpu40-s16p5 <same seven objects>
# Cleaning... 7 files.
# [1/7] ... [7/7] successful
```

This is selected-source composition evidence only.  It demonstrates that the
original controller and adjacent host objects still compile after the current
minimal mirror changes; it does not claim a live keyboard/mouse device path.

## Diagnostic disposition

The rebuild retained visible original warnings for unused parameters, old C
declarations, fixed guest scalar narrowing, source-quality control flow and
historical macro collisions.  The active project profile is Win32/x86 CCPU40;
none of these diagnostics establishes a new pointer-width or component-ABI
crossing.  They are therefore recorded as source context, not suppressed and
not changed opportunistically.  S19 continues with the table/initializer and
focused behavior review; S20 owns public host console input binding.
