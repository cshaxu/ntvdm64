/* NTSRV-private service core; project adaptation,
 * not an original OpenNT mirror. Physical separation only: existing function
 * bodies, state authority and lock/resource contracts are preserved. */
#include <service_internal.h>

/* Original guarded USER notification hook remains unavailable. */
PFNNOTIFYPROCESSCREATE UserNotifyProcessCreate=NULL;



/* Original guarded USER hook is absent in standalone CLI composition. */

OPENNT_BASE_SERVICE *OpenNtBaseServiceStart(void)
{
    OPENNT_BASE_SERVICE *service=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*service));
    FILETIME now;
    if (!service) return NULL;
    InitializeConditionVariable(&service->frontend_changed);
    if (!InitializeCriticalSectionEx(&service->lock,0,0)) {
        HeapFree(GetProcessHeap(),0,service); return NULL;
    }
    InitializeListHead(&service->connections);
    InitializeListHead(&service->retired_connections);
    InitializeListHead(&service->worker_watches);
    InitializeListHead(&service->frontend_routes);
    InitializeListHead(&service->console_contexts);
    InitializeListHead(&service->gui_records);
    service->frontend_lifetime_changed=CreateEventW(NULL,FALSE,FALSE,NULL);
    if(!service->frontend_lifetime_changed) {
        DeleteCriticalSection(&service->lock);HeapFree(GetProcessHeap(),0,service);return NULL;
    }
    GetSystemTimeAsFileTime(&now);
    service->management_epoch=((uint64_t)now.dwHighDateTime<<32)|now.dwLowDateTime;
    service->management_epoch^=(uint64_t)GetCurrentProcessId()<<17;
    if (!service->management_epoch) service->management_epoch=1;
    if (!OpenNtBaseInitializeProcessRegistry(&service->registry)) {
        CloseHandle(service->frontend_lifetime_changed);
        DeleteCriticalSection(&service->lock); HeapFree(GetProcessHeap(),0,service); return NULL;
    }
    if (!OpenNtBaseReservationsInitialize(&service->reservations)) {
        OpenNtBaseDestroyProcessRegistry(&service->registry);
        CloseHandle(service->frontend_lifetime_changed);
        DeleteCriticalSection(&service->lock);HeapFree(GetProcessHeap(),0,service);return NULL;
    }
    /* srvvdm.c normally resolves this private winsrv entry point lazily.  The
     * standalone broker has no winsrv DLL, but the original CheckWOW body
     * already accepts precisely this predicate.  Supply the existing adapter
     * binding and keep its trusted local logon scope outside the wire format. */
    if (!OpenNtBaseInitializeInteractiveScope(&service->interactive)) {
        OpenNtBaseReservationsDestroy(service->reservations);
        OpenNtBaseDestroyProcessRegistry(&service->registry);
        CloseHandle(service->frontend_lifetime_changed);
        DeleteCriticalSection(&service->lock); HeapFree(GetProcessHeap(),0,service); return NULL;
    }
    UserTestTokenForInteractive=_UserTestTokenForInteractive;
    BaseSrvVDMInit();
    return service;
}

