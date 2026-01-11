#ifndef _REQ_H_
#define _REQ_H_

#include <curl/curl.h>
#include <sys/types.h>

extern CURL *curl;

void req_init_global();
void req_cleanup_global();
void req_get(const char *url);
void req_post(const char *url, const char *data, ssize_t len);

#endif
