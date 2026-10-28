"""Verify the bounded S10 guest-source-role checkpoint, not guest behavior."""
import argparse
import collections
import csv
import hashlib
import io
import json
import re
import runpy
import struct
import subprocess
import zipfile
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BASE = 'd050fa4b4'


def rows(path, delimiter=','):
    with path.open(encoding='utf-8-sig', newline='') as stream:
        return list(csv.DictReader(stream, delimiter=delimiter))


def original_rows(path, base):
    data = subprocess.check_output(['git', 'show', f'{base}:{path.as_posix()}'])
    return list(csv.DictReader(io.StringIO(data.decode('utf-8-sig'))))


def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest().upper()


def write_csv_like_base(path, content, base):
    raw = subprocess.check_output(['git', 'show', f'{base}:{path.as_posix()}'])
    ending = '\r\n' if b'\r\n' in raw.split(b'\n', 1)[0] + b'\n' else '\n'
    encoding = 'utf-8-sig' if raw.startswith(b'\xef\xbb\xbf') else 'utf-8'
    with path.open('w', encoding=encoding, newline='') as stream:
        writer = csv.DictWriter(stream, list(content[0]), lineterminator=ending)
        writer.writeheader()
        writer.writerows(content)


def apply_dispositions(dispositions, base, review, note):
    """Update one exact reviewed cohort, retaining base CSV framing and other rows."""

    changed = {r['relative_path']: r for r in dispositions}
    assert len(changed) == len(dispositions)
    ledger = rows(A / 'combined-initial-status.csv')
    prior = {r['relative_path']: r for r in original_rows(A / 'combined-initial-status.csv', base)}
    for row in ledger:
        if row['relative_path'] in changed:
            proof = changed[row['relative_path']]
            original = prior[row['relative_path']]
            assert original['status'] == 'pending'
            assert row == original or row == dict(original, status=proof['status'])
            row['status'] = proof['status']
    specs = [('shard-0/review.csv', 'classification'), ('shard-1/file-audit.csv', 'audit_status'),
             ('shard-2/file-audit.csv', 'decision'), ('shard-3/file-review.csv', 'review_class')]
    for filename, column in specs:
        content = rows(A / filename)
        if not any(row['relative_path'] in changed for row in content):
            continue
        for row in content:
            proof = changed.get(row['relative_path'])
            if not proof:
                continue
            row[column] = ({'partial': '部分复通', 'excluded': '整体排除', 'full': '全部复通'}[proof['status']]
                           if column in ('classification', 'audit_status') else proof['status'])
            row['estimated_restored_code_pct'] = proof.get('estimated_restored_code_percent', '')
            if 'restored_estimate_status' in row:
                row['restored_estimate_status'] = proof['restored_estimate_status']
            for field in ('finding', 'role_or_prior_finding', 'decision_basis', 'review_basis'):
                if field in row:
                    row[field] = proof['role'] + '; ' + proof['original_edge']
            for field in ('evidence', 'reviewed_test_evidence', 'specific_evidence'):
                if field in row:
                    row[field] = review + '; ' + proof['evidence_table']
            for field in ('remaining_evidence', 'next_required_evidence', 'review_limits', 'remaining_proof'):
                if field in row:
                    row[field] = proof['limits']
        write_csv_like_base(A / filename, content, base)
    write_csv_like_base(A / 'combined-initial-status.csv', ledger, base)
    summary = json.loads((A / 'initial-summary.json').read_text())
    summary['counts'] = dict(collections.Counter(r['status'] for r in ledger))
    summary['note'] = note
    (A / 'initial-summary.json').write_text(json.dumps(summary, indent=2) + '\n')


