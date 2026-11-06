"""Bind selected original WOW shell, hook and subclass owners to released code."""
import sys
sys.dont_write_bytecode = True
import hashlib
import json
import runpy
from pathlib import Path

A = Path('artifacts/mvdm-audit')
B = Path('build/.ninja/msvc-x86-release/wow32')
ROOTS = (Path('src/mvdm-host'), Path('O:/repos.external/OpenNT/base/mvdm'),
         Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'))
STEMS = ('wshell', 'wsubcls', 'wuhook', 'wowhooks', 'wkman')


def sha(p):
    return hashlib.sha256(Path(p).read_bytes()).hexdigest().upper()


def inspect():
    h = runpy.run_path(str(A / 'host-tail-image-observer.py'))
    image, cached = h['Image'](Path('assets/release/WOW32.DLL')), h['Image'](B / 'wow32.dll')
    assert image.base == cached.base and image.code == cached.code
    assert all(image.data[a:b] == cached.data[a:b] for a, b, _ in image.code)
    graph = (B / 'build.ninja').read_text().replace('$:', ':')
    link = next(x for x in graph.splitlines() if x.startswith('build wow32.dll |'))
    symbols = h['symbols']((B / 'wow32.dll.map').read_text())
    sources = []
    for stem in STEMS:
        rel = 'wow32/' + stem + '.c'
        hashes = [sha(root / rel) for root in ROOTS]
        # Record current differences; runtime coverage is a separate receipt.
        target = 'obj/' + stem + '.obj'
        assert target in link.split()
        edge = next(x for x in graph.splitlines() if x.startswith('build ' + target + ':'))
        own = [s for s in symbols if s['owner'].lower() == stem + '.obj' and s['function'] and not s['name'].startswith('$')]
        if stem == 'wkman':
            own = [s for s in own if s['name'] in ('_WowOriginalFreeTask@4','_W32DestroyTask@4')]
            assert len(own) == 2
        assert own
        obj = h['Coff'](B / target)
        bound=[]
        for symbol in own:
            body, relocs = obj.function(symbol['name'])
            # x86 IMAGE_REL_I386_SECREL (0x0b) is a four-byte section-relative
            # offset emitted for TLS variables; the linker resolves it too.
            assert body and all(kind in (6, 11, 20) for _, _, kind in relocs)
            if symbol['name'] == '_WowOriginalFreeTask@4':
                assert any(name == '_W32FreeOwnedHooks@4' and kind == 20 for _,name,kind in relocs)
            ignored = {i for offset, _, _ in relocs for i in range(offset, offset+4)}
            actual = h['data_at'](image, symbol['va'], len(body))
            assert len(actual) == len(body)
            assert all(i in ignored or b == actual[i] for i,b in enumerate(body)), symbol
            bound.append(dict(**symbol, bytes=len(body), relocation_words=len(relocs),
                              relocation_types=sorted({kind for _,_,kind in relocs}),
                              compared_bytes=len(body)-len(ignored)))
        sources.append(dict(path=rel, hashes=hashes, edge=edge, object_sha256=sha(B / target),bindings=bound))
    return dict(result='pass', sources=sources, release_sha256=sha('assets/release/WOW32.DLL'),
        cached_sha256=sha(B / 'wow32.dll'), map_sha256=sha(B / 'wow32.dll.map'),
        limits='Static body selection only; S36 guest and lifecycle evidence is separate.')


if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
