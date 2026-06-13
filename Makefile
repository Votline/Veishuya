CC = gcc
CFLAGS = -Wall -Wextra -I. -Iinternal/render -Iinternal/window -O3
TARGET = veishuya

SRCS = main.c internal/render/render.c internal/render/shaders.c internal/window/window.c
OBJS = $(SRCS:.c=.o)

LIBS = -lglfw -lGL -lX11 -lpthread -lXrandr -lXi -ldl -lm

ifeq ($(STATIC), 1)
    LIBS = -Wl,-Bstatic -lglfw3 -Wl,-Bdynamic -lGL -lX11 -lpthread -lXrandr -lXi -ldl -lm
endif

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f main.o internal/render/*.o internal/window/*.o $(TARGET)

.PHONY: all clean
