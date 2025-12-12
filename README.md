# Another generic dynamic array implementation in C

A generic array implementation in C, using void pointers and minimal macros, because they are ugly and shouldn't be like 30 lines long.
Stores values directly in allocated memory, works very nicely with the basics. You can use heap allocated pointers too, you just have to set the element_free struct member.

## Building the tests and example
Setup meson's builddir

`meson setup builddir`

Run one or the other or both

`meson compile -C builddir && ./builddir/example`

`meson compile -C builddir && ./builddir/arraytest`

## Using it

Just `#include array.h` and add `array.c` to your sources or whatever.

## Simple datatype example

```C
Array *array = array_new(int);
// Set the size if you want
array_resize(array, 32);
int i = 12; // Some index
array_at(array, int, i) = 24;
array_at(array, int, -1) = 12; // Use a negative index if you want

int v = array_at(array, int, i);

array_print(array, itos);

int x = 12390; // Or use push/pop let it handle size for you
array_push_back(array, &x); // call this n

array_push_back(array, &x); // Value is copied from address, changing n wont affect n + 1
array_at(array, int, -1) -= 24;

array_push_back(array, &(int){ 324243 }); // Using literals

array_at(array, int, 0) = 8675309;

int c = array_pop_front(array, int);
// Now array_size(a) == 34
// Capacity is managed for you btw :)

array_print(array, itos);
printf("Popped: %d\n", c);

// when you're done
array_free(array);
```

## Heap allocated example

```C
Array *array = array_new(char *);
// string_free is provided in array.c as an example, or if you want to use it
array_set_element_free(array, string_free);

for (size_t i = 0; i < 16; i++) {
    char *s = malloc(256);
    snprintf(s, 256, "String thats on the HEAP yo! %zd", i);
    array_push_back(array, &s);
}

array_print(array, stos); // Will print the array and all strings

char *sfind = "I wonder if im in the array...";

// if you use pointers as the value, you need to do this (string_compare also provided ;))
int r = array_find_custom(array, string_compare, &sfind);
// r == -1
printf("Index of \"%s\": %d\n", sfind, r);

// or if you know the address you can let it compare those...
// but you alread have the pointer so?
int j = array_find(array, &sfind); // also -1
printf("Index of \"%s\": %d\n", sfind, j);

// also for a few others
array_remove_custom(array, string_compare, &sfind); // Wont actaully remove since its not there

bool has_sfind = array_contains_custom(array, string_compare, &sfind); // false

Array *b = array_duplicate_custom(array, string_duplicate);

printf("Contains \"%s\": %d\n", sfind, has_sfind);

// when you're done, it'll call element_free for you so set it!
array_free(array);

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
