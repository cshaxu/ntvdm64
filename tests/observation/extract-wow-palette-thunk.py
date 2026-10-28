"""Extract original palette thunk bodies for the controlled native fixture."""
import argparse
import hashlib
import json
from pathlib import Path

repo = Path(__file__).resolve().parents[2]
parser = argparse.ArgumentParser()
parser.add_argument('output', type=Path)
args = parser.parse_args()
output = args.output.resolve()
output.relative_to((repo / 'build').resolve())
output.mkdir(parents=True, exist_ok=False)


def extract(source, signature, name):
    text = (repo / source).read_text(encoding='latin1')
    assert text.count(signature) == 1
    begin = text.index(signature)
    brace = text.index('{', begin)
    depth = 0
    for position in range(brace, len(text)):
        if text[position] == '{':
            depth += 1
        elif text[position] == '}':
            depth -= 1
            if depth == 0:
                data = (text[begin:position + 1] + '\n').encode('latin1')
                (output / name).write_bytes(data)
                return hashlib.sha256(data).hexdigest().upper()
    raise ValueError('Unbalanced source function')


result = {
    'get_thunk_sha256': extract('src/mvdm/wow32/wgpal.c',
        'ULONG FASTCALL WG32GetPaletteEntries(PVDMFRAME pFrame)', 'original-getpaletteentries.inc'),
    'copy_sha256': extract('src/mvdm/wow32/wstruc.c',
        'VOID putpaletteentry16(VPPALETTEENTRY16 vppe, INT c, LPPALETTEENTRY lpp)', 'original-putpaletteentry.inc'),
}
(output / 'extract.json').write_text(json.dumps(result, indent=2) + '\n', encoding='utf-8')
print(json.dumps(result))
