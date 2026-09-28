GENERATES = prog prog-a prog-so liboutput_static.a liboutput.so README *.log
TRASH = *.o *~ o.* *.log
CFLAGS = -Wall -fPIC

all: README prog prog-a prog-so liboutput_static.a liboutput.so

liboutput_static.a: fun.o const.o
	ar rcs $@ $^

liboutput.so: fun.o const.o
	$(CC) -shared -o $@ $^

prog: prog.o fun.o const.o
	$(CC) -o $@ $^

prog-a: prog.o liboutput_static.a
	$(CC) -o $@ $< -L. -loutput_static

prog-so: prog.o liboutput.so
	$(CC) -o $@ $< -L. -loutput -Wl,-rpath,.

README: prog
	./$< 2> $@

fun.o prog.o: outlib.h

test: prog prog-a prog-so
	./prog 2> prog_no_args.log || true
	./prog-a 2> prog-a_no_args.log || true
	./prog-so 2> prog-so_no_args.log || true
	
	./prog test > prog_one_arg.log 2>&1
	./prog-a test > prog-a_one_arg.log 2>&1
	./prog-so test > prog-so_one_arg.log 2>&1
	
	./prog arg1 arg2 arg3 > prog_three_args.log 2>&1
	./prog-a arg1 arg2 arg3 > prog-a_three_args.log 2>&1
	./prog-so arg1 arg2 arg3 > prog-so_three_args.log 2>&1
	
	cmp prog_no_args.log prog-a_no_args.log || echo "No args test: prog vs prog-a failed"
	cmp prog_no_args.log prog-so_no_args.log || echo "No args test: prog vs prog-so failed"
	
	cmp prog_one_arg.log prog-a_one_arg.log || echo "One arg test: prog vs prog-a failed"
	cmp prog_one_arg.log prog-so_one_arg.log || echo "One arg test: prog vs prog-so failed"
	
	cmp prog_three_args.log prog-a_three_args.log || echo "Three args test: prog vs prog-a failed"
	cmp prog_three_args.log prog-so_three_args.log || echo "Three args test: prog vs prog-so failed"

clean:
	rm -f $(TRASH)

distclean: clean
	rm -rf $(GENERATES)
