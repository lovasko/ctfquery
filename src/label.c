#include "query.h"

struct label_arg
{
	char* name;
	ctf_label label;
};

static void
compare_label_name(void* label, void* arg)
{
	char* name;

	ctf_label_get_name(label, &name);
	if (strcmp(name, ((struct label_arg*)arg)->name) == 0) {
		((struct label_arg*)arg)->label = label;	
	}
}

int
find_label(ctf_file file, char* name)
{
	struct label_arg arg;
	ctf_label_index index;

	arg.name = name;
	arg.label = NULL;
	ctf_file_foreach_label(file, &arg, compare_label_name);

	if (arg.label == NULL) {
		printf("Label '%s' not found.\n", name);
		return EXIT_FAILURE;
	} else {
		ctf_label_get_index(arg.label, &index);
		printf("%d\n", index);
		return EXIT_SUCCESS;
	}
}

