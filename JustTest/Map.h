#pragma once

#include "Object.h"
#include "EventBuffer.h"
#include "MapParser.h"

void fixCollision(Object * obj1, Object * obj2) {
	const double min_speed = 0.1;

	CollisionModel * col1 = obj1->getCollisionModel();
	CollisionModel * col2 = obj2->getCollisionModel();

	Point speed1 = col1->getSpeed();
	Point speed2 = col2->getSpeed();

	// set previous position and add to it half of speed

	speed1 /= 2;
	speed2 /= 2;
	col1->changePosition(Point() - speed1);
	col2->changePosition(Point() - speed2);
	
	while (speed1.getLength() > min_speed || speed2.getLength() > min_speed) {
		speed1 /= 2;
		speed2 /= 2;
		if (checkModelCollision(col1, col2)) {
			col1->changePosition(Point() - speed1);
			col2->changePosition(Point() - speed2);
		}
		else {
			col1->changePosition(Point() + speed1);
			col2->changePosition(Point() + speed2);
		}
	}
}

class Map {
	std::vector<std::vector<Object *>> objects;                // vector of objects layers, it is defining order for render and reducing amount of collisions
	std::vector<Object> object_set;
	std::vector<std::string> animation_type;

	EventBuffer event_buffer;

	Object * last_clicked_object = nullptr;

	//////////////////////////////////////////////

	void processCollisionFrame(){
		for (int cnt = 0; cnt < objects.size(); cnt++) {
			for (int i = 0; i < objects[cnt].size(); i++) {
				for (int j = 0; j < objects[cnt].size(); j++) {
					if (i != j) {
						if (checkObjectCollision(objects[cnt][i], objects[cnt][j])) {
							event_buffer.addEvent(objects[cnt][i], objects[cnt][j]);
							fixCollision(objects[cnt][i], objects[cnt][j]);
						}
					}
				}
			}
		}
		
	}

	void processEventBuffer() {
		while (true) {
			Event buffer_elem = event_buffer.getEvent();

			if (buffer_elem.getFirstObject() == nullptr) {
				break;
			}
			switch (buffer_elem.getEventType()) {
			case null:
				break;
			case default_collision:
				break;
			case clicked:
				buffer_elem.getFirstObject()->changeAngle(10);
				break;
			};
		}
	}

	bool processClick(Point click) {
		Object * cursor = new Object(click);
		if (!objects.empty()) {
			for (int layer = objects.size() - 1; layer >= 0; layer--) {
				for (int i = 0; i < objects[layer].size(); i++) {
					if (checkObjectCollision(objects[layer][i], cursor)) {
						if (objects[layer][i] != last_clicked_object) {

							event_buffer.addEvent(clicked, objects[layer][i], nullptr);

							last_clicked_object = objects[layer][i];
						}
						else {
							event_buffer.addEvent(pressed, objects[layer][i], nullptr);
						}
						return true;
					}
					else if (last_clicked_object != nullptr) {
						event_buffer.addEvent(released, objects[layer][i], nullptr);

						last_clicked_object = nullptr;
					}
				}
			}
		}
		return false;
	}

	void processObjectSpeed() {
		for (int layer = 0; layer < objects.size(); layer++) {
			for (int i = 0; i < objects[layer].size(); i++) {
				objects[layer][i]->changePosition(objects[layer][i]->getSpeed());
			}
		}
	}

public:

	Map() : objects(parseMap("maps/test_map.map")) {}

	std::vector<std::vector<Object *>> * getObjectsBuffer() {
		return &objects;
	}

	void addObject(Object * object, int layer) {
		objects[layer].push_back(object);
	}

	void processFrame(Point click) {
		processClick(click);
		processObjectSpeed();
		processCollisionFrame();
		processEventBuffer();
	}

};