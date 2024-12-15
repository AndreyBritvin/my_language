PROG_NAME=kvadratka
#TODO: make vars with paths
all:
	make front
	make back
#	make rfront
	make proc

proc:
	make -C Processor/assembler
	Processor/assembler/assembler.out programs/assembler_programs/$(PROG_NAME)_asm.txt programs/assembler_programs/compiled/$(PROG_NAME)_comp.txt

	make -C Processor/processor
	Processor/processor/proc.out programs/assembler_programs/compiled/$(PROG_NAME)_comp.txt

front:
	make -C frontend
	./frontend/frontend.out programs/$(PROG_NAME).txt programs/trees_binary/$(PROG_NAME)_tree.txt

rfront:
	make -C reverse_frontend
	./reverse_frontend/rfrontend.out programs/trees_binary/$(PROG_NAME)_tree.txt programs/rebuild_codes/$(PROG_NAME)_rebuild.txt

back:
	make -C backend
	./backend/backend.out programs/trees_binary/$(PROG_NAME)_tree.txt programs/assembler_programs/$(PROG_NAME)_asm.txt

clean:
	rm frontend/frontend.out
	rm backend/backend.out
