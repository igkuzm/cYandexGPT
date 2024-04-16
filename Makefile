CC=gcc
CFLAGS=-c -Wall
LDFLAGS=-lcurl
DEPS=cYandexGPT.h
SOURCES=cYandexGPT.c cJSON.c yandex_gpt.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=yandex_gpt

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm $(OBJECTS)
	rm $(EXECUTABLE)

.phony: clean
