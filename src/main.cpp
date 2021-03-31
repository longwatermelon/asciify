#include "../include/asciify.h"
#include "../include/args.h"


int main(int argc, char** argv)
{
	asciify::generate_ascii(asciify::generate_greyscale(argc, argv));

	return 0;
}