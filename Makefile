CC = gcc
CFLAGS = -Wall -Wextra -I. -Iinternal/render -Iinternal/window
OPT = -O3
TARGET = veishuya

SANITIZE_FLAGS = -fsanitize=address -fno-omit-frame-pointer -g

SRCS = main.c internal/render/render.c internal/render/shaders.c internal/window/window.c \
       internal/veishik/veishik.c internal/objparser/parser.c \
       internal/mat4/mat4.c

OBJS = $(SRCS:.c=.o)

LIBS = -lglfw -lGL -lX11 -lpthread -lXrandr -lXi -ldl -lm

all: $(TARGET)

debug: CFLAGS += $(SANITIZE_FLAGS) -O0
debug: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LIBS) $(if $(findstring -fsanitize,$(CFLAGS)),-fsanitize=address)

%.o: %.c
	$(CC) $(CFLAGS) $(OPT) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean debug
