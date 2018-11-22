#pragma once

#include "Object.h"
#include "EventBuffer.h"
#include "MapParser.h"
#include "GameConstants.h"
#include "Timer.h"
#include "AStarAlgorithm.h"

void fixCollision(Object * obj1, Object * obj2) { 
	const double min_speed = 0.1;
	Point eps_speed(0.01, 0.01);

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

	if (checkModelCollision(col1, col2)) {
		col1->changePosition((col1->getPosition() - col2->getPosition()).getNormal() * eps_speed.getLength() * 10); 
		col2->changePosition((col2->getPosition() - col1->getPosition()).getNormal() * eps_speed.getLength() * 10);
	}
}

class Map {
	std::vector<std::vector<Object *>> objects;                // vector of objects layers, it is defining order for render and reducing amount of collisions
	std::vector<std::string> animation_type;

	int main_layer = 0;

	std::vector<std::vector<int>> navigation_grid;
	std::vector<std::vector<Point>> navigation_paths;
	double grid_size = 10;
	Point grid_offset;

	Object * hero = nullptr;

	EventBuffer event_buffer;

	Object * last_clicked_object = nullptr;

	//////////////////////////////////////////////

	void createNavigationGrid() {
		Point left_up(1e9, 1e9), right_down(-1e9, -1e9);

		for (int layer = 0; layer < objects.size(); layer++) {
			for (int i = 0; i < objects[layer].size(); i++) {
				Object * object = objects[layer][i];
				//if (object->getCollisionModel()->isStatic()) {
					if (-object->getOrigin().x + object->getPosition().x < left_up.x) {
						left_up.x = -object->getOrigin().x + object->getPosition().x;
					}
					if (-object->getOrigin().y + object->getPosition().y < left_up.y) {
						left_up.y = -object->getOrigin().y + object->getPosition().y;
					}
					if (-object->getOrigin().x + object->getSquareBorder().x + object->getPosition().x > right_down.x) {
						right_down.x = -object->getOrigin().x + object->getSquareBorder().x + object->getPosition().x;
					}
					if (-object->getOrigin().y + object->getSquareBorder().y + object->getPosition().y > right_down.y) {
						right_down.y = -object->getOrigin().y + object->getSquareBorder().y + object->getPosition().y;
					}
				//}
			}
		}

		grid_offset = left_up * 1.5;

		// init empty navigation grid
		navigation_grid.resize((right_down.x - left_up.x) / grid_size * 1.5 + 1);
		if (!navigation_grid.empty()) {
			for (int i = 0; i < navigation_grid.size(); i++) {
				navigation_grid[i].resize((right_down.y - left_up.y) / grid_size * 1.5 + 1);
			}
			for (int i = 0; i < navigation_grid.size(); i++) {
				navigation_grid[i][0] = 1;
				navigation_grid[i][(right_down.y - left_up.y) / grid_size * 1.5] = 1;
			}
			for (int i = 0; i < int((right_down.y - left_up.y) / grid_size * 1.5 + 1); i++) {
				navigation_grid[0][i] = 1;
				navigation_grid[navigation_grid.size() - 1][i] = 1;
			}
		}
		// grid initialisation end

		for (int layer = 0; layer < objects.size(); layer++) {
			for (int i = 0; i < objects[layer].size(); i++) {
				Object * object = objects[layer][i];

				if (object->getCollisionModel()->isStatic()) {
					double grid_width = object->getCollisionModel()->getSquareBorder().x / grid_size * 1.5 + 1;
					double grid_height = object->getCollisionModel()->getSquareBorder().y / grid_size * 1.5 + 1;
					
					Point start_object_grid = object->getPosition() - grid_offset - (object->getOrigin() * 1.5);
					for (double i = 0; i < grid_width; i++) {
						for (double j = 0; j < grid_height; j++) {
							Point cur_point = start_object_grid + Point(i * grid_size, j * grid_size);
							Object * grid_elem = new Object();
							grid_elem->getCollisionModel()->addCircle(Point(), grid_size * 3);
							grid_elem->setPosition(cur_point + grid_offset);
							if (checkObjectCollision(grid_elem, object) && navigation_grid[cur_point.x / grid_size][cur_point.y / grid_size] != 1) {
								navigation_grid[cur_point.x / grid_size][cur_point.y / grid_size] = 1;
							}
							delete grid_elem;
						}
					}
				}
			}
		}
	}

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

	Point convertToNavigation(Point input) {
		Point output = (input - grid_offset) / grid_size;
		output.x = int(output.x);
		output.y = int(output.y);

		return output;
	}

	Point convertFromNavigation(Point input) {
		return input * grid_size + grid_offset;
	}

	std::vector<Point> findPath(Object * object1, Object * object2) {
		Point
			point1 = convertToNavigation(object1->getPosition()),
			point2 = convertToNavigation(object2->getPosition());

		std::vector<Point> output = getAStarPath(navigation_grid, point1, point2);
		for (int i = 0; i < output.size(); i++) {
			output[i] = convertFromNavigation(output[i]);
		}
		if (!output.empty()) {
			//output[0] = object2->getPosition();
			//output[output.size() - 1] = object1->getPosition();
		}

		return output;
	}

