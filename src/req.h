#ifndef _REQ_H_
#define _REQ_H_

#include <curl/curl.h>

extern CURL *curl;

void req_init();
void req_cleanup();
void req_get(const char *url);

#endif
