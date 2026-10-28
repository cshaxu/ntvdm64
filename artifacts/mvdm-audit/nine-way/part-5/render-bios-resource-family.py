import json
from pathlib import Path
p=Path('artifacts/mvdm-audit/nine-way/part-5');d=json.loads((p/'bios-resource-family-observations.json').read_text(encoding='utf-8-sig'));head='''# Part 5: DOS NTIO BIOS sources and firmware resources\n\nThe 33 owned MVDM files match both frozen original editions. Each row's SHA-256 and literal NTIO recipe references are in the companion JSON. `assets/release/ntvdm.exe` is read as data only: its BIOS1, BIOS4 and V7VGA RT_RCDATA resources match both current `roms/` and `data/` ROM copies byte-for-byte. ZIP and installed NTIO.SYS match each other. Neither resource binding nor guest image identity proves runtime startup, BIOS interrupts, mouse, timer, reset, teardown or guest source-to-image provenance.\n\n| Owned key | Captured source/build evidence | Bounded status |\n| --- | --- | --- |\n''';lines=[]
for x in d['files']:
 n=x['path'];refs=x['recipe_references'];ref=refs[0].replace('|','\\|') if refs else 'No direct filename reference in captured NTIO makefile/link';status='Pending exact source/profile use and runtime behavior.'
 if n.startswith('softpc.new/bios/'):
  if refs:status='Historical NTIO recipe input; retention/behavior and binary lineage separate.'
  else:status='Historical BIOS source/input; exact selected use pending.'
 elif n.lower().endswith(('bios1.rom','bios4.rom','v7vga.rom')):status='Published NTVDM RCDATA exact bytes (duplicate data/roms copies); partial content evidence, no runtime claim.'
 elif n.lower().endswith(('cmos.ram','profile.spc')):status='Duplicate data/roms bytes; selected mutable file, path and lifecycle pending.'
 elif n.lower().endswith('bios2.rom'):status='Original non-CPU40 alternative ROM; selected runtime/profile pending.'
 elif n.lower().endswith('.asm'):status='Original firmware assembly; relation to shipped prebuilt ROM pending.'
 lines.append(f'| `{n}` | {ref}; {len(refs)} recipe matches | {status} |')
tail='''\n\nEach recommendation concerns the specific owned file only. Preserve alternate firmware builds and the real DOS/Win16 execution contract; do not rebuild or modify guest/ROM. Replay `PYTHONDONTWRITEBYTECODE=1 python artifacts/mvdm-audit/nine-way/part-5/bios-resource-family-observer.py` and compare JSON. The original shard-1 ROM residency review retains the separate failure-handling boundary. No shared ledger, guest, product or build tree was changed.\n''';(p/'bios-resource-family-review.md').write_text(head+'\n'.join(lines)+tail,encoding='utf-8')
