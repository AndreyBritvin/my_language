PROG_NAME=test_x86
#TODO: make vars with paths
all:
	make front
	make middle
	make back
	make rm_cm
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
	./backend/backend.out programs/trees_binary/$(PROG_NAME)_tree.txt programs/assembler_programs/$(PROG_NAME)_asm_com.txt

middle:
	make -C middle_end
	./middle_end/middle_end.out programs/trees_binary/$(PROG_NAME)_tree.txt programs/optimised/$(PROG_NAME)_opt.txt

rm_cm:
	make -C remove_comments
	./remove_comments/comm_removal.out programs/assembler_programs/$(PROG_NAME)_asm_com.txt programs/assembler_programs/$(PROG_NAME)_asm.txt

nasm:
	nasm -f elf64 -o stdlib.o    -l listing.lst programs/assembler_programs/stdlib.asm
	nasm -f elf64 -o nasm_prog.o -l listing.lst programs/assembler_programs/$(PROG_NAME)_asm_com.txt
	ld nasm_prog.o stdlib.o -o nasm_prog.elf

clean:
	rm frontend/frontend.out
	rm backend/backend.out
	rm reverse_frontend/rfrontend.out
