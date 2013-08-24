#ifndef URL_PARSE_H
#define URL_PARSE_H

#define MAX_URL_LEN (2048)
#define MAX_HOST_LEN (255)
#define DEFAULT_PORT (80)

typedef struct {
	char host[MAX_HOST_LEN + 1];
	int port;
	char page[1786]; // max length of URL - max length of host - max length of port spe
} url_schema;

#ifdef __cplusplus
extern "C" {
#endif

	url_schema* url_parse(const char *url);

	void url_free(url_schema *urls);

#ifdef __cplusplus
}
#endif

#endif
