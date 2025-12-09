#include "array.h"

// Example struct
typedef struct Point {
	float x;
	float y;
	float z;
} Point;

void point_to_string(char *buffer, Point *point) {
	snprintf(buffer, ELEMENT_STRING_BUFFER_SIZE, "Point {x: %f, y: %f, z: %f}", point->x, point->y, point->z);
}

int main(int argc, char **argv) {
	Array *points = array_new(Point);

	for (size_t i = 0; i < 16; i++) {
		array_push_back(points, &(Point){ (float)i, (float)i, (float)i });
	}

	array_print(points, point_to_string);

    Point p = array_at(points, Point, 4);

    printf("Point at index 4: {x: %.2f, y: %.2f, z: %.2f}\n", p.x, p.y, p.z);

    int i = array_find(points, &(Point){ 4.0, 4.0, 4.0 });

    printf("Index of Point {x: 4.0, y: 4.0, z: 4.0}: %d\n", i);

	return 0;
}