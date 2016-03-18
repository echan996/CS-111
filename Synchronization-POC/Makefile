all: addtest sltest
addtest:
	gcc -o addtest addTest.c -std=c11 -lrt
sltest:
	gcc -o sltest slTest.c SortedList.c -std=c11 -lrt
clean:
	rm addtest
	rm sltest