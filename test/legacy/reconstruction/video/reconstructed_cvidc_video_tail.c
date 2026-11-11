#include "reconstructed_cvidc_video_tail.h"

#include "insignia.h"
#include "evidgen.h"

extern void setWritePointers(void);
extern void setReadPointers(IUH readset);
extern void setMarkPointers(IUH markset);

int runner_reconstructed_cvidc_video_tail_initialize(void)
{
    /* The 0--77 prefix is supplied by the separately compiled original vglfunc.c. */
    if ((Video.setWritePointers != NULL && Video.setWritePointers != setWritePointers) ||
        (Video.setReadPointers != NULL && Video.setReadPointers != setReadPointers) ||
        (Video.setMarkPointers != NULL && Video.setMarkPointers != setMarkPointers)) {
        return 0;
    }

    Video.setWritePointers = setWritePointers;
    Video.setReadPointers = setReadPointers;
    Video.setMarkPointers = setMarkPointers;
    return 1;
}
