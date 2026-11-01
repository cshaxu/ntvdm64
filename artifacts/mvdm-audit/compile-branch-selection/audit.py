import concurrent.futures, csv, json, os, pathlib, re, shutil, subprocess, sys
from collections import defaultdict

ROOT = pathlib.Path.cwd()
OUT = ROOT / 'build/t473-s13/branch-inventory'
SHADOW = OUT / 'shadow' / 'src'
NINJA = pathlib.Path(r'C:\Users\neko\AppData\Local\Microsoft\WinGet\Packages\BrechtSanders.WinLibs.POSIX.MSVCRT_Microsoft.Winget.Source_8wekyb3d8bbwe\mingw64\bin\ninja.exe')
DEV = r'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'

def env_for(arch):
    result = subprocess.run(f'"{DEV}" -arch={arch} -host_arch=x64 >nul && set', shell=True, capture_output=True, check=True)
    return dict(line.split('=', 1) for line in result.stdout.decode('mbcs', errors='replace').splitlines() if '=' in line and not line.startswith('='))

def main():
    rows = list(csv.DictReader((OUT / 'branches.csv').open(encoding='utf-8', newline='')))
    by_file = defaultdict(list)
    for idx, row in enumerate(rows):
        by_file[row['path']].append((int(row['line']), idx))
    if '--prepared' not in sys.argv:
      shutil.copytree(ROOT / 'src', SHADOW, dirs_exist_ok=True)
    file_ids = {path: i for i, path in enumerate(by_file)}
    for path, entries in ([] if '--prepared' in sys.argv else by_file.items()):
        original = ROOT / path
        target = SHADOW / pathlib.PurePosixPath(path).relative_to('src')
        data = target.read_bytes()
        lines = data.splitlines(keepends=True)
        inserted = defaultdict(list)
        for line, idx in entries:
            end = line - 1
            while lines[end].rstrip(b'\r\n \t').endswith(b'\\'):
                end += 1
            inserted[end].append(idx)
        for i,line in enumerate(lines):
            if re.match(rb'^\s*#\s*endif\b',line): inserted.setdefault(i,[])
        name = str(original).replace('\\', '/').replace('"', '\\"')
        prefix=f'#pragma message("FILE_MARK_{file_ids[path]:06d}")\n#line 1 "{name}"\n'.encode('mbcs')
        if lines and lines[0].startswith(b'\xef\xbb\xbf'):
            lines[0]=lines[0][3:]; prefix=b'\xef\xbb\xbf'+prefix
        output = [prefix]
        logical_offset = 0
        for i, line in enumerate(lines):
            output.append(line)
            original_line = re.match(rb'^\s*#\s*line\s+(\d+)',line)
            if original_line: logical_offset=int(original_line.group(1))-(i+2)
            if i in inserted:
                if not line.endswith((b'\n', b'\r')):
                    output.append(b'\n')
                for idx in inserted[i]:
                    output.append(f'#pragma message("BRANCH_MARK_{idx:06d}")\n'.encode())
                output.append(f'#line {i+2+logical_offset} "{name}"\n'.encode('mbcs'))
        target.write_bytes(b''.join(output))
    envs = {a: env_for(a) for a in ('x86', 'x64')}
    compilers = {a: shutil.which('cl.exe', path=envs[a].get('PATH', envs[a].get('Path', ''))) for a in envs}
    if not all(compilers.values()): raise RuntimeError(f'Compiler resolution failed: {compilers}')
    jobs = []
    for arch in ('x86', 'x64'):
        for build in (ROOT / f'build/.ninja/msvc-{arch}-release').rglob('build.ninja'):
            folder = build.parent
            if '--product' in sys.argv and folder.name not in ({'worker','wow32'} if arch=='x86' else {'frontend','hook','launcher','monitor','service','worker'}): continue
            result = subprocess.run([str(NINJA), '-C', str(folder), '-t', 'compdb'], capture_output=True, text=True)
            if result.returncode:
                continue
            for entry in json.loads(result.stdout):
                command = entry.get('command', '')
                if not re.search(r'(?i)\bcl(?:\.exe)?\s', command):
                    continue
                source = entry.get('file', '')
                if not source.lower().endswith(('.c','.cpp')) or '/mvdm-guest/' in source.lower().replace('\\','/'):
                    continue
                if not source.lower().replace('\\','/').startswith(str(ROOT).lower().replace('\\','/') + '/src/'):
                    continue
                if not pathlib.Path(source).is_file(): continue
                jobs.append((arch, entry))
    if '--smoke' in sys.argv: jobs=jobs[:1]
    print(f'instrumented {len(by_file)} files; {len(jobs)} host compiler contexts', flush=True)
    def run(job):
        arch, entry = job
        command = re.split(r'(?i)\bcl(?:\.exe)?\s+', entry['command'], maxsplit=1)[1]
        command = re.sub(r'(?i)(?<!\S)/Fo(?:"[^"]*"|\S+)', '', command)
        command = re.sub(r'(?i)(?<!\S)/(?:c|showIncludes)(?=\s|$)', '', command)
        command = re.sub(re.escape(str(ROOT).replace('\\','/')+'/src/'), str(SHADOW).replace('\\','/')+'/', command, flags=re.I)
        command = re.sub(re.escape(str(ROOT).replace('\\','/')+'/src"'), str(SHADOW).replace('\\','/')+'"', command, flags=re.I)
        try:
            p = subprocess.run('"' + compilers[arch] + '" /EP ' + command, cwd=entry['directory'], env=envs[arch], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            marks = sorted(set(int(m) for m in re.findall(rb'BRANCH_MARK_(\d{6})', p.stdout + p.stderr)))
            files = sorted(set(int(m) for m in re.findall(rb'FILE_MARK_(\d{6})', p.stdout + p.stderr)))
            return {'arch':arch, 'file':entry['file'], 'directory':entry['directory'], 'exit':p.returncode, 'marks':marks, 'files':files, 'error':p.stderr.decode('mbcs',errors='replace')[-3000:] if p.returncode else ''}
        except Exception as e:
            return {'arch':arch,'file':entry['file'],'exit':-1,'marks':[],'error':str(e)}
    results=[]
    journal=(OUT/'contexts-progress.jsonl').open('w',encoding='utf-8')
    with concurrent.futures.ThreadPoolExecutor(max_workers=8) as pool:
        for i, item in enumerate(pool.map(run,jobs),1):
            results.append(item)
            journal.write(json.dumps(item)+'\n'); journal.flush()
            if i==1 and item['exit']!=0: print(json.dumps(item),flush=True)
            if i%100==0: print(f'{i}/{len(jobs)} done; failed={sum(x["exit"]!=0 for x in results)}',flush=True)
    journal.close()
    (OUT/'contexts.json').write_text(json.dumps(results),encoding='utf-8')
    seen=defaultdict(set); file_seen=defaultdict(set)
    for item in results:
        if item['exit']==0:
            for idx in item['marks']: seen[idx].add(item['arch'])
            for idx in item['files']: file_seen[idx].add(item['arch'])
    selected=len(seen)
    with (OUT/'observed.csv').open('w',encoding='utf-8',newline='') as f:
        writer=csv.DictWriter(f,fieldnames=list(rows[0])+['observed_profiles','file_observed_profiles'])
        writer.writeheader()
        for idx,row in enumerate(rows):
            profiles=file_seen.get(file_ids[row['path']],set())
            row['status']='selected' if idx in seen else ('not-selected-in-observed-contexts' if profiles else 'unobserved')
            writer.writerow(dict(row,observed_profiles=';'.join(sorted(seen.get(idx,()))),file_observed_profiles=';'.join(sorted(profiles))))
    print(json.dumps({'contexts':len(results),'success':sum(x['exit']==0 for x in results),'failed':sum(x['exit']!=0 for x in results),'selected':selected,'unobserved':len(rows)-selected}),flush=True)

if __name__ == '__main__': main()
