#include "../libctf/src/libctf.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NOT_FOUND 0
#define SUCCESS   1

/**
 * Print usage information to stdout.
 */
static void
usage ()
{
	printf("USAGE: ctfquery file\n");
}

/**
 * Search within all types of a file and check for type_name.
 *
 * @param type_name name of the searched type
 * @param file file containing all the types
 * @param out_file resulting type, if found
 * @return if a match was found - SUCCESS, NOT_FOUND otherwise
 */
static int
search (char* type_name, ctf_file file, ctf_type* out_type)
{
	int rv = 0;
	ctf_type type = NULL;

	int i = 0;
	while ((rv = ctf_file_get_next_type(file, type, &type)) == CTF_OK)
	{
		ctf_kind kind;
		ctf_type_get_kind(type, &kind);

		printf("%d ", i++);
		switch (kind)
		{
			case CTF_KIND_INT:
			{
				ctf_int _int;
				ctf_int_init(type, &_int);

				char* name;
				ctf_int_get_name(_int, &name);

				if (strcmp(name, type_name) == 0)
				{
					*out_type = type;
					return SUCCESS;
				}
			}
			break;

			case CTF_KIND_FLOAT:
			{
				ctf_float _float;
				ctf_float_init(type, &_float);

				char* name;
				ctf_float_get_name(_float, &name);

				if (strcmp(name, type_name) == 0)
				{
					*out_type = type;
					return SUCCESS;
				}
			}
			break;

			case CTF_KIND_TYPEDEF:
			{
				ctf_typedef _typedef;
				ctf_typedef_init(type, &_typedef);

				char* name;
				ctf_typedef_get_name(_typedef, &name);

				if (strcmp(name, type_name) == 0)
				{
					*out_type = type;
					return SUCCESS;
				}
			}
			break;

			case CTF_KIND_FWD_DECL:
			{
				ctf_fwd_decl fwd_decl;
				ctf_fwd_decl_init(type, &fwd_decl);

				char* name;
				ctf_fwd_decl_get_name(fwd_decl, &name);

				if (strcmp(name, type_name) == 0)
				{
					*out_type = type;
					return SUCCESS;
				}
			}
			break;

			case CTF_KIND_ARRAY:
			{
				ctf_array array;
				ctf_array_init(type, &array);

				char* name;
				ctf_array_get_name(array, &name);

				if (strcmp(name, type_name) == 0)
				{
					*out_type = type;
					return SUCCESS;
				}
			}
			break;

			case CTF_KIND_ENUM:
			{
				ctf_enum _enum;
				ctf_enum_init(type, &_enum);

				char* name;
				ctf_enum_get_name(_enum, &name);

				if (strcmp(name, type_name) == 0)
				{
					*out_type = type;
					return SUCCESS;
				}
			}
			break;

			/* fall through */
			case CTF_KIND_STRUCT:
			case CTF_KIND_UNION:
			{
				ctf_struct_union struct_union;
				ctf_struct_union_init(type, &struct_union);

				char* name;
				ctf_struct_union_get_name(struct_union, &name);

				if (strcmp(name, type_name) == 0)
				{
					*out_type = type;
					return SUCCESS;
				}
			}
			break;
		}
	}

	if (rv == CTF_EMPTY)
		return NOT_FOUND;

	if (rv != CTF_END)
		fprintf(stderr, "ERROR: listing types: %s\n", ctf_get_error_string(rv));

	return NOT_FOUND;
}

/**
 * Print type as in real C declaration.
 *
 * This means that arrays are printed as content_type name[length], struct are
 * listed with all their members and their respective types and so on.
 *
 * @param type type to print
 */
static void
print_type (ctf_type type)
{
	
}

/**
 * Enable user to query the CTF data stored in a file. 
 *
 * User is repeatadly prompted to enter type name, while the program searches 
 * for such type in the file's CTF data.
 * @param argc only accepted argument count is 2
 * @param argv index 1 contains the relative path to a file with CTF data
 * @return in case of wrong argc or failure while loading the data -
 *     EXIT_FAILURE, EXIT_SUCCESS otherwise
 */
int
main (int argc, char* argv[])
{
	int rv = 0;

	if (argc != 2)
	{
		fprintf(stderr, "ERROR: wrong argument count\n");
		usage();
		return EXIT_FAILURE;
	}

	/* load CTF data */
	ctf_file file;
	if ((rv = ctf_file_read(argv[1], &file)) != CTF_OK)
	{
		fprintf(stderr, "ERROR: loading CTF data: %s\n", ctf_get_error_string(rv));
		return EXIT_FAILURE;
	}

	while (1)
	{
		fprintf(stdout, "> ");
		fflush(stdout);
		
		/* create the query string */
		char query[1024];
		memset(query, '\0', 1024);
		fgets(query, 1024, stdin);
		query[strlen(query)-1] = '\0';

		/* check for exit string */
		if (query[0] == '!')
			break;

		/* check for empty string */
		if (query[0] == '\0')
			continue;

		/* search for the type */
		ctf_type type;
		if (search(query, file, &type) == SUCCESS)
			print_type(type);
		else
			printf("FAILURE: query not successful\n");	
	}

	return EXIT_SUCCESS;	
}

