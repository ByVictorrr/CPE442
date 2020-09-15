# Tutorial 1: Compiling with Make and GCC

## Author(s)

Victor Delaplaine

## Makefile Template

Build a Makefile template with the following features:

1. Option to setC and C++ compiler (gcc, llvm, etc.)

2. Field to input LDFLAGS to be used by the linker

3. Field to input CFLAGS to be used by the compiler

4. Field to input C source and header files(or automated mechanism for grabbing all source files in current and child directories).

5. Field to set name of output binary6)Ability to Make with “make”or “make all”, and clean outputs with “make clean”.

## Tutorial
1. Make sure to put the Makefile in the parent or same directory as your "hello_world.c" file

2. Open up the Make file and set the corresponding variables:

	CC : This is the c/c++ complier 

	LDFLAGS : This should be a list of link ( or load ) directives such as loading the math.h library

	CFLAGS : This should be a list of complier directives/options 

	BINARY : This is the name of the desired binary output ( the program name )

3. After setting the above variables to needed specifcations, run make or make all. This will build your binary
4. If you want to get rid of any binarys or .o files run make clean
	

	
