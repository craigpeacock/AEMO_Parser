#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "http.h"

size_t header_callback(char *buffer, size_t size, size_t nitems, void *user_data)
{
	//printf("%.*s", (int)(nitems * size), buffer);
	return(size * nitems);
}

size_t write_callback(char *ptr, size_t size, size_t nmemb, void *user_data)
{
	struct buffer *out_buf = (struct buffer *)user_data;
	size_t buffersize = size * nmemb;

	//printf("size %ld, nmemb %ld\r\n",size, nmemb);

	char *newptr = realloc(out_buf->data, (out_buf->pos + buffersize + 1));
	if (newptr == NULL) {
		printf("Failed to allocate buffer\r\n");
		return 0;
	}

	out_buf->data = newptr;
	memcpy((void *)(out_buf->data + out_buf->pos), ptr, buffersize);
	out_buf->pos += buffersize;

	return(size * nmemb);
}

int http_json_request(struct buffer *out_buf)
{
	CURL *curl;
	CURLcode res;

	curl_global_init(CURL_GLOBAL_DEFAULT);

	curl = curl_easy_init();

	if(curl) {
		curl_easy_setopt(curl, CURLOPT_URL, "https://visualisations.aemo.com.au/aemo/apps/api/report/ELEC_NEM_SUMMARY");
		curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, header_callback);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, out_buf);

#ifdef SKIP_PEER_VERIFICATION
		/*
		 * If you want to connect to a site who isn't using a certificate that is
		 * signed by one of the certs in the CA bundle you have, you can skip the
		 * verification of the server's certificate. This makes the connection
		 * A LOT LESS SECURE.
		 *
		 * If you have a CA cert for the server stored someplace else than in the
		 * default bundle, then the CURLOPT_CAPATH option might come handy for
		 * you.
		*/
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
#endif

#ifdef SKIP_HOSTNAME_VERIFICATION
		/*
		 * If the site you're connecting to uses a different host name that what
		 * they have mentioned in their server certificate's commonName (or
		 * subjectAltName) fields, libcurl will refuse to connect. You can skip
		 * this check, but this will make the connection less secure.
		 */
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
#endif

		/* Perform the request, res will get the return code */
		res = curl_easy_perform(curl);
		/* Check for errors */
		if(res != CURLE_OK) {
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		} else {
			//printf("Bytes read %ld\r\n",out_buf->pos);
			//printf("[%.*s]\r\n",(int)out_buf.pos, out_buf.data);
		}

		/* always cleanup */
		curl_easy_cleanup(curl);
	}

	curl_global_cleanup();

	return res;
}
