
struct buffer {
	char *data;
	size_t pos;
};

size_t header_callback(char *buffer, size_t size, size_t nitems, void *user_data);
size_t write_callback(char *ptr, size_t size, size_t nmemb, void *user_data);
int http_json_request(struct buffer *out_buf);

