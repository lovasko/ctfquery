#include "../lctf/src/libctf.h"

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

	while ((rv = ctf_file_get_next_type(file, type, &type)) == CTF_OK)
	{
		ctf_kind kind;
		ctf_type_get_kind(type, &kind);

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

char*
type_to_string (ctf_type type)
{
	ctf_kind kind;
	ctf_type_get_kind(type, &kind);

	switch (kind)
	{
		case CTF_KIND_INT:
		{
			ctf_int _int;
			ctf_int_init(type, &_int);

			char* name;
			ctf_int_get_name(_int, &name);

			return strdup(name);
		}

		case CTF_KIND_FLOAT:
		{
			ctf_float _float;
			ctf_float_init(type, &_float);

			char* name;
			ctf_float_get_name(_float, &name);

			return strdup(name);
		}

		case CTF_KIND_POINTER:
		{
			ctf_type ref_type;
			ctf_type_init(type, &ref_type);

			char result[1024];
			char* type_string = type_to_string(ref_type);

			memset(result, '\0', 1024);
			snprintf(result, 1024, "%s*", type_string);
			free(type_string);

			return strdup(result);
		}

		/* FALL THROUGH */
		case CTF_KIND_STRUCT:
		case CTF_KIND_UNION:
		{
			ctf_struct_union struct_union;
			ctf_struct_union_init(type, &struct_union);

			char* name;
			ctf_struct_union_get_name(struct_union, &name);

			char result[1024];
			memset(result, '\0', 1024);
			snprintf(result, 1024, "%s %s", 
			    (kind == CTF_KIND_STRUCT ? "struct" : "union"), name);

			return strdup(result);
		}

		case CTF_KIND_TYPEDEF:
		{
			ctf_typedef _typedef;
			ctf_typedef_init(type, &_typedef);

			char* name;
			ctf_typedef_get_name(_typedef, &name);

			return strdup(name);
		}

		case CTF_KIND_CONST:
		{
			/* TODO special case const pointer! */
			ctf_type ref_type;
			ctf_type_init(type, &ref_type);

			char* ref_type_string = type_to_string(ref_type);

			char result[1024];
			memset(result, '\0', 1024);
			snprintf(result, 1024, "const %s", ref_type_string);
			free(ref_type_string);

			return strdup(result);
		}

		case CTF_KIND_RESTRICT:
		{
			/* TODO special case restrict pointer! */
			ctf_type ref_type;
			ctf_type_init(type, &ref_type);

			char* ref_type_string = type_to_string(ref_type);

			char result[1024];
			memset(result, '\0', 1024);
			snprintf(result, 1024, "restrict %s", ref_type_string);
			free(ref_type_string);

			return strdup(result);
		}

		case CTF_KIND_VOLATILE:
		{
			/* TODO special case volatile pointer! */
			ctf_type ref_type;
			ctf_type_init(type, &ref_type);

			char* ref_type_string = type_to_string(ref_type);

			char result[1024];
			memset(result, '\0', 1024);
			snprintf(result, 1024, "volatile %s", ref_type_string);
			free(ref_type_string);

			return strdup(result);
		}

		case CTF_KIND_FWD_DECL:
		{
			ctf_fwd_decl fwd_decl;
			ctf_fwd_decl_init(type, &fwd_decl);

			char* name;
			ctf_fwd_decl_get_name(fwd_decl, &name);

			char result[1024];
			memset(result, '\0', 1024);
			snprintf(result, 1024, "forward declaration of %s", name);

			return strdup(result);
		}

		case CTF_KIND_ARRAY:
		{
			ctf_array array;
			ctf_array_init(type, &array);
			
			ctf_array_length length;
			ctf_array_get_length(array, &length);

			ctf_type content_type;
			ctf_array_get_content_type(array, &content_type);

			char* type_string = type_to_string(content_type);

			char result[1024];
			memset(result, '\0', 1024);
			snprintf(result, 1024, "%s [%d]", type_string, length);
			free(type_string);

			return strdup(result);
		}

		case CTF_KIND_ENUM:
		{
			ctf_enum _enum;
			ctf_enum_init(type, &_enum);

			char* name;
			ctf_enum_get_name(_enum, &name);

			char result[1024];
			memset(result, '\0', 1024);
			snprintf(result, 1024, "enum %s", name);
			
			return strdup(result);
		}

		case CTF_KIND_FUNC:
		{
			ctf_function function;
			ctf_function_init(type, &function);

			ctf_type return_type;
			ctf_function_get_return_type(function, &return_type);

			char* return_type_string;
			return_type_string = type_to_string(return_type);

			char result[1024];
			memset(result, '\0', 1024);
			snprintf(result, 1024, "%s ()", return_type_string);
			free(return_type_string);

			return strdup(result);
		}

		case CTF_KIND_NONE:
			return strdup("none");
	}

	char result[1024];
	memset(result, '\0', 1024);

	ctf_id id;
	ctf_type_get_id(type, &id);

	snprintf(result, 1024, "%d %d unresolvable", id, kind);
	return strdup(result);
}

static void
print_type_name (ctf_type type)
{
	ctf_kind kind;
	ctf_type_get_kind(type, &kind);

	if (kind == CTF_KIND_INT)
	{
		ctf_int _int;
		ctf_int_init(type, &_int);

		char* name;
		ctf_int_get_name(_int, &name);

		printf("%s", name);
	}
}

static void
print_typedef_chain (ctf_type type)
{
	ctf_kind kind;
	ctf_type_get_kind(type, &kind);

	if (kind == CTF_KIND_TYPEDEF)
	{
		ctf_typedef _typedef;
		ctf_typedef_init(type, &_typedef);

		char* name;
		ctf_typedef_get_name(_typedef, &name);

		printf("%s -> ", name);
		
		ctf_type ref_type;
		ctf_typedef_get_type(_typedef, &ref_type);

		print_typedef_chain(ref_type);
	}
	else
	{
		print_type_name(type);
		printf("\n");
	}
}

#define STRUCT_NORMAL               1
#define STRUCT_LINKED_LIST          2
#define STRUCT_BINARY_TREE          4
#define STRUCT_N_TREE               8
#define STRUCT_SYS_QUEUE_SLIST     16 
#define STRUCT_SYS_QUEUE_LIST      32
#define STRUCT_SYS_QUEUE_STAILQ    64
#define STRUCT_SYS_QUEUE_TAILQ    128
#define STRUCT_SYS_TREE_SPLAY     256
#define STRUCT_SYS_TREE_RED_BLACK 512

static char*
struct_type_to_string (int type)
{
	if (type == STRUCT_NORMAL)
		return strdup("normal");

	static const char* table[] = {
		"normal",
		"linked list",
		"binary tree",
		"n-ary tree",
		"queue(3) slist",
		"queue(3) list",
		"queue(3) tailq",
		"queue(3) stailq",
		"tree(3) splay",
		"tree(3) red-black"
	};

	char result[2048];
	memset(result, '\0', 2048);

	unsigned int index = 0;

	for (unsigned int i = 2, ti = 1; i < 512; i *= 2, ti++)
	{
		if (type & i) 
		{
			snprintf(&result[index], 2048-index, "%s, ", table[ti]);
			index += strlen(table[ti]);
		}
	}

	result[strlen(result)-2] = '\0';

	return strdup(result);
}

#define GUESS_DESC_POINTER            1
#define GUESS_DESC_POINTER_TO_POINTER 2
#define GUESS_DESC_INT                3

static int
guess_check_member (ctf_member member, ctf_id id, int desc, char* name)
{
	printf("Checking struct member\n");
	ctf_type type;
	ctf_member_get_type(member, &type);

	ctf_kind kind;
	ctf_type_get_kind(type, &kind);

	char* _name;
	ctf_member_get_name(member, &_name);

	if (strcmp(name, _name) != 0)
		return 0;

	if (desc == GUESS_DESC_INT)
		return (kind == CTF_KIND_INT);
	else if (desc == GUESS_DESC_POINTER)
	{
		if (kind != CTF_KIND_POINTER)
			return 0;

		ctf_type ref_type;
		ctf_type_init(type, &ref_type);

		ctf_id ref_id;
		ctf_type_get_id(ref_type, &ref_id);

		return (ref_id == id);
	}
	else if (desc == GUESS_DESC_POINTER_TO_POINTER)
	{
		if (kind != CTF_KIND_POINTER)
			return 0;

		ctf_type ref_type;
		ctf_type_init(type, &ref_type);

		ctf_kind ref_kind;
		ctf_type_get_kind(ref_type, &ref_kind);

		if (ref_kind != CTF_KIND_POINTER)
			return 0;

		ctf_type ref_ref_type;
		ctf_type_init(ref_type, &ref_ref_type);

		ctf_id ref_ref_id;
		ctf_type_get_id(ref_ref_type, &ref_ref_id);

		return (ref_ref_id == id);
	}
	else
		return 0;
}

static int
guess_check_struct (ctf_struct_union struct_union, ctf_id id, int* desc, 
    char* names[], size_t size)
{
	printf("Checking struct\n");
	ctf_count member_count;
	ctf_struct_union_get_member_count(struct_union, &member_count);

	if (member_count != size)
		return 0;

	size_t checksum = 0;
	unsigned int idx = 0;

	ctf_member member = NULL;
	while (ctf_struct_union_get_next_member(struct_union, member, &member) 
	    == CTF_OK)
	{
		checksum += guess_check_member(member, id, desc[idx], names[idx]);
		idx++;
	}

	return (checksum == size);
}

static int
guess_struct_type (ctf_type type)
{
	printf("Guessing struct type\n");
	int result = STRUCT_NORMAL;
	int self_ref_with_pointer = 0;

	ctf_struct_union struct_union;
	ctf_struct_union_init(type, &struct_union);

	ctf_id id;
	ctf_type_get_id(type, &id);

	ctf_member member = NULL;
	while (ctf_struct_union_get_next_member(struct_union, member, &member) 
	    == CTF_OK)
	{
		printf("Member!\n");
		printf("Member! Getting type\n");
		ctf_type member_type;
		ctf_member_get_type(member, &member_type);
		
		printf("Member! Getting kind\n");
		ctf_kind member_kind;
		ctf_type_get_kind(member_type, &member_kind);

		printf("Member! Starting checks\n");
		if (member_kind == CTF_KIND_POINTER)
		{
			printf("Checking simple pointer\n");
			ctf_type ref_type;
			ctf_type_init(member_type, &ref_type);

			ctf_id ref_id;
			ctf_type_get_id(ref_type, &ref_id);

			if (id == ref_id)
				self_ref_with_pointer++;
		}

		if (member_kind == CTF_KIND_STRUCT)
		{
			char* member_name;
			ctf_member_get_name(member, &member_name);
			printf("Member '%s' seems to be a struct.\n", member_name);

			ctf_struct_union member_su;
			ctf_struct_union_init(member_type, &member_su);
			printf("Initialized the struct_union.\n");

			char* name;
			ctf_struct_union_get_name(member_su, &name);
			printf("Checking member struct with name '%s'\n", name);

			if (name != NULL && name[0] == '\0')
			{
				/* check for single linked list */
				int slist_description[] = {GUESS_DESC_POINTER};
				char* slist_names[] = {"le_next"};

				if (guess_check_struct(member_su, id, slist_description, slist_names, 1))
					result |= STRUCT_SYS_QUEUE_SLIST;

				/* check for double linked list */
				int list_description[] = {GUESS_DESC_POINTER,
				    GUESS_DESC_POINTER_TO_POINTER};
				char* list_names[] = {"le_next", "le_prev"};

				if (guess_check_struct(member_su, id, list_description, list_names, 2))
					result |= STRUCT_SYS_QUEUE_LIST;

				/* check for tail queue */
				int stailq_description[] = {GUESS_DESC_POINTER};
				char* stailq_names[] = {"stqe_next"};

				if (guess_check_struct(member_su, id, stailq_description, stailq_names, 
				    1))
					result |= STRUCT_SYS_QUEUE_STAILQ;

				/* check for tail queue */
				int tailq_description[] = {GUESS_DESC_POINTER,
				    GUESS_DESC_POINTER_TO_POINTER};
				char* tailq_names[] = {"tqe_next", "tqe_prev"};

				if (guess_check_struct(member_su, id, tailq_description, tailq_names, 2))
					result |= STRUCT_SYS_QUEUE_TAILQ;

				/* check for splay tree */
				int splay_description[] = {GUESS_DESC_POINTER, GUESS_DESC_POINTER};
				char* splay_names[] = {"spe_left", "spe_right"};

				if (guess_check_struct(member_su, id, splay_description, tailq_names, 2))
					result |= STRUCT_SYS_TREE_SPLAY;

				/* check for red black tree */
				int red_black_description[] = {GUESS_DESC_POINTER, GUESS_DESC_POINTER,
				    GUESS_DESC_POINTER, GUESS_DESC_INT};
				char* red_black_names[] = {"rbe_left", "rbe_right", "rbe_parent",
				    "rbe_color"};

				if (guess_check_struct(member_su, id, red_black_description, 
				    red_black_names, 4))
					result |= STRUCT_SYS_TREE_RED_BLACK;

			}
		}
	}

	if (self_ref_with_pointer == 1)
		result |= STRUCT_LINKED_LIST;
	else if (self_ref_with_pointer == 2)
		result |= STRUCT_BINARY_TREE;
	else if (self_ref_with_pointer > 2)
		result |= STRUCT_N_TREE;

	return result;
}

static void
print_struct_union (ctf_type type, unsigned int indent)
{
	printf("Printing struct or union\n");
	ctf_struct_union struct_union;
	ctf_struct_union_init(type, &struct_union);

	ctf_kind kind;
	ctf_type_get_kind(type, &kind);

	if (kind == CTF_KIND_STRUCT)
	{
		int struct_type = guess_struct_type(type);
		char* type_string = struct_type_to_string(struct_type);
		printf("This struct seems to be %s.\n", type_string);	
		free(type_string);
	}

	printf("{\n");

	ctf_member member = NULL;
	while (ctf_struct_union_get_next_member(struct_union, member, &member) 
	    == CTF_OK)
	{
		ctf_type member_type;
		ctf_member_get_type(member, &member_type);

		char* member_name;
		ctf_member_get_name(member, &member_name);

		char* member_type_string = type_to_string(member_type);

		for (unsigned int i = 0; i < indent+1; i++)
			printf("  ");

		printf("%s %s\n", member_type_string, member_name);
			
		free(member_type_string);
	}

	printf("}\n");
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
	printf("Printing type\n");
	ctf_kind kind;
	ctf_type_get_kind(type, &kind);

	if (kind == CTF_KIND_TYPEDEF)
		print_typedef_chain(type);

	if (kind == CTF_KIND_STRUCT
	 || kind == CTF_KIND_UNION)
	 print_struct_union(type, 0);
}

/**
 * Enable user to query the CTF data stored in a file. 
 *
 * User is repeatedly prompted to enter type name, while the program searches 
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

