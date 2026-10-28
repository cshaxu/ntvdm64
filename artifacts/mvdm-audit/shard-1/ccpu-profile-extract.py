"""Retain and verify exact selected-profile preprocessor excerpts."""
import argparse
import hashlib
import io
import json
import re
import zipfile
from pathlib import Path

HERE = Path(__file__).resolve().parent
BUILD = Path('build/t473-s10/shard1-ccpu-preprocess-r1')
EVIDENCE = json.loads((HERE / 'ccpu-profile-evidence.json').read_text(encoding='utf-8'))


def sha(data):
    return hashlib.sha256(data).hexdigest().upper()


def marked(blob, path):
    out, selected = [], False
    for line in blob.splitlines(keepends=True):
        m = re.match(rb'#line\s+\d+\s+"([^"]+)"', line)
        if m:
            normalized = re.sub(r'[/\\]+', '/', m.group(1).decode('latin1')).lower()
            selected = normalized.endswith('/src/mvdm/' + path)
            if selected:
                out.append(line)
        elif selected:
            out.append(line)
    return b''.join(out)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--check', action='store_true')
    parser.add_argument('--verify-retained', action='store_true')
    args = parser.parse_args()
    if args.verify_retained:
        saved = json.loads((HERE / 'ccpu-profile-excerpts.json').read_text(encoding='utf-8'))
        assert saved['result'] == 'pass' and len(saved['records']) == 3
        archive = (HERE / saved['archive']).read_bytes()
        assert sha(archive) == saved['archive_sha256']
        with zipfile.ZipFile(io.BytesIO(archive)) as z:
            assert len(z.namelist()) == len(saved['records'])
            for item in saved['records']:
                assert sha(z.read(item['member'])) == item['sha256']
        print('PASS retained profile excerpts')
        return
    units = {Path(x['target']).stem.lower(): x['sha256'] for x in EVIDENCE['preprocess']}
    records, payload = [], {}
    for stem in ('invd', 'bswap'):
        blob = (BUILD / ('ccpu_' + stem + '.i')).read_bytes()
        assert sha(blob) == units[stem]
        excerpt = marked(blob, 'softpc.new/base/ccpu386/' + stem + '.h')
        assert excerpt and (stem.upper().encode() in excerpt)
        name = 'ccpu-profile-' + stem + '-header.txt'
        payload[name] = excerpt
        records.append(dict(member=name, sha256=sha(excerpt), input_sha256=sha(blob)))
    blob = (BUILD / 'ccpu_c_main.i').read_bytes()
    assert sha(blob) == units['c_main']
    start = blob.index(b'      case  0x08:')
    end = blob.index(b'      case  0x0f:', start)
    excerpt = blob[start:end]
    assert excerpt.count(b'Int6();') == 2 and b'INVD();' not in excerpt and b'WBINVD();' not in excerpt
    name = 'ccpu-profile-selected-decoder.txt'
    payload[name] = excerpt
    records.append(dict(member=name, sha256=sha(excerpt), input_sha256=sha(blob)))
    data = io.BytesIO()
    with zipfile.ZipFile(data, 'w', compression=zipfile.ZIP_DEFLATED, compresslevel=9) as z:
        for name, content in payload.items():
            entry = zipfile.ZipInfo(name, (1980, 1, 1, 0, 0, 0))
            entry.compress_type = zipfile.ZIP_DEFLATED
            entry.external_attr = 0o600 << 16
            z.writestr(entry, content, compress_type=zipfile.ZIP_DEFLATED, compresslevel=9)
    archive = data.getvalue()
    archive_path = HERE / 'ccpu-profile-excerpts.zip'
    if args.check:
        assert archive_path.read_bytes() == archive
    else:
        archive_path.write_bytes(archive)
    manifest = json.dumps(dict(result='pass', archive=archive_path.name, archive_sha256=sha(archive), records=records,
        scope='Preprocessed declaration and decoder excerpts only'), indent=2) + '\n'
    path = HERE / 'ccpu-profile-excerpts.json'
    if args.check:
        assert path.read_text(encoding='utf-8') == manifest
    else:
        path.write_text(manifest, encoding='utf-8')
    print('PASS exact original preprocessor excerpts', len(records))


if __name__ == '__main__':
    main()
