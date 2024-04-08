#include "cYandexGPT.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ini.h"

int main(int argc, char *argv[])
{
	if (argc < 2){
		printf(
				"Ask Yandex GPT for something...\n"
				"set C_YANDEX_GPT_ID and C_YNDEX_GPT_API_KEY "
				"as ENV or in ~/.cYandexGPT.conf\n"
				"Usage: \n"
				"\t%s \"TEXT\"\n", argv[0]);
		return 0;
	}

	const char *id = getenv("C_YANDEX_GPT_ID");
	const char *api_key = getenv("C_YANDEX_GPT_API_KEY");

	// read ini file
	if (!id || !api_key){
		char *home = getenv("HOME");
		if (home){
			char path[BUFSIZ];
			sprintf(path, "%s/.cYandexGPT.conf", home);
			if (!id) 
				id = ini_get(path,
						NULL, "C_YANDEX_GPT_ID");
			if (!api_key) 
				api_key = ini_get(path,
						NULL, "C_YANDEX_GPT_API_KEY");
		}
	}

	if (!id || !api_key){
		printf(
				"config is not set: C_YANDEX_GPT_ID: %s,"
				" C_YNDEX_GPT_API_KEY: %s\n", id, api_key);
		return 1;
	}

	char *str = 
		c_yandex_gpt_ask_for(argv[1], id, api_key);
	if (str) {
		printf("%s\n", str);
	}
	
	return 0;
}
