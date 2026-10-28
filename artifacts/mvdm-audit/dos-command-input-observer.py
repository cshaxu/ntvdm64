"""Trace original DOS command source inputs; never build or replace guests.

Object roots below were checked against the actual link responses or inline
link16 commands. Includes are recorded as conditional source edges, not as a
claim that every assembler/C branch executes in the distributed guest.
"""
import csv
import hashlib
import io
import json
import re
import subprocess
from collections import deque
from pathlib import Path

A = Path('artifacts/mvdm-audit')
M = Path('src/mvdm')
ROOT = M / 'dos/v86/cmd'
BASE = 'bba17e229'
RESPONSE_FAMILIES = ('command', 'debug', 'edlin', 'exe2bin', 'graphics', 'keyb', 'share')
INLINE_ROOTS = {
    'append': ['append'],
    'loadfix': ['loadfix'],
    'mem': ['mem', '_msgret', '_parse', 'memex', 'submsg', 'membase', 'chkvdisk', 'memctrlc'],
    'nlsfunc': ['nlsfunc', 'nlsparm'],
    'setver': ['device', 'setver', 'parse', 'dos'],
}


def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest().upper()


def uncomment(path):
    text = path.read_text(encoding='latin1')
    if path.suffix.casefold() in ('.c', '.h'):
        text = re.sub(r'/\*.*?\*/', lambda m: '\n' * m[0].count('\n'), text, flags=re.S)
    delimiter = None
    for number, line in enumerate(text.splitlines(), 1):
        if delimiter:
            if delimiter in line:
                delimiter = None
            continue
        comment = re.match(r'^\s*COMMENT\s+(\S)', line, re.I)
        if comment:
            delimiter = comment[1] if comment[1] not in line[comment.end():] else None
            continue
        if path.suffix.casefold() not in ('.c', '.h'):
            line = line.split(';', 1)[0]
        yield number, line


