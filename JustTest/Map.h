#pragma once

#include "Object.h"
#include "EventBuffer.h"
#include "MapParser.h"
#include "GameConstants.h"

void fixCollision(Object * obj1, Object * obj2) { 
	const double min_speed = 0.1;
	const Point eps_speed(0.01, 0.01);

	CollisionModel * col1 = obj1->getCollisionModel();
	CollisionModel * col2 = obj2->getCollisionModel();

	Point tmp_speed1 = col1->getSpeed();
	Point tmp_speed2 = col2->getSpeed();

	Point speed1 = tmp_speed1, speed2 = tmp_speed2;
	Point origin1 = col1->getOrigin(), origin2 = col2->getOrigin();


	// set previous position and add to it half of speed

	tmp_speed1 /= 2;
	tmp_speed2 /= 2;
	col1->changePosition(Point() - tmp_speed1);
	col2->changePosition(Point() - tmp_speed2);
	
	while (tmp_speed1.getLength() > min_speed || tmp_speed2.getLength() > min_speed) {
		tmp_speed1 /= 2;
		tmp_speed2 /= 2;
		if (checkModelCollision(col1, col2)) {
			col1->changePosition(Point() - tmp_speed1);
			col2->changePosition(Point() - tmp_speed2);
		}
		else {
			col1->changePosition(Point() + tmp_speed1);
			col2->changePosition(Point() + tmp_speed2);
		}
	}

	Point pos1 = col1->getPosition();
	Point pos2 = col2->getPosition();

	Point normal_vect = (pos1 - pos2).getNormal();
	normal_vect = Point(normal_vect.y, -normal_vect.x);

	Point normal_speed1 = (normal_vect * (normal_vect.x * (speed1 - tmp_speed1).x + normal_vect.y * (speed1 - tmp_speed1).y));
	col1->changePosition(normal_speed1);
	Point normal_speed2 = (normal_vect * (normal_vect.x * (speed2 - tmp_speed2).x + normal_vect.y * (speed2 - tmp_speed2).y));
	col2->changePosition(normal_speed2);
}

class Map {
	std::vector<std::vector<Object *>> objects;                // vector of objects layers, it is defining order for render and reducing amount of collisions
	std::vector<std::string> animation_type;

	Object * hero = nullptr;

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
			Object
				* obj1 = buffer_elem.getFirstObject(),
				*obj2 = buffer_elem.getSecondObject();
			double thresh = consts.getSpeedDamageThreshold();
			double coef = consts.getSpeedDamageCoef();

			switch (buffer_elem.getEventType()) {
			case null:
				break;
			case default_collision:

				
				obj1->getUnitInfo()->dealDamage(std::max(0.0, (obj1->getSpeed().getLength() - thresh) * coef));
				obj2->getUnitInfo()->dealDamage(std::max(0.0, (obj2->getSpeed().getLength() - thresh) * coef));
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
						delete cursor;
						return true;
					}
				}
			}
		}
		
		event_buffer.addEvent(released, last_clicked_object, nullptr);
		last_clicked_object = nullptr;

		delete cursor;
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

	Map() : objects(parseMap("maps/test_map.map")) {
		for (int layer = 0; layer < objects.size(); layer++) {
			for (int i = 0; i < objects[layer].size(); i++) {
				if (objects[layer][i]->getObjectType() == ObjectType::hero) {
					hero = objects[layer][i];
				}
			}
		}
	}

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

	Object * getHero() {
		return hero;
	}

};