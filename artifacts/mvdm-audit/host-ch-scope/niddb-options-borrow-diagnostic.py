"""Create a nonpublishable page-domain source copy with failure-only tracing."""
from pathlib import Path
root = Path(__file__).resolve().parents[3]
out = root / 'build/t473-s10/options-borrow-diagnostic-r3'
out.mkdir(parents=True, exist_ok=True)
source = (root / 'src/ntvdm-exe/softpc/mvdm_softpc_wow_page_domain.c').read_text()
old = '''    if (!page_domain_menu_context() ||
            !(backing = page_domain_window_lookup(window))) return 0u;
    return borrow_window_backing(backing, NULL) ? backing : 0u;'''
new = '''    int observed_context = page_domain_menu_context();
    if (!observed_context ||
            !(backing = page_domain_window_lookup(window))) {
        char path[MAX_PATH], text[512]; DWORD saved=GetLastError(), n, written;
        HANDLE file;
        n=GetEnvironmentVariableA("MVDM_WOW_WINDOW_TRACE_PATH",path,sizeof(path));
        if(n && n<sizeof(path)) {
            file=CreateFileA(path,FILE_APPEND_DATA,FILE_SHARE_READ|FILE_SHARE_WRITE,
                NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
            if(file!=INVALID_HANDLE_VALUE) {
                n=(DWORD)wsprintfA(text,"BorrowFailure window=%08lX context=%d active=%d delta=%08lX owner=%p current=%p cr0=%08lX cr3=%08lX directory=%08lX index=%04X backing=%08lX desktop=%08lX head=%08lX sas=%08lX\\r\\n",
                    window,observed_context,domain.active,domain.client_delta,domain.owner,session_thread_current(),
                    c_getCR0(),c_getCR3(),domain.directory,LOWORD(window),
                    domain.guest_windows[LOWORD(window)],domain.desktop_handle,
                    page_domain_menu_context() && domain.guest_windows[LOWORD(window)] ?
                        page_domain_read_owned_word(domain.guest_windows[LOWORD(window)]) : 0u,
                    page_domain_menu_context() && domain.guest_windows[LOWORD(window)] ?
                        c_sas_dw_at(domain.guest_windows[LOWORD(window)]) : 0u);
                (void)WriteFile(file,text,n,&written,NULL);CloseHandle(file);
            }
        }
        SetLastError(saved);return 0u;
    }
    return borrow_window_backing(backing, NULL) ? backing : 0u;'''
if source.count(old) != 1: raise SystemExit('unexpected source shape')
lookup = '    return client && page_domain_read_owned_word(client) == window ? client : 0u;'
observed = '''    if (client) {
        ULONG actual = page_domain_read_owned_word(client);
        if (actual != window) {
            char path[MAX_PATH], text[256]; DWORD saved=GetLastError(), n, written; HANDLE file;
            n=GetEnvironmentVariableA("MVDM_WOW_WINDOW_TRACE_PATH",path,sizeof(path));
            if(n && n<sizeof(path)) {
                file=CreateFileA(path,FILE_APPEND_DATA,FILE_SHARE_READ|FILE_SHARE_WRITE,
                    NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
                if(file!=INVALID_HANDLE_VALUE) {
                    n=(DWORD)wsprintfA(text,"LookupMismatch requested=%08lX observed=%08lX backing=%08lX\\r\\n",window,actual,client);
                    (void)WriteFile(file,text,n,&written,NULL);CloseHandle(file);
                }
            }
            SetLastError(saved);return 0u;
        }
        return client;
    }
    return 0u;'''
if source.count(lookup) != 1: raise SystemExit('unexpected lookup shape')
(out / 'mvdm_softpc_wow_page_domain.c').write_text(source.replace(old, new).replace(lookup, observed))
