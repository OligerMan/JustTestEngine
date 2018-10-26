#pragma once

#include <vector>
#include <string>

enum CollisionType {
	null_col,
	stone_col,
	unit_col,

	COUNT_COLLISION
};

std::vector<std::string> collision_type;


void collision_type_init() {
	collision_type.resize(COUNT_COLLISION);

	collision_type[null_col] = "null";
	collision_type[stone_col] = "stone";
	collision_type[unit_col] = "unit";
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

	COUNT_OBJECT
};

std::vector<std::string> object_type;

void object_type_init() {
	object_type.resize(COUNT_OBJECT);

	object_type[null] = "null";
	object_type[stone] = "stone";
	object_type[hero] = "hero";
}

int is_object_type_exists(std::string example) {
	for (int i = 0; i < COUNT_OBJECT; i++) {
		if (object_type[i] == example) {
			return i;
		}
	}
	return -1;
}