#include "../include/asciify.h"
#include "../include/args.h"


int main(int argc, char** argv)
{
	generate_ascii(generate_greyscale(argc, argv));

	return 0;
}