#include "cYandexGPT.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ini.h"

int main(int argc, char *argv[])
{
	if (argc < 2){
		printf(
				"As Yndex GPT for someting...\n"
				"set C_YANDEX_GPT_ID and C_YNDEX_GPT_API_KEY "
				"as ENV or in ~/.cYandexGPT.conf\n"
				"\n"
				"usage: %s [options] \"TEXT\"\n"
				"  -s  --summary\tsummarize the text\n"
				"  if TEXT is '-'\tuse stdin\n", argv[0]);
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

	// check options
	int summary = 0;
	char *text = argv[1];
	if (
			strcmp(text, "-s") == 0 ||
			strcmp(text, "--summary") == 0
			)
	{
		text = argv[2];
		summary = 1;
	}

	// check stdin
	if (strcmp(text, "-") == 0) {
		text = malloc(1);
		if (!text)
			return -1;
		// read from stdin 
		int i,   // iterator 
				k=0, // buflen
				l=0, // textlen
				n=0; // number of buffers
		char buf[BUFSIZ];
		while ((
					k=fread(
						buf, 1, BUFSIZ, stdin)))
		{
			n++;
			// realloc text
			text = realloc(text, n*BUFSIZ + 1);
			if (!text)
				return -1;

			// copy buffer to text
			for (i = 0; i < k; ++i)
				text[l++] = buf[i];	
		}

		text[l] = 0;
	}

	char *str = NULL;
	if (summary) 
		str = c_yandex_gpt_summary(text, id, api_key);
	else
		str = c_yandex_gpt_ask_for(text, id, api_key);
	
	if (str) {
		printf("%s\n", str);
	}
	
	return 0;
}
