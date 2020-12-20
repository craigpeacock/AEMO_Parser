
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <cjson/cJSON.h>
#include <time.h>
#include <stdbool.h>
#include <unistd.h>

#define REGION "SA1"

char *strptime(const char *s, const char *format, struct tm *tm);

struct buffer {
	char *data;
	size_t pos;
};

int http_json_request(struct buffer *out_buf);

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

struct AEMO {
	struct tm settlement;
	double price;
	double totaldemand;
	double netinterchange;
	double scheduledgeneration;
	double semischeduledgeneration;
};

void parse_aemo_request(char *ptr, struct AEMO *aemo)
{
	const cJSON *regions;
	const cJSON *parameter;

	cJSON *NEM = cJSON_Parse(ptr);
	if (NEM == NULL) {
		printf("Unable to parse JSON file\r\n");
		return;
	}

	regions = cJSON_GetObjectItemCaseSensitive(NEM, "ELEC_NEM_SUMMARY");
	if (regions == NULL) {
		printf("Cannot find ELEC_NEM_SUMMARY object\r\n");
		return;
	}

	cJSON_ArrayForEach(parameter, regions)
	{
		cJSON *name = cJSON_GetObjectItemCaseSensitive(parameter, "REGIONID");
		if (name != NULL) {
			//printf("Region %s\n",name->valuestring);
			if (strcmp(name->valuestring, REGION) == 0) {
				cJSON *settlement = cJSON_GetObjectItemCaseSensitive(parameter, "SETTLEMENTDATE");
				if (settlement != NULL) {
					/* String in the format of 2020-12-19T15:10:00 */
					if (strptime((char *)settlement->valuestring, "%Y-%m-%dT%H:%M:%S", &aemo->settlement) == NULL)
						printf("Unable to parse settlement time\r\n");
				}

				cJSON *price = cJSON_GetObjectItemCaseSensitive(parameter, "PRICE");
				if (price != NULL) aemo->price = price->valuedouble;
				else printf("Can't find %s\r\n", price->string);

				cJSON *totaldemand = cJSON_GetObjectItemCaseSensitive(parameter, "TOTALDEMAND");
				if (totaldemand != NULL) aemo->totaldemand = totaldemand->valuedouble;
				else printf("Can't find %s\r\n", totaldemand->string);

				cJSON *netinterchange = cJSON_GetObjectItemCaseSensitive(parameter, "NETINTERCHANGE");
				if (netinterchange != NULL) aemo->netinterchange = netinterchange->valuedouble;
				else printf("Can't find %s\r\n", netinterchange->string);

				cJSON *scheduledgeneration = cJSON_GetObjectItemCaseSensitive(parameter, "SCHEDULEDGENERATION");
				if (scheduledgeneration != NULL) aemo->scheduledgeneration = scheduledgeneration->valuedouble;
				else printf("Can't find %s\r\n", scheduledgeneration->string);

				cJSON *semischeduledgeneration = cJSON_GetObjectItemCaseSensitive(parameter, "SEMISCHEDULEDGENERATION");
				if (semischeduledgeneration != NULL) aemo->semischeduledgeneration = semischeduledgeneration->valuedouble;
				else printf("Can't find %s\r\n", semischeduledgeneration->string);
			}
		}
	}

	cJSON_Delete(NEM);
}

int print_aemo_data(struct AEMO *aemo)
{
	printf(" Settlement Period: %04d-%02d-%02dT%02d:%02d:%02d\r\n",
		aemo->settlement.tm_year + 1900,
		aemo->settlement.tm_mon + 1,
		aemo->settlement.tm_mday,
		aemo->settlement.tm_hour,
		aemo->settlement.tm_min,
		aemo->settlement.tm_sec);
	printf(" Price: $%.02f\r\n",aemo->price);
	printf(" Total Demand: %.02f MW\r\n",aemo->totaldemand);
	printf(" Export: %.02f MW\r\n",aemo->netinterchange);
	printf(" Scheduled Generation (Baseload): %.02f MW\r\n",aemo->scheduledgeneration);
	printf(" Semi Scheduled Generation (Renewable): %.02f MW\r\n",aemo->semischeduledgeneration);
}

