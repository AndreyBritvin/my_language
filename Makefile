all:
#	make front
	make back

front:
	make -C frontend
	./frontend/frontend.out programs/proga.txt trees_binary/test_output.txt

back:
	make -C backend
	./backend/backend.out

clean:
	rm frontend/frontend.out
	rm backend/backend.out
