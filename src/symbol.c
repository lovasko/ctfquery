#include <stdlib.h>
#include <errno.h>
#include <stdio.h>

#include "query.h"

struct sym_arg
{
	char* name;
	ctf_data_object data_object;
};

struct all_sym_arg {
	ctf_id id;
	unsigned int count;
};

static void
compare_symbol_type_id(void* data_object, void* arg)
{
	ctf_type type;
	ctf_id id;
	char* name;

	ctf_data_object_get_type(data_object, &type);
	ctf_type_get_id(type, &id);

	if (id == ((struct all_sym_arg*)arg)->id) {
		ctf_data_object_get_name(data_object, &name);
		printf("%s\n", name);
		((struct all_sym_arg*)arg)->count++;
	}
}

static void
compare_symbol_name(void* data_object, void* arg)
{
	char* name;

	ctf_data_object_get_name(data_object, &name);
	if (strcmp(name, ((struct sym_arg*)arg)->name) == 0) {
		((struct sym_arg*)arg)->data_object = (ctf_data_object)data_object;	
	}
}

int
find_symbol(ctf_file file, char* symbol)
{
	struct sym_arg arg;
	ctf_type type;
	ctf_id id;

	arg.name = symbol;
	arg.data_object = NULL;
	ctf_file_foreach_data_object(file, &arg, compare_symbol_name);

	if (arg.data_object == NULL) {
		printf("Symbol '%s' not found.\n", symbol);
		return EXIT_FAILURE;
	} else {
		ctf_data_object_get_type(arg.data_object, &type);
		ctf_type_get_id(type, &id);
		printf("%d\n", id);
		return EXIT_SUCCESS;
	}
}

int
find_all_symbols(ctf_file file, char* input)
{
	struct all_sym_arg arg;
	long int input_num;
	
	errno = 0;
	input_num = strtol(input, NULL, 10);
	if (errno != 0) {
		perror("ERROR");
		return EXIT_FAILURE;
	}

	if (input_num > 65535 || input_num < 1) {
		fprintf(stderr, "ERROR: type ID out of range\n");
		return EXIT_FAILURE;
	}

	arg.id = (ctf_id)input_num;
	arg.count = 0;
	ctf_file_foreach_data_object(file, &arg, compare_symbol_type_id);
	
	if (arg.count == 0) {
		printf("No symbols with type '%ld'.\n", input_num);
		return EXIT_FAILURE;
	} else
		return EXIT_SUCCESS;
}

