
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "cJSON.h"

#define REGION "SA1"

size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
	//printf("Number of bytes %ld\r\n",nmemb);
	//printf("[%s]",ptr);

	const cJSON *regions;
	const cJSON *parameter;

	cJSON *NEM = cJSON_Parse(ptr);

	//char *string = cJSON_Print(json);
	//printf("%s\r",string);
	//free(string);

	regions = cJSON_GetObjectItemCaseSensitive(NEM, "ELEC_NEM_SUMMARY");

	cJSON_ArrayForEach(parameter, regions)
	{
		cJSON *name = cJSON_GetObjectItemCaseSensitive(parameter, "REGIONID");
		//printf("Region %s\n",name->valuestring);
		if (strcmp(name->valuestring, REGION) == 0) {
			cJSON *settlement = cJSON_GetObjectItemCaseSensitive(parameter, "SETTLEMENTDATE");
			printf("South Australia %s\r\n",settlement->valuestring);
			cJSON *price = cJSON_GetObjectItemCaseSensitive(parameter, "PRICE");
			cJSON *totaldemand = cJSON_GetObjectItemCaseSensitive(parameter, "TOTALDEMAND");
			cJSON *netinterchange = cJSON_GetObjectItemCaseSensitive(parameter, "NETINTERCHANGE");
			cJSON *scheduledgeneration = cJSON_GetObjectItemCaseSensitive(parameter, "SCHEDULEDGENERATION");
			cJSON *semischeduledgeneration = cJSON_GetObjectItemCaseSensitive(parameter, "SEMISCHEDULEDGENERATION");

			printf("Price: $%.02f\r\n",price->valuedouble);
			printf("Total Demand: %.02f MW\r\n",totaldemand->valuedouble);
			printf("Export: %.02f MW\r\n",netinterchange->valuedouble);
			printf("Scheduled Generation (Baseload): %.02f MW\r\n",scheduledgeneration->valuedouble);
			printf("Semi Scheduled Generation (Renewable): %.02f MW\r\n",semischeduledgeneration->valuedouble);

		}

	}

	cJSON_Delete(NEM);

	return(nmemb);
}

int main(void)
{
  CURL *curl;
  CURLcode res;

  curl_global_init(CURL_GLOBAL_DEFAULT);

  curl = curl_easy_init();
  if(curl) {
    curl_easy_setopt(curl, CURLOPT_URL, "https://aemo.com.au/aemo/apps/api/report/ELEC_NEM_SUMMARY");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);

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
