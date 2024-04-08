#ifndef C_YANDEX_GPT_H
#define C_YANDEX_GPT_H

// ask Yandex GPT for something
char *c_yandex_gpt_ask_for(
		const char *text, const char *id, const char *api_key);

// make summary of the text
char *c_yandex_gpt_summary(
		const char *text, const char *id, const char *api_key);

/* GPT model type */
typedef enum {
	YANDEX_GPT_MODEL_GPT,
	YANDEX_GPT_MODEL_LITE,
	YANDEX_GPT_MODEL_SUMM, // Краткий пересказ 
	YANDEX_GPT_MODEL_DS,   // Модель, дообученная в Yandex
												 // DataSphere

} YANDEX_GPT_MODEL;

int c_yandex_gpt(
		YANDEX_GPT_MODEL model,
		const char *id,           // directory or model id
		const char *api_key, 
		const char *system_text,
		const char *user_text,
		void *user_data,
		int (*callback)(void *user_data, const char *answer));

#endif /* ifndef C_YANDEX_GPT_H */
