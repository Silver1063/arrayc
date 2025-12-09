#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "array.h"

int main(int argc, char **argv) {
	Array *a, *b;
	// array_new
	a = array_new(int);

	assert(array_element_size(a) == sizeof(int));
	assert(array_capacity(a) == MIN_CAPACITY);
	assert(array_element_free(a) == NULL);
	assert(array_size(a) == 0);
	assert(array_data(a));

	array_free(a);

	// array_new_with_size
	a = array_new_with_size(int, 32);

	assert(array_element_size(a) == sizeof(int));
	assert(array_capacity(a) == 64);
	assert(array_element_free(a) == NULL);
	assert(array_size(a) == 32);
	assert(array_data(a));

	array_free(a);

	// array_set_element_free
	a = array_new(char *);

	assert(array_element_free(a) == NULL);
	array_set_element_free(a, string_free);
	assert(array_element_free(a) == string_free);

	array_free(a);

	// array_duplicate
	a = array_new_with_size(int, 32);
	array_at(a, int, 0) = 23;
	array_at(a, int, 23) = -4;
	array_at(a, int, 31) = 55;

	b = array_duplicate(a);

	assert(a != b);
	assert(array_size(a) == array_size(b));
	assert(array_capacity(a) == array_capacity(b));
	assert(array_element_size(a) == array_element_size(b));
	assert(array_element_free(a) == array_element_free(b));

	for (size_t i = 0; i < array_size(a); i++) {
		assert(array_at(a, int, i) == array_at(b, int, i));
	}

	array_free(a);
	array_free(b);

	// array_duplicate_custom
	a = array_new_with_size(char *, 16);
	array_set_element_free(a, string_free);

	char *hello = malloc(32);
	strcpy(hello, "Hello!");
	char *world = malloc(32);
	strcpy(world, "World!");

	array_at(a, char *, 0) = hello;
	array_at(a, char *, 1) = world;

	array_duplicate_custom(a, string_duplicate);

	assert(strcmp(array_at(a, char *, 0), array_at(b, char *, 0)) == 0);
	assert(strcmp(array_at(a, char *, 1), array_at(b, char *, 1)) == 0);

	assert(array_at(a, char *, 0) != array_at(b, char *, 0));
	assert(array_at(a, char *, 1) != array_at(b, char *, 1));

	array_free(a);
	array_free(b);

	// array_size
	a = array_new_with_size(int, 12);

	assert(array_size(a) == 12);

	array_free(a);

	// array_empty
	a = array_new(int);

	assert(array_empty(a) == true);
	for (size_t i = 0; i < 64; i++) {
		array_push_back(a, &i);
	}
	assert(array_empty(a) == false);

	array_free(a);

	// array_find
	a = array_new(int);

	assert(array_find(a, &(int){ 23 }) == -1);

	for (size_t i = 0; i < 64; i++) {
		int v = rand() % (201) - 100;
		v = v == 23 ? 0 : v;
		array_push_back(a, &v);
	}
	int t = array_at(a, int, 34);

	assert(array_find(a, &t) == 34);
	assert(array_find(a, &(int){ 23 }) == -1);

	array_free(a);

	// array_find_custom
	a = array_new(char *);
	array_set_element_free(a, string_free);

	char *s_fail = "Anything";
	char *s_find = "String 23";

	assert(array_find_custom(a, string_compare, &s_find) == -1);

	for (size_t i = 0; i < 64; i++) {
		char *s = malloc(32);
		snprintf(s, 32, "String %zu", i);
		array_push_back(a, &s);
	}

	assert(array_find_custom(a, string_compare, &s_find) == 23);
	assert(array_find_custom(a, string_compare, &s_fail) == -1);

	array_free(a);

	// array_contains
	a = array_new(int);

	assert(array_contains(a, &(int){ 23 }) == false);

	array_push_back(a, &(int){ 54 });
	array_push_back(a, &(int){ 23 });
	array_push_back(a, &(int){ 64 });

	assert(array_contains(a, &(int){ 23 }) == true);
	assert(array_contains(a, &(int){ -23 }) == false);

	array_free(a);

	// array_contains_custom
	a = array_new(char *);
	array_set_element_free(a, string_free);

	s_fail = "Anything";
	s_find = "String 23";

	assert(array_contains_custom(a, string_compare, &s_find) == false);

	for (size_t i = 0; i < 64; i++) {
		char *s = malloc(32);
		snprintf(s, 32, "String %zu", i);
		array_push_back(a, &s);
	}

	assert(array_contains_custom(a, string_compare, &s_find) == true);
	assert(array_contains_custom(a, string_compare, &s_fail) == false);

	array_free(a);

	// array_count
	a = array_new(int);

	assert(array_count(a, &(int){ 23 }) == 0);

	array_push_back(a, &(int){ 0 });
	array_push_back(a, &(int){ 23 });
	array_push_back(a, &(int){ 2 });
	array_push_back(a, &(int){ 23 });
	array_push_back(a, &(int){ -23 });
	array_push_back(a, &(int){ 32 });
	array_push_back(a, &(int){ 23 });
	array_push_back(a, &(int){ 1 });

	assert(array_count(a, &(int){ 0 }) == 1);
	assert(array_count(a, &(int){ 2 }) == 1);
	assert(array_count(a, &(int){ 63 }) == 0);
	assert(array_count(a, &(int){ 23 }) == 3);

	array_free(a);

	// array_count_custom
	a = array_new(char *);
	array_set_element_free(a, string_free);

	assert(array_count_custom(a, string_compare, &(char *){ "String 2" }) == 0);

	for (size_t i = 0; i < 4; i++) {
		char *s = malloc(32);
		snprintf(s, 32, "String %zu", i);
		array_push_back(a, &s);
	}
	for (size_t i = 0; i < 8; i++) {
		char *s = malloc(32);
		snprintf(s, 32, "String %zu", i);
		array_push_back(a, &s);
	}
	for (size_t i = 0; i < 12; i++) {
		char *s = malloc(32);
		snprintf(s, 32, "String %zu", i);
		array_push_back(a, &s);
	}

	assert(array_count_custom(a, string_compare, &(char *){ "String 2" }) == 3);
	assert(array_count_custom(a, string_compare, &(char *){ "String 5" }) == 2);
	assert(array_count_custom(a, string_compare, &(char *){ "String 11" }) == 1);
	assert(array_count_custom(a, string_compare, &(char *){ "String -2" }) == 0);

	// array_clear
	// reusing array from last test
	assert(array_size(a) == 24);
	assert(array_capacity(a) == 32);

	array_clear(a);

	assert(array_size(a) == 0);
	assert(array_capacity(a) == 8);

	array_free(a);

	// array_reverse
	a = array_new(int);

	for (size_t i = 0; i < 16; i++) {
		array_push_back(a, &(int){ i });
	}

	array_reverse(a);

	for (size_t i = 0; i < 16; i++) {
		assert(array_at(a, int, i) == 15 - i);
	}

	array_free(a);

	// array_resize
	a = array_new(int);

	for (size_t i = 0; i < 8; i++) {
		array_push_back(a, &(int){ i });
	}

	assert(array_size(a) == 8);
	assert(array_capacity(a) == 16);

	array_resize(a, 16);

	assert(array_size(a) == 16);
	assert(array_capacity(a) == 32);

	array_resize(a, 4);

	assert(array_size(a) == 4);
	assert(array_capacity(a) == 8);

	array_free(a);

	// array_scale_capacity
	// called automatically by array_push_back/other array functions
	a = array_new(int);

	for (size_t i = 0; i < 8; i++) {
		array_push_back(a, &(int){ i });
	}

	assert(array_size(a) == 8);
	assert(array_capacity(a) == 16);

	for (size_t i = 0; i < 8; i++) {
		array_push_back(a, &(int){ i });
	}

	assert(array_size(a) == 16);
	assert(array_capacity(a) == 32);

	// array_shrink_to_fit
	a = array_new(int);

	for (size_t i = 0; i < 23; i++) {
		array_push_back(a, &i);
	}

	assert(array_size(a) == 23);
	assert(array_capacity(a) == 32);

	array_shrink_to_fit(a);

	assert(array_size(a) == 23);
	assert(array_capacity(a) == 23);

	array_free(a);

	// array_at
	a = array_new(int);

	for (size_t i = 0; i < 24; i++) {
		array_push_back(a, &i);
	}

	assert(array_at(a, int, 0) == 0);
	assert(array_at(a, int, 7) == 7);
	assert(array_at(a, int, -1) == 23);
	assert(array_at(a, int, -3) == 21);
	assert(_array_at(a, 100) == NULL);

	array_free(a);

	// array_unsafe_at
	a = array_new(int);

	for (size_t i = 0; i < 24; i++) {
		array_push_back(a, &i);
	}

	assert(array_unsafe_at(a, int, 0) == 0);
	assert(array_unsafe_at(a, int, 7) == 7);
	assert(array_unsafe_at(a, int, -1) == 23);
	assert(array_unsafe_at(a, int, -3) == 21);
	assert(_array_unsafe_at(a, 100)); // Don't know whats there, but it not NULL

	array_free(a);

	// array_set
	a = array_new_with_size(int, 24);

	for (size_t i = 0; i < 24; i++) {
		array_set(a, i, &i);
	}

	for (size_t i = 0; i < 24; i++) {
		assert(array_at(a, int, i) == i);
	}

	array_free(a);

	// array_get
	a = array_new_with_size(int, 24);

	for (size_t i = 0; i < 24; i++) {
		array_set(a, i, &i);
	}

	for (size_t i = 0; i < 24; i++) {
		assert(*array_get(a, int, i) == i);
	}

	array_free(a);

	// array_insert_at
	a = array_new_with_size(int, 4);

	array_insert_at(a, 0, &(int){ 6 });
	array_insert_at(a, 3, &(int){ 7 });
	array_insert_at(a, -1, &(int){ 8 });

	assert(array_at(a, int, 0) == 6);
	assert(array_at(a, int, 3) == 7);
	assert(array_at(a, int, -2) == 8);

	// array_remove_at
	// reusing a from previous
	array_remove_at(a, 0);
	array_remove_at(a, 2);
	array_remove_at(a, -2);

	assert(array_at(a, int, 0) == 0);
	assert(array_at(a, int, 3) == 0);
	assert(array_at(a, int, -1) == 0);

	array_free(a);

	// array_remove
	a = array_new(int);

	for (size_t i = 0; i < 24; i++) {
		array_push_back(a, &i);
	}

	array_remove(a, &array_at(a, int, 4));
	assert(array_at(a, int, 4) == 5);

	array_remove(a, &array_at(a, int, -1));
	assert(array_at(a, int, -1) == 22);

	array_free(a);

	// array_remove_custom
	a = array_new(char *);

	for (size_t i = 0; i < 24; i++) {
		char *s = malloc(32);
		snprintf(s, 32, "String %zu", i);
		array_push_back(a, &s);
	}

	array_remove_custom(a, string_compare, &(char *){ "Not there..." });

	assert(array_size(a) == 24);

	array_remove_custom(a, string_compare, &(char *){ "String 4" });

	assert(array_size(a) == 23);

	array_free(a);

	// array_front
	a = array_new(int);

	assert(_array_front(a) == NULL);

	array_push_back(a, &(int){ 1 });
	array_push_back(a, &(int){ 2 });
	array_push_back(a, &(int){ 3 });

	assert(array_front(a, int) == 1);

	array_free(a);

	// array_back
	a = array_new(int);

	assert(_array_back(a) == NULL);

	array_push_back(a, &(int){ 1 });
	array_push_back(a, &(int){ 2 });
	array_push_back(a, &(int){ 3 });

	assert(array_back(a, int) == 3);

	array_free(a);

	// array_push_back
	a = array_new(int);

	array_push_back(a, &(int){ 1 });
	assert(array_at(a, int, 0) == 1);

	array_push_back(a, &(int){ 2 });
	assert(array_at(a, int, 1) == 2);

	array_push_back(a, &(int){ 3 });
	assert(array_at(a, int, 2) == 3);
	assert(array_size(a) == 3);

	array_free(a);

	// array_push_front
	a = array_new(int);

	array_push_front(a, &(int){ 1 });
	assert(array_at(a, int, 0) == 1);

	array_push_front(a, &(int){ 2 });
	assert(array_at(a, int, 0) == 2);

	array_push_front(a, &(int){ 3 });
	assert(array_at(a, int, 0) == 3);
	assert(array_size(a) == 3);

	array_free(a);

	// array_pop_front
	a = array_new(int);

	assert(_array_pop_front(a, false) == NULL);

	array_push_back(a, &(int){ 1 });
	array_push_back(a, &(int){ 2 });
	array_push_back(a, &(int){ 3 });

	assert(array_pop_front(a, int) == 1);
	assert(array_at(a, int, 0) == 2);
	assert(array_size(a) == 2);

	// array_pop_back
	a = array_new(int);

	assert(_array_pop_back(a) == NULL);

	array_push_back(a, &(int){ 1 });
	array_push_back(a, &(int){ 2 });
	array_push_back(a, &(int){ 3 });

	assert(array_pop_back(a, int) == 3);
	assert(array_at(a, int, -1) == 2);
	assert(array_size(a) == 2);

	array_free(a);

	// array_pop_at
	a = array_new(int);

	assert(_array_pop_at(a, 1) == NULL);

	array_push_back(a, &(int){ 1 });
	array_push_back(a, &(int){ 2 });
	array_push_back(a, &(int){ 3 });

	assert(array_pop_at(a, int, 1) == 2);
	assert(array_at(a, int, 1) == 3);
	assert(array_size(a) == 2);

	array_free(a);

	// array_map
	a = array_new(int);
	for (size_t i = 0; i < 16; i++) {
		array_push_back(a, &(int){ i });
	}

	b = array_map(a, int_squared);
	for (size_t i = 0; i < 16; i++) {
		assert(array_at(b, int, i) == i * i);
	}

	array_free(a);
	array_free(b);

	// array_filter
	a = array_new(int);
	for (size_t i = 0; i < 16; i++) {
		array_push_back(a, &(int){ i });
	}

	b = array_filter(a, int_even);
	for (size_t i = 0; i < 8; i++) {
		assert(array_at(b, int, i) == 2 * i);
	}

	array_free(a);
	array_free(b);

	// array_reduce
	a = array_new(int);
	for (size_t i = 0; i < 16; i++) {
		array_push_back(a, &(int){ i });
	}

	int reduced = 0;
	array_reduce(a, int_summation, &reduced);
	assert(reduced == 120);

	reduced = 12;
	array_reduce(a, int_summation, &reduced);
	assert(reduced == 132);

	array_free(a);

	// array_print verify by using your EYES
	a = array_new(int);

	array_print(a, NULL);
	array_print(a, itos);

	for (size_t i = 0; i < 16; i++) {
		array_push_back(a, &(int){ i });
	}

	array_print(a, NULL);
	array_print(a, itos);

	array_free(a);

	printf("Tests passed!\n");
	return 0;
}
