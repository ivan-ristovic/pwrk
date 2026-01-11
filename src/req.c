#include "req.h"
#include "debug.h"

CURL *_curl = NULL;

void req_init()
{
    dbg("Initializing curl globally");
    CURLcode result = curl_global_init(CURL_GLOBAL_ALL);
    panic_if(result, "curl_global_init failed");
}

void req_cleanup() 
{
    dbg("Cleaning up curl globally");
    curl_global_cleanup();
}

void req_get(const char *url)
{
    dbg("Initializing curl locally");
    _curl = curl_easy_init();
    if (_curl) {
        curl_easy_setopt(_curl, CURLOPT_URL, url);
        // curl_easy_setopt(_curl, CURLOPT_FOLLOWLOCATION, 1L);

        dbg("Performing get request: %s", url);
        CURLcode result = curl_easy_perform(_curl);
        if(result != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(result));
        }

        dbg("Cleaning up curl locally");
        curl_easy_cleanup(_curl);
    }
}
