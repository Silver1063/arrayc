# Another generic dynamic array implementation in C

A generic array implementation in C, using void pointers and minimal macros, because they are ugly and shouldn't be like 30 lines long.
Stores values directly in allocated memory, works very nicely with the basics. You can use heap allocated pointers too, you just have to set the element_free struct member.

## Building the test and example
Run one or the other or both
```meson compile -C builddir && ./builddir/example```
```meson compile -C builddir && ./builddir/arraytest```

## Using it

Just ```#include array.h``` and add ```array.c``` to your sources or whatever.

## Simple datatype example
```C
Array *a = array_new(int);
// Set the size if you want
array_resize(a, 32);

// get and set the value this way
int i = 12 // Some index
array_at(a, int, i) = 24;
array_at(a, int, -1) = 12; // Use a negative index if you want

int v = array_at(a, int, i);

int x = 12390; // Or use push/pop let it handle size for you
array_push_back(array, &x) // call this n
array_push_back(array, &x) // Value is copied from address, changing n wont affect n + 1
array_push_back(array, &(int){324243}) // Using literals

int c = array_pop_front(a, int);
// Now array_size(a) == 34
// Capacity is managed for you btw :)

// when you're done
array_free(a);
```
## Heap allocated example
```C
Array *a = array_new(char *);
// string_free is provided in array.c as an example, or if you want to use it
array_set_element_free(string_free);


for (size_t i = 0; i < 16; i++) {
    char *s = malloc(256);
    snprintf(s, 256, "String thats on the HEAP yo! %d", i);
    array_push_back(a, &s);
}

char *sfind = "I wonder if im in the array...";

// if you use pointers as the value, you need to  (string_compare also provided ;))
int r = array_find_custom(a, string_compare, &sfind);
// r == -1

// or if you know the address you can let it compare those...
// but you alread have the pointer so?
int j = array_find(a, &sfind);

// also for a few others 
array_remove_custom(a, string_compare, &sfind);
bool has_sfind = array_contains_custom(a, string_compare, &sfind);
Array *b = array_duplicate_custom(a, string_compare)


// when you're done, it'll call element_free for you so set it!
array_free(a);

```

## Other things
Map, Filter, Reduce, Print functions are provided

```C
Array *array = array_new(int);

... Imagine array has a bunch of stuff...

// int_squared, int_even, int_summation, itos provided in array.c as example ;)
Array *mapped = array_map(array, int_squared);
Array *filtered = array_map(mapped, int_even);

array_print(array, itos);

int reduced = 0;
array_reduce(array, int_summation, &reduced);

```
