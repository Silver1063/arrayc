#include "array.h"

struct Array {
	size_t size;
	size_t capacity;
	size_t element_size;
	void (*element_free)(void *);
	void *data;
};

Array *_array_new(size_t type_size) {
	Array *array = malloc(sizeof(Array));
	if (!array) {
		return NULL;
	}

	array->size = 0;
	array->capacity = MIN_CAPACITY;
	array->element_size = type_size;
	array->element_free = NULL;
	// Fine for most platforms, not guaranteed to be 0.0 or NULL ptr technically
	array->data = calloc(MIN_CAPACITY, type_size);
	if (!array->data) {
		free(array);
		return NULL;
	}

	return array;
}

Array *_array_new_with_size(size_t type_size, size_t size) {
	Array *array = _array_new(type_size);
	array_resize(array, size);
	return array;
}

void array_free(Array *array) {
	if (!array) {
		return;
	}

	// Optionally free elements that are heap allocated
	if (array->element_free) {
		for (size_t i = 0; i < array->size; i++) {
			if (!_array_at(array, i)) {
				continue;
			}
			array->element_free(_array_at(array, i));
		}
	}

	free(array->data);
	free(array);
}

void array_set_element_free(Array *array, void (*p_element_free)(void *)) {
	array->element_free = p_element_free;
}

Array *array_duplicate(Array *array) {
	return array_duplicate_custom(array, NULL);
}

Array *array_duplicate_custom(Array *array,
		void (*element_duplicate)(void *, void *)) {
	Array *duplicate = _array_new_with_size(array->element_size, array->size);
	duplicate->element_free = array->element_free;

	for (size_t i = 0; i < array->size; i++) {
		if (!element_duplicate) {
			memcpy(_array_at(duplicate, i), _array_at(array, i), array->element_size);
		} else {
			element_duplicate(_array_at(duplicate, i), _array_at(array, i));
		}
	}
	return duplicate;
}

size_t array_size(Array *array) {
	return array->size;
}

size_t array_capacity(Array *array) {
	return array->capacity;
}

size_t array_element_size(Array *array) {
	return array->element_size;
}

void *array_element_free(Array *array) {
	return array->element_free;
}

void *array_data(Array *array) {
	return array->data;
}

bool array_empty(Array *array) {
	return array->size == 0;
}

ptrdiff_t array_find(Array *array, void *element) {
	return array_find_custom(array, NULL, element);
}

ptrdiff_t array_find_custom(Array *array, int (*compare)(void *, void *),
		void *element) {
	for (size_t i = 0; i < array->size; i++) {
		int diff = compare
				? compare(_array_at(array, i), element)
				: memcmp(_array_at(array, i), element, array->element_size);
		if (diff == 0) {
			return i;
		}
	}
	return -1;
}

bool array_contains(Array *array, void *element) {
	return array_find(array, element) != -1;
}

bool array_contains_custom(Array *array, int (*compare)(void *, void *),
		void *element) {
	return array_find_custom(array, compare, element) != -1;
}

size_t array_count(Array *array, void *element) {
	return array_count_custom(array, NULL, element);
}

size_t array_count_custom(Array *array, int (*compare)(void *, void *),
		void *element) {
	size_t count = 0;
	for (size_t i = 0; i < array->size; i++) {
		int diff = compare
				? compare(_array_at(array, i), element)
				: memcmp(_array_at(array, i), element, array->element_size);
		if (diff == 0) {
			count++;
		}
	}
	return count;
}

void array_clear(Array *array) {
	if (array->element_free) {
		for (size_t i = 0; i < array->size; i++) {
			array->element_free(_array_at(array, i));
		}
	}

	void *temp = calloc(MIN_CAPACITY, array->element_size);
	if (!temp) {
		printf("calloc failed\n");
		return;
	}

	free(array->data);

	array->size = 0;
	array->capacity = MIN_CAPACITY;
	array->data = temp;
}
//
void array_reverse(Array *array) {
	for (size_t i = 0; i < array->size / 2; i++) {
		memcpy(_array_unsafe_at(array, array->size), _array_at(array, i),
				array->element_size);
		memcpy(_array_at(array, i), _array_at(array, -1 - i), array->element_size);
		memcpy(_array_at(array, -1 - i), _array_unsafe_at(array, array->size),
				array->element_size);
	}
}

void array_resize(Array *array, size_t new_size) {
	size_t old_size = array->size;
	array->size = new_size;

	if (array->element_free) {
		for (size_t i = array->size; i < old_size; i++) {
			array->element_free(_array_at(array, i));
		}
	}

	array_scale_capacity(array);
}

