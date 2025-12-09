#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MIN_CAPACITY 8
#define ELEMENT_STRING_BUFFER_SIZE 256

// Macros call _prepended functions with syntactic sugar
#define array_new(type) _array_new(sizeof(type))
#define array_new_with_size(type, size) _array_new_with_size(sizeof(type), size)

// All other getters return a pointer, these two are already dereferenced
// e.g. a[3] = 123; -> array_at(a, int, 3) = 123;
#define array_at(array, type, index) *(type *)_array_at(array, index)
#define array_unsafe_at(array, type, index) *(type *)_array_unsafe_at(array, index)

#define array_get(array, type, index) (type *)_array_get(array, index)
#define array_front(array, type) *(type *)_array_front(array)
#define array_back(array, type) *(type *)_array_back(array)
#define array_pop_front(array, type) *(type *)_array_pop_front(array, false)
#define array_pop_front_fast(array, type) *(type *)_array_pop_front(array, true)
#define array_pop_back(array, type) *(type *)_array_pop_back(array)
#define array_pop_at(array, type, index) *(type *)_array_pop_at(array, index)

typedef struct Array Array;

Array *_array_new(size_t type_size);
Array *_array_new_with_size(size_t type_size, size_t size);

void array_free(Array *array);
void array_set_element_free(Array *array, void (*p_element_free)(void *));

Array *array_duplicate(Array *array);
Array *array_duplicate_custom(Array *array, void (*element_duplicate)(void *, void *));

size_t array_size(Array *array);
size_t array_capacity(Array *array);
size_t array_element_size(Array *array);
void *array_element_free(Array *array);
void *array_data(Array *array);
bool array_empty(Array *array);

ptrdiff_t array_find(Array *array, void *element);
ptrdiff_t array_find_custom(Array *array, int (*compare)(void *, void *), void *element);

bool array_contains(Array *array, void *element);
bool array_contains_custom(Array *array, int (*compare)(void *, void *), void *element);

size_t array_count(Array *array, void *element);
size_t array_count_custom(Array *array, int (*compare)(void *, void *), void *element);

void array_clear(Array *array);
void array_reverse(Array *array);

void array_resize(Array *array, size_t new_size);
void array_scale_capacity(Array *array);
void array_shrink_to_fit(Array *array);

void *_array_at(Array *array, ptrdiff_t index);
void *_array_unsafe_at(Array *array, ptrdiff_t index);

void array_set(Array *array, ptrdiff_t index, void *element);
void *_array_get(Array *array, ptrdiff_t index);

void array_insert_at(Array *array, ptrdiff_t index, void *element);
void array_remove_at(Array *array, ptrdiff_t index);

void array_remove(Array *array, void *element);
void array_remove_custom(Array *array, int (*compare)(void *, void *), void *element);

void *_array_front(Array *array);
void *_array_back(Array *array);

void array_push_front(Array *array, void *element);
void array_push_back(Array *array, void *element);

void *_array_pop_front(Array *array, bool fast);
void *_array_pop_back(Array *array);
void *_array_pop_at(Array *array, ptrdiff_t index);

Array *array_map(Array *array, void (*map)(void *, void *));
Array *array_filter(Array *array, bool (*filter)(void *));
void array_reduce(Array *array, void (*reduce)(void *, void *), void *accumulator);

void array_print(Array *array, void (*element_to_string)(char *, void *));

// Example functions for print, map, filter, reduce
void itos(char *buffer, int *element);
void ftos(char *buffer, float *element);
void dtos(char *buffer, double *element);
void ctos(char *buffer, char *element);
void stos(char *buffer, char **element);

void int_squared(int *element, int *result);
bool int_even(int *element);
void int_summation(int *element, int *accumulator);
void string_duplicate(char **destination, char **source);
int string_compare(char **a, char **b);
void string_free(char **string);