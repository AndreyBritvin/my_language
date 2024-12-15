PROG_NAME=test_asm

all:
	make front
	make back
#	make rfront
	make proc

proc:
	make -C Processor/assembler
	Processor/assembler/assembler.out assembler_programs/$(PROG_NAME).txt assembler_programs/compiled/$(PROG_NAME)_asm.txt

	make -C Processor/processor
	Processor/processor/proc.out assembler_programs/compiled/$(PROG_NAME)_asm.txt

front:
	make -C frontend
	./frontend/frontend.out programs/proga.txt trees_binary/test_output.txt

rfront:
	make -C reverse_frontend
	./reverse_frontend/rfrontend.out programs/proga.txt rebuild_codes/test_output.txt

back:
	make -C backend
	./backend/backend.out

clean:
	rm frontend/frontend.out
	rm backend/backend.out
