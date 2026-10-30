import io, subprocess, zipfile
for revision in ('f47d951b0', '6275809e0', 'dd7cddd7d', '535e3d4da'):
    data = subprocess.check_output(['git', 'show', revision + ':assets/winnt.zip'])
    archive = zipfile.ZipFile(io.BytesIO(data))
    names = [n for n in archive.namelist() if n.upper().endswith('/D38K.COM')]
    markers = [n for n in archive.namelist() if n.upper().endswith('.COM') and
               b'S38_INT32_RETURN_OK' in archive.read(n)]
    print(revision, len(data), 'name=', names, 'marker=', markers)
