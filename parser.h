
struct AEMO {
	struct tm settlement;
	double price;
	double totaldemand;
	double netinterchange;
	double scheduledgeneration;
	double semischeduledgeneration;
};

void parse_aemo_request(char *ptr, struct AEMO *aemo, char *region);
