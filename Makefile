all:
#	make front
	make back
#	make rfront
	make proc

proc:
	make -C Processor PROG_NAME=circle

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