	void processUnitAI() {
		navigation_paths.clear();
		for (int i = 0; i < objects[main_layer].size(); i++) {
			Object * object1 = objects[main_layer][i];
			int faction1 = object1->getUnitInfo()->getFaction();

			if (faction1 != FactionList::null_faction) {
				for (int j = 0; j < objects[main_layer].size(); j++) {
					if (i != j) {
						Object * object2 = objects[main_layer][j];
						int faction2 = object2->getUnitInfo()->getFaction();

						if (faction1 != faction2 && faction1 != hero_faction && faction1 != null_faction && faction2 != null_faction) {
							std::vector<Point> path = findPath(object2, object1);
							if (!path.empty()) {
								if (settings.isNavigationGridMode()) {
									navigation_paths.push_back(path);
								}
								object1->setSpeed((path[2] - path[0]).getNormal() * consts.getDefaultHeroSpeed() * 0.9);
								object1->setAnimationType(move_anim);
								object1->setAngle(-atan2(-object1->getSpeed().y, object1->getSpeed().x) * 180 / PI);
							}
							else {
								object1->setAnimationType(hold_anim);
							}
							if ((object1->getPosition() - object2->getPosition()).getLength() < object2->getUnitInfo()->getAttackRange1()) {
								object1->setAnimationType(attack1_anim);
							}
						}
					}
				}
			}
		}
	}

	void garbageCollector() {
		for (int layer = 0; layer < objects.size(); layer++) {
			for (int i = 0; i < objects[layer].size(); i++) {
				if (objects[layer][i]->isDeleted() || objects[layer][i]->getUnitInfo()->isDead()) {
					if (objects[layer][i] == hero) {
						hero = nullptr;
					}
					objects[layer].erase(objects[layer].begin() + i);
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
				* obj2 = buffer_elem.getSecondObject();
			double thresh = consts.getSpeedDamageThreshold();
			double coef = consts.getSpeedDamageCoef();

			switch (buffer_elem.getEventType()) {
			case null:
				break;
			case attack:

				obj1->getUnitInfo()->dealDamage(std::max(0.0, (obj1->getSpeed().getLength() - thresh) * coef));
				obj2->getUnitInfo()->dealDamage(std::max(0.0, (obj2->getSpeed().getLength() - thresh) * coef));

				if (obj1->getObjectAnimationType() == attack1_anim) {
					obj2->getUnitInfo()->dealDamage(obj1->getUnitInfo()->getAttackRange1());
				}
				if (obj2->getObjectAnimationType() == attack1_anim) {
					obj1->getUnitInfo()->dealDamage(obj2->getUnitInfo()->getAttackRange1());
				}
				break;
			case clicked:
				if (settings.isRedactorMode()) {
					buffer_elem.getFirstObject()->deleteObject();
				}
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
				objects[layer][i]->changePosition(objects[layer][i]->getSpeed()/* * timer.getTimeDelay() / 1000*/);
			}
		}
	}

public:

	Map() {}

	Map(std::string path) : objects(parseMap(path)) {
		for (int layer = 0; layer < objects.size(); layer++) {
			for (int i = 0; i < objects[layer].size(); i++) {
				if (objects[layer][i]->getObjectType() == ObjectType::hero) {
					hero = objects[layer][i];
				}
			}
		}
		createNavigationGrid();
	}

	bool isClickable(Point click) {
		Object * cursor = new Object(click);
		if (!objects.empty()) {
			for (int layer = objects.size() - 1; layer >= 0; layer--) {
				for (int i = 0; i < objects[layer].size(); i++) {
					if (checkObjectCollision(objects[layer][i], cursor)) {
						delete cursor;
						return true;
					}
				}
			}
		}
		delete cursor;
		return false;
	}

	std::vector<std::vector<Object *>> * getObjectsBuffer() {
		return &objects;
	}

	void addObject(Object * object, int layer) {
		if (objects.size() <= layer) {
			objects.resize(layer + 1);
		}
		objects[layer].push_back(object);
		last_clicked_object = object;
		std::cout << "New object created" << std::endl;
	}

	void processFrame(Point click, bool recount_navigation) {
		processClick(click);
		if (recount_navigation && !settings.isRedactorMode()) {
			processUnitAI();
		}
		processObjectSpeed();
		processCollisionFrame();
		processEventBuffer();
		garbageCollector();
	}

	Object * getHero() {
		return hero;
	}

	Point getNavigationGridOffset() {
		return grid_offset;
	}

	int getNavigationGridElem(int x, int y) {
		if (!navigation_grid.empty() && x >= 0 && y >= 0 && x < navigation_grid.size() && y < navigation_grid[0].size()) {
			return navigation_grid[x][y];
		}
		else {
			return 0;
		}
	}

	std::vector<std::vector<Point>> getNavigationPaths() {
		return navigation_paths;
	}

	double getNavigationGridSize() {
		return grid_size;
	}
};