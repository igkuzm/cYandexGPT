CC=gcc
CFLAGS=-c -Wall
LDFLAGS=-lcurl
SOURCES=cYandexGPT.c cJSON.c yandex_gpt.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=yandex_gpt

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) $(SOURCES)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

*.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm $(OBJECTS)
	rm $(EXECUTABLE)

.phony: clean
