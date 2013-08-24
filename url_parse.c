#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include "url_parse.h"

url_schema* url_parse(const char *url) 
{
    char tmp[MAX_HOST_LEN + 6] = { 0 };
    const char *p1, *p2, *p3;
    size_t len = 0;
    url_schema *urls;
    
    urls = (url_schema *)calloc(sizeof(url_schema), 1);
    assert(urls);

    if (!(p1 = strstr(url, "://"))) {
        free(urls);
        return NULL;
    }
    p1 += (sizeof("://") - 1);
    if (p2 = strstr(p1, "/")) {
        const char *cp = p2;
        char *dst = urls->page;
        while (*cp)
            *dst++ = *cp++;
    }
    len = p2 ? (p2 - p1) : strlen(p1);
    assert(len < sizeof(tmp) - 1);
    strncpy(tmp, p1, len);
    if (!(p3 = strstr(tmp, ":"))) {
        strncpy(urls->host, tmp, strlen(tmp));
        urls->port = DEFAULT_PORT;
    }
    else {
        strncpy(urls->host, tmp, p3 - tmp);
        urls->port = atoi(p3 + 1);
    }
    return urls;
}

void url_free(url_schema *urls)
{
	free(urls);
}