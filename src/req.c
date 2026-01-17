#include "debug.h"
#include "req.h"
#include "stopwatch.h"
#include <stdbool.h>

// TODO reuse curl handle?

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

static bool req_init_local()
{
    dbg("Initializing curl locally");
    _curl = curl_easy_init();
    panic_if(_curl == NULL, "Failed to initialize curl handle");
    return _curl != NULL;
}

static void req_cleanup_local()
{
    panic_if(_curl == NULL, "Local curl cleanup called before init");
    dbg("Cleaning up curl locally");
    curl_easy_cleanup(_curl);
}

static void req_error(CURLcode result)
{
    fprintf(stderr, "curl_easy_perform() failed: %s\n",
            curl_easy_strerror(result));
}

static size_t cb(void *data, size_t size, size_t nmemb, void *userp) {
    unused(data);
    unused(size);
    unused(userp);
    dbg("libcurl: %zu bytes received", nmemb);
    return nmemb;
}

int req_get(const char *url, Measurement *measurement)
{
    int status = -1;

    if (req_init_local()) {
        curl_easy_setopt(_curl, CURLOPT_URL, url);
        // curl_easy_setopt(_curl, CURLOPT_FOLLOWLOCATION, 1L);
        
        curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, cb);
        /* curl_easy_setopt(_curl, CURLOPT_WRITEDATA, (void *)&data); */

        dbg("GET : %s", url);
        CURLcode result;
        uint64_t latency;

        STOPWATCH_MEASURE(&latency, { result = curl_easy_perform(_curl); });
        measurement->latency_ns = latency;

        if (result == CURLE_OK) {
            curl_easy_getinfo(_curl, CURLINFO_RESPONSE_CODE, &status);
        } else {
            req_error(result);
        }

        req_cleanup_local();
    }

    return status;
}

int req_post(const char *url, const char *data, size_t len, Measurement *measurement)
{
    int status = -1;

    if (req_init_local()) {
        curl_easy_setopt(_curl, CURLOPT_URL, url);
        curl_easy_setopt(_curl, CURLOPT_POSTFIELDS, data);
        curl_easy_setopt(_curl, CURLOPT_POSTFIELDSIZE, len);

        curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, cb);
        /* curl_easy_setopt(_curl, CURLOPT_WRITEDATA, (void *)&data); */

        dbg("POST: %s", url);
        CURLcode result;
        uint64_t latency;

        STOPWATCH_MEASURE(&latency, { result = curl_easy_perform(_curl); });
        measurement->latency_ns = latency;

        if (result == CURLE_OK) {
            curl_easy_getinfo(_curl, CURLINFO_RESPONSE_CODE, &status);
        } else {
            req_error(result);
        }

        req_cleanup_local();
    }

    return status;
}

// TODO async get/post with timeout 
// https://curl.se/libcurl/c/multi-post.html
// this is needed in order to counter coordinated ommission!
