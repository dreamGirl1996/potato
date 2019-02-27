TARGETS=player ringmaster
all: $(TARGETS)
clean:
	rm -f $(TARGETS)
player: player.c
	gcc -g -o $@ $<
ringmaster:ringmaster.c
	gcc -g -o $@ $<
