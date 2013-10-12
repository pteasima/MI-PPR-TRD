CC 		= gcc
TARGET 		= trd
FLAGS_BASE	= -Wall -O2
FLAGS 		= $(FLAGS_BASE)
OBJFILES 	:= $(patsubst %.c,%.o,$(wildcard *.c))


$(TARGET): $(OBJFILES)
	$(CC) -o $(TARGET) $(OBJFILES) $(FLAGS)

%.o: %.c
	$(CC) $(FLAGS) -c -o $@ $<