def observe():
    tracked_inputs = {}

    def read(path):
        tracked_inputs[path.as_posix()] = sha(path)
        return path.read_text(encoding='latin1')

    common = M / 'dos/v86/make.inc'
    rules = read(common)
    assert '.asm.obj:' in rules and '$*.asm;' in rules
    assert '.c.obj:' in rules and '$*.c' in rules and 'cc      =cl16' in rules
    reached = {}
    families = {}
    for family in RESPONSE_FAMILIES + tuple(INLINE_ROOTS):
        directory = ROOT / family
        makefile = directory / 'makefile'
        make = read(makefile)
        assert re.search(r'(?im)^include\s+\.\.\\\.\.\\make.inc', make)
        assert 'link16' in make
        response = directory / (family + '.lnk')
        if family in RESPONSE_FAMILIES:
            text = read(response)
            # The first output-image token ends the object list. Link switches
            # are removed; bare object stems and explicit .obj are both real.
            match = re.search(r'(?i)\b[\w]+\.(?:exe|exc)\b', text)
            assert match, family
            object_text = re.sub(r'/\w+', '', text[:match.start()])
            stems = re.findall(r'\b([A-Za-z_][\w]*)(?:\.obj)?\b', object_text, re.I)
        else:
            stems = INLINE_ROOTS[family]
            for stem in stems:
                assert re.search(r'(?i)\b' + re.escape(stem) + r'\.obj\b', make), (family, stem)
        roots = []
        for stem in stems:
            sources = [p for suffix in ('.asm', '.c') if (p := directory / (stem + suffix)).is_file()]
            assert len(sources) == 1, (family, stem, sources)
            source = sources[0]
            roots.append(source.as_posix())
            reached.setdefault(source.as_posix().casefold(), (source, [
                f'{makefile.as_posix()}: original link16 guest target',
                f'{response.as_posix()}: {stem}.obj' if family in RESPONSE_FAMILIES else f'{makefile.as_posix()}: inline {stem}.obj',
                f'{common.as_posix()}: source-to-object inference for {source.name}',
            ]))
        families[family] = dict(makefile=makefile.as_posix(), response=response.as_posix() if family in RESPONSE_FAMILIES else None, roots=roots)
    queue = deque(reached.values())
    while queue:
        path, chain = queue.popleft()
        read(path)
        for line, body in uncomment(path):
            match = re.match(r'^\s*(?:include\s+|#\s*include\s*)[<\"]?([^\s>\";]+)', body, re.I)
            if not match:
                continue
            token = match[1].replace('\\', '/')
            utility = Path(chain[0].split(': original link16')[0]).parent
            candidates = [utility / token, M / 'dos/v86/inc' / token,
                          M / 'inc' / token] if path.suffix.casefold() not in ('.c', '.h') else [path.parent / token, M / 'dos/v86/h' / token]
            selected = next((p for p in candidates if p.is_file()), None)
            if selected is None:
                continue
            selected = selected.resolve().relative_to(Path.cwd())
            key = selected.as_posix().casefold()
            if key not in reached:
                edge = (selected, chain + [f'{path.as_posix()}:{line}: {body.strip()} (original branch conditions retained)'])
                reached[key] = edge
                queue.append(edge)
    special = {}
    command = ROOT / 'command'
    tool_make = command / 'makefil0'
    tool_recipe = read(tool_make)
    assert 'cl16 -I.\\$(COUNTRY)' in tool_recipe and 'forcedos link buildmsg,' in tool_recipe
    buildmsg = command / 'buildmsg.c'
    assert '#include "bldmsg.h"' in read(buildmsg)
    assert not (command / 'bldmsg.h').exists()
    for leaf in ('buildmsg.c', 'usa-ms/bldmsg.h'):
        special['dos/v86/cmd/command/' + leaf] = ('message-tool-input-observed', [
            f'{tool_make.as_posix()}: COUNTRY/buildmsg.exe <- cl16 buildmsg.c; forcedos link buildmsg',
            'buildmsg.c:16 includes bldmsg.h; cl16 -I.\\$(COUNTRY) selects usa-ms/bldmsg.h when COUNTRY=usa-ms',
            'This is the historical message-generation tool, not an object linked into COMMAND.COM',
        ])
    obsolete = command / 'tcmd1a.asm'
    assert 'THIS FILE IS OBSOLETE, NO LONGER USED' in read(obsolete)
    assert 'This module replaces TCMD1A.ASM.' in read(command / 'dir.asm')
    assert 'Obsolete DIR command.  Module no longer used.' in read(command / 'readme.txt')
    special['dos/v86/cmd/command/tcmd1a.asm'] = ('obsolete-source-observed', [
        'tcmd1a.asm:8 explicitly obsolete; command/readme.txt:62 confirms no longer used',
        'dir.asm:13 declares replacement; actual command.lnk selects dir.obj, not tcmd1a.obj',
    ])
    history = ROOT / 'exe2bin/e2bchng.inc'
    read(history)
    active = [line.strip().strip('\x1a') for _, line in uncomment(history)]
    assert {line.casefold() for line in active if line} == {'.xlist', '.list'}
    special['dos/v86/cmd/exe2bin/e2bchng.inc'] = ('source-history-observed', [
        'Original e2bchng.inc: change-history comments with only .xlist/.list controls; no code/data declarations',
        'Actual EXE2BIN link roots are e2binit.obj and display.obj; no source-include edge observed',
    ])
    setver = ROOT / 'setver'
    alias = setver / 'usa-ms/message.h'
    assert 'message.h : $(COUNTRY)\\message.h' in read(setver / 'makefile')
    assert sha(alias) == sha(setver / 'message.h')
    special['dos/v86/cmd/setver/usa-ms/message.h'] = ('source-chain-observed', [
        'setver/makefile: message.h <- $(COUNTRY)\\message.h; copy locale input into working directory',
        'COUNTRY=usa-ms branch: usa-ms/message.h equals retained local message.h byte-for-byte',
        'setver.c:69 includes message.h; setver.obj -> inline link16 SETVER.EXE',
    ])
    append_legacy = ROOT / 'append/appendm.asm'
    old_append_symbols = re.findall(r'(?im)^\s*public\s+(\w+)', read(append_legacy))
    append_source = ROOT / 'append/append.asm'
    append_active = '\n'.join(body for _, body in uncomment(append_source))
    assert len(old_append_symbols) == 18
    assert all(not re.search(r'\b' + re.escape(name) + r'\b', append_active, re.I) for name in old_append_symbols)
    assert 'MSG_SERVICES <APPEND.CLA,APPEND.CL1,APPEND.CTL>' in append_active
    assert re.search(r'(?i)call\s+sysdispmsg', append_active)
    special['dos/v86/cmd/append/appendm.asm'] = ('obsolete-source-observed', [
        'Actual APPEND link16 root is append.obj only; appendm.asm exports eighteen old message/length symbols',
        'All eighteen names are absent from active append.asm; old extern declarations and uses are commented out',
        'append.asm:439-441 retains MSG_SERVICES APPEND message tables; LOADmsg and active SYSDISPMSG calls retain message behavior',
    ])
    keyb_cmd = ROOT / 'keyb/keybcmd.asm'
    keyb_active = '\n'.join(body for _, body in uncomment(keyb_cmd))
    old_keyb = ROOT / 'keyb/kbmsg.inc'
    old_keyb_declarations = ROOT / 'keyb/keybmsg.inc'
    old_keyb_symbols = re.findall(r'(?im)^\s*(\w+)\s+DB\b', read(old_keyb))
    keyb_imports = re.findall(r'(?im)^\s*EXTRN\s+(\w+)\s*:\s*BYTE\b', read(old_keyb_declarations))
    assert {name.casefold() for name in keyb_imports} == {name.casefold() for name in old_keyb_symbols}
    local_keyb_symbols = set(name.casefold() for name in re.findall(r'(?im)^\s*(\w+)\s+(?:EQU|DB)\b', keyb_active))
    assert len(old_keyb_symbols) == 18
    for name in old_keyb_symbols:
        assert name.casefold() in local_keyb_symbols or not re.search(r'\b' + re.escape(name) + r'\b', keyb_active, re.I)
    assert re.search(r'(?im)^ACT_KEYB\s+EQU\s+2\b', keyb_active)
    assert re.search(r'(?im)^INV_KEYB_Q\s+EQU\s+10\b', keyb_active)
    assert 'MSG_SERVICES <KEYB.CL1>' in keyb_active and 'CALL    SYSDISPMSG' in keyb_active
    for leaf in ('kbmsg.inc', 'keybmsg.inc'):
        special['dos/v86/cmd/keyb/' + leaf] = ('obsolete-source-observed', [
            'Actual KB16 link response omits the older KEYBMSG object mentioned only in source comments',
            'kbmsg.inc defines eighteen old message data names; keybmsg.inc imports the old byte-label ABI',
            'Selected keybcmd.asm defines referenced names locally, e.g ACT_KEYB EQU 2 and INV_KEYB_Q EQU 10, and calls SYSDISPMSG using MSG_SERVICES KEYB.CL1/CL2/CLA',
            'No referenced legacy name needs the old external byte-label model; keyboard/locale message behavior remains retained',
        ])
    prior = subprocess.check_output(['git', 'show', BASE + ':artifacts/mvdm-audit/combined-initial-status.csv'])
    cohort = [r for r in csv.DictReader(io.StringIO(prior.decode('utf-8-sig'))) if r['status'] == 'pending' and r['relative_path'].startswith('dos/v86/cmd/')
              and Path(r['relative_path']).suffix.casefold() in ('.asm', '.inc', '.h', '.c')]
    records = []
    for row in cohort:
        path = M / row['relative_path']
        hit = reached.get(path.as_posix().casefold())
        explicit = special.get(row['relative_path'])
        original = Path('O:/repos.external/OpenNT/base/mvdm') / row['relative_path']
        assert original.is_file() and sha(original) == sha(path), path
        records.append(dict(relative_path=row['relative_path'], shard=row['shard'], sha256=sha(path),
                            original_path=str(original), original_edition='OpenNT',
                            result=explicit[0] if explicit else 'source-chain-observed' if hit else 'unresolved',
                            chain=explicit[1] if explicit else hit[1] if hit else [],
                            limitation='Original guest build-input role only; not active branch, shipped-image derivation or runtime acceptance'))
    read(Path(__file__).resolve().relative_to(Path.cwd()))
    return dict(base=BASE, result='observed', reviewed_count=len(records),
                source_chain_count=sum(r['result'] == 'source-chain-observed' for r in records),
                resolved_role_count=sum(r['result'] != 'unresolved' for r in records),
                families=families, records=records,
                inputs=[dict(path=name, sha256=digest) for name, digest in sorted(tracked_inputs.items())])


if __name__ == '__main__':
    result = observe()
    (A / 'dos-command-input-observations.json').write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps({key: result[key] for key in ('base', 'result', 'reviewed_count', 'source_chain_count', 'resolved_role_count')}))
    for row in result['records']:
        if row['result'] == 'unresolved':
            print('UNRESOLVED ' + row['relative_path'])
