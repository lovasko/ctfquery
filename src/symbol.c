#include "query.h"

struct sym_arg
{
	char* name;
	ctf_data_object data_object;
};

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

