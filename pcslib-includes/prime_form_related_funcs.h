
/*
 Prime form related functions
 */

void prime_form_data(int *pitches, int n, int *bin_value, int *ncar, int *tr, int *inv);
void filter_and_sort(int *pitches, long n, int *sorted, int *nelem);
void minimum_bin_value(int *vector, int n, int *mbi, int *t);

void minimum_binary_value(int *vector, int n, int *mbi, int *t);

unsigned int rotate_left(unsigned int value, int shift, unsigned int width);


int is_descendent(int *vector, int n);

