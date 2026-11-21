#include "include/mvdm_softpc_text_video.h"
#include <windows.h>

/* Immutable provider lives for the worker image; bind before original entry.
 * Calls remain synchronous on the existing video owner and synchronization. */
static const mvdm_text_video_provider *binding;

int mvdm_softpc_text_video_bind(const mvdm_text_video_provider *provider)
{
    const mvdm_text_video_provider *previous;
    if (!provider || !provider->copy || !provider->refresh_palette ||
            !provider->sync_route || !provider->local || !provider->flush ||
            !provider->pause) {
        SetLastError(ERROR_INVALID_PARAMETER); return 0;
    }
    previous=InterlockedCompareExchangePointer((PVOID volatile *)&binding,
        (PVOID)provider,NULL);
    if (previous && previous!=provider) {
        SetLastError(ERROR_ALREADY_INITIALIZED); return 0;
    }
    return 1;
}

static const mvdm_text_video_provider *current_provider(void)
{
    if (!binding) RaiseFailFastException(NULL,NULL,0);
    return binding;
}

int mvdm_softpc_text_video_copy(mvdm_softpc_text_video *copy)
{ return current_provider()->copy(copy); }
void mvdm_softpc_text_video_refresh_palette(void)
{ current_provider()->refresh_palette(); }
int mvdm_softpc_text_video_sync_route(void)
{ return current_provider()->sync_route(); }
int mvdm_softpc_text_video_local(void)
{ return current_provider()->local(); }
void mvdm_softpc_text_video_flush(void (*publish)(void))
{ current_provider()->flush(publish); }
void mvdm_softpc_text_video_pause(void)
{ current_provider()->pause(); }