def main(output, base=BASE, prefix='guest-source-role-', expected_count=187):
    frozen_path = A / 'frozen-file-reconciliation.csv'
    assert sha(frozen_path) == 'F73FFD45DFF44FCA4ECEFC49EF769BEAB5C085542CD04FFE4D6A7B5F942E3196'
    frozen = {r['relative_path']: r for r in rows(frozen_path)}
    merged_path = A / 'combined-initial-status.csv'
    merged = {r['relative_path']: r for r in rows(merged_path)}
    old = {r['relative_path']: r for r in original_rows(merged_path, base)}
    dispositions = rows(A / (prefix + 'dispositions.csv'))
    changed = {r['relative_path']: r for r in dispositions}
    assert len(dispositions) == len(changed) == expected_count
    assert len(frozen) == len(merged) == len(old) == 4684
    assert set(frozen) == set(merged) == set(old)
    delta = {p for p in merged if merged[p] != old[p]}
    assert delta == set(changed)
    supplemental = None
    if any(r['edition'] == 'OpenNT-src-2' for r in dispositions):
        supplement_summary = json.loads((A / 'supplemental-original-provenance-summary.json').read_text())
        assert sha(A / 'supplemental-original-provenance.csv') == supplement_summary['csv_sha256']
        assert sha(Path(supplement_summary['manifest'])) == supplement_summary['manifest_sha256'] == '84D314A2BBBE9B78304EDBFFBB92BDB7ABA6FD209010CC1FF0341AE761910EDF'
        supplemental = {r['relative_path']: r for r in rows(A / 'supplemental-original-provenance.csv')}
        source_manifest = {r['source_relative_path']: r for r in rows(Path(supplement_summary['manifest']), '\t')}
    for path, row in merged.items():
        assert int.from_bytes(hashlib.sha256(path.encode()).digest()[:8], 'big') % 4 == int(row['shard'])
        if path in changed:
            proof = changed[path]
            assert proof['status'] in ('excluded', 'partial', 'full')
            expected = dict(old[path], status=proof['status'])
            assert old[path]['status'] == changed[path]['prior_status'] == 'pending'
            assert row == expected
            original_hash = sha(Path(proof['original_path']))
            if prefix == 'retained-tools16-role-':
                assert path.startswith('tools16/') and not (Path('src/mvdm') / path).exists()
                current_hash = sha(Path('tools/historical/opennt-4.5') / Path(path).name)
                assert not frozen[path]['current_sha256']
                assert current_hash == proof['sha256'] == frozen[path]['opennt45_sha256']
            else:
                current_hash = sha(Path('src/mvdm') / path)
                assert current_hash == proof['sha256']
                assert proof['sha256'] == frozen[path]['current_sha256']
            if proof['edition'] == 'OpenNT-src-2':
                assert frozen[path]['upstream_origin'] == 'current-only' and not frozen[path]['opennt_sha256'] and not frozen[path]['opennt45_sha256']
                provenance = supplemental[path]
                record = source_manifest[path]
                assert original_hash == provenance['source_sha256'] == record['source_sha256'].upper()
                assert record['source_sha256'] == record['target_sha256']
                assert provenance['git_match'] in ('raw', 'CRLF-normalized')
                assert Path(provenance['source_path']).resolve() == Path(proof['original_path']).resolve()
                if proof['status'] == 'excluded':
                    assert provenance['provenance'] == 'verified-supplemental-original'
            else:
                assert original_hash in (frozen[path]['opennt_sha256'], frozen[path]['opennt45_sha256'])
            if proof['status'] == 'full':
                assert prefix == 'system-native-role-' and path == 'softpc.new/base/system/dummy_nt.c'
                assert proof['role'] == 'selected-native-leaf-full' and current_hash == original_hash
                assert proof['estimated_restored_code_percent'] == '100'
            elif proof['status'] == 'excluded':
                legacy_roles = ('historical-guest-build-source-file-only', 'historical-guest-library-build-artifact-only',
                                'historical-guest-build-input-only',
                                'historical-cpu-test-source-only', 'historical-empty-printscreen-source-only',
                                'historical-build-recipe-file-only', 'historical-inline-helper-source-only',
                                'historical-cpu-test-declaration-only', 'historical-guest-build-tool-source-only',
                                'historical-obsolete-guest-source-only', 'historical-source-note-only',
                                'historical-alternative-nt-provider-source-only', 'historical-superseded-source-revision-only',
                                'historical-alternative-cpu-video-interface-source-only',
                                'historical-superseded-nt-error-table-source-only',
                                'historical-alternative-memory-provider-source-only',
                                'historical-superseded-nt-host-source-only',
                                'historical-debug-profile-host-source-only',
                                'historical-alternative-virtual-floppy-source-only',
                                'historical-alternate-host-header-source-only', 'historical-profile-header-source-only', 'historical-relocated-build-artifact-only', 'selected-debug-only-source-file', 'selected-empty-source-file-only', 'historical-empty-declaration-only', 'original-disabled-source-body-only')
                artifact_roles = ('historical-symbol-artifact-only', 'historical-host-build-artifact-only',
                                  'historical-guest-build-object-artifact-only', 'historical-source-index-artifact-only',
                                  'historical-generated-message-artifact-only')
                assert proof['role'] in legacy_roles + artifact_roles and current_hash == original_hash
                if proof['role'] == 'historical-guest-build-input-only':
                    assert (prefix == 'parallel-guest-resource-role-' and path in (
                        'wow16/ole/client/funchead.c', 'wow16/ole/client/mk.cmd',
                        'wow16/ole/client/utils.c', 'wow16/regedit/reg1632.h',
                        'wow16/drivers/display/display.rc', 'wow16/drivers/display/DISPLAY.RES',
                        'wow16/drivers/keyboard/keyboard.rcv')) or (prefix == 'parallel-gdi-ctl3d-role-' and path in (
                        'wow16/gdi/gdi.rcv', 'wow16/ctl3dv2/readme.txt', 'wow16/ctl3dv2/ctl3d.doc',
                        'wow16/ctl3dv2/ctl3d.c', 'wow16/ctl3dv2/ctl3d.h', 'wow16/ctl3dv2/common.ver',
                        'dos/v86/cmd/graphics/GRAPHICS.EXC', 'dos/v86/cmd/graphics/graphics.tag')) or (prefix == 'parallel-ddeml-wfwnet-role-' and path in (
                        'wow16/ddeml/STRIPPED.DEF', 'wow16/ddeml/tests/ddestrs/ddestrs.c',
                        'wow16/ddeml/tests/ddestrs/ddestrs.rc', 'wow16/ddeml/tests/ddestrs/globals.h',
                        'wow16/ddeml/tests/ddestrs/mk4dos.cmd', 'wow16/ddeml/tests/src/client/ddemlcl.h',
                        'wow16/wfwnet/bseerr.h', 'wow16/wfwnet/libentry.asm', 'wow16/wfwnet/wfwnet.c',
                        'wow16/wfwnet/wfwnet.rcv', 'wow16/wfwnet/WFWNET.RES', 'dos/v86/messages/state.rst')) or (prefix == 'parallel-debug-write-role-' and path in (
                        'dos/v86/cmd/debug/sysver.fal', 'dos/v86/cmd/debug/sysver.tru',
                        'wow16/write/kmake.bat', 'wow16/write/state.rst', 'wow16/write/WRITE.RES')) or (prefix == 'parallel-shld-tools-role-' and path in (
                        'dos/v86/tools/src/h2inc/h2inc.c', 'dos/v86/tools/src/stripdd/stripdd.c',
                        'dos/v86/tools/src/stripz/stripz.c')) or (prefix == 'dosx-guest-role-' and path in (
                        'dpmi/486/dxintr.asm', 'dpmi/dxini.asm', 'dpmi/dxmmgr.asm', 'dpmi/dxnetbio.asm',
                        'dpmi/dxoem.asm', 'dpmi/ntnpxem.asm', 'dpmi/usa/dxmsg.asm', 'dpmi/mk.cmd', 'dpmi/migrate.bat')) or (prefix == 'dosx-x86-inputs-role-' and path in ('dpmi/486/dxfunc.asm', 'dpmi/486/dxint31.asm', 'dpmi/486/dxmain.asm', 'dpmi/486/dxstrt.asm', 'dpmi/dxdma.asm', 'dpmi/dxemm.asm', 'dpmi/dxemm2.asm', 'dpmi/dxfind.asm', 'dpmi/dxmsg.asm', 'dpmi/cmacros.inc', 'dpmi/interupt.inc', 'dpmi/pmdefs.inc')) or (prefix == 'dos-kernel-image-role-' and path == 'dos/v86/doskrnl/dos/NTDOS.BIN') or (prefix == 'dos-message-input-role-' and path in ('dos/v86/messages/loadmsg.msg', 'dos/v86/messages/usa-ms.idx'))
                if proof['role'] in artifact_roles:
                    assert prefix == 'generated-message-role-'
                if proof['role'] == 'historical-empty-declaration-only':
                    assert (prefix == 'shard1-cvidc-empty-guard-role-' and path in {'softpc.new/base/cvidc/' + name + '_c.h' for name in ('btest', 'c2cput', 'cpumod', 'lxs', 'mark', 'misc', 'nanoc', 'strsub', 'sub', 'test', 'xchg')}) or (prefix == 'parallel-empty-declaration-role-' and (path == 'inc/APISTRUC.INC' or path.startswith('softpc.new/base/cvidc/'))) or (prefix == 'shard2-empty-declaration-role-' and path in {
                        'softpc.new/base/cvidc/' + name + '_c.h' for name in ('bound', 'c2cpug', 'conv', 'dec', 'gdinit', 'inc', 'light', 'nanoa', 'pm', 'simple', 'stub')}) or (prefix == 'shard3-empty-declaration-role-' and path in {
                        'softpc.new/base/cvidc/' + name + '.h' for name in ('add_c', 'bswap_c', 'cmp_c', 'copies_c', 'fmevid_c', 'gdmain_c', 'gicvid_c', 'idmain_c', 'logic_c', 'move_c', 'nanoe_c', 'unchn_c')})
                if proof['role'] == 'selected-empty-source-file-only':
                    assert prefix == 'shard0-empty-gdi-role-' and path == 'wow32/wgman.c'
                if proof['role'] == 'original-disabled-source-body-only':
                    assert prefix == 'wow-multimedia-disabled-structures-role-' and path in ('wow32/wmmstru1.c', 'wow32/wmmstruc.c')
                if proof['role'] == 'selected-debug-only-source-file':
                    assert prefix == 'dpmi-debug-profile-role-' and path == 'dpmi32/debug.c'
                if proof['role'] == 'historical-guest-library-build-artifact-only':
                    assert prefix == 'guest-library-role-' and path.lower().endswith('.lib')
                if proof['role'] == 'historical-cpu-test-source-only':
                    assert prefix == 'native-source-role-' and path in (
                        'softpc.new/base/ccpu386/c_getset.c', 'softpc.new/base/ccpu386/ccpupig.c')
                if proof['role'] == 'historical-empty-printscreen-source-only':
                    assert prefix == 'native-source-next-role-' and path == 'softpc.new/base/comms/prt_scrn.c'
                if proof['role'] == 'historical-build-recipe-file-only':
                    assert prefix == 'historical-build-role-' or (prefix == 'shard0-ddeml-test-role-' and path in ('wow16/ddeml/tests/src/client/mk4dos.cmd', 'wow16/ddeml/tests/src/ddemo/mk4dos.cmd', 'wow16/ddeml/tests/src/server/mk4dos.cmd'))
                if proof['role'] == 'historical-inline-helper-source-only':
                    assert prefix == 'ccpu-control-role-' and path == 'softpc.new/base/ccpu386/nop.c'
                if proof['role'] == 'historical-cpu-test-declaration-only':
                    assert prefix == 'ccpu-header-macro-role-' and path == 'softpc.new/base/ccpu386/ccpupig.h'
                if proof['role'] == 'historical-guest-build-tool-source-only':
                    assert prefix == 'dos-command-input-role-' and path in ('dos/v86/cmd/command/buildmsg.c', 'dos/v86/cmd/command/usa-ms/bldmsg.h')
                if proof['role'] == 'historical-obsolete-guest-source-only':
                    assert prefix == 'dos-command-input-role-' and path in (
                        'dos/v86/cmd/command/tcmd1a.asm', 'dos/v86/cmd/append/appendm.asm',
                        'dos/v86/cmd/keyb/kbmsg.inc', 'dos/v86/cmd/keyb/keybmsg.inc')
                if proof['role'] == 'historical-source-note-only':
                    assert (prefix == 'dos-command-input-role-' and path == 'dos/v86/cmd/exe2bin/e2bchng.inc') or (prefix == 'parallel-guest-delivery-role-' and path in ('dos/v86/doskrnl/dos/dos.tag', 'dos/v86/inc/oemnum.inc'))
                if proof['role'] == 'historical-alternative-nt-provider-source-only':
                    assert prefix == 'system-native-role-' and path == 'softpc.new/base/system/cmos.c'
                if proof['role'] == 'historical-superseded-source-revision-only':
                    assert prefix == 'system-native-role-' and path == 'softpc.new/base/system/ica.old'
                if proof['role'] == 'historical-alternative-cpu-video-interface-source-only':
                    assert prefix == 'video-native-role-' and path == 'softpc.new/base/video/ga_glue.c'
                if proof['role'] == 'historical-superseded-nt-error-table-source-only':
                    assert prefix == 'input-native-role-' and path in ('softpc.new/host/src/nt_ertbl.c', 'softpc.new/host/src/nt_mess.c')
                if proof['role'] == 'historical-alternative-memory-provider-source-only':
                    assert prefix == 'memory-native-role-' and path in ('softpc.new/host/src/nt_sas.c', 'softpc.new/host/src/nt_sec.c')
                if proof['role'] == 'historical-superseded-nt-host-source-only':
                    assert prefix == 'retired-host-role-' and path in (
                        'softpc.new/host/src/nt_term.c', 'softpc.new/host/src/nt_smenu.c',
                        'softpc.new/host/src/nt_hfx.c', 'softpc.new/host/src/stf_conf.c')
                if proof['role'] == 'historical-debug-profile-host-source-only':
                    assert prefix == 'host-tail-role-' and path in (
                        'softpc.new/host/src/nt_hunt.c', 'softpc.new/host/src/nt_yoda.c')
                if proof['role'] == 'historical-alternative-virtual-floppy-source-only':
                    assert prefix == 'host-tail-role-' and path == 'softpc.new/host/src/nt_vflop.c'
                if proof['role'] == 'historical-alternate-host-header-source-only':
                    assert prefix == 'base-inc-alternate-role-' and path in (
                        'softpc.new/base/inc/host_fio.h', 'softpc.new/base/inc/unix_asy.h',
                        'softpc.new/base/inc/unix_cnf.h', 'softpc.new/base/inc/unix_loc.h',
                        'softpc.new/base/inc/swinhelp.h', 'softpc.new/base/inc/swinmgre.h')
                if proof['role'] == 'historical-profile-header-source-only':
                    assert prefix == 'monitor-profile-headers-role-' and path in (
                        'softpc.new/host/inc/monsim32.h', 'softpc.new/host/inc/nt_hunt.h',
                        'softpc.new/base/inc/egatrace.h', 'softpc.new/base/inc/hg_stat.h')
                if proof['role'] == 'historical-relocated-build-artifact-only':
                    assert prefix == 'retained-tools16-role-' and path.startswith('tools16/')
            else:
                if prefix == 'nine-part1-bin86-image-role-':
                    assert path in ('bin86/ctl3dv2.dll', 'bin86/i386/win87em.dll', 'bin86/lanman.drv', 'bin86/lzexpand.dll', 'bin86/netapi.dll', 'bin86/olecli.dll', 'bin86/pmspl.dll', 'bin86/ver.dll', 'bin86/winhelp.exe') and proof['role'] == 'selected-guest-image-partial' and current_hash == original_hash
                elif prefix == 'guest-user-timer-image-role-':
                    assert path in ('wow16/timer/TIMER.DRV', 'wow16/user/k/daytona/USER.EXE') and proof['role'] == 'selected-guest-image-partial' and current_hash == original_hash
                elif prefix == 'guest-package-image-family-role-':
                    assert path in ('dos/v86/cmd/fastopen/FASTOPEN.EXE', 'wow16/mmsystem/MMSYSTEM.DLL', 'wow16/rasapi16/RASAPI16.DLL', 'wow16/test/shell/WOWEXEC.EXE', 'wow16/winsock/WINSOCK.DLL') and proof['role'] == 'selected-guest-image-partial' and current_hash == original_hash
                elif prefix == 'ole-server-guest-image-role-':
                    assert path == 'wow16/ole/server/retail/OLESVR.DLL' and proof['role'] == 'selected-guest-image-partial' and current_hash == original_hash
                elif prefix == 'win16-guest-image-payload-role-':
                    assert path in ('wow16/bin/winstub.exe', 'wow16/wfwnet/WFWNET.DRV') and proof['role'] == 'selected-guest-image-partial' and current_hash == original_hash
                elif prefix == 'mvdm-inc-header-owner-role-':
                    assert path in {'inc/' + n for n in ('apistruc.h', 'bop.h', 'dbgsvc.h', 'doswow.h', 'exterr.h', 'netb.h', 'sharewow.h', 'vrdlctab.h', 'vrmisc.h', 'vrmslot.h', 'vrnetapi.h', 'wowclip.h', 'wowcmdlg.h', 'wowinfo.h', 'wowth.h')} and proof['role'] == 'selected-native-source-partial' and current_hash == original_hash
                elif prefix == 'shard0-host-headers-owner-role-':
                    assert path in {'softpc.new/host/inc/' + n for n in ('error.h', 'host_gfx.h', 'hostsync.h', 'nt_cgalt.h', 'nt_com.h', 'nt_det.h', 'nt_egalt.h', 'nt_extra.h', 'nt_graph.h', 'nt_mem.h', 'nt_timer.h', 'nt_uis.h', 'ntcheese.h')} and proof['role'] == 'selected-native-source-partial' and current_hash == original_hash
                elif prefix == 'softpc-host-header-owner-role-':
                    assert path in {'softpc.new/host/inc/' + n for n in ('host_rrr.h', 'host_trc.h', 'nt_eoi.h', 'nt_pif.h')} and proof['role'] == 'selected-native-source-partial' and current_hash == original_hash
                elif prefix == 'softpc-header-owner-role-':
                    assert path in {'softpc.new/base/inc/' + n for n in ('dsktrace.h', 'gendrvr.h', 'host_gwi.h', 'host_sas.h', 'hunter.h')} and proof['role'] == 'selected-native-source-partial' and current_hash == original_hash
                elif prefix == 'wow-message-table-image-role-':
                    assert path == 'wow32/wmtbl32.c' and proof['role'] == 'selected-native-source-partial' and current_hash == original_hash
                elif prefix == 'wow-media-resource-image-role-':
                    assert path in ('wow32/wmmedia.c', 'wow32/wres32.c') and proof['role'] == 'selected-native-source-partial' and current_hash == original_hash
                elif prefix == 'wow-core-header-family-role-':
                    assert path in {'wow32/' + n for n in ('isz.h', 'mapembed.h', 'waccel.h', 'wcmdgtbl.h', 'wcntl32.h', 'wdos.h', 'wgtbl2.h', 'wkbman.h', 'wmsgsbm.h', 'wow32.h', 'wstbl.h', 'wstruc.h', 'wthman.h', 'wucaret.h', 'wuclass.h', 'wutext.h', 'wutmr.h')} and proof['role'] == 'selected-native-source-partial' and current_hash == original_hash
                elif prefix == 'wow-user-header-family-role-':
                    assert path in {'wow32/' + n for n in ('wkbdtbl2.h', 'wmsg16.h', 'wmsgbm.h', 'wmsgem.h', 'wparam.h', 'wsdynmc.h', 'wsubcls.h', 'wucursor.h', 'wuhook.h', 'wulang.h', 'wuwind.h')} and proof['role'] == 'selected-native-source-partial' and current_hash == original_hash
                elif prefix == 'wow-shell-hook-role-':
                    assert path in ('wow32/wshell.c', 'wow32/wsubcls.c', 'wow32/wuhook.c') and proof['role'] == 'selected-native-source-partial' and current_hash == original_hash
                elif prefix == 'redir-dlc-command-role-':
                    assert path == 'vdmredir/vrdlc5c.c' and proof['role'] == 'selected-native-source-partial' and current_hash == original_hash
                elif prefix == 'redir-dlc-post-role-':
                    assert path == 'vdmredir/vrdlcpst.c' and proof['role'] == 'selected-native-source-partial' and current_hash == original_hash
                elif prefix == 'selected-interface-callback-role-':
                    assert path in ('vdmredir/dlstruct.h', 'vdmredir/vrica.h', 'vdmredir/vrputil.h', 'vdmredir/vdmredir.rc', 'softpc.new/host/inc/host_inc.h', 'softpc.new/host/inc/nt_ega.h', 'wow32/wcall32.c') and proof['role'] == 'selected-native-source-partial' and current_hash == original_hash
                elif prefix == 'redir-service-owners-role-':
                    assert path in ('vdmredir/vrinit.c', 'vdmredir/vrmisc.c', 'vdmredir/vrmslot.c', 'vdmredir/vrputil.c') and proof['role'] == 'selected-native-source-partial' and current_hash == original_hash
                elif prefix == 'redir-dispatch-release-role-':
                    assert path == 'vdmredir/vrdisp.c' and proof['role'] == 'selected-native-source-partial' and current_hash == original_hash
                elif prefix == 'wow-keyboard-image-role-':
                    assert path == 'wow32/wkbman.c' and proof['role'] == 'selected-native-source-partial' and current_hash == original_hash
                elif prefix == 'cvidc-selected-core-role-':
                    assert path in ('softpc.new/base/cvidc/accessfn.c', 'softpc.new/base/cpu/src/evid/vglob.c', 'softpc.new/base/cvidc/sinit013.c') and proof['role'] == 'selected-native-source-partial' and current_hash == original_hash
                elif prefix == 'xms-runtime-owner-role-':
                    assert path in ('xms.486/xms.c', 'xms.486/xmsa20.c', 'xms.486/xmsmisc.c', 'xms.486/xmsumb.c') and proof['role'] == 'selected-native-source-partial' and current_hash == original_hash
                elif prefix == 'ems-sim32-headers-role-':
                    assert path in ('softpc.new/host/inc/host_emm.h', 'softpc.new/host/inc/sim32.h') and proof['role'] == 'selected-native-source-partial' and current_hash == original_hash
                elif prefix == 'monitor-profile-headers-role-':
                    assert path in ('softpc.new/host/inc/host_bop.h', 'softpc.new/host/inc/nt_reset.h', 'softpc.new/host/inc/nt_thred.h') and proof['role'] == 'selected-native-source-partial' and current_hash == original_hash
                elif prefix == 'directory-interrupt-headers-role-':
                    assert path in ('inc/curdir.h', 'dpmi32/precomp.h', 'inc/intapi.h', 'softpc.new/base/keymouse/mouse.c', 'softpc.new/base/keymouse/ppi.c') and proof['role'] == 'selected-native-source-partial' and current_hash == original_hash
                elif prefix == 'selected-command-input-headers-role-':
                    assert path in ('inc/cmdsvc.h', 'dos/dem/dosdef.h', 'softpc.new/host/inc/nt_cga.h', 'softpc.new/host/inc/nt_mouse.h') and proof['role'] == 'selected-native-source-partial' and current_hash == original_hash
                elif prefix == 'xms-suballoc-image-role-':
                    assert path in ('xms.486/xmsdisp.c', 'suballoc/suballoc.c') and proof['role'] == 'selected-native-source-partial' and current_hash == original_hash
                elif prefix == 'guest-service-header-role-':
                    assert path in ('inc/dossvc.h', 'inc/dpmi.h', 'inc/xmssvc.h') and proof['role'] == 'selected-native-source-partial' and current_hash == original_hash
                elif prefix == 'wow-shared-thunk-header-role-':
                    assert path in ('inc/wowcomm.h', 'inc/wowgdi.h', 'inc/wowkbd.h', 'wow32/wkgthunk.h', 'wow32/wshtbl2.h', 'wow32/wsocktbl.h') and proof['role'] == 'selected-native-source-partial' and current_hash == original_hash
                elif prefix == 'wow-cross-language-header-role-':
                    assert path in ('inc/wowcmpat.h', 'inc/wowusr.h') and proof['role'] == 'selected-native-source-partial' and current_hash == original_hash
                elif prefix == 'wow-media-dispatch-header-role-':
                    assert path in ('wow32/wmmedia.h', 'wow32/wmmtbl2.h', 'wow32/wmtbl32.h', 'wow32/wowtbl.h') and proof['role'] == 'selected-native-source-partial' and current_hash == original_hash
                elif prefix == 'wow-user-header-role-':
                    assert path in ('wow32/wuclip.h', 'wow32/wucomm.h', 'wow32/wudlg.h', 'wow32/wuman.h', 'wow32/wuser31.h') and proof['role'] == 'selected-native-source-partial' and current_hash == original_hash
                elif prefix == 'dos-kernel-image-role-':
                    assert path in ('dos/v86/doskrnl/bios/NTIO.SYS', 'dos/v86/doskrnl/dos/NTDOS.SYS') and proof['role'] == 'selected-guest-image-partial' and current_hash == original_hash
                elif prefix == 'dosx-guest-role-':
                    assert path == 'dpmi/486/dosx.exe' and proof['role'] == 'selected-guest-image-partial' and current_hash == original_hash
                elif prefix == 'shard0-dpmi-buffer-state-role-':
                    assert path in ('dpmi32/buffer.c', 'dpmi32/savestat.c') and proof['role'] == 'selected-native-source-partial' and current_hash == original_hash
                elif prefix == 'parallel-shld-tools-role-':
                    assert ((path == 'softpc.new/base/ccpu386/shld.c' and proof['role'] == 'selected-native-source-partial') or
                            (path == 'dos/v86/cmd/graphics/GRAPHICS.COM' and proof['role'] == 'selected-guest-image-partial')) and current_hash == original_hash
                elif prefix in ('parallel-guest-delivery-role-', 'parallel-gdi-ctl3d-role-'):
                    assert proof['role'] in ('selected-guest-image-partial', 'selected-guest-data-partial') and current_hash == original_hash
                elif prefix == 'parallel-embedding-sound-role-':
                    assert proof['role'] in ('selected-native-source-partial', 'selected-guest-image-partial') and current_hash == original_hash
                elif prefix in ('native-source-role-', 'native-source-next-role-', 'selected-ccpu-header-role-', 'ccpu-foundation-role-', 'ccpu-control-role-', 'ccpu-header-macro-role-', 'bios-native-role-', 'system-native-role-', 'disk-native-role-', 'video-native-role-', 'input-native-role-', 'planar-native-role-', 'memory-native-role-', 'vdd-native-role-', 'host-tail-role-', 'base-inc-role-', 'base-inc-multi-role-', 'base-inc-wide-role-', 'cvidc-generated-role-', 'cvidc-vector-role-', 'parallel-native-role-', 'parallel-cvidc-role-', 'parallel-wow-gdi-role-', 'parallel-cross-role-', 'parallel-dpmi-role-', 'parallel-text-dem-role-', 'parallel-resource-redir-role-', 'parallel-heap-video-role-', 'parallel-controls-directory-role-', 'parallel-user-sound-role-', 'parallel-user-interaction-role-', 'shard0-cvidc-declaration-role-', 'shard0-wow-palette-role-', 'parallel-wow-memory-spool-role-', 'parallel-wow-gdi-table-role-', 'shard0-winsock-raw-role-', 'shard0-bit-rotate-role-'):
                    assert proof['role'] == 'selected-native-source-partial' and current_hash == original_hash
                else:
                    assert proof['role'] in ('active-export-declaration-mirror-diff', 'partially-integrated-export-template')
                    assert (current_hash != original_hash) == (proof['role'] == 'active-export-declaration-mirror-diff')
            assert proof['original_edge'] and proof['limits']
    counts = dict(collections.Counter(r['status'] for r in merged.values()))
    expected_counts = collections.Counter(r['status'] for r in old.values())
    expected_counts['pending'] -= expected_count
    expected_counts.update(r['status'] for r in dispositions)
    assert counts == expected_counts
    summary = json.loads((A / 'initial-summary.json').read_text())
    assert counts == summary['counts'] and summary['complete'] is False
    specs = [('shard-0/review.csv', 'classification'), ('shard-1/file-audit.csv', 'audit_status'),
             ('shard-2/file-audit.csv', 'decision'), ('shard-3/file-review.csv', 'review_class')]
    translations = {'待核验': 'pending', '部分复通': 'partial', '整体排除': 'excluded',
                    '全复通': 'full', '全部复通': 'full'}
    covered = set()
    for shard, (name, column) in enumerate(specs):
        source_path = A / name
        previous = {r['relative_path']: r for r in original_rows(source_path, base)}
        for row in rows(source_path):
            path = row['relative_path']
            assert path not in covered and int(merged[path]['shard']) == shard
            covered.add(path)
            assert translations.get(row[column], row[column]) == merged[path]['status']
            if path not in changed:
                assert row == previous[path], path
    assert covered == set(merged)
    observation = json.loads((A / (prefix + 'observations.json')).read_text())
    assert observation['result'] == 'pass' and observation['candidate_count'] == expected_count
    for record in observation['source_tables']:
        assert sha(A / record['path']) == record['sha256']
    if prefix == 'parallel-empty-declaration-role-':
        guards = ('chain2_c.h', 'enter_c.h', 'nanob_c.h', 'nanod_c.h', 'pti_c.h', 'read_c.h', 'rotate_c.h', 'shift_c.h', 'string_c.h', 'xlat_c.h')
        assert set(changed) == {'inc/APISTRUC.INC'} | {'softpc.new/base/cvidc/' + n for n in guards}
        assert all(changed[p]['status'] == 'excluded' and changed[p]['role'] == 'historical-empty-declaration-only' for p in changed)
    if prefix == 'shard0-ddeml-test-role-':
        assert len(changed) == 14 and all(p.startswith('wow16/ddeml/tests/') and changed[p]['status'] == 'excluded' for p in changed)
    if prefix == 'shard0-winsock-raw-role-':
        assert set(changed) == {'wow32/wsraw.c'} and changed['wow32/wsraw.c']['status'] == 'partial'
        assert changed['wow32/wsraw.c']['original_text_retained_percent'] == '100' and not changed['wow32/wsraw.c']['estimated_restored_code_percent']
    if prefix == 'shard0-empty-gdi-role-':
        assert set(changed) == {'wow32/wgman.c'} and changed['wow32/wgman.c']['status'] == 'excluded'
    if prefix == 'parallel-wow-gdi-table-role-':
        assert set(changed) == {'wow32/wgdi.c', 'wow32/wowtbl.c'}
        assert all(changed[p]['status'] == 'partial' and changed[p]['original_text_retained_percent'] == '100' and not changed[p]['estimated_restored_code_percent'] for p in changed)
    if prefix == 'parallel-embedding-sound-role-':
        assert set(changed) == {'wow32/mapembed.c', 'wow16/drivers/sound/sound.drv'}
        assert all(changed[p]['status'] == 'partial' and changed[p]['original_text_retained_percent'] == '100' and not changed[p]['estimated_restored_code_percent'] for p in changed)
    if prefix == 'parallel-wow-memory-spool-role-':
        assert set(changed) == {'wow32/walloc16.c', 'wow32/wkmem.c', 'wow32/wspool.c'}
        assert all(changed[p]['status'] == 'partial' and changed[p]['original_text_retained_percent'] == '100' and not changed[p]['estimated_restored_code_percent'] for p in changed)
    if prefix == 'shard0-wow-palette-role-':
        assert set(changed) == {'wow32/wgpal.c'} and changed['wow32/wgpal.c']['status'] == 'partial'
        assert changed['wow32/wgpal.c']['original_text_retained_percent'] == '100' and not changed['wow32/wgpal.c']['estimated_restored_code_percent']
    if prefix == 'shard0-cvidc-declaration-role-':
        assert set(changed) == {'softpc.new/base/cvidc/cpu4gen.h', 'softpc.new/base/cvidc/evid_c.h', 'softpc.new/base/cvidc/gdpvar.h', 'softpc.new/base/cvidc/univer_c.h'}
        assert all(changed[p]['status'] == 'partial' and changed[p]['original_text_retained_percent'] == '100' and not changed[p]['estimated_restored_code_percent'] for p in changed)
    if prefix == 'parallel-guest-delivery-role-':
        assert set(changed) == {'dos/v86/doskrnl/bios/msbio.skl', 'dos/v86/doskrnl/dos/msdos.skl', 'dos/v86/doskrnl/dos/dos.tag', 'dos/v86/dev/country/nls863.inc', 'dos/v86/inc/oemnum.inc', 'dos/v86/cmd/loadfix/LOADFIX.COM', 'dos/v86/cmd/mem/mem.exe', 'dos/v86/cmd/qbasic/edit.com', 'dos/v86/cmd/qbasic/edit.hlp', 'wow16/drivers/comm/comm.drv'}
        assert all(changed[p]['original_text_retained_percent'] == '100' and not changed[p]['estimated_restored_code_percent'] for p in changed)
    if prefix == 'parallel-user-interaction-role-':
        assert set(changed) == {'wow32/waccel.c', 'wow32/wmsgbm.c', 'wow32/wulang.c'}
        assert all(changed[p]['status'] == 'partial' and changed[p]['original_text_retained_percent'] == '100'
                   and not changed[p]['estimated_restored_code_percent'] for p in changed)
    if prefix == 'parallel-user-sound-role-':
        assert set(changed) == {'wow32/wsman.c', 'wow32/wstbl2.h', 'wow32/wusercli.c', 'wow32/wusercli.h'}
        assert all(changed[p]['status'] == 'partial' and changed[p]['original_text_retained_percent'] == '100'
                   and not changed[p]['estimated_restored_code_percent'] for p in changed)
    if prefix == 'parallel-controls-directory-role-':
        assert set(changed) == {'wow32/wmsgsbm.c', 'wow32/wucaret.c', 'wow32/wutbl2.h',
                                'dos/dem/demdir.c', 'dos/dem/demlabel.c'}
        assert all(changed[p]['status'] == 'partial' and changed[p]['original_text_retained_percent'] == '100'
                   and not changed[p]['estimated_restored_code_percent'] for p in changed)
    if prefix == 'parallel-heap-video-role-':
        assert set(changed) == {'wow32/wheap.c', 'wow32/wheap.h', 'softpc.new/base/cvidc/sinit011.c'}
        assert all(changed[p]['status'] == 'partial' and changed[p]['original_text_retained_percent'] == '100'
                   and not changed[p]['estimated_restored_code_percent'] for p in changed)
    if prefix == 'parallel-resource-redir-role-':
        assert set(changed) == {'wow32/wcuricon.c', 'wow32/wucursor.c', 'wow32/wres16.c', 'wow32/wres16.h',
            'vdmredir/vrdll.c', 'vdmredir/vrnetb.c', 'vdmredir/vrdlcbuf.c'}
        assert all(changed[p]['status'] == 'partial' and changed[p]['original_text_retained_percent'] == '100'
                   and not changed[p]['estimated_restored_code_percent'] for p in changed)
    if prefix == 'parallel-text-dem-role-':
        assert set(changed) == {'wow32/wutext.c', 'dos/dem/demerror.c'}
        assert all(changed[p]['status'] == 'partial' and changed[p]['original_text_retained_percent'] == '100'
                   and not changed[p]['estimated_restored_code_percent'] for p in changed)
    if prefix == 'parallel-dpmi-role-':
        observed = json.loads((A / 'shard-3/dpmi-register-stack-image-observations.json').read_text())
        assert {s['relative_path'] for s in observed['sources']} == set(changed)
        assert all(changed[p]['status'] == 'partial' and changed[p]['original_text_retained_percent'] == '100'
                   and not changed[p]['estimated_restored_code_percent'] for p in changed)
    if prefix == 'parallel-cross-role-':
        assert set(changed) == {'wow32/wumenu.c', 'wow32/wucomm.c', 'softpc.new/base/support/time_day.c'}
        menu = json.loads((A / 'shard-2/parallel-review-fourth-evidence.json').read_text())
        clock = json.loads((A / 'shard-3/support-clock-env-observations.json').read_text())
        assert sum(len(o['entries']) for o in menu['owners']) == 37
        assert len(next(s for s in clock['sources'] if s['relative_path'].endswith('time_day.c'))['functions']) == 3
        assert all(changed[p]['status'] == 'partial' and changed[p]['original_text_retained_percent'] == '100'
                   and not changed[p]['estimated_restored_code_percent'] for p in changed)
    if prefix == 'parallel-wow-gdi-role-':
        observed = json.loads((A / 'shard-2/parallel-review-third-evidence.json').read_text())
        assert {'wow32/' + s['name'] for s in observed['sources'] if s['name'] in ('wgfont.c', 'wgtext.c', 'wgmeta.c')} == set(changed)
        assert sum(len(o['entries']) for o in observed['owners']) == 28
        assert all(changed[p]['status'] == 'partial' and changed[p]['original_text_retained_percent'] == '100'
                   and not changed[p]['estimated_restored_code_percent'] for p in changed)
    if prefix == 'parallel-cvidc-role-':
        assert len(changed) == 6
        assert set(changed) == {'softpc.new/base/cvidc/' + n for n in
            ('cpu_c.h', 'pigger_c.h', 'cpuint_c.h', 'fm_c.h', 'fpu_c.h', 'vglfunc.c')}
        assert all(changed[p]['status'] == 'partial' and changed[p]['original_text_retained_percent'] == '100'
                   and not changed[p]['estimated_restored_code_percent'] for p in changed)
    if prefix == 'parallel-native-role-':
        native = json.loads((A / 'shard-2/parallel-review-evidence.json').read_text())
        sinit = json.loads((A / 'shard-3/sinit012-image-observations.json').read_text())
        assert len(native['native_bindings']) == 4 and len(sinit['functions']) == 33
        assert set(changed) == set(native['native_bindings']) | {sinit['relative_path']}
        assert all(changed[p]['status'] == 'partial' and changed[p]['original_text_retained_percent'] == '100'
                   and not changed[p]['estimated_restored_code_percent'] for p in changed)
    if prefix == 'cvidc-vector-role-':
        observed = json.loads((A / 'cvidc-vector-image-observations.json').read_text())
        assert observed['result'] == 'pass' and observed['relative_path'] in changed
        assert len(observed['tables']) == 14 and sum(len(t['entries']) for t in observed['tables']) == 692
        proof = changed[observed['relative_path']]
        assert observed['sha256'] == proof['sha256'] and proof['status'] == 'partial'
        assert proof['original_text_retained_percent'] == '100' and not proof['estimated_restored_code_percent']
    if prefix == 'cvidc-generated-role-':
        observed = json.loads((A / 'cvidc-generated-image-observations.json').read_text())
        assert observed['result'] == 'pass' and len(observed['sources']) == 17
        assert {r['relative_path'] for r in observed['sources']} == set(changed)
        for source in observed['sources']:
            assert source['sha256'] == changed[source['relative_path']]['sha256']
            assert len(source['functions']) == 33 and source['table_names']
            assert changed[source['relative_path']]['status'] == 'partial'
            assert changed[source['relative_path']]['original_text_retained_percent'] == '100'
            assert not changed[source['relative_path']]['estimated_restored_code_percent']
    if prefix == 'base-inc-alternate-role-':
        observed = json.loads((A / 'base-inc-alternate-observations.json').read_text())
        assert observed['result'] == 'pass' and len(observed['sources']) == 6
        assert observed['dependency_blocks'] > 0 and observed['source_include_matches'] == 0
        assert len(observed['providers']) == 6 and {r['relative_path'] for r in observed['sources']} == set(changed)
        for source in observed['sources']:
            assert source['sha256'] == changed[source['relative_path']]['sha256']
        for proof in changed.values():
            assert proof['status'] == 'excluded' and proof['original_text_retained_percent'] == '100'
            assert not proof['estimated_restored_code_percent']
            assert proof['restored_estimate_status'].startswith('not-applicable:')
    if prefix in ('base-inc-role-', 'base-inc-multi-role-', 'base-inc-wide-role-'):
        name, cohort, selected, remaining, units = {
            'base-inc-role-': ('base-inc-selection-observations.json', 146, 5, 141, 8),
            'base-inc-multi-role-': ('base-inc-multi-observations.json', 141, 30, 111, 13),
            'base-inc-wide-role-': ('base-inc-wide-observations.json', 111, 19, 92, 14),
        }[prefix]
        observed = json.loads((A / name).read_text())
        assert observed['result'] == 'pass' and observed['pending_cohort'] == cohort
        assert observed['selected_headers'] == selected and observed['nonselected_headers_still_pending'] == remaining
        assert len(observed['units']) == units and {r['relative_path'] for r in observed['sources']} == set(changed)
        for source in observed['sources']:
            assert source['sha256'] == changed[source['relative_path']]['sha256']
            assert source['selections'] and all(s['seen_in_line_markers'] and s['selected_nonblank_lines']
                                                for s in source['selections'])
        for proof in changed.values():
            assert proof['status'] == 'partial' and proof['original_text_retained_percent'] == '100'
            assert not proof['estimated_restored_code_percent']
            assert proof['restored_estimate_status'].startswith('unmeasurable:')
    if prefix == 'host-tail-role-':
        observed = json.loads((A / 'host-tail-image-observations.json').read_text())
        assert len(observed['sources']) == 7 and len(observed['functions']) == 18
        assert len(observed['registration_calls']) == 2
        assert len(observed['disk_providers']) == 3 and len(observed['wow_comm_functions']) == 2
        assert {r['relative_path'] for r in observed['sources']} == set(changed)
        assert sum(r['status'] == 'partial' for r in changed.values()) == 4
        assert sum(r['status'] == 'excluded' for r in changed.values()) == 3
        for proof in changed.values():
            assert proof['original_text_retained_percent'] == '100'
            assert not proof['estimated_restored_code_percent']
            assert proof['restored_estimate_status'].startswith(
                'unmeasurable:' if proof['status'] == 'partial' else 'not-applicable:')
    if prefix == 'retired-host-role-':
        observed = json.loads((A / 'retired-host-image-observations.json').read_text())
        assert runpy.run_path(str(A / 'retired-host-image-observer.py'))['inspect']() == observed
        assert len(observed['sources']) == 4 and len(observed['providers']) == 15
        assert len(observed['dem_slots']) == 73 and len(observed['cleanup_sequences']) == 3
        assert {r['relative_path'] for r in observed['sources']} == set(changed)
        assert all(r['status'] == 'excluded' for r in changed.values())
        negative = Path('build/t473-s10/retired-host-image-final')
        completion = json.loads((negative / 'negative.completion.json').read_text(encoding='utf-8-sig'))
        assert completion['exit'] == completion['expected_exit'] == 1
        assert 'DEM_SLOT_EXPECTATION_MISMATCH' in (negative / 'negative.stderr.txt').read_text()
    if prefix == 'vdd-native-role-':
        observed = json.loads((A / 'vdd-image-observations.json').read_text())
        assert runpy.run_path(str(A / 'vdd-image-observer.py'))['inspect']() == observed
        assert len(observed['sources']) == 2 and len(observed['functions']) == 9
        root = A / 'vdd-object-runs'
        receipt = json.loads((root / 'result.json').read_text(encoding='utf-8-sig'))
        assert receipt['result'] == 'pass' and receipt['contract'] == 'vdd'
        assert receipt['checks'] == 305 and receipt['callbacks'] == 82
        assert [r['exit'] for r in receipt['runs']] == [0, 2, 0]
        for row in receipt['inputs']:
            assert sha(Path(row['path'])) == row['sha256']
        assert sha(Path(receipt['fixture_path'])) == receipt['fixture_sha256']
        for mode, marker in (
                ('deinstall-contract', 'checks=57 failures=14 callbacks=21'),
                ('dma-boundary', 'checks=8 failures=7 callbacks=0')):
            completion = json.loads((root / (mode + '.completion.json')).read_text(encoding='utf-8-sig'))
            assert completion['exit'] == 1
            assert marker in (root / (mode + '.stdout.txt')).read_text()
        assert (root / 'deinstall-contract.stdout.txt').read_text().count('read_calls=1 write_calls=1 owner_released=1') == 7
        assert 'operation=set result=1 accesses=1 guard_changed=1' in (root / 'dma-boundary.stdout.txt').read_text()
        for row in json.loads((root / 'manifest.json').read_text(encoding='utf-8-sig')):
            path = root / row['path']
            assert path.stat().st_size == row['bytes'] and sha(path) == row['sha256']
    if prefix == 'memory-native-role-':
        observed = json.loads((A / 'memory-image-observations.json').read_text())
        assert runpy.run_path(str(A / 'memory-image-observer.py'))['inspect']() == observed
        assert len(observed['sources']) == 3 and sum(len(r['functions']) for r in observed['sources']) == 7
        assert len(observed['sas_providers']) == 3 and len(observed['public_callers']) == 2
        assert {r['relative_path'] for r in observed['sources']} == set(changed)
        assert sorted(r['status'] for r in changed.values()) == ['excluded', 'excluded', 'partial']
        root = A / 'memory-object-runs'
        for contract, count in (('umb', 4240), ('ios', 70)):
            receipt = json.loads((root / contract / 'result.json').read_text(encoding='utf-8-sig'))
            assert receipt['result'] == 'pass' and receipt['contract'] == contract and receipt['checks'] == count
            assert [r['exit'] for r in receipt['runs']] == [0, 2, 0]
            for row in receipt['inputs']:
                assert sha(Path(row['path'])) == row['sha256']
            assert sha(Path(receipt['fixture_path'])) == receipt['fixture_sha256']
        boundary = json.loads((root / 'umb/boundary.completion.json').read_text(encoding='utf-8-sig'))
        assert boundary['exit'] == boundary['expected_exit'] == 1
        text = (root / 'umb/boundary.stdout.txt').read_text()
        assert text.count('accepted=1 calls=1') == 6
        assert 'checks=12 failures=12 callbacks=6 notifies=0 boundary=1' in text
        for row in json.loads((root / 'manifest.json').read_text(encoding='utf-8-sig')):
            assert sha(root / row['path']) == row['sha256']
    if prefix == 'planar-native-role-':
        observed = json.loads((A / 'planar-image-observations.json').read_text())
        assert runpy.run_path(str(A / 'planar-image-observer.py'))['inspect']() == observed
        assert len(observed['sources']) == 3 and sum(len(r['functions']) for r in observed['sources']) == 40
        assert len(observed['dispatch_vectors']) == 3 and sum(len(r['slots']) for r in observed['dispatch_vectors']) == 33
        assert len(observed['munge_calls']) == 6
        assert {r['relative_path'] for r in observed['sources']} == set(changed)
        assert all(r['status'] == 'partial' for r in changed.values())
        root = A / 'planar-pixel-runs'
        receipt = json.loads((root / 'result.json').read_text(encoding='utf-8-sig'))
        assert receipt['result'] == 'pass' and receipt['checks'] == 182801
        assert receipt['callbacks'] == receipt['cases'] == 26114
        normal = json.loads((root / 'normal.completion.json').read_text(encoding='utf-8-sig'))
        negative = json.loads((root / 'negative.completion.json').read_text(encoding='utf-8-sig'))
        assert normal['exit'] == 0 and negative['exit'] == 1
        assert 'checks=9 failures=2 callbacks=1 cases=1 wrong_oracle=1' in (root / 'negative.stdout.txt').read_text()
        for row in rows(root / 'inputs.csv'):
            assert sha(Path(row['Path'])) == row['Hash']
        for row in json.loads((root / 'manifest.json').read_text(encoding='utf-8-sig')):
            assert sha(root / row['path']) == row['sha256']
        for proof in changed.values():
            assert proof['original_text_retained_percent'] == '100'
            assert proof['restored_estimate_status'].startswith('unmeasurable:')
    if prefix == 'input-native-role-':
        observed = json.loads((A / 'input-image-observations.json').read_text())
        assert runpy.run_path(str(A / 'input-image-observer.py'))['inspect']() == observed
        assert len(observed['sources']) == 5 and sum(len(r['functions']) for r in observed['sources']) == 6
        assert len(observed['keyboard_slots']) == 6 and [r['bytes'] for r in observed['key_arrays']] == [133, 84]
        assert {r['relative_path'] for r in observed['sources']} == set(changed)
        assert sum(r['status'] == 'partial' for r in changed.values()) == 3
        root = Path('build/t473-s10/input-object-contracts')
        for contract, count in (('keycode', 268322), ('nls', 3645), ('ios', 70)):
            receipt = json.loads((root / contract / 'result.json').read_text(encoding='utf-8-sig'))
            assert receipt['result'] == 'pass' and receipt['contract'] == contract and receipt['checks'] == count
            assert [r['exit'] for r in receipt['runs']] == [0, 2, 0]
            for r in receipt['inputs']:
                assert sha(Path(r['path'])) == r['sha256']
            assert sha(Path(receipt['fixture_path'])) == receipt['fixture_sha256']
        done = json.loads((root / 'boundary.completion.json').read_text(encoding='utf-8-sig'))
        text = (root / 'boundary.stdout.txt').read_text(encoding='utf-8-sig')
        assert done['exit'] == done['expected_exit'] == 1
        assert 'KEYCODE_BOUNDARY expected_rejected=2 actual_rejected=0' in text
        assert text.count('sentinel=123 observed=123 restored=0') == 2
        assert 'enhanced=0 scan=133 before=0' in text and 'enhanced=1 scan=84 before=0' in text
    if prefix == 'video-native-role-':
        observed = json.loads((A / 'video-image-observations.json').read_text())
        assert runpy.run_path(str(A / 'video-image-observer.py'))['inspect']() == observed
        assert len(observed['sources']) == 7 and sum(len(r['functions']) for r in observed['sources']) == 21
        assert len(observed['generic_slots']) == len(observed['ega_slots']) == 29
        assert len(observed['cpu40_reads']) == 3 and len(observed['cpu40_writes']) == 6
        assert {r['relative_path'] for r in observed['sources'] if r['disposition'] != 'pending-diagnostic-profile'} == set(changed)
        assert sum(r['status'] == 'partial' for r in changed.values()) == 5
        assert changed['softpc.new/base/video/ga_glue.c']['status'] == 'excluded'
        assert merged['softpc.new/base/video/ega_trcr.c']['status'] == 'pending'
        for case in ('baseline', 'baseline-negative', 'candidate', 'candidate-negative'):
            root = Path('build/t473-s10/video-invalid-guest')
            completion = json.loads((root / (case + '.completion.json')).read_text(encoding='utf-8-sig'))
            report = (root / case / 'observer.txt').read_text(encoding='utf-8-sig')
            screen = (root / case / 'observer.txt.console.txt').read_text(encoding='utf-8-sig')
            assert completion['exit'] == 0 and 'result=exited' in report
            if case.endswith('-negative'):
                assert 'exit=0x00000001' in report and 'VIDEO INVALID FAIL phase=V' in screen and 'VIDEO INVALID PASS' not in screen
            else:
                assert 'exit=0x00000000' in report and 'VIDEO INVALID PASS cases=1812' in screen and 'GENERIC908 EGA904' in screen
        root = Path('build/t473-s10/video-dispatch-control')
        assert sha(root / 'baseline/EMSPROBE.COM') == 'F6E667ABFE8FE83EE9CA31B31C66D7460D662B644C4153724000172D17CAFEE3'
        for case in ('baseline', 'baseline-negative'):
            done = json.loads((root / (case + '.completion.json')).read_text(encoding='utf-8-sig'))
            assert done['exit'] == 0
            screen = (root / case / 'observer.txt.console.txt').read_text(encoding='utf-8-sig')
            assert ('BIOS DISPATCH FAIL phase=C' if case.endswith('-negative') else 'BIOS DISPATCH PASS cases=836') in screen
    if prefix == 'disk-native-role-':
        observed = json.loads((A / 'disk-image-observations.json').read_text())
        assert runpy.run_path(str(A / 'disk-image-observer.py'))['inspect']() == observed
        assert len(observed['sources']) == 10
        selected = {r['relative_path'] for r in observed['sources'] if r['functions']}
        assert selected == set(changed) and all(r['status'] == 'partial' for r in changed.values())
        assert sum(len(r['functions']) for r in observed['sources']) == 102
        assert len(observed['disk_slots']) == 22 and len(observed['floppy_slots']) == 25
        assert all(merged[r['relative_path']]['status'] == 'pending' for r in observed['sources'] if not r['functions'])
        for contract, root, checks in (
                ('gfi', Path('build/t473-s10/gfi-object-contract'), 433),
                ('ios', Path('build/t473-s10/disk-owner-review/ios'), 70),
                ('qevnt', Path('build/t473-s10/disk-owner-review/qevnt'), 37)):
            receipt = json.loads((root / 'result.json').read_text(encoding='utf-8-sig'))
            assert receipt['result'] == 'pass' and receipt['contract'] == contract and receipt['checks'] == checks
            for record in receipt['inputs']:
                assert sha(Path(record['path'])) == record['sha256']
            assert sha(Path(receipt['fixture_path'])) == receipt['fixture_sha256']
            assert [r['exit'] for r in receipt['runs']] == [0, 2, 0]
        for case in ('baseline', 'baseline-negative', 'candidate', 'candidate-negative'):
            root = Path('build/t473-s10/disk-guest')
            completion = json.loads((root / (case + '.completion.json')).read_text(encoding='utf-8-sig'))
            report = (root / case / 'observer.txt').read_text(encoding='utf-8-sig')
            screen = (root / case / 'observer.txt.console.txt').read_text(encoding='utf-8-sig')
            assert completion['exit'] == 0 and 'result=exited' in report
            if case.endswith('-negative'):
                assert 'exit=0x00000001' in report and 'S28_INT13_RESET_FAIL' in screen and 'S28_DISKS_OK' not in screen
            else:
                assert 'exit=0x00000000' in report and 'S28_INT13_RESET_OK S28_DISKS_OK' in screen
    if prefix == 'system-native-role-':
        observed = json.loads((A / 'system-image-observations.json').read_text())
        assert runpy.run_path(str(A / 'system-image-observer.py'))['inspect']() == observed
        assert len(observed['sources']) == 6 and len(observed['alternatives']) == 2
        assert sum(len(r['functions']) for r in observed['sources']) == 77
        assert {r['relative_path'] for r in observed['sources'] + observed['alternatives']} == set(changed)
        assert observed['dummy_leaf_code'] == 'C20000' and observed['dummy_bios_slot'] == 1
        leaf = 'softpc.new/base/system/dummy_nt.c'
        assert changed[leaf]['status'] == 'full'
        assert all(changed[r['relative_path']]['status'] == 'partial' for r in observed['sources'] if r['relative_path'] != leaf)
        assert all(changed[r['relative_path']]['status'] == 'excluded' for r in observed['alternatives'])
        previous = original_rows(A / 'full-selected-leaf.csv', base)
        current = rows(A / 'full-selected-leaf.csv')
        assert len(previous) == 26 and len(current) == 27 and current[:-1] == previous
        assert current[-1]['relative_path'] == leaf and current[-1]['estimated_restored_code_pct'] == '100'
        assert {r['relative_path'] for r in current + rows(A / 'full-selected-declarations.csv')} == {p for p, r in merged.items() if r['status'] == 'full'}
        root = Path('build/t473-s10/system-integration-r5')
        for case in ('dummy-baseline', 'dummy-baseline-negative', 'dummy-candidate', 'dummy-candidate-negative'):
            completion = json.loads((root / (case + '.completion.json')).read_text(encoding='utf-8-sig'))
            report = (root / case / 'observer.txt').read_text(encoding='utf-8-sig')
            screen = (root / case / 'observer.txt.console.txt').read_text(encoding='utf-8-sig')
            assert completion['exit'] == 0 and 'result=exited' in report
            if case.endswith('-negative'):
                assert 'exit=0x00000001' in report and 'DUMMY INT FAIL phase=D' in screen and 'DUMMY INT PASS' not in screen
            else:
                assert 'exit=0x00000000' in report and 'DUMMY INT PASS cases=16' in screen
    if prefix == 'bios-native-role-':
        observed = json.loads((A / 'bios-image-observations.json').read_text())
        inspector = runpy.run_path(str(A / 'bios-image-observer.py'))['inspect']
        assert inspector() == observed and observed['result'] == 'pass'
        assert {r['relative_path'] for r in observed['sources']} == set(changed)
        assert sum(len(r['functions']) for r in observed['sources']) == 52
        assert len(observed['bios_slots']) == 256 and len(observed['ems_slots']) == 30
        assert all(r['status'] == 'partial' and r['original_text_retained_percent'] == '100' for r in dispositions)
        negative = json.loads((A / 'bios-image-negative-result.json').read_text())
        bad_image = Path('build/t473-s10/bios-image-negative/changed-dispatch.exe')
        assert sha(bad_image) == negative['changed_image_sha256'] and negative['executed'] is False
        try:
            inspector(bad_image)
        except AssertionError as error:
            assert error.args == ((255, '_control_bop'),)
        else:
            raise AssertionError('Corrupt BIOS dispatch pointer accepted')
    if prefix == 'dos-command-input-role-':
        observed = json.loads((A / 'dos-command-input-observations.json').read_text())
        actual = runpy.run_path(str(A / 'dos-command-input-observer.py'))['observe']()
        assert observed == actual and observed['reviewed_count'] == 85
        selected = {r['relative_path'] for r in observed['records'] if r['result'] != 'unresolved'}
        assert selected == set(changed) and len(selected) == 85
        assert observed['source_chain_count'] == 78 and observed['resolved_role_count'] == 85
        assert all(proof['status'] == 'excluded' and proof['original_text_retained_percent'] == '100' for proof in dispositions)
        for record in observed['records']:
            if record['relative_path'] not in selected:
                assert merged[record['relative_path']]['status'] == 'pending'
    if prefix == 'ccpu-header-macro-role-':
        observed = json.loads((A / 'ccpu-header-selection-observations.json').read_text())
        actual = runpy.run_path(str(A / 'ccpu-header-selection-extractor.py'))['inspect']()
        assert observed == actual and observed['source_header_count'] == 22
        assert observed['selected_declaration_headers'] == 7
        selected = {r['relative_path'] for r in observed['headers'] if r['emitted_declaration_units']}
        selected.update('softpc.new/base/ccpu386/' + n for n in ('c_oprnd.h', 'fault.h', 'ccpupig.h'))
        assert selected == set(changed) and len(selected) == 10
        for record in observed['headers']:
            name = record['relative_path']
            if name in changed:
                assert record['valid_consumers'] and record['seen_units']
                assert changed[name]['status'] == ('excluded' if name.endswith('/ccpupig.h') else 'partial')
            else:
                assert merged[name]['status'] == 'pending'
        for proof in dispositions:
            assert proof['original_text_retained_percent'] == '100' and not proof['estimated_restored_code_percent']
        for record in observation['normal_cache_inputs']:
            assert sha(Path(record['Path'])) == record['Hash']
    if prefix == 'ccpu-control-role-':
        expected = {'cli', 'clts', 'wait', 'rsrvd', 'div', 'lidt', 'sidt', 'jcxz', 'str', 'nop'}
        assert set(changed) == {'softpc.new/base/ccpu386/' + n + '.c' for n in expected}
        evidence = A / 'ccpu-control'
        for record in json.loads((evidence / 'records.json').read_text(encoding='utf-8-sig')):
            assert sha(evidence / record['file']) == record['sha256']
        guest = json.loads((evidence / 'observations.json').read_text(encoding='utf-8-sig'))
        assert guest['result'] == 'pass' and not guest['production_changed'] and not guest['guest_originals_changed']
        assert {r['name'] for r in guest['cases']} == {'initial-narrow-nonpass', 'baseline', 'negative', 'candidate', 'sign-flags', 'sign-flags-negative'}
        for case in guest['cases']:
            assert case['context']['integrity_sid'] == 'S-1-16-8192' and case['context']['private_desktop'] is True
            expected_exit = 1 if case['name'] == 'initial-narrow-nonpass' else 0
            assert case['runner_exit'] == expected_exit
            root = evidence / case['name']
            if expected_exit:
                continue
            for row in rows(root / 'inputs.csv'):
                assert sha(Path(row['Path'])) == row['Hash']
            report = (root / 'observer.txt').read_text(encoding='utf-8-sig')
            console = (root / 'observer.txt.console.txt').read_text(encoding='utf-8-sig')
            negative = case['name'] in ('negative', 'sign-flags-negative')
            assert 'result=exited' in report and ('exit=0x00000001' if negative else 'exit=0x00000000') in report
            if case['name'] in ('baseline', 'candidate'):
                assert all(s in console for s in ('CCPU CONTROL PASS', 'NOP32 CLI32 CLTS4 WAIT4 IDT16', 'DIVFAULT6 JCXZ4 STRFAULT1 RSRVD5'))
            if negative:
                assert 'FAIL phase=A' in console and 'CCPU CONTROL PASS' not in console
        image = json.loads((evidence / 'image/result.json').read_text())
        assert image['result'] == 'pass' and image['executable_sections_equal'] is True and len(image['functions']) == 14
        assert image['checker_sha256'] == sha(Path('tests/observation/verify_ccpu_control_image.py'))
        assert image['parser_sha256'] == sha(Path('tests/observation/verify_ccpu_sign_flag_image.py'))
        assert json.loads((evidence / 'image/wrong.completion.json').read_text(encoding='utf-8-sig'))['exit'] == 1
        for proof in dispositions:
            assert proof['original_text_retained_percent'] == '100' and not proof['estimated_restored_code_percent']
            assert proof['status'] == ('excluded' if Path(proof['relative_path']).stem == 'nop' else 'partial')
    if prefix == 'ccpu-foundation-role-':
        observed = json.loads((A / 'ccpu-foundation-observations.json').read_text())
        actual = runpy.run_path(str(A / 'ccpu-foundation-observer.py'))['inspect']()
        assert observed == actual and len(actual['records']) == 6
        assert sum(len(r['functions']) for r in actual['records']) == 33
        paths = {r['relative_path'] for r in actual['records']}
        paths.update(h['relative_path'] for r in actual['records'] for h in r['headers'])
        assert paths == set(changed) and len(paths) == 9
        for proof in dispositions:
            assert proof['status'] == 'partial' and proof['original_text_retained_percent'] == '100'
            assert not proof['estimated_restored_code_percent']
            assert proof['restored_estimate_status'].startswith('unmeasurable:')
    if prefix == 'selected-ccpu-header-role-':
        cohort=sorted(p for p,r in old.items() if r['status']=='pending' and
                      p.startswith('softpc.new/base/ccpu386/') and p.endswith('.h'))
        assert len(cohort)==observation['reviewed_cohort_count']==96
        table_path=A/'ccpu-selected-header-observations.csv'
        selection=rows(table_path)
        inspector=runpy.run_path(str(A/'ccpu-selected-header-extractor.py'))
        pp=observation['preprocessor_output']
        assert sha(Path(pp['path']))==pp['sha256']
        actual,seen=inspector['inspect_output'](Path(pp['path']),cohort)
        assert len(seen)==74
        for first,second in zip(selection,actual):
            assert first=={k:str(v) for k,v in second.items()}
        assert len(selection)==len(actual)==96
        selected={r['relative_path'] for r in selection if int(r['selected_nonblank_lines'])>0}
        assert selected==set(changed) and len(selected)==71
        assert all(merged[p]['status']=='pending' for p in set(cohort)-selected)
        for record in observation['normal_cache_inputs']:
            assert sha(Path(record['Path']))==record['Hash']
        for record in observation['configuration_review_inputs']:
            assert sha(Path(record['path']))==record['sha256']
        for proof in dispositions:
            assert proof['status']=='partial' and proof['original_text_retained_percent']=='100'
            assert not proof['estimated_restored_code_percent']
            assert proof['restored_estimate_status'].startswith('unmeasurable:')
    if prefix == 'historical-build-role-':
        cohort = {p for p, r in old.items() if r['status'] == 'pending' and
                  re.fullmatch(r'(sources|dirs|makefil\w*(\..*)?|_objects\.mac)', Path(p).name, re.I)}
        assert cohort == set(changed) and len(cohort) == observation['reviewed_cohort_count'] == 36
        assert len(observation['active_source_manifests']) == len(set(observation['active_source_manifests'])) == 19
        assert not cohort.intersection(observation['active_source_manifests'])
        for record in observation['configuration_review_inputs']:
            assert sha(Path(record['path'])) == record['sha256']
        for record in observation['host_graph_observations']:
            assert sha(Path(record['path'])) == record['sha256']
            text = Path(record['path']).read_text().replace('\\', '/').lower()
            assert all('src/mvdm/' + p.lower() not in text for p in cohort)
        for proof in dispositions:
            assert proof['status'] == 'excluded' and proof['original_text_retained_percent'] == '100'
            assert proof['restored_estimate_status'].startswith('not-applicable:')
            assert not proof['estimated_restored_code_percent']
    if prefix == 'native-source-role-':
        reviewed = {}
        for shard, record in enumerate(observation['source_tables'], 1):
            proposal = rows(A / f'shard-{shard}/native-pending-owner-review.csv')
            cohort = sorted(r['relative_path'] for r in proposal if old[r['relative_path']]['status'] == 'pending')[:30]
            batch = rows(A / record['path'])
            assert sorted(r['relative_path'] for r in batch) == cohort and len(batch) == 30
            for row in batch:
                path = row['relative_path']
                assert path not in reviewed and int(merged[path]['shard']) == shard
                assert sha(Path('src/mvdm') / path) == row['current_sha256'] == row['frozen_sha256']
                originals = [Path(p.strip()) for p in row['original_path'].split(';')]
                assert any(p.is_file() and sha(p) == row['original_sha256'] for p in originals)
                assert row['original_sha256'] == row['current_sha256']
                reviewed[path] = row
        accepted = {p for p, r in reviewed.items() if r['proposal_status'] in ('partial', 'excluded-file-role-only')}
        assert accepted == set(changed) and len(reviewed) == observation['reviewed_cohort_count'] == 90
        assert len(reviewed.keys() - accepted) == observation['remaining_cohort_pending'] == 52
        assert all(merged[p]['status'] == 'pending' for p in reviewed.keys() - accepted)
        for path in ('softpc.new/base/ccpu386/c_getset.c', 'softpc.new/base/ccpu386/ccpupig.c'):
            text = (Path('src/mvdm') / path).read_text()
            assert re.search(r'#\s*if(?:def\s+PIG|\s+defined\(PIG\))', text)
        for record in observation['host_graph_observations']:
            graph = Path(record['path'])
            assert sha(graph) == record['sha256']
            text = graph.read_text()
            flags = [line for line in text.splitlines()
                     if line.startswith(('host_cflags = ', 'cflags = ', 'ccpu_flags = '))]
            assert flags and all('/DCPU_40_STYLE' in line and
                                 not re.search(r'/DPIG(?:\s|=|$)', line) for line in flags)
            assert all(not re.search(r'/DPIG(?:\s|=|$)', line) for line in text.splitlines()
                       if line.startswith('release_cflags = '))
            if any(line.startswith('ccpu_flags = ') for line in flags):
                rule = re.search(r'(?m)^rule cc_ccpu\r?\n([^\n]+)', text)
                assert rule and '$ccpu_flags' in rule[1] and '$release_cflags' in rule[1]
                assert all(f'build obj/ccpu/{name}.obj: cc_ccpu ' in text
                           for name in ('c_getset', 'ccpupig'))
        objects = observation['compiled_empty_objects']
        assert {r['relative_path'] for r in objects} == {
            'softpc.new/base/ccpu386/c_getset.c', 'softpc.new/base/ccpu386/ccpupig.c'}
        for record in objects:
            obj = Path(record['object_path'])
            assert sha(obj) == record['object_sha256']
            assert sha(A / record['report_path']) == record['report_sha256']
            data = obj.read_bytes()
            machine, sections = struct.unpack_from('<HH', data)
            optional = struct.unpack_from('<H', data, 16)[0]
            assert machine == 0x14c and 0 < sections < 32 and optional == 0
            for i in range(sections):
                offset = 20 + i * 40
                size = struct.unpack_from('<I', data, offset + 16)[0]
                flags = struct.unpack_from('<I', data, offset + 36)[0]
                assert not size or not flags & (0x20 | 0x20000000)
    if prefix == 'native-source-next-role-':
        cohort_old = {r['relative_path']: r for r in original_rows(merged_path, observation['cohort_base'])}
        reviewed = {}
        for shard, record in enumerate(observation['source_tables'], 1):
            proposal = rows(A / f'shard-{shard}/native-pending-owner-review.csv')
            cohort = sorted(r['relative_path'] for r in proposal if cohort_old[r['relative_path']]['status'] == 'pending')[30:90]
            batch = rows(A / record['path'])
            assert sorted(r['relative_path'] for r in batch) == cohort and len(batch) == 60
            for item in batch:
                name = item['relative_path']
                assert name not in reviewed and int(old[name]['shard']) == shard
                assert sha(Path('src/mvdm') / name) == item['current_sha256'] == item['frozen_sha256']
                assert sha(Path(item['original_path'])) == item['original_sha256'] == item['current_sha256']
                reviewed[name] = item
        accepted = {p for p, r in reviewed.items() if r['proposal_status'] in ('partial', 'excluded-file-role-only')}
        assert accepted == set(changed) and len(reviewed) == observation['reviewed_cohort_count'] == 180
        assert len(reviewed.keys() - accepted) == observation['remaining_cohort_pending'] == 132
        assert all(merged[p]['status'] == 'pending' for p in reviewed.keys() - accepted)
        for record in observation['host_graph_observations']:
            assert sha(Path(record['path'])) == record['sha256']
        map_record = observation['selected_product_map']
        assert sha(Path(map_record['path'])) == map_record['sha256']
        selected_map = Path(map_record['path']).read_text().lower()
        dependencies = '\n'.join(Path(r['path']).read_text().replace('\\', '/').lower()
                                 for r in observation['host_input_observations'])
        graph = Path(observation['host_graph_observations'][0]['path']).read_text().replace('\\', '/').lower()
        for name, proof in changed.items():
            if name.endswith('.h'):
                assert 'src/mvdm/' + name.lower() in dependencies
            elif proof['status'] == 'partial':
                obj = Path(name).stem.lower() + '.obj'
                assert ':' + obj in selected_map and 'src/mvdm/' + name.lower() in graph
            assert proof['original_text_retained_percent'] == '100'
            assert not proof['estimated_restored_code_percent']
            assert proof['restored_estimate_status'].startswith('unmeasurable:')
        empty = observation['compiled_empty_object']
        data = Path(empty['path']).read_bytes()
        assert sha(Path(empty['path'])) == empty['sha256']
        machine, sections = struct.unpack_from('<HH', data)
        assert machine == 0x14c and 0 < sections < 32 and struct.unpack_from('<H', data, 16)[0] == 0
        for i in range(sections):
            offset = 20 + i * 40
            size = struct.unpack_from('<I', data, offset + 16)[0]
            flags = struct.unpack_from('<I', data, offset + 36)[0]
            assert not size or not flags & (0x20 | 0x20000000)
        empty_name = 'softpc.new/base/comms/prt_scrn.c'
        empty_text = (Path('src/mvdm') / empty_name).read_text()
        assert not re.sub(r'/\*.*?\*/', '', empty_text, flags=re.S).strip()
        assert 'bios4.rom' in empty_text and 'src/mvdm/' + empty_name in graph
    if prefix in ('guest-declaration-resource-role-', 'generated-message-role-', 'shared-guest-include-role-'):
        generated = prefix == 'generated-message-role-'
        shared = prefix == 'shared-guest-include-role-'
        expected_reviewed, expected_remaining = (76, 0) if generated else (104, 60) if shared else (434, 305)
        extensions = {'.h', '.inc', '.mac', '.rc', '.rcv', '.dlg', '.bmp', '.ico', '.cur', '.bms', '.def', '.msg', '.skl', '.ext'}
        cohort = {p for p, r in old.items() if r['status'] == 'pending'
                  and (re.search(r'\.(CL[0-9A-Z]|CTL|pch|sym|OBJ|tmp)$', p, re.I) if generated
                       else p.startswith(('wow16/inc/', 'dos/v86/h/', 'dos/v86/inc/')) and Path(p).suffix.lower() in ('.h', '.inc', '.mac') if shared
                       else p.startswith(('dos/', 'wow16/')) and Path(p).suffix.lower() in extensions)}
        reviewed = {}
        for shard, record in enumerate(observation['source_tables']):
            for row in rows(A / record['path']):
                path = row['relative_path']
                assert path not in reviewed and int(old[path]['shard']) == shard
                assert sha(Path('src/mvdm') / path) == row['sha256'] == frozen[path]['current_sha256']
                originals = [Path(p.strip()) for p in row['original_path'].split(';')]
                assert any(p.is_file() and sha(p) == row['sha256'] for p in originals)
                reviewed[path] = row
        assert set(reviewed) == cohort and len(cohort) == observation['reviewed_cohort_count'] == expected_reviewed
        accepted = {p for p, r in reviewed.items() if r['proposal_status'] in ('excluded', 'excluded-file-role-only')}
        if generated:
            overrides = {r['relative_path']: r for r in observation['main_overrides']}
            assert set(overrides) == cohort - accepted and all(r['basis'] for r in overrides.values())
            accepted |= set(overrides)
        assert accepted == set(changed)
        assert len(cohort - accepted) == observation['remaining_cohort_pending'] == expected_remaining
        assert all(merged[p]['status'] == 'pending' for p in cohort - accepted)
        if shared:
            for record in observation['host_graph_observations']:
                assert sha(Path(record['path'])) == record['sha256']
    if prefix == 'generated-message-role-':
        format_path = A / observation['format_observations_path']
        assert sha(format_path) == observation['format_observations_sha256']
        formats = json.loads(format_path.read_text())
        script = A / 'generated-message-format-observations.py'
        assert formats['script_sha256'] == sha(script) and formats['paths'] == 76
        for record in formats['references']:
            assert sha(Path(record['path'])) == record['sha256']
        readers = runpy.run_path(str(script))
        assert {r['relative_path'] for r in formats['records']} == set(changed)
        for record in formats['records']:
            path = Path('src/mvdm') / record['relative_path']
            assert record['observation'] == 'pass' and sha(path) == record['sha256']
            if record['format'] in ('empirical-16-bit-SYM', 'OMF'):
                reader = 'inspect_sym' if record['format'] == 'empirical-16-bit-SYM' else 'inspect_omf'
                assert readers[reader](path.read_bytes()) == record['details']
            if 'compared_source_sha256' in record:
                assert sha(Path(record['compared_source'])) == record['compared_source_sha256']
        archive = Path(observation['archive_path'])
        assert sha(archive) == observation['archive_sha256']
        with zipfile.ZipFile(archive) as z:
            archive_hashes = {hashlib.sha256(z.read(i)).hexdigest().upper() for i in z.infolist() if not i.is_dir()}
        package_hashes = set()
        for record in observation['package_files']:
            assert sha(Path(record['path'])) == record['sha256']
            package_hashes.add(record['sha256'])
        assert all(r['sha256'] not in archive_hashes | package_hashes for r in dispositions)
    if prefix == 'guest-library-role-':
        assert observation['archive_count'] == 27 and observation['macro_source_count'] == 1
        assert sha(A / 'guest-library-format-observations.csv') == observation['format_observations_sha256']
        assert sha(A / 'guest-library-format-observations.json') == observation['format_details_sha256']
        formats = {r['relative_path']: r for r in rows(A / 'guest-library-format-observations.csv')}
        details = json.loads((A / 'guest-library-format-observations.json').read_text())
        assert details['result'] == 'pass' and details['paths'] == len(formats) == 32
        assert details['script_sha256'] == sha(A / 'guest-library-format-observations.py')
        assert details['nonzero_checksum_observations'] == 0
        assert set(observation['unresolved']) == set(formats) - set(changed)
        for path, proof in changed.items():
            fmt = formats[path]
            assert fmt['sha256'] == proof['sha256'] and fmt['checksum_nonzero_mismatches'] == '0'
            if proof['role'] == 'historical-guest-library-build-artifact-only':
                assert fmt['library_end'] == 'True' and int(fmt['modules']) > 0
                assert fmt['format'] == 'OMF LIBHDR/module framing/LIBEND/dictionary bounds'
            else:
                assert path == 'dos/v86/cmd/append/sysmac.lib' and fmt['modules'] == '0'
        supplements = {r['relative_path']: r for r in rows(A / 'supplemental-original-provenance.csv')}
        for record in observation['original_recipe_inputs']:
            path = record['relative_path']
            assert sha(Path('src/mvdm') / path) == record['sha256'] == frozen[path]['current_sha256']
            assert sha(Path(record['original_path'])) == record['sha256']
            if record['sha256'] not in (frozen[path]['opennt_sha256'], frozen[path]['opennt45_sha256']):
                source = supplements[path]
                assert frozen[path]['upstream_origin'] == 'current-only' and source['provenance'] == 'verified-supplemental-original'
                assert source['source_sha256'] == record['sha256'] and Path(source['source_path']) == Path(record['original_path'])
    if prefix == 'dos-core-source-role-':
        definition = json.loads((A / 'supplemental-export-definition-review.json').read_text())
        assert definition['result'] == 'pass' and definition['template_declared_names'] == 140 and definition['matched_profile_names'] == 138
        assert definition['missing_profile_names'] == ['VdmQueryFreeVirtualMemory', 'VdmReallocateVirtualMemory']
        assert definition['retained_definition_lines'] == definition['original_definition_lines'] == 136
        for record in definition['inputs']:
            assert sha(Path(record['path'])) == record['sha256']
    for record in observation['host_input_observations']:
        path = Path(record['path'])
        assert sha(path) == record['sha256'] and '(STALE)' not in path.read_text()
        text = path.read_text().replace('\\', '/').lower()
        for relative in changed:
            if changed[relative]['status'] == 'excluded' and changed[relative]['role'] not in ('historical-cpu-test-source-only', 'historical-empty-printscreen-source-only', 'historical-inline-helper-source-only', 'historical-cpu-test-declaration-only'):
                assert 'src/mvdm/' + relative.lower() not in text, relative
    for row in rows(A / 'full-selected-leaf.csv') + rows(A / 'full-selected-declarations.csv'):
        assert merged[row['relative_path']]['status'] == 'full'
        assert sha(Path('src/mvdm') / row['relative_path']) == row['source_sha256']
    drift = []
    for path, row in frozen.items():
        source = Path('src/mvdm') / path
        if not source.exists():
            assert not row['current_sha256']
            continue
        current = sha(source)
        if current != row['current_sha256']:
            drift.append(dict(relative_path=path, frozen=row['current_sha256'], current=current))
    assert drift == summary['live_source_hash_drift']
    result = dict(result='pass', base=base, counts=counts, source_role_changes=expected_count,
                  unrelated_shard_rows_unchanged=True, frozen_inventory_unchanged=True,
                  product_or_guest_changed_by_checkpoint=False,
                  scope='Source roles, identities, recorded input absence and ledger consistency; no guest capability acceptance')
    if prefix == 'retired-host-role-':
        result.pop('product_or_guest_changed_by_checkpoint')
        result.update(package_and_installed_state_verification='separate delivery gate')
    output.write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps(result))


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('output', type=Path)
    parser.add_argument('--base', default=BASE)
    parser.add_argument('--prefix', choices=('guest-source-role-', 'kernel31-source-role-', 'dos-core-source-role-', 'wow16-core-source-role-', 'wow16-extension-source-role-', 'guest-library-role-', 'wow16-support-source-role-', 'guest-declaration-resource-role-', 'generated-message-role-', 'shared-guest-include-role-', 'native-source-role-', 'native-source-next-role-', 'historical-build-role-', 'selected-ccpu-header-role-', 'ccpu-foundation-role-', 'ccpu-control-role-', 'ccpu-header-macro-role-', 'dos-command-input-role-', 'bios-native-role-', 'system-native-role-', 'disk-native-role-', 'video-native-role-', 'input-native-role-', 'planar-native-role-', 'memory-native-role-', 'retired-host-role-', 'vdd-native-role-', 'host-tail-role-', 'base-inc-role-', 'base-inc-multi-role-', 'base-inc-wide-role-', 'base-inc-alternate-role-', 'cvidc-generated-role-', 'cvidc-vector-role-', 'parallel-native-role-', 'parallel-cvidc-role-', 'parallel-wow-gdi-role-', 'parallel-cross-role-', 'parallel-dpmi-role-', 'parallel-text-dem-role-', 'parallel-resource-redir-role-', 'parallel-heap-video-role-', 'parallel-controls-directory-role-', 'parallel-user-sound-role-', 'parallel-user-interaction-role-', 'parallel-guest-delivery-role-', 'shard0-cvidc-declaration-role-', 'shard0-wow-palette-role-', 'parallel-wow-memory-spool-role-', 'parallel-embedding-sound-role-', 'parallel-wow-gdi-table-role-', 'shard0-empty-gdi-role-', 'shard0-winsock-raw-role-', 'shard0-ddeml-test-role-', 'parallel-empty-declaration-role-'), default='guest-source-role-')
    parser.add_argument('--expected-count', type=int, default=187)
    args = parser.parse_args()
    main(args.output, args.base, args.prefix, args.expected_count)
