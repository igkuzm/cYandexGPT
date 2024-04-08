/**
 * File              : cYandexGPT.c
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 13.03.2024
 * Last Modified Date: 08.04.2024
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */
/**
 * cYandexGPT.c
 * Copyright (c) 2024 Igor V. Sementsov <ig.kuzm@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <curl/curl.h>

#include "cJSON.h"
#include "cYandexGPT.h"

struct string {
	char *ptr;
	size_t len;
};

static int 
init_string(struct string *s) {
	s->len = 0;
	void *p = malloc(s->len+1);
	if (!p)
		return -1;
	s->ptr = p;
	s->ptr[0] = '\0';
	return 0;
}

static size_t 
writefunc(void *ptr, size_t size, size_t nmemb, struct string *s)
{
	size_t new_len = s->len + size*nmemb;
	void *p = realloc(s->ptr, new_len+1);
	if (!ptr)
		return 0;
	s->ptr = p;
	memcpy(s->ptr+s->len, ptr, size*nmemb);
	s->ptr[new_len] = '\0';
	s->len = new_len;
	return size*nmemb;
}

static int 
_c_yandex_gpt_do_request(
		const char *api_key, const char *request_json, 
		void *user_data,
		int (*callback)(void *user_data, const char *answer)) 
{
	CURL *curl;
	CURLcode res;
	
	struct string s;
	if (init_string(&s)){
		fprintf(stderr, "_c_yandexgpt_do_request() failed: can't allocate memory\n");
		return -1;
	}
	
	char *url = "https://llm.api.cloud.yandex.net/foundationModels/v1/completion";
	char auth[256];
	sprintf(auth, "Authorization: Api-Key %s", api_key);
	
	// Создаем заголовок запроса
	struct curl_slist *headers = NULL;
	headers = curl_slist_append(headers, "Content-Type: application/json");
	headers = curl_slist_append(headers, auth);

	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request_json);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);
		
		// Отправляем запрос и получаем ответ
		res = curl_easy_perform(curl);
		if (res != CURLE_OK) {
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
					curl_easy_strerror(res));
			
			// Освобождаем заголовки запроса
			curl_slist_free_all(headers);
			curl_easy_cleanup(curl);
			return -1;
		}
		
		// Освобождаем заголовки запроса
		curl_slist_free_all(headers);
		curl_easy_cleanup(curl);

		// parse JSON answer
		cJSON *json = cJSON_ParseWithLength(s.ptr, s.len);
		if (json){
			if (cJSON_IsObject(json)){
				
				// get json result
				cJSON *result = cJSON_GetObjectItem(json, "result");
				if (cJSON_IsObject(result)){
					
					// get json alternatives
					cJSON *alternatives = cJSON_GetObjectItem(result, "alternatives");
					if (cJSON_IsArray(alternatives)){

						// get json message
						cJSON *item = cJSON_GetArrayItem(alternatives, 0);
						if (cJSON_IsObject(item)){

							cJSON *message = cJSON_GetObjectItem(item, "message");
							if (cJSON_IsObject(message)){

								// get text
								cJSON *text = cJSON_GetObjectItem(message, "text");
								if (cJSON_IsString(text)){
								
									// handle with answer	
									if (callback)
										callback(user_data, cJSON_GetStringValue(text));
								
									cJSON_free(json);
									free(s.ptr);
									return 0;
								}
							}
						}
					}
				}
			}
			cJSON_free(json);
		}
		
		// handle with errors
		fprintf(stderr, "cJSON parse failed: %s\n", s.ptr);
		free(s.ptr);
	}
	
	return -1;
}

static cJSON *
_c_yandex_gpt_make_request(
		YANDEX_GPT_MODEL model,
		const char *id,
		const char *system_text,
		const char *user_text)
{
	cJSON *json = cJSON_CreateObject();

	// add model
	char model_uri_str[256];
	switch (model) {
		case YANDEX_GPT_MODEL_GPT:
			sprintf(model_uri_str, "gpt://%s/yandexgpt/latest", id);
			break;
		case YANDEX_GPT_MODEL_LITE:
			sprintf(model_uri_str, "gpt://%s/yandexgpt-lite/latest", id);
			break;
		case YANDEX_GPT_MODEL_SUMM:
			sprintf(model_uri_str, "gpt://%s/summarization/latest", id);
			break;
		case YANDEX_GPT_MODEL_DS:
			sprintf(model_uri_str, "ds://%s", id);
			break;
	}

	cJSON_AddStringToObject(json, "modelUri", model_uri_str);

	// add options
	cJSON *completionOptions = cJSON_CreateObject();
	cJSON_AddFalseToObject(completionOptions, "stream");
	cJSON_AddNumberToObject(completionOptions, "temperature", 0.6);
	cJSON_AddStringToObject(completionOptions, "maxTokens", "2000");
	cJSON_AddItemToObject(json, "completionOptions", completionOptions);
	
	// add messages
	cJSON *messages = cJSON_CreateArray();
	if (system_text){
		cJSON *object = cJSON_CreateObject();	
		cJSON_AddStringToObject(object, "role", "system");
		cJSON_AddStringToObject(object, "text", system_text);
		cJSON_AddItemToArray(messages, object);
	}
	if (user_text){
		cJSON *object = cJSON_CreateObject();	
		cJSON_AddStringToObject(object, "role", "user");
		cJSON_AddStringToObject(object, "text", user_text);
		cJSON_AddItemToArray(messages, object);
	}
	cJSON_AddItemToObject(json, "messages", messages);
	
	return json;
}

int c_yandex_gpt(
		YANDEX_GPT_MODEL model,
		const char *id,
		const char *api_key, 
		const char *system_text,
		const char *user_text,
		void *user_data,
		int (*callback)(void *user_data, const char *answer)) 
{
	// make json
	cJSON *json = _c_yandex_gpt_make_request(model, id, 
			system_text, user_text);
	if (!json){
		// handle errors
		return -1;
	}

	// do request
	int ret = _c_yandex_gpt_do_request(
			api_key, cJSON_Print(json), 
			user_data, callback);

	return ret;
}


static int 
_str_callback(void *user_data, const char *answer)
{
	char **str = user_data;
	*str = strdup(answer);
	return 0;
} 

char *c_yandex_gpt_ask_for(
		const char *text, const char *id, const char *api_key)
{
	char *str = NULL;
	c_yandex_gpt(YANDEX_GPT_MODEL_GPT, id, api_key, 
			NULL, text,
		 	&str, _str_callback);	
	return str;
}

char *c_yandex_gpt_summary(
		const char *text, const char *id, const char *api_key)
{
	char *str = NULL;
	c_yandex_gpt(YANDEX_GPT_MODEL_SUMM, id, api_key, 
			NULL, text,
		 	&str, _str_callback);	
	return str;
}
