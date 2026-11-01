import collections, csv, hashlib, json, pathlib, re, subprocess
root=pathlib.Path.cwd(); out=root/'build/t473-s13/branch-inventory'
files=subprocess.check_output(['git','ls-files','-z','src']).decode().split('\0')
files=[p for p in files if p.lower().endswith(('.c','.cpp','.h')) and (root/p).is_file()]
rows=[]; stats=collections.Counter(); errors=[]
for path in files:
    raw=(root/path).read_bytes(); text=raw.decode('latin-1')
    clean=re.sub(r'/\*[\s\S]*?\*/|//[^\r\n]*|"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'',lambda m: re.sub(r'[^\r\n]',' ',m.group()) if m.group().startswith('/') else m.group(),text)
    lines=clean.split('\n'); stack=[]; i=0
    while i<len(lines):
        n=i+1; line=lines[i]
        while line.rstrip().endswith('\\') and i+1<len(lines): i+=1; line=line.rstrip()[:-1]+' '+lines[i].lstrip()
        m=re.match(r'^\s*#\s*(if|ifdef|ifndef|elif|else|endif)\b(.*)',line)
        if m:
            directive,expr=m.group(1),m.group(2).strip()
            if directive in ('if','ifdef','ifndef'): stack.append(n)
            elif directive in ('elif','else') and not stack: errors.append((path,n,directive))
            if directive!='endif':
                rows.append((path,n,directive,expr,stack[-1] if stack else '',len(stack),'unobserved',hashlib.sha256(raw).hexdigest()));stats[directive]+=1
            if directive=='endif':
                if stack: stack.pop()
                else: errors.append((path,n,directive))
        i+=1
    if stack: errors.append((path,stack,'unclosed'))
with (out/'branches.csv').open('w',newline='',encoding='utf-8') as f:
    w=csv.writer(f);w.writerow(('path','line','directive','expression','chain_start','depth','status','sha256'));w.writerows(rows)
summary={'source_files':len(files),'branch_alternatives':len(rows),'directives':dict(stats),'parser_errors':len(errors),'meaning':'lexical only; all unobserved until real preprocessing'}
(out/'summary.json').write_text(json.dumps(summary,indent=2));(out/'errors.json').write_text(json.dumps(errors,indent=2));print(json.dumps(summary))
