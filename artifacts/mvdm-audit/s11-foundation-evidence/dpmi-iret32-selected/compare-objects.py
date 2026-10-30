import hashlib, re, subprocess
paths = ['build/.ninja/msvc-x86-release/worker/obj/dpmi/dpmiint.obj',
         'build/t473-s11/dpmi-fault-iret32-selected/dpmiint.obj']
values = []
for path in paths:
    disassembly = subprocess.check_output(['objdump', '-d', path], text=True)
    match = re.search(r'<_DpmiFaultHandlerIret32>:\s*\n(.*?)(?=\n\s*Disassembly of section|\n[0-9a-f]+ <|\Z)', disassembly, re.S)
    assert match, path
    values.append(bytes.fromhex(' '.join(re.findall(r'^\s*[0-9a-f]+:\s*((?:[0-9a-f]{2} )+)', match.group(1), re.M))))
    print(path, len(values[-1]), hashlib.sha256(values[-1]).hexdigest())
print('identical=', values[0] == values[1])
assert values[0] == values[1]
