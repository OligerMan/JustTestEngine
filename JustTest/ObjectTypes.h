#pragma once

#include <vector>
#include <string>

enum CollisionType {
	null,
	static_obj,
	unit,

	COUNT_COLLISION
};

std::vector<std::string> collision_type;


void collision_type_init() {
	collision_type.resize(COUNT_COLLISION);

	collision_type[null] = "null";
	collision_type[static_obj] = "static_obj";
	collision_type[unit] = "unit";
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
	null_obj,
	stone,
	hero,

	COUNT_OBJECT
};

std::vector<std::string> object_type;

void object_type_init() {
	object_type.resize(COUNT_OBJECT);

	object_type[null_obj] = "null";
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