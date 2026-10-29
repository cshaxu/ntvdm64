"""Compare unmodified and minimal-candidate source with diagnostic allocation hooks."""
from pathlib import Path
import hashlib
import subprocess

root = Path(__file__).resolve().parents[3]
out = root / 'build/t473-s10/niddb-rollback'
out.mkdir(parents=True, exist_ok=True)
source = (root / 'src/mvdm/softpc.new/base/bios/virtual.c').read_text()
start = source.index('/* Allocate data structures required for new data instance. */')
end = source.index('/* Copy data items from one instance to another */', start)
part = source[start:end]
fix = ('\t    /* MVDM-HOST-DIV-468: the table is not published in vrecs on\n'
       '\t       failure, so it must be released along with its data blocks. */\n'
       '\t    host_free(instance_ptr);\n')
assert part.count(fix) == 1
part = part.replace(fix, '')
for old, new in [('host_malloc(', 'niddb_test_malloc('),
                 ('host_calloc(', 'niddb_test_calloc('),
                 ('host_free(', 'niddb_test_free(')]:
    assert part.count(old) == 1
    part = part.replace(old, new)
declarations = ('extern void *niddb_test_malloc(size_t);\n'
                'extern void *niddb_test_calloc(size_t,size_t);\n'
                'extern void niddb_test_free(void *);\n')
needle = '\t    return FALSE;\n\t    }'
assert part.count(needle) == 1
compile_cmd = (root / 'build/t473-s10/niddb-retirement/compile.cmd').read_text()
fixture = root / 'artifacts/mvdm-audit/host-ch-scope/niddb-rollback-fixture.c'
driver = root / 'build/.ninja/msvc-x86-release/wow32/msvc-x86.cmd'
for name in ('baseline', 'candidate'):
    body = part if name == 'baseline' else part.replace(needle, '\t    niddb_test_free(instance_ptr);\n' + needle)
    generated = out / (name + '.c')
    generated.write_text(source[:start] + declarations + body + source[end:])
    obj = out / (name + '.obj')
    command = compile_cmd.replace('build\\t473-s10\\niddb-retirement\\candidate.obj', str(obj))
    command = command.replace('src\\mvdm\\softpc.new\\base\\bios\\virtual.c', str(generated))
    script = out / (name + '-compile.cmd')
    script.write_text(command)
    build = subprocess.run(['cmd', '/c', str(script)], cwd=root, capture_output=True, text=True)
    (out / (name + '-compile.log')).write_text(build.stdout + build.stderr)
    if build.returncode: raise SystemExit(f'{name} compile failed: {build.returncode}')
    exe = out / (name + '.exe')
    link = f'@echo off\ncall "{driver}"\ncl.exe /nologo /MT /Od "{fixture}" "{obj}" /Fe:"{exe}" /link kernel32.lib\n'
    link_script = out / (name + '-link.cmd')
    link_script.write_text(link)
    p = subprocess.run(['cmd', '/c', str(link_script)], cwd=out, capture_output=True, text=True)
    (out / (name + '-link.log')).write_text(p.stdout + p.stderr)
    if p.returncode: raise SystemExit(f'{name} link failed: {p.returncode}')
    for ordinal in ('1', '2') if name == 'baseline' else (None,):
        args = [str(exe)] + ([ordinal] if ordinal else [])
        run = subprocess.run(args, capture_output=True, text=True)
        print(name, 'exit', run.returncode, run.stdout.strip(),
              'object_sha256', hashlib.sha256(obj.read_bytes()).hexdigest(), flush=True)
        expected = f'ordinal={ordinal} calls={ordinal} live=1'
        if (name == 'baseline' and (run.returncode != 1 or expected not in run.stdout)) or \
           (name == 'candidate' and (run.returncode != 0 or 'ROLLBACK_PASS' not in run.stdout)):
            raise SystemExit('unexpected diagnostic result')