BOOL OpenNtBaseServiceConfigureEmptyNotify(OPENNT_BASE_SERVICE *service,
    OPENNT_BASE_EMPTY_NOTIFY notify,void *context)
{
    BOOL result=FALSE;
    if (!service || !notify) { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
    EnterCriticalSection(&service->lock);
    if (IsListEmpty(&service->connections) && IsListEmpty(&service->worker_watches)) {
        service->empty_notify=notify;
        service->empty_notify_context=context;
        result=TRUE;
    } else SetLastError(ERROR_BUSY);
    LeaveCriticalSection(&service->lock);
    return result;
}

BOOL OpenNtBaseServiceStop(OPENNT_BASE_SERVICE *service)
{
    LIST_ENTRY *entry;
    /* Transport has stopped and joined all calls/rundowns before stop. */
    if (!service || !IsListEmpty(&service->connections) || !IsListEmpty(&service->worker_watches) ||
        !IsListEmpty(&service->gui_records) ||
        !OpenNtBaseReservationsDestroy(service->reservations) ||
        !OpenNtBaseDestroyProcessRegistry(&service->registry)) return FALSE;
    /* A cancelled route may precede worker Connect, hence have no worker
     * exit watch. Calls and rundowns are joined; release its retained handles
     * without terminating a process or changing original DOS task policy. */
    while (!IsListEmpty(&service->frontend_routes))
        service_delete_frontend(CONTAINING_RECORD(service->frontend_routes.Flink,
            OPENNT_FRONTEND_ROUTE,link));
    while (!IsListEmpty(&service->console_contexts))
        service_delete_console_context(CONTAINING_RECORD(service->console_contexts.Flink,
            OPENNT_BASE_CONSOLE_CONTEXT,link));
    while (!IsListEmpty(&service->retired_connections)) {
        OPENNT_BASE_CONNECTION *connection;
        entry=RemoveHeadList(&service->retired_connections);
        connection=CONTAINING_RECORD(entry,OPENNT_BASE_CONNECTION,retired_link);
        if (connection->frontend_capability) CloseHandle(connection->frontend_capability);
        if (connection->frontend_retire) CloseHandle(connection->frontend_retire);
        if (connection->frontend_restored) CloseHandle(connection->frontend_restored);
        if (connection->frontend_state_changed) CloseHandle(connection->frontend_state_changed);
        if (connection->worker_state_changed) CloseHandle(connection->worker_state_changed);
        if (connection->worker_io_release) CloseHandle(connection->worker_io_release);
        service_release_console_identities(connection);
        HeapFree(GetProcessHeap(),0,connection);
    }
    DeleteCriticalSection(&service->lock);
    CloseHandle(service->frontend_lifetime_changed);
    HeapFree(GetProcessHeap(),0,service);
    return TRUE;
}

DWORD OpenNtBaseServiceConnect(OPENNT_BASE_SERVICE *service,HANDLE process,
    OPENNT_BASE_CONNECTION **output,DWORD *generation)
{
    OPENNT_BASE_CONNECTION *connection;
    DWORD error=0;
    if (!service || !output || !generation) return ERROR_INVALID_PARAMETER;
    *output=NULL; *generation=0;
    connection=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*connection));
    if (!connection) return ERROR_NOT_ENOUGH_MEMORY;
    connection->service=service;
    InitializeListHead(&connection->win32records);
    EnterCriticalSection(&service->lock);
    if (!OpenNtBaseRegisterProcess(&service->registry,&connection->process,process)) error=GetLastError();
    else {
        uint64_t reservation=0;
        ULONG task=0;
        HANDLE console=NULL;
        HANDLE reserved_worker=NULL;
        BOOL shared_wow=FALSE,watch_added=FALSE;
        OPENNT_BASE_WORKER_KIND kind=OPENNT_BASE_WORKER_DOS;
        broker_vdm_receipts_initialize(&connection->streams,connection->process.SequenceNumber);
        error=OpenNtBaseReservationClaimWorkerKind(service->reservations,
            (DWORD)connection->process.ClientId.UniqueProcess,connection->process.SequenceNumber,
            &reservation,&task,&console,&kind,&reserved_worker);
        if (error==ERROR_NOT_FOUND) error=ERROR_SUCCESS;
        else if (!error) {
            OPENNT_BASE_WORKER_WATCH *watch;
            shared_wow=kind==OPENNT_BASE_WORKER_WOW;
            connection->native_worker=kind==OPENNT_BASE_WORKER_NATIVE;
            connection->reservation=reservation;connection->task=task;connection->reservation_kind=kind;
            connection->console=console;connection->wow=shared_wow;
            /* This is the post-create registration performed by original
             * BaseSrvCreateProcess: publish the authenticated worker as a
             * VDM and attach its CSR sequence to the source ConsoleRecord.
             * The standalone learns that fact only when the reservation-bound
             * worker connects; it does not invent another worker state. */
            connection->process.fVDM=!connection->native_worker;
            /* Before first PIF binding a new DOS ConsoleRecord is keyed by
             * DosSesId, not the reserved transport Console identity. */
            if(!connection->native_worker)BaseSrvUpdateVDMSequenceNumber(shared_wow ? OPENNT_BASE_CONSOLE_WOW : task ? NULL : console,
                connection->process.SequenceNumber,task);
            watch=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*watch));
            if (!watch || !reserved_worker) {
                error=watch ? ERROR_INVALID_HANDLE : ERROR_NOT_ENOUGH_MEMORY;
                if (watch) HeapFree(GetProcessHeap(),0,watch);
                if(!connection->native_worker)BaseSrvCleanupVDMResources(&connection->process);
            } else {
                watch->process.ProcessHandle=reserved_worker;
                reserved_worker=NULL;
                watch->service=service;
                watch->process.ClientId=connection->process.ClientId;
                watch->process.SequenceNumber=connection->process.SequenceNumber;
                watch->process.fVDM=connection->process.fVDM;
                watch->kind=kind;
                watch->console=console;
                watch->wow=shared_wow;
                watch->reservation=reservation;
                watch->shutdown=CreateEventW(NULL,TRUE,FALSE,NULL);
                if(!watch->shutdown)error=GetLastError();
                InitializeListHead(&watch->management_labels);
                /* The first DOS record precedes the worker's first RPC.  Copy
                 * only its label before original GetNextVDMCommand releases
                 * VDMINFO; the record chain still owns all task state. */
                service_capture_initial_management_labels(watch);
                {
                    FILETIME ignored;
                    if (!GetProcessTimes(watch->process.ProcessHandle,&watch->started,&ignored,
                            &ignored,&ignored)) ZeroMemory(&watch->started,sizeof(watch->started));
                }
                /* Both resident worker kinds retain the authenticated root's
                 * outer-Console identity. NTVWM may later refresh it at direct
                 * delivery; DOS keeps the initial root for re-entry. */
                if(!error && !shared_wow && console) {
                    LIST_ENTRY *root_link;
                    for(root_link=service->connections.Flink;
                        root_link!=&service->connections;root_link=root_link->Flink) {
                        OPENNT_BASE_CONNECTION *root=CONTAINING_RECORD(root_link,
                            OPENNT_BASE_CONNECTION,service_link);
                        if(root->frontend_capability && service_root_console_matches(root,console) &&
                            root->console_member_count && !root->frontend_closing &&
                            WaitForSingleObject(root->process.ProcessHandle,0)==WAIT_TIMEOUT) {
                            error=service_copy_execution_console_members(connection,root);
                            watch->frontend_associated=TRUE;
                            watch->management_root_generation=root->process.SequenceNumber;
                            watch->management_root_pid=GetProcessId(root->process.ProcessHandle);
                            break;
                        }
                    }
                }
                if(connection->native_worker)watch->unbound_native_deadline=GetTickCount64()+FRONTEND_STARTUP_DEADLINE_MS;
                if (error || !RegisterWaitForSingleObject(&watch->wait,watch->process.ProcessHandle,
                    service_worker_terminated,watch,INFINITE,WT_EXECUTEONLYONCE)) {
                    if(!error)error=GetLastError();
                    CloseHandle(watch->process.ProcessHandle);
                    if(watch->shutdown)CloseHandle(watch->shutdown);
                    HeapFree(GetProcessHeap(),0,watch);
                    if(!connection->native_worker)BaseSrvCleanupVDMResources(&connection->process);
                } else {
                    InsertTailList(&service->worker_watches,&watch->link);
                    watch_added=TRUE;
}
            }
        }
        if (reserved_worker) CloseHandle(reserved_worker);
        if (!error) {
            service_prune_cancelled_frontends(service);
            InsertTailList(&service->connections,&connection->service_link);
            if(watch_added) {
                LIST_ENTRY *root_link;
                for(root_link=service->connections.Flink;root_link!=&service->connections;
                    root_link=root_link->Flink) {
                    OPENNT_BASE_CONNECTION *root=CONTAINING_RECORD(root_link,
                        OPENNT_BASE_CONNECTION,service_link);
                    if(root->frontend_capability && root->console==console)
                        root->frontend_admission_deadline=0;
                }
                service_signal_frontend_states(service);
            }
            if(connection->native_worker)service_signal_worker_states(service);
            *output=connection; *generation=connection->process.SequenceNumber;
        }
        else (void)OpenNtBaseRemoveProcess(&service->registry,&connection->process);
    }
    LeaveCriticalSection(&service->lock);
    if (error) {
        service_release_console_identities(connection);
        HeapFree(GetProcessHeap(),0,connection);
    }
    return error;
}

