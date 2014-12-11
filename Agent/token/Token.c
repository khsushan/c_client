/*
 * Token.c
 *
 *  Created on: Nov 12, 2014
 *      Author: ushan
 */
#include <curl/curl.h>
#include "filehandler.h"

struct array_inside result;

size_t write_clbk(void *data, size_t blksz, size_t nblk, void *ctx) {
	static size_t sz = 0;
	size_t currsz = blksz * nblk;

	size_t prevsz = sz;
	sz += currsz;
	void *tmp = realloc(*(char **) ctx, sz);
	if (tmp == NULL ) {
		// handle error
		free(*(char **) ctx);
		*(char **) ctx = NULL;
		return 0;
	}
	*(char **) ctx = tmp;

	memcpy(*(char **) ctx + prevsz, data, currsz);
	return currsz;
}

void send_request(char* jsonObj, char *type) {
	CURL *curl;
	CURLcode res;
	char *url;
	curl = curl_easy_init();
	struct curl_slist *headers = NULL;
	headers = curl_slist_append(headers, "Accept: application/json");
	headers = curl_slist_append(headers, "Content-Type: application/json");
	headers = curl_slist_append(headers, "charsets: utf-8");
	if (curl) {
		char *buf = NULL;
		struct array_inside response;
		if ((strcmp(type, "token") == 0)) {
			url = deblank(result.array[0]);
			curl_easy_setopt(curl, CURLOPT_URL, url);
			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
			curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonObj);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buf);
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_clbk);
			printf("%s", "requesting token\n");
			printf("type %s", type);
			while ((strcmp(response.array[0], "false") != 0)) {
				printf("%s", "please claim your device \n");
				res = curl_easy_perform(curl);
				json_object * jobj = json_tokener_parse(buf);
				response = json_parse(jobj);
			}
			char *token = response.array[1];
			cleanup(token);
			write_value(7, token);
			strncpy(result.array[7], token, 100);
			printf("error: %s \n", response.array[0]);
			printf("token : %s \n", response.array[1]);
		} else if ((strcmp(type, "publish") == 0)) {
			url = deblank(result.array[6]);
			printf("API end point %s", url);
			url[strlen(url) - 1] = '\0';
			char header_ary[60];
			char *token = deblank(result.array[7]);
			token[strcspn(token, "\n")] = '\0';
			cleanup(token);
			token[strcspn(token, "\n")] = '\0';
			printf("token value is %s", token);
			snprintf(header_ary, sizeof header_ary, "%s%s",
					"Authorization:  Bearer ", token);
			char *header = &header_ary[0];
			headers = curl_slist_append(headers, header);
			headers = curl_slist_append(headers,
					"Content-Type: application/json");
			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
			curl_easy_setopt(curl, CURLOPT_URL, url);
			curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT"); /* !!! */
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonObj);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buf);
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_clbk);
			res = curl_easy_perform(curl);
			if (res == 0 && buf == "null") {
				printf("result : %s \n", "data published");
				printf("result is buf %s", buf);
			} else {
				printf("result : %s \n", "data publishing error");
			}
		}
		curl_easy_cleanup(curl);
	}

}

int main(void) {
	result = readConfFile(0);
	char id_ary[50];
	char *str_id_s = "{\"deviceID\" : \"";
	char *id_value = deblank(result.array[5]);
	id_value[strcspn(id_value, "\n")] = '\0';
	snprintf(id_ary,sizeof id_ary,"%s%s%s",str_id_s,id_value,"\"}");
	char *token = deblank(result.array[7]);
	char *type = "publish";
	char* device_id = &id_ary[0];
	if (strcmp(token, "not_set") == 10) {
		type = "token";

		send_request(device_id, type);
	}
	//publishing data
	char *first_part = "{\"dataStream\":{\"data\":\"";
	char *data, text[5];
	sprintf(text, "%d", (rand() % 100));
	data = text;
	char jsonObj[150];
	char *str_id = "\",\"id\":\"";
	char *id = deblank(result.array[5]);
	id[strlen(id) - 1] = '\0';
	printf("id is %s", id);
	char *lastPart = "\",\"stream\": \"Color\",\"version\": \"1.0.0\" }}";
	snprintf(jsonObj, sizeof jsonObj, "%s%s%s%s%s", first_part, data, str_id,
			id, lastPart);
	char* p = &jsonObj[0];
	printf("char pointer json obj %s \n", p);
	type = "publish";
	send_request(p, type);

	return 0;
}

