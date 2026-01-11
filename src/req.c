#include "req.h"
#include "debug.h"
#include <stdbool.h>

CURL *_curl = NULL;

void req_init_global()
{
    dbg("Initializing curl globally");
    CURLcode result = curl_global_init(CURL_GLOBAL_ALL);
    panic_if(result, "curl_global_init failed");
}

void req_cleanup_global() 
{
    dbg("Cleaning up curl globally");
    curl_global_cleanup();
}

static bool _req_init_local()
{
    dbg("Initializing curl locally");
    _curl = curl_easy_init();
    panic_if(_curl == NULL, "Failed to initialize curl handle");
    return _curl != NULL;
}

static void _req_cleanup_local()
{
    panic_if(_curl == NULL, "Local curl cleanup called before init");
    dbg("Cleaning up curl locally");
    curl_easy_cleanup(_curl);
}

static void _req_error(CURLcode result)
{
    fprintf(stderr, "curl_easy_perform() failed: %s\n",
            curl_easy_strerror(result));
}

void req_get(const char *url)
{
    if (_req_init_local()) {
        curl_easy_setopt(_curl, CURLOPT_URL, url);
        // curl_easy_setopt(_curl, CURLOPT_FOLLOWLOCATION, 1L);

        dbg("GET : %s", url);
        CURLcode result = curl_easy_perform(_curl);
        if (result != CURLE_OK) {
            _req_error(result);
        }

        _req_cleanup_local();
    }
}

void req_post(const char *url, const char *data, ssize_t len)
{
    if (_req_init_local()) {
        curl_easy_setopt(_curl, CURLOPT_URL, url);
        curl_easy_setopt(_curl, CURLOPT_POSTFIELDS, data);
        curl_easy_setopt(_curl, CURLOPT_POSTFIELDSIZE, len);

        dbg("POST: %s", url);
        CURLcode result = curl_easy_perform(_curl);
        if (result != CURLE_OK) {
            _req_error(result);
        }

        _req_cleanup_local();
    }
}
