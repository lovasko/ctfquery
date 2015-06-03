#ifndef CTFQUERY_H
#define CTFQUERY_H

#include <ctf/ctf.h>

int
find_symbol(ctf_file file, char* symbol);

int
solve_typedef_chain(ctf_file file, char* input);

int
print_type(ctf_file file, char* input);

int
find_label(ctf_file, char* name);

#endif

