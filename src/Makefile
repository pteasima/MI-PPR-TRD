CC 		= mpicc
TARGET 		= trd
FLAGS_BASE	= -Wall -O2 -std=c99
FLAGS 		= $(FLAGS_BASE)
OBJFILES 	:= $(patsubst %.c,%.o,$(wildcard *.c))


$(TARGET): $(OBJFILES)
	$(CC) -o $(TARGET) $(OBJFILES) $(FLAGS)

%.o: %.c
	$(CC) $(FLAGS) -c -o $@ $<
