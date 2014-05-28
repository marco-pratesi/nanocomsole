#CC = gcc -Wall -pedantic -Wunused
STRIP ?= strip
EXECUTABLE=nanocomsole

$(EXECUTABLE): *.c *.h
	$(CC) *.c -o $@
	$(STRIP) $@

clean:
	rm -rf $(EXECUTABLE)

