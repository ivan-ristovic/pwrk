#ifndef _REQ_H_
#define _REQ_H_

#include <curl/curl.h>
#include "measure.h"

extern CURL *curl;

void req_init_global();
void req_cleanup_global();
int req_get(const char *url, Measurement *measurement);
int req_post(const char *url, const char *data, size_t len, Measurement *measurement);

#endif