int main(void)
{
	CURLcode res;

	char *data;

	struct buffer out_buf = {
		.data = data,
		.pos = 0
	};

	struct AEMO aemo;

	time_t now;
	struct tm timeinfo;
	int previous_period;

	#define IDLE 	0
	#define FETCH 	1

	unsigned int state = IDLE;

	FILE *fhandle;

	fhandle = fopen("AEMOdata.csv","a+");
	if (fhandle == NULL) {
		printf("Unable to open AEMOdata.csv for writing\r\n");
		exit(1);
	}

	/* Populate data */
	out_buf.data = malloc(16384);
	out_buf.pos = 0;

	res = http_json_request(&out_buf);
	if(res == CURLE_OK) {
		parse_aemo_request(out_buf.data, &aemo);

		printf("Current Settlement Period %04d-%02d-%02dT%02d:%02d:%02d\r\n",
							aemo.settlement.tm_year + 1900,
							aemo.settlement.tm_mon + 1,
							aemo.settlement.tm_mday,
							aemo.settlement.tm_hour,
							aemo.settlement.tm_min,
							aemo.settlement.tm_sec);

		previous_period = aemo.settlement.tm_min;

	} else {
		printf("Failed to download AEMO ELEC_NEM_SUMMARY\r\n");
	}

	while (1) {

		/* Get Time */
		time(&now);
		localtime_r(&now, &timeinfo);
		//printf("[%02d:%02d] %d\r\n",timeinfo.tm_min, timeinfo.tm_sec, state);

		switch (state) {

			case IDLE:
				/* 20 seconds after a 5 minute period, start fetching a new JSON file */
				if ((!(timeinfo.tm_min % 5)) & (timeinfo.tm_sec == 20)) {
					state = FETCH;
					printf("Fetching data for next settlement period\r\n");
				}
				break;

			case FETCH:
				/* Start fetching a new JSON file. We keep trying every 5 seconds until */
				/* the settlement time is different from the previous period */

				/* Allocate a modest buffer now, we can realloc later if needed */
				out_buf.data = malloc(16384);
				out_buf.pos = 0;

				/* Fetch JSON file */
				res = http_json_request(&out_buf);
				if(res == CURLE_OK) {
					/* If HTTP request was successful, parse request */
					parse_aemo_request(out_buf.data, &aemo);

					/* Print a dot each time we make a HTTP request */
					printf(".");
					fflush(stdout);

#if DEBUG
					/* Print out some statistics for debugging */
					printf("Fetched Settlement Period %04d-%02d-%02dT%02d:%02d:%02d @",
							aemo.settlement.tm_year + 1900,
							aemo.settlement.tm_mon + 1,
							aemo.settlement.tm_mday,
							aemo.settlement.tm_hour,
							aemo.settlement.tm_min,
							aemo.settlement.tm_sec);

					printf("Current Time %04d-%02d-%02dT%02d:%02d:%02d\r\n",
						timeinfo.tm_year + 1900,
						timeinfo.tm_mon + 1,
						timeinfo.tm_mday,
						timeinfo.tm_hour,
						timeinfo.tm_min,
						timeinfo.tm_sec);
#endif

					if (aemo.settlement.tm_min != previous_period) {
						/* Change in settlement time, log new period */
						previous_period = aemo.settlement.tm_min;

						printf("\r\n");
						print_aemo_data(&aemo);
						fflush(stdout);

						fprintf(fhandle,"%04d-%02d-%02d %02d:%02d:%02d,",
							timeinfo.tm_year + 1900,
							timeinfo.tm_mon + 1,
							timeinfo.tm_mday,
							timeinfo.tm_hour,
							timeinfo.tm_min,
							timeinfo.tm_sec);

						fprintf(fhandle,"%04d-%02d-%02d %02d:%02d:%02d,",
							aemo.settlement.tm_year + 1900,
							aemo.settlement.tm_mon + 1,
							aemo.settlement.tm_mday,
							aemo.settlement.tm_hour,
							aemo.settlement.tm_min,
							aemo.settlement.tm_sec);

						fprintf(fhandle,"%.02f,%.02f,%.02f,%.02f,%.02f\r\n",
							aemo.price,
							aemo.totaldemand,
							aemo.netinterchange,
							aemo.scheduledgeneration,
							aemo.semischeduledgeneration);

						fflush(fhandle);
						/* Success, go back to IDLE */
						state = IDLE;
					}
				}
				/* No luck, we will try again in five */
				sleep(5);
				break;
		}
		sleep(1);
	}
	fclose(fhandle);
}

int http_json_request(struct buffer *out_buf)
{
	CURL *curl;
	CURLcode res;

	curl_global_init(CURL_GLOBAL_DEFAULT);

	curl = curl_easy_init();
	if(curl) {
		curl_easy_setopt(curl, CURLOPT_URL, "https://aemo.com.au/aemo/apps/api/report/ELEC_NEM_SUMMARY");
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
		if(res != CURLE_OK)
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
				curl_easy_strerror(res));

		/* always cleanup */
		curl_easy_cleanup(curl);
	}

	curl_global_cleanup();

	return 0;
}
