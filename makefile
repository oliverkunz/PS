all: $(ps)

clean:
	rm -f *.o *~ $(ps) 

$(ps): $(ps).c
	gcc $(ps).c -g -F dwarf -o $(ps)