DWORD OpenNtBaseServiceDisconnect(OPENNT_BASE_CONNECTION *connection)
{
    OPENNT_BASE_SERVICE *service;
    DWORD error=0;
    if (!connection) return ERROR_INVALID_PARAMETER;
    service=connection->service;
    EnterCriticalSection(&service->lock);
    connection->vdm_exit_cancelled=TRUE;
    service_abandon_launch(connection);
    /* A client RPC context may close while resident COMMAND remains alive.
     * Only the retained process-exit watch may invoke the original cleanup. */
    if (connection->retired) {
        RemoveEntryList(&connection->retired_link);
    }
    else if (!OpenNtBaseRemoveProcess(&service->registry,&connection->process)) error=GetLastError();
    else {
        RemoveEntryList(&connection->service_link);
        broker_vdm_receipts_drain(&connection->streams);
        service_release_launcher_results(service,connection->process.SequenceNumber);
    }
    if (!error) service_clear_frontend(connection);
    if (!error) {
        LIST_ENTRY *link;
        /* Result latches and receipt signalling share the service lock with
         * process-exit rundown and the launcher's result query. */
        service_clear_pending_win32record(connection);
        service_clear_win32records(connection);
        for(link=service->connections.Flink;link!=&service->connections;link=link->Flink) {
            OPENNT_BASE_CONNECTION *root=CONTAINING_RECORD(link,OPENNT_BASE_CONNECTION,service_link);
            if(root->frontend_join_caller==connection) {
                root->frontend_join_caller=NULL;
                root->frontend_join_decision=-1;
                (void)service_refresh_frontend_work(root);
            }
        }
        WakeAllConditionVariable(&service->frontend_changed);
    }
    LeaveCriticalSection(&service->lock);
    if (!error) {
        if(connection->vdm_exit_watch)
            (void)UnregisterWaitEx(connection->vdm_exit_watch,INVALID_HANDLE_VALUE);
        if(connection->vdm_exit_process)CloseHandle(connection->vdm_exit_process);
        if(connection->frontend_exit_watch)
            (void)UnregisterWaitEx(connection->frontend_exit_watch,INVALID_HANDLE_VALUE);
        if(connection->frontend_exit_process)CloseHandle(connection->frontend_exit_process);
        if (connection->frontend_capability) CloseHandle(connection->frontend_capability);
        if (connection->frontend_retire) CloseHandle(connection->frontend_retire);
        if (connection->frontend_restored) CloseHandle(connection->frontend_restored);
        if (connection->frontend_state_changed) CloseHandle(connection->frontend_state_changed);
        if (connection->worker_state_changed) CloseHandle(connection->worker_state_changed);
        if (connection->worker_io_release) CloseHandle(connection->worker_io_release);
        if (connection->native_stop) CloseHandle(connection->native_stop);
        if (connection->native_closed) CloseHandle(connection->native_closed);
        if (connection->wow_start_event) CloseHandle(connection->wow_start_event);
        service_release_console_identities(connection);
        HeapFree(GetProcessHeap(),0,connection);
    }
    return error;
}