void array_scale_capacity(Array *array) {
	size_t old_capacity = array->capacity;

	if (array->size < MIN_CAPACITY) {
		array->capacity = MIN_CAPACITY;
	} else if (array->size < array->capacity / 2 ||
			array->size >= array->capacity) {
		array->capacity = (size_t)exp2(floor(log2((double)array->size) + 1.0));
	}

	if (array->capacity != old_capacity) {
		array->data = realloc(array->data, array->capacity * array->element_size);
		if (!array->data) {
			printf("realloc failed\n");
			return;
		}
		for (size_t i = old_capacity; i < array->capacity; i++) {
			memset(_array_unsafe_at(array, i), 0, array->element_size);
		}
	}
}

void array_shrink_to_fit(Array *array) {
	array->capacity = array->size;
	array->data = realloc(array->data, array->capacity * array->element_size);
	if (!array->data) {
		printf("realloc failed\n");
		return;
	}
}

void *_array_at(Array *array, ptrdiff_t index) {
	// Supports Python style negative indexing
	// TODO: throw error
	if (index < 0) {
		index += array->size;
	}
	if (index < 0 || index >= array->size) {
		return NULL;
	}
	return (char *)array->data + index * array->element_size;
}

void *_array_unsafe_at(Array *array, ptrdiff_t index) {
	// Supports Python style negative indexing
	if (index < 0) {
		index += array->size;
	}
	return (char *)array->data + index * array->element_size;
}

void array_set(Array *array, ptrdiff_t index, void *element) {
	memcpy(_array_at(array, index), element, array->element_size);
}

void *_array_get(Array *array, ptrdiff_t index) {
	return _array_at(array, index);
}

void array_insert_at(Array *array, ptrdiff_t index, void *element) {
	void *target = _array_at(array, index);
	if (!target) {
		return;
	}

	index = index < 0 ? index + array->size : index;

	array->size++;
	array_scale_capacity(array);
	memmove(_array_unsafe_at(array, index + 1), target,
			(array->size - index) * array->element_size);
	memcpy(target, element, array->element_size);
}

void array_remove_at(Array *array, ptrdiff_t index) {
	void *target = _array_at(array, index);
	if (!target) {
		return;
	}
	if (array->element_free) {
		array->element_free(target);
	}

	index = index < 0 ? array->size + index : index;

	array->size--;
	array_scale_capacity(array);
	memmove(target, _array_unsafe_at(array, index + 1),
			(array->size - index) * array->element_size);
}

void array_remove(Array *array, void *element) {
	array_remove_custom(array, NULL, element);
}

void array_remove_custom(Array *array, int (*compare)(void *, void *),
		void *element) {
	ptrdiff_t index = array_find_custom(array, compare, element);
	if (index < 0) {
		return;
	}
	array_remove_at(array, index);
}

void *_array_front(Array *array) {
	return _array_at(array, 0);
}

void *_array_back(Array *array) {
	return _array_at(array, -1);
}

void array_push_front(Array *array, void *element) {
	array->size++;
	array_scale_capacity(array);
	memmove(_array_unsafe_at(array, 1), _array_front(array),
			array->size * array->element_size);
	memcpy(_array_front(array), element, array->element_size);
}

void array_push_back(Array *array, void *element) {
	array->size++;
	array_scale_capacity(array);
	memcpy(_array_back(array), element, array->element_size);
}

void *_array_pop_front(Array *array, bool fast) {
	if (array->size <= 0) {
		// TODO: Throw error
		return NULL;
	}
	if (array->size == 1) {
		array->size--;
		return _array_unsafe_at(array, 0);
	}

	memcpy(_array_unsafe_at(array, array->size), _array_at(array, 0),
			array->element_size);

	array->size--;

	// if fast, copy just the last to the front, else everything moves left
	size_t from = fast ? array->size : 1;
	size_t n = fast ? 1 : array->size;

	memmove(_array_at(array, 0), _array_unsafe_at(array, from),
			n * array->element_size);
	memcpy(_array_unsafe_at(array, array->size),
			_array_unsafe_at(array, array->size + 1), array->element_size);

	array_scale_capacity(array);

	return _array_unsafe_at(array, array->size);
}

void *_array_pop_back(Array *array) {
	if (array->size <= 0) {
		// TODO: Throw error
		return NULL;
	}

	array->size--;
	array_scale_capacity(array);

	return _array_unsafe_at(array, array->size);
}

