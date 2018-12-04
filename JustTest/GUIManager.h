#pragma once

#include <vector>

#include "Object.h"
#include "EventBuffer.h"
#include "GUIParser.h"


class GUIManager {

	std::vector<std::vector<Object *>> gui_elements;

	EventBuffer gui_event_buffer;
	EventBuffer external_event_buffer;

	Object * last_clicked_object = nullptr;
	Object * selected_object = nullptr;

	bool checkClick(Point click) {
		Object * cursor = new Object(click);

		if (!gui_elements.empty()) {
			for (int layer = (int)gui_elements.size() - 1; layer >= 0; layer--) {
				for (int i = 0; i < gui_elements[layer].size(); i++) {
					if (!settings.isRedactorMode()) {
						if (checkObjectCollision(gui_elements[layer][i], cursor)) {
							if (gui_elements[layer][i] != last_clicked_object) {

								gui_event_buffer.addEvent(clicked, gui_elements[layer][i], nullptr);

								last_clicked_object = gui_elements[layer][i];
							}
							else {

								gui_event_buffer.addEvent(pressed, gui_elements[layer][i], nullptr);
							}
							delete cursor;
							return true;
						}
						else if (last_clicked_object != nullptr) {
							gui_event_buffer.addEvent(released, gui_elements[layer][i], nullptr);

							last_clicked_object = nullptr;
						}
					}
					else {
						Point pos = gui_elements[layer][i]->getPosition();
						if (layer == 1 && (pos - click).getLength() <= 50) {
							if (gui_elements[layer][i] != last_clicked_object) {

								gui_event_buffer.addEvent(clicked, gui_elements[layer][i], nullptr);

								last_clicked_object = gui_elements[layer][i];
							}
							else {
								gui_event_buffer.addEvent(pressed, gui_elements[layer][i], nullptr);
							}
							delete cursor;
							return true;
						}
						else if (last_clicked_object != nullptr) {
							gui_event_buffer.addEvent(released, gui_elements[layer][i], nullptr);

							last_clicked_object = nullptr;
						}
					}
				}
			}
		}
		delete cursor;
		return false;
	}

	void processEventBuffer() {
		Event cur_event = gui_event_buffer.getEvent();
		if (cur_event.getEventType() != null_event) {
			if (cur_event.getEventType() == clicked && cur_event.getFirstObject()->getObjectType() != redactor_back) {
				delete selected_object;
				selected_object = new Object(*cur_event.getFirstObject());
				selected_object->setAutoOrigin();
			}
		}
		
	}

public:

	GUIManager() {}

	GUIManager(std::vector<std::vector<Object*>> * redactor_mode_objects) {
		if (settings.isRedactorMode()) {
			gui_elements.resize(redactor_mode_objects->size() + 1);

			// redactor background init
			Object * redactor_back = new Object(
				Point(settings.getWindowWidth() / 2 - 200, -settings.getWindowHeight() / 2), 
				Point(), 
				(ObjectType)is_object_type_exists("redactor_back"), 
				(CollisionType)is_collision_type_exists("redactor_back"),
				VisualInfo(
					(SpriteType)is_sprite_type_exists("redactor_back"), 
					(AnimationType)is_animation_type_exists("hold"),
					1));

			gui_elements[0].push_back(redactor_back);

			Point new_position = Point((redactor_back->getPosition().x + settings.getWindowWidth() / 2) / 2, 100 - settings.getWindowHeight() / 2);
			int objects_offset = 150;
			for (int layer = 0; layer < redactor_mode_objects->size(); layer++) {
				for (int i = 0; i < (*redactor_mode_objects)[layer].size(); i++) {
					Object * object = new Object(*(*redactor_mode_objects)[layer][i]);
					object->setPosition(new_position);
					object->setOrigin(Point(50,50));
					new_position.y += objects_offset;

					gui_elements[1].push_back(object);
				}
			}
		}
	}

	void scrollRedactor(double offset) {
		if (settings.isRedactorMode()) {
			int scroll_bar_layer = 1;
			if (gui_elements.size() > scroll_bar_layer) {
				for (int i = 0; i < gui_elements[scroll_bar_layer].size(); i++) {
					if (gui_elements[scroll_bar_layer][0]->getPosition().y < -settings.getWindowHeight() / 2 + 200 && 
						gui_elements[scroll_bar_layer][gui_elements[scroll_bar_layer].size() - 1]->getPosition().y > settings.getWindowHeight() / 2 - 200) {
						gui_elements[scroll_bar_layer][i]->forceChangePosition(Point(0, offset));
					}
				}
			}
		}
	}

	bool loadFromFile(std::string path) {
		gui_parser(path, &gui_elements);
	}

	bool processFrame(Point click, Point viewport_pos) {
		bool output = checkClick(click);
		processEventBuffer();
		if (settings.isRedactorMode() && !output && click != Point(10000000, 100000000)) {
			if (selected_object != nullptr) {
				Object * output_obj = new Object(*selected_object);
				output_obj->setPosition(click + viewport_pos);
				external_event_buffer.addEvent(create_new, output_obj, nullptr);
				selected_object = nullptr;
				output = true;
			}

		}

		return output;
	}

	Event getEvent() {
		return external_event_buffer.getEvent();
	}

	std::vector<std::vector<Object *>> * getObjectsBuffer() {
		return &gui_elements;
	}

	Object * getSelectedObject() {
		return selected_object;
	}
};