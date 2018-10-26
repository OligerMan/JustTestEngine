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
			for (unsigned layer = gui_elements.size() - 1; layer >= 0; layer--) {
				for (unsigned i = 0; i < gui_elements[layer].size(); i++) {
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
			}
		}
		delete cursor;
		return false;
	}

	void processEventBuffer() {
		Event event = gui_event_buffer.getEvent();

	}

public:

	GUIManager() {}

	bool loadFromFile(std::string path) {
		gui_parser(path, &gui_elements);
	}

	void loadRedactorGUI() {
		const int GUI_LAYERS = 2;
		gui_elements.resize(GUI_LAYERS);


	}

	bool processFrame(Point click) {
		bool output = checkClick(click);
		processEventBuffer();
		if (settings.isRedactorMode() && !output) {
			if (selected_object != nullptr) {
				Object * output = new Object(*selected_object);
				output->setPosition(click);
				external_event_buffer.addEvent(create_new, output, nullptr);
			}

			output = true;
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