import hashlib, json, re, subprocess
from pathlib import Path

repo = Path(__file__).resolve().parents[3]
roots = {'current': repo/'src/mvdm', 'opennt': Path('O:/repos.external/OpenNT/base/mvdm'), 'opennt45': Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm')}
paths = ['inc/vint.h', 'inc/VINT.INC', 'softpc.new/host/src/nt_msscs.c']
names = ['INTERRUPT_PENDING_BIT', 'VIRTUAL_INTERRUPT_BIT', 'MIPS_BIT_MASK', 'EXEC_BIT_MASK', 'RM_BIT_MASK', 'RI_BIT_MASK', 'FIXED_NTVDMSTATE_SEGMENT', 'FIXED_NTVDMSTATE_OFFSET', 'FIXED_NTVDMSTATE_REL40', 'FIXED_NTVDMSTATE_SIZE', 'NTIO_LOAD_SEGMENT', 'NTIO_LOAD_OFFSET', 'VDM_TIMECHANGE']
def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest().upper() if path.is_file() else None
def values(path, pattern):
    source = path.read_text(encoding='latin1')
    result = {}
    for name in names:
        match = re.search(pattern.format(name=re.escape(name)), source, re.MULTILINE | re.IGNORECASE)
        if not match:
            raise ValueError(f'{path}: {name} missing')
        value = match.group(1)
        result[name] = int(value[:-1], 16) if value.upper().endswith('H') else int(value, 0)
    return result
c = values(roots['current']/paths[0], r'^\s*#define\s+{name}\s+(0x[0-9a-f]+|\d+)\b')
a = values(roots['current']/paths[1], r'^\s*{name}\s+EQU\s+([0-9a-f]+H|\d+)\b')
build = repo/'build/.ninja/msvc-x86-release/worker'
p = subprocess.run(['ninja', '-C', str(build), '-t', 'deps', 'obj/host/nt_msscs.obj', 'obj/dpmi/dpmi32.obj'], capture_output=True, text=True, encoding='utf-8', errors='replace', check=True)
deps = [s.strip().replace('\\', '/') for s in p.stdout.splitlines() if 'vint.h' in s.lower() or (s.startswith('obj/') and ('VALID' in s or 'STALE' in s))]
print(json.dumps({'sha256': {f: {n: sha(r/f) for n, r in roots.items()} for f in paths}, 'c_constants': c, 'asm_constants': a, 'constants_match': c == a, 'deps': deps}, indent=2))
