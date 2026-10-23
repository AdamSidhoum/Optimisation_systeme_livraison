CC      = gcc
CFLAGS  = -Wall -Wextra -std=c99 -O2
OBJ     = main.o colis.o collection.o hachage.o tri.o chargement.o bench.o
EXEC    = livraison

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

main.o:       main.c colis.h collection.h hachage.h tri.h chargement.h bench.h
colis.o:      colis.c colis.h
collection.o: collection.c collection.h colis.h
hachage.o:    hachage.c hachage.h
tri.o:        tri.c tri.h colis.h
chargement.o: chargement.c chargement.h tri.h colis.h
bench.o:      bench.c bench.h collection.h hachage.h tri.h chargement.h colis.h

clean:
	rm -f $(OBJ) $(EXEC)

.PHONY: all clean
