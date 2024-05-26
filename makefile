C = gcc

# Options de compilation
CFLAGS = -Wall -Wextra -std=c11

# Nom de l'exécutable
TARGET = cerjo

# Fichiers source
SRCS = main.c

# Fichiers objets
OBJS = $(SRCS:.c=.o)

# Règle par défaut : compilation de l'exécutable
all: $(TARGET)

# Compilation de l'exécutable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Compilation des fichiers objets
%.o: %.c
	$(CC) $(CFLAGS) -c $<

# Nettoyage des fichiers objets et de l'exécutable
clean:
	rm -f $(OBJS) $(TARGET)

# Exécution du programme
run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
