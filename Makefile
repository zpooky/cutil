SOURCES = $(wildcard *.c)

# BUILD_DIR = .
BUILD_DIR = make_build
OBJECTS = $(SOURCES:%.c=$(BUILD_DIR)/%.o)

DEPENDS = $(OBJECTS:.o=.d)

LDFLAGS = -fno-omit-frame-pointer -fstack-protector -fsanitize=address
#-fsanitize=undefined
#-fsanitize=thread

LDLIBS =

CC = clang
PROG = test

CFLAGS += -std=gnu11
CFLAGS += -Wall -Wextra -Wpointer-arith -Wconversion -Wshadow
CFLAGS += -Wnull-dereference -Wdouble-promotion
CFLAGS += -Wreturn-type -Wcast-align -Wcast-qual -Wuninitialized -Winit-self
CFLAGS += -Wformat=2 -Wformat-security -Wmissing-include-dirs
CFLAGS += -Wstrict-prototypes
CFLAGS += -ggdb -O0

ifeq ($(CC), gcc)
CFLAGS += -Wpedantic -Wduplicated-cond -Wlogical-op
endif

ifeq ($(CC), clang)
# CFLAGS += 
endif

.PHONEY: all
all: $(PROG)

$(PROG): $(OBJECTS)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

-include $(DEPENDS)
$(BUILD_DIR)/%.o: %.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -MMD -c $< -o $@

.PHONEY: clean
clean:
	$(RM) $(OBJECTS)
	$(RM) $(PROG)
	$(RM) $(DEPENDS)