BOOL OpenNtBaseServicePeer(OPENNT_BASE_CONNECTION *connection,DWORD pid,DWORD generation)
{
    HANDLE process=NULL;
    DWORD error=OpenNtBaseServiceRetainPeer(connection,pid,generation,&process);
    if (process) CloseHandle(process);
    return error==0;
}

DWORD OpenNtBaseServiceAttachStream(OPENNT_BASE_CONNECTION *connection,DWORD pid,DWORD generation,
    DWORD role,HANDLE stream,DWORD *receipt)
{
    uint32_t id=0;
    DWORD error;
    if (!receipt) return ERROR_INVALID_PARAMETER;
    *receipt=0;
    if (!connection) return ERROR_ACCESS_DENIED;
    if (role<BROKER_VDM_STDIN || role>BROKER_VDM_STDERR) return ERROR_INVALID_PARAMETER;
    EnterCriticalSection(&connection->service->lock);
    error=OpenNtBaseServicePeer(connection,pid,generation) ?
        broker_vdm_receipt_accept(&connection->streams,role,stream,&id) : ERROR_ACCESS_DENIED;
    LeaveCriticalSection(&connection->service->lock);
    if (!error) *receipt=id;
    return error;
}

DWORD OpenNtBaseServiceRevokeStream(OPENNT_BASE_CONNECTION *connection,DWORD pid,DWORD generation,DWORD receipt)
{
    DWORD error;
    if (!connection) return ERROR_ACCESS_DENIED;
    EnterCriticalSection(&connection->service->lock);
    error=OpenNtBaseServicePeer(connection,pid,generation) ?
        broker_vdm_receipt_revoke(&connection->streams,generation,receipt) : ERROR_ACCESS_DENIED;
    LeaveCriticalSection(&connection->service->lock);
    return error;
}

DWORD OpenNtBaseServiceRetainPeer(OPENNT_BASE_CONNECTION *connection,DWORD pid,
    DWORD generation,HANDLE *output)
{
    DWORD error=0;
    if (!output) return ERROR_INVALID_PARAMETER;
    *output=NULL;
    if (!connection || !pid || !generation) return ERROR_ACCESS_DENIED;
    EnterCriticalSection(&connection->service->lock);
    if ((DWORD)connection->process.ClientId.UniqueProcess!=pid ||
        connection->process.SequenceNumber!=generation) error=ERROR_ACCESS_DENIED;
    else if (!OpenNtBaseRetainRegisteredProcess(&connection->service->registry,pid,generation,output))
        error=GetLastError();
    LeaveCriticalSection(&connection->service->lock);
    return error;
}
