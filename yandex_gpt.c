#include "cYandexGPT.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ini.h"

static int show_help(char *argv[]){
	printf(
				"As Yndex GPT for someting...\n"
				"set C_YANDEX_GPT_ID and C_YNDEX_GPT_API_KEY "
				"as ENV or in ~/.cYandexGPT.conf\n"
				"\n"
				"usage: %s [options] \"TEXT\"\n"
				"  -h  --help\tshow this message and exit\n"
				"  -s  --summary\tsummarize the TEXT\n"
				"  -a  --append\tappend stdin to TEXT\n"
				"  -           \tread from stdin\n"
				, argv[0]);
	return 0;
}

int main(int argc, char *argv[])
{
	// check options
	int summary = 0;
	int read_stdin = 0;
	int append = 0;
	int argv_n = 1;
	char *text = argv[argv_n];

	if (!text)
		return show_help(argv);
	
	if (
			strcmp(text, "-h") == 0 ||
			strcmp(text, "--help") == 0
			)
		return show_help(argv);
	
	if (
			strcmp(text, "-s") == 0 ||
			strcmp(text, "--summary") == 0
			)
	{
		text = argv[++argv_n];
		summary = 1;
		// check text again	
		if (!text)
			return show_help(argv);
	}
	if (
			strcmp(text, "-a") == 0 ||
			strcmp(text, "--append") == 0
			)
	{
		text = argv[++argv_n];
		append = 1;
		// check text again	
		if (!text)
			return show_help(argv);
	}
	if (
			strcmp(text, "-") == 0
			)
	{
		read_stdin = 1;
	}


	// summarize arguments to text
	if (!read_stdin){
		int i,k, // iterators
				l=0, // length 
				s=1; // allocated size
		char *t = malloc(1);
		if (!t){
			perror("malloc");
			return -1;
		}
		for (i = argv_n; i < argc; ++i) {
			int len = strlen(argv[i]);
			s += len;
			// realloc
			t = realloc(t, s);
			if (!t){
				perror("realloc");
				return -1;
			}
			// copy text
			for (k=0;k<len;++k)
				t[l++] = (argv[i])[k];

			// add space after argument
			t[l++] = ' ';
		}

		// terminate
		t[l] = 0;
		text = t;
	}
	
	// get enviroment
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

	// check stdin
	if (read_stdin || append) {
		char buf[BUFSIZ], // buffer
				 *s;          // allocated text string
		
		int i,   // iterator 
				k=0, // buflen
				l=0, // textlen
				t=1, // text allocated mem
				n=0; // number of buffers

		s = malloc(t);
		if (!s)
			return -1;
		
		// add text if append
		if (append && text){
			int len = strlen(text);
			t = len + 1;
			s = realloc(s, t);
			if (!s)
				return -1;

			// copy to text
			for (i = 0; i < len; ++i)
				s[l++] = text[i];	

			// new line before append from stdin
			s[l++] = '\n';
		}

		// read from stdin 
		while ((
					k=fread(
						buf, 1, BUFSIZ, stdin)))
		{
			n++;
			// realloc text
			s = realloc(s, n*BUFSIZ + t);
			if (!s)
				return -1;

			// copy buffer to text
			for (i = 0; i < k; ++i)
				s[l++] = buf[i];	
		}

		// terminate string
		s[l] = 0;

		text = s;
	}

	char *str = NULL;
	if (summary) 
		str = c_yandex_gpt_summary(text, id, api_key);
	else
		str = c_yandex_gpt_ask_for(text, id, api_key);
	
	if (str) 
		printf("%s\n", str);
	
	
	return 0;
}
