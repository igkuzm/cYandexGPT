CC=gcc
CFLAGS=-c -Wall
LDFLAGS=-lcurl
SOURCES=cYandexGPT.c cJSON.c yandex_gpt.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=yandex_gpt

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm $(OBJECTS)
	rm $(EXECUTABLE)

.phony: clean
