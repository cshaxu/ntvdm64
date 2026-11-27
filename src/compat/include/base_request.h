#ifndef OPENNT_BASE_REQUEST_H
#define OPENNT_BASE_REQUEST_H

struct _CSR_THREAD;

/* Borrow a local record until the caller restores the prior binding. */
struct _CSR_THREAD *OpenNtBaseServerRequestThread(void);
struct _CSR_THREAD *OpenNtBaseBindServerRequestThread(struct _CSR_THREAD *);

/* Synchronous local dispatch, after bounded decoding; never a wire identity. */
const void *OpenNtBaseServerRequestMessage(void);
const void *OpenNtBaseBindServerRequestMessage(const void *);
#endif
