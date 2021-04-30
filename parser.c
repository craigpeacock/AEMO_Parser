#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cjson/cJSON.h>
#include <time.h>
#include "parser.h"

char *strptime(const char *s, const char *format, struct tm *tm);

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

