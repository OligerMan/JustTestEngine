#pragma once

#include "ObjectTypes.h"

enum EventType{
	null_event,
	default_collision,
	clicked,
	pressed,
	released,
	create_new,

	COUNT_EVENT
};

std::vector<std::string> event_type;

void event_type_init() {
	event_type.resize(COUNT_EVENT);

	event_type[null_event] = "null";
	event_type[default_collision] = "default_collision";
	event_type[clicked] = "clicked";
	event_type[pressed] = "pressed";
	event_type[released] = "released";
	event_type[create_new] = "create";
}

int is_event_type_exists(std::string example) {
	for (int i = 0; i < COUNT_EVENT; i++) {
		if (event_type[i] == example) {
			return i;
		}
	}
	return -1;
}

EventType getEventType(CollisionType type1, CollisionType type2){
	if (type1 == stone && type2 == stone) {
		return default_collision;
	}
	return null_event;
}