/* Separate acceptance variant: actual CCPU page-domain owner, with only
 * its externally consumed publish leaf intercepted for one declared failure.
 * All original implementation and internal calls remain in this TU. */
#define mvdm_softpc_wow_page_domain_publish_handle wow_dialog_real_publish_handle
#define mvdm_softpc_wow_page_domain_callback_window wow_dialog_real_callback_window
#include "ntvdm-exe/softpc/mvdm_softpc_wow_page_domain.c"
#undef mvdm_softpc_wow_page_domain_publish_handle
#undef mvdm_softpc_wow_page_domain_callback_window
unsigned wow_dialog_fail_publication;
unsigned wow_dialog_publication_failures;
void *wow_dialog_failed_allocation;
unsigned wow_dialog_fail_callback,wow_dialog_callback_rejections;
int mvdm_softpc_wow_page_domain_callback_window(unsigned long window,unsigned long saved[2])
{
    if(wow_dialog_fail_callback){
        wow_dialog_fail_callback=0;++wow_dialog_callback_rejections;
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);return 0;
    }
    return wow_dialog_real_callback_window(window,saved);
}
int mvdm_softpc_wow_page_domain_publish_handle(unsigned short index,
    unsigned short uniqueness,unsigned char type,unsigned char flags,
    unsigned long window,unsigned long cls,const unsigned long words[3])
{
    if(wow_dialog_fail_publication && type==1){
        wow_dialog_fail_publication=0;++wow_dialog_publication_failures;
        wow_dialog_failed_allocation=GetPropW((HWND)(ULONG_PTR)window,L"NTVDM.WOW.WindowWords");
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);return 0;
    }
    return wow_dialog_real_publish_handle(index,uniqueness,type,flags,window,cls,words);
}
