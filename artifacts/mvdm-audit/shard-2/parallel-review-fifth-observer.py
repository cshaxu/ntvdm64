"""Bind original WOW callback registration and EDIT borrower edges."""
import hashlib
import json
import re
import runpy
import struct
import subprocess
from pathlib import Path

OUT = Path('artifacts/mvdm-audit/shard-2')
BUILD = Path('build/.ninja/msvc-x86-release/wow32')
SRC = Path('src/mvdm/wow32')
ORIG = (Path('O:/repos.external/OpenNT/base/mvdm/wow32'),
        Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm/wow32'))
CALLBACKS = {
    'pfnLocalAlloc': '_W32LocalAlloc@12', 'pfnLocalReAlloc': '_W32LocalReAlloc@20',
    'pfnLocalLock': '_W32LocalLock@8', 'pfnLocalUnlock': '_W32LocalUnlock@8',
    'pfnLocalSize': '_W32LocalSize@8', 'pfnLocalFree': '_W32LocalFree@8',
    'pfnGetExpWinVer': '_W32GetExpWinVer@4', 'pfnInitDlgCb': '_W32InitDlg@8',
    'pfn16GlobalAlloc': '_W32GlobalAlloc16@8', 'pfn16GlobalFree': '_W32GlobalFree16@4',
    'pfnWowEditNextWord': '_W32EditNextWord@20',
}


def sha(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest().upper()


def inspect():
    h = runpy.run_path('artifacts/mvdm-audit/host-tail-image-observer.py')
    image_path = Path('assets/release/WOW32.DLL')
    image, cached = h['Image'](image_path), h['Image'](BUILD / 'wow32.dll')
    assert image.base == cached.base and image.code == cached.code
    assert all(image.data[s:e] == cached.data[s:e] for s, e, _ in image.code)
    source = SRC / 'wcall32.c'
    assert sha(source) == sha(ORIG[0] / 'wcall32.c') == sha(ORIG[1] / 'wcall32.c')
    deps = subprocess.check_output(['ninja', '-C', str(BUILD), '-t', 'deps'], text=True)
    for obj in ('obj/wcall32.obj', 'obj/wow32.obj', 'obj/opennt-user/control-body-edecrare.obj',
                'obj/wow32-dll/wow_user_registration_bridge.obj'):
        dep = next(line for line in deps.splitlines() if line.startswith(obj + ':'))
        assert '(VALID)' in dep and '(STALE)' not in dep
    syms = h['symbols']((BUILD / 'wow32.dll.map').read_text())
    def find(name, owner):
        return next(s for s in syms if s['name'] == name and s['owner'] == owner and s['function'])
    init = find('_W32Init@0', 'wow32.obj')
    init_obj = h['Coff'](BUILD / 'obj/wow32.obj')
    code, relocations = init_obj.function(init['name'])
    init_bound = h['bind'](image, init_obj, init)
    init_source = (SRC / 'wow32.c').read_text(encoding='latin1')
    bindings = []
    callback_obj = h['Coff'](BUILD / 'obj/wcall32.obj')
    for field, symbol_name in CALLBACKS.items():
        c_name = symbol_name[1:].split('@')[0]
        assert re.search(r'\bpfnIn\.' + re.escape(field) + r'\s*=\s*' + re.escape(c_name) + r'\s*;', init_source)
        symbol = find(symbol_name, 'wcall32.obj')
        offsets = [p for p, n, kind in relocations if n == symbol_name and kind == 6]
        assert len(offsets) == 1
        offset = offsets[0]
        addend = struct.unpack_from('<I', code, offset)[0]
        actual = struct.unpack('<I', h['data_at'](image, init['va'] + offset, 4))[0]
        assert actual == (symbol['va'] + addend) & 0xffffffff
        bindings.append(dict(field=field, offset=offset, image_value=actual,
                             callback=h['bind'](image, callback_obj, symbol)))
    def call_edge(caller, caller_owner, caller_obj, dest, dest_owner, count):
        s, d = find(caller, caller_owner), find(dest, dest_owner)
        coff = h['Coff'](BUILD / caller_obj)
        body, rel = coff.function(caller)
        offsets = [p for p, n, k in rel if n == dest and k == 20]
        assert len(offsets) == count
        for offset in offsets:
            assert body[offset - 1] in (0xe8, 0xe9)
            actual = struct.unpack('<i', h['data_at'](image, s['va'] + offset, 4))[0]
            addend = struct.unpack_from('<i', body, offset)[0]
            assert s['va'] + offset + 4 + actual == d['va'] + addend
        return dict(caller=caller, target=dest, offsets=offsets, binding=h['bind'](image, coff, s))
    edges = [
        call_edge('_W32Init@0', 'wow32.obj', 'obj/wow32.obj', '_UserRegisterWowHandlers@8', 'wow_user_registration_bridge.obj', 1),
        call_edge('_ECNcCreate@12', 'control-body-edecrare.obj', 'obj/opennt-user/control-body-edecrare.obj', '_wow_user_registered_callbacks@0', 'wow_user_registration_bridge.obj', 2),
        call_edge('_ECNcDestroyHandler@8', 'control-body-edecrare.obj', 'obj/opennt-user/control-body-edecrare.obj', '_wow_user_registered_callbacks@0', 'wow_user_registration_bridge.obj', 1),
    ]
    bridge = find('_UserRegisterWowHandlers@8', 'wow_user_registration_bridge.obj')
    bridge_bound = h['bind'](image, h['Coff'](BUILD / 'obj/wow32-dll/wow_user_registration_bridge.obj'), bridge)
    return dict(result='pass', original_wcall32_sha256=sha(source), init=init_bound, callbacks=bindings,
        edges=edges, bridge=bridge_bound,
        inputs=[dict(path=str(p), sha256=sha(p)) for p in (image_path, BUILD / 'wow32.dll',
            BUILD / 'wow32.dll.map', BUILD / 'build.ninja', BUILD / 'obj/wcall32.obj', BUILD / 'obj/wow32.obj',
            BUILD / 'obj/opennt-user/control-body-edecrare.obj', BUILD / 'obj/wow32-dll/wow_user_registration_bridge.obj',
            SRC / 'wow32.c', Path('src/wow32-dll/source/wow_user_registration_bridge.c'),
            Path('src/wow32-dll/include/wow_edit_memory_bindings.h'))],
        scope='Emitted address assignments and selected call edges; no callback execution or failure/lifetime proof')


if __name__ == '__main__':
    result = inspect()
    (OUT / 'parallel-review-fifth-evidence.json').write_text(json.dumps(result, indent=2) + '\n', encoding='utf-8')
    print('pass', len(result['callbacks']), 'callbacks and', len(result['edges']), 'edges')
