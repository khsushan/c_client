#include <stdlib.h>
#include <string.h>

#include <assert.h>
#include <stdio.h>
#include <json/json.h>

struct array_inside {
	char array[9][100];
};

char* deblank(char* input) {
	int i, j;
	char *output = input;
	for (i = 0, j = 0; i < strlen(input); i++, j++) {
		if (input[i] != ' ')
			output[j] = input[i];
		else
			j--;
	}
	output[j] = 0;
	return output;
}

void cleanup(char *string) {
	while (*string) {
		if (!isprint(*string)) {
			*string = ' ';
		}
		string++;
	}
}

void concatenate_string(char *original, char *add) {
	while (*original)
		original++;

	while (*add) {
		*original = *add;
		add++;
		original++;
	}
	*original = '\0';
}

int my_strlen(char *str) {
	int i;

	for (i = 0; str[i]; i++)
		;
	return i;
}

char* strdup(const char* org) {
	if (org == NULL )
		return NULL ;

	char* newstr = malloc(strlen(org) + 1);
	char* p;

	if (newstr == NULL )
		return NULL ;

	p = newstr;

	while (*org)
		*p++ = *org++; /* copy the string. */
	return newstr;
}

int invalidChar(char c) {
	if ((c >= 0 && c < 128)) {
		return 0;
	}
	return 1;
}

struct array_inside readConfFile() {

	FILE *fp;
	char line[200];
	char *ch;
	struct array_inside result;

	const char *s = "=";
	fp = fopen("configuration.conf", "r"); // read mode
	if (fp == NULL ) {
		perror("Error while opening the file.\n");
	}
	int i = 0;
	while (fgets(line, sizeof(line), fp)) {
		ch = strtok(line, "=");
		ch = strtok(NULL, "=");
		int j = my_strlen(ch);
		int x = 0;
		for (x = 0; x < j; x++) {
			if (ch[x] != '\0' && invalidChar(ch[x]) == 0) {
				result.array[i][x] = ch[x];
			}

		}
		i++;
	}
	fclose(fp);
	return result;
}

void write_value(int index, char *value) {
	struct array_inside result = readConfFile();
	FILE *f = fopen("configuration.conf", "w");
	if (f == NULL ) {
		printf("Error opening file!\n");
		exit(1);
	}
	const char *key_values = "";
	const char *stored_value = "";
	char name_with_extension[100];
	char header_name[8][100] = { "emm_endpoint =", "timer = ",
			"timer_interval = ", "autoload = ", "enrollment = ", "device_id = ",
			"api_endpoint = ", "access_token = " };
	int i = 0;
	for (i = 0; i < 8; i++) {
		key_values = header_name[i];
		//cleanup(key_values);
		if (index == i) {
			stored_value = value;
			//cleanup(stored_value);
		} else {
			stored_value = result.array[i];
			//cleanup(stored_value);
		}
		snprintf(name_with_extension, sizeof name_with_extension, "%s%s",
				key_values, stored_value);
		fputs(name_with_extension, f);

	}
	fclose(f);

}

void print_json_value(json_object *jobj) {

	enum json_type type;
	printf("type: ", type);
	type = json_object_get_type(jobj); /*Getting the type of the json object*/
	switch (type) {
	case json_type_boolean:
		printf("  value: %sn",
				json_object_get_boolean(jobj) ? "true" : "false");
		break;
	case json_type_double:
		printf("json_type_doublen");
		printf("          value: %lfn", json_object_get_double(jobj));
		break;
	case json_type_int:
		printf("json_type_intn");
		printf("          value: %dn", json_object_get_int(jobj));
		break;
	case json_type_string:
		printf("json_type_stringn");
		printf("          value: %sn", json_object_get_string(jobj));

		break;
	}

}

void json_parse_array(json_object *jobj, char *key) {
	printf("in parse %s", "json_parse_array");
	struct array_inside json_parse(json_object * jobj); /*Forward Declaration*/
	enum json_type type;

	json_object *jarray = jobj; /*Simply get the array*/
	if (key) {
		jarray = json_object_object_get(jobj, key); /*Getting the array if it is a key value pair*/
	}

	int arraylen = json_object_array_length(jarray); /*Getting the length of the array*/
	printf("Array Length: %dn", arraylen);
	int i;
	json_object * jvalue;

	for (i = 0; i < arraylen; i++) {
		jvalue = json_object_array_get_idx(jarray, i); /*Getting the array element at position i*/
		type = json_object_get_type(jvalue);
		if (type == json_type_array) {
			json_parse_array(jvalue, NULL );

		} else if (type != json_type_object) {
			printf("value[%d]: ", i);
			//print_json_value(jvalue);

		} else {
			json_parse(jvalue);
		}
	}
}

/*Parsing the json object*/
struct array_inside json_parse(json_object * jobj) {
	enum json_type type;
	int i = 0;
	const char *value;
	char *converted;
	struct array_inside result;
	json_object_object_foreach(jobj, key, val) {
		/*Passing through every array element*/
		//printf("type: ", type);
		type = json_object_get_type(val);
		switch (type) {
		case json_type_boolean:
		case json_type_double:
		case json_type_int:
		case json_type_string:
			value = json_object_get_string(val);
			converted = strdup(value);
			if (converted != "") {
				int j = my_strlen(converted);
				int x = 0;
				for (x = 0; x < j; x++) {
					if (value[x] != '\0') {
						result.array[i][x] = converted[x];
					}
				}
			}
			break;
		case json_type_object:
			printf("json_type_objectn");
			jobj = json_object_object_get(jobj, key);

			json_parse(jobj);
			break;
		case json_type_array:
			printf("type: json_type_array, ");
			json_parse_array(jobj, key);
			break;
		}
		i++;
	}
	return result;
}

