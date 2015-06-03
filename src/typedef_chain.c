#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <m_list.h>

#include "query.h"

struct id_arg
{
	ctf_id id;
	ctf_type type;
};

static void
follow_chain(struct m_list* list, ctf_type type)
{
	ctf_kind kind;
	ctf_typedef _typedef;
	ctf_type ref_type;
	ctf_id id;
	char* name;
	char chain_link[1024];

	while (1) {
		ctf_type_get_kind(type, &kind);
		if (kind == CTF_KIND_TYPEDEF) {
			ctf_typedef_init(type, &_typedef);
			ctf_typedef_get_name(_typedef, &name);
			ctf_typedef_get_type(_typedef, &ref_type);
			ctf_type_get_id(type, &id);

			memset(chain_link, '\0', 1024);
			snprintf(chain_link, 1024, "%s (%d)", name, id);

			m_list_append(list, M_LIST_COPY_DEEP, chain_link, strlen(chain_link)+1);
			type = ref_type;
		} else {
			ctf_type_to_string(type, &name);
			ctf_type_get_id(type, &id);
			memset(chain_link, '\0', 1024);
			snprintf(chain_link, 1024, "%s (%d)", name, id);
			free(name);
			m_list_append(list, M_LIST_COPY_DEEP, chain_link, strlen(chain_link)+1);
			break;
		}
	}
}

static void
print_string(void* string, void* payload)
{
	(void)payload;
	printf("%s", string);
}

static void
compare_type_id(void* type, void* arg)
{
	ctf_id id;

	ctf_type_get_id(type, &id);
	if (id == ((struct id_arg*)arg)->id) 
		((struct id_arg*)arg)->type = type;	
}

int
solve_typedef_chain(ctf_file file, char* input)
{
	struct m_list list;
	ctf_kind kind;
	struct id_arg arg;
	long int input_num;
	char* arrow;

	/* convert and validate the input */
	errno = 0;
	input_num = strtol(input, NULL, 10);
	if (errno != 0) {
		perror("ERROR:");
		return EXIT_FAILURE;
	}

	if (input_num > 65535 || input_num < 1) {
		fprintf(stderr, "ERROR: type ID out of range\n");
		return EXIT_FAILURE;
	}

	/* find the corresponding type */
	arg.id = (ctf_id)input_num;
	arg.type = NULL;
	ctf_file_foreach_type(file, &arg, compare_type_id);
	if (arg.type == NULL) {
		printf("Type with ID '%ld' not found.\n", input_num);
		return EXIT_FAILURE;
	}

	/* check if it is a typedef */
	ctf_type_get_kind(arg.type, &kind);
	if (kind != CTF_KIND_TYPEDEF) {
		fprintf(stderr, "ERROR: type is not a typedef\n");
		return EXIT_FAILURE;
	}

	/* create and print a list of typedef names */
	m_list_init(&list);
	follow_chain(&list, arg.type);

	arrow = strdup(" -> ");
	m_list_join(&list, M_LIST_COPY_SHALLOW, arrow, 0);

	m_list_map(&list, print_string, NULL);
	printf("\n");

	return EXIT_SUCCESS;
}

