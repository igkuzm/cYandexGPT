# simple C function and program to handle with Yandex GPT

You need to get Yandex API key and directory id first

How to do this you may read: [](https://habr.com/ru/articles/780008)

### Program example
```
$ export C_YANDEX_GPT_ID="yandex_gpt_directory_id"
$ export C_YANDEX_GPT_API_KEY="yandex_gpt_api_key"
$ yandex_gpt "write in c hello world"
```

### Function example
```
#define C_YANDEX_GPT_ID ""
#define C_YANDEX_GPT_API_KEY ""

int main(int argc, char *argv[])
{
	char *str = 
		c_yandex_gpt_ask_for("write in c hello world", 
			C_YANDEX_GPT_ID, C_YANDEX_GPT_API_KEY);
	if (str) {
		printf("%s\n", str);
	}
	return 0;
}
```
