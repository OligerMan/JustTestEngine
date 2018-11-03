#pragma once

#include <vector>
#include <string>

enum CollisionType {
	null_col,
	stone_col,
	unit_col,
	redactor_back_col,

	COUNT_COLLISION
};

std::vector<std::string> collision_type;


void collision_type_init() {
	collision_type.resize(COUNT_COLLISION);

	collision_type[null_col] = "null";
	collision_type[stone_col] = "stone";
	collision_type[unit_col] = "unit";
	collision_type[redactor_back_col] = "redactor_back";
}

int is_collision_type_exists(std::string example) {
	for (int i = 0; i < COUNT_COLLISION; i++) {
		if (collision_type[i] == example) {
			return i;
		}
	}
	return -1;
}


enum ObjectType {
	null,
	stone,
	hero,
	redactor_back,

	COUNT_OBJECT
};

std::vector<std::string> object_type;

void object_type_init() {
	object_type.resize(COUNT_OBJECT);

	object_type[null] = "null";
	object_type[stone] = "stone";
	object_type[hero] = "hero";
	object_type[redactor_back] = "redactor_back";

}

int is_object_type_exists(std::string example) {
	for (int i = 0; i < COUNT_OBJECT; i++) {
		if (object_type[i] == example) {
			return i;
		}
	}
	return -1;
}