void *_array_pop_at(Array *array, ptrdiff_t index) {
	void *ptr = _array_at(array, index);
	if (ptr == _array_front(array)) {
		return _array_pop_front(array, false);
	}
	if (ptr == _array_back(array)) {
		return _array_pop_back(array);
	}

	index = index < 0 ? array->size + index : index;

	memmove(_array_unsafe_at(array, array->size), ptr, array->element_size);
	memmove(ptr, _array_unsafe_at(array, index + 1),
			(array->size - index) * array->element_size);

	array->size--;
	array_scale_capacity(array);

	return _array_unsafe_at(array, array->size);
}

bool array_all(Array *array, bool (*predicate)(void *)) {
	for (size_t i = 0; i < array->size; i++) {
		void *element = _array_at(array, i);
		if (!predicate(element)) {
			return false;
		}
	}
	return true;
}

bool array_any(Array *array, bool (*predicate)(void *)) {
	for (size_t i = 0; i < array->size; i++) {
		void *element = _array_at(array, i);
		if (predicate(element)) {
			return true;
		}
	}
	return false;
}

// Could be parallelized, not the point though.
Array *array_map(Array *array, void (*map)(void *, void *)) {
	Array *mapped_array = _array_new(array->element_size);

	void *mapped_element = malloc(array->element_size);
	if (!mapped_element) {
		printf("malloc failed\n");
		return NULL;
	}

	for (size_t i = 0; i < array->size; i++) {
		void *element = _array_at(array, i);
		map(element, mapped_element);
		array_push_back(mapped_array, mapped_element);
	}
	free(mapped_element);

	return mapped_array;
}

Array *array_filter(Array *array, bool (*filter)(void *)) {
	Array *filtered_array = _array_new(array->element_size);
	for (size_t i = 0; i < array->size; i++) {
		void *element = _array_at(array, i);
		if (filter(element)) {
			array_push_back(filtered_array, element);
		}
	}
	return filtered_array;
}

void array_reduce(Array *array, void (*reduce)(void *, void *),
		void *accumulator) {
	for (size_t i = 0; i < array->size; i++) {
		void *element = _array_at(array, i);
		reduce(element, accumulator);
	}
}

void array_print(Array *array, void (*element_to_string)(char *, void *)) {
	printf("Array {size: %zu, capacity: %zu, element_size: %zu, data: {",
			array->size, array->capacity, array->element_size);

	if (!element_to_string) {
		printf("%p to %p}}\n", array->data,
				(char *)array->data + (array->size - 1) * array->element_size);
		return;
	}

	char buffer[ELEMENT_STRING_BUFFER_SIZE];
	for (int i = 0; i < array->size; i++) {
		void *element = _array_at(array, i);
		element_to_string(buffer, element);
		printf("%s", buffer);
		if (i < (array->size - 1)) {
			printf(", ");
		}
	}

	printf("}}\n");
}

// Element to String functions
void itos(char *buffer, int *element) {
	snprintf(buffer, ELEMENT_STRING_BUFFER_SIZE, "%d", *element);
}

void ftos(char *buffer, float *element) {
	snprintf(buffer, ELEMENT_STRING_BUFFER_SIZE, "%.2f", *element);
}

void dtos(char *buffer, double *element) {
	snprintf(buffer, ELEMENT_STRING_BUFFER_SIZE, "%.4f", *element);
}

void ctos(char *buffer, char *element) {
	snprintf(buffer, ELEMENT_STRING_BUFFER_SIZE, "%c", *element);
}

void stos(char *buffer, char **element) {
	char *format = *element ? "\"%s\"" : "NULL";
	snprintf(buffer, ELEMENT_STRING_BUFFER_SIZE, format, *element);
}

// Example map, filter, reduce functions
void int_squared(int *element, int *result) {
	*result = *element * *element;
}

bool int_even(int *element) {
	return *element % 2 == 0;
}

void int_summation(int *element, int *accumulator) {
	*accumulator += *element;
}

void string_duplicate(char **destination, char **source) {
	if (!*source) {
		return;
	}
	int length = strlen(*source) + 1;
	*destination = malloc(length);
	strcpy(*destination, *source);
}

int string_compare(char **a, char **b) {
	// dereference as, and decide what to compare
	return strcmp(*a, *b);
}
// THINK about it
void string_free(char **string) {
	// element could have its own heap allocated memory it frees
	if (!*string) {
		return;
	}
	free(*string);
}