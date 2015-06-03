/* -s SYMBOL = find a symbol and print its type id */
/* -t TYPE_ID = find a type and describe it */
/* -c TYPE_ID = print out a typedef chain */
/* -l LABEL = print the type index referring to the label */
/* -v = print the CTF version of the file */
/* -z = print yes/no based on the compression state */
/* -h = print help */

#include <ctf/ctf.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "query.h"

#define MODE_NONE        0
#define MODE_SYMBOL      1
#define MODE_CHAIN       2
#define MODE_LABEL       3
#define MODE_VERSION     4
#define MODE_COMPRESSION 5
#define MODE_HELP        6
#define MODE_TYPE        7

int
main(int argc, char* argv[])
{
	int option;
	int retval;
	uint8_t mode;
	ctf_file file;
	char* arg;
	ctf_version version;
	
	mode = MODE_NONE;
	while ((option = getopt(argc, argv, "c:hl:s:t:v")) != -1) {
		switch(option) {
			case 'c': 
				mode = MODE_CHAIN;
				arg = strdup(optarg);
			break;

			case 'h': 
				mode = MODE_HELP;
			break;

			case 'l': 
				mode = MODE_LABEL;
				arg = strdup(optarg);
			break;

			case 's': 
				mode = MODE_SYMBOL;
				arg = strdup(optarg);
			break;

			case 't':
				mode = MODE_TYPE;
				arg = strdup(optarg);
			break;

			case 'v': 
				mode = MODE_VERSION;
			break;

			case '?':
				fprintf(stderr, "ERROR: invalid option %c\n", optopt);	
				mode = MODE_HELP;
			break;

			default: 
				fprintf(stderr, "ERROR: unknown error during option parsing\n");	
			return EXIT_FAILURE;
		}
	}

	if (mode == MODE_HELP || mode == MODE_NONE) {
		printf("ctfquery [-s SYM | -t TYPE_ID | -c TYPE_ID | -l LABEL | -v | -h ] path\n");
		return EXIT_FAILURE;
	}

	if (argc - optind < 1) {
		fprintf(stderr, "ERROR: path to an ELF file expected\n");
		return EXIT_FAILURE;
	}

	if ((retval = ctf_file_read(argv[optind], &file)) != CTF_OK) {
		fprintf(stderr, "ERROR: %s\n", ctf_get_error_string(retval));
		return EXIT_FAILURE;
	}

	switch (mode) {
		case MODE_SYMBOL:
			return find_symbol(file, arg);

		case MODE_CHAIN:
			return solve_typedef_chain(file, arg);

		case MODE_TYPE:
			return print_type(file, arg);

		case MODE_VERSION:
			ctf_file_get_version(file, &version);
			printf("%d\n", version);
		return EXIT_SUCCESS;

		default:
			printf("ERROR: unknown mode\n");
		return EXIT_FAILURE;
	}
}

