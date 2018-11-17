#include "VisualController.h"
#include "GUIVisualController.h"
#include "GUIManager.h"
#include "Timer.h"

#include <chrono>
#include <Windows.h>

enum XBOXGamepadButtons {
	A,B,X,Y,LB,RB,BACK,START,LSTICK,RSTICK
};

void gameCycle(std::string map_name) {
	collision_type_init();
	object_type_init();
	event_type_init();
	sprite_type_init();
	animation_type_init();
	faction_type_init();

	std::chrono::time_point<std::chrono::system_clock> frame_start = std::chrono::system_clock::now();
	std::chrono::time_point<std::chrono::system_clock> frame_end = std::chrono::system_clock::now();
	int frame_latency = 1;

	if (!settings.isLoaded()) {
		std::cout << "Settings file error, enabling default settings" << std::endl;
		settings.setDefaults();
	}

	sf::RenderWindow window(sf::VideoMode(settings.getWindowWidth(), settings.getWindowHeight()), "JustTest", sf::Style::None);
	sf::View view1(sf::Vector2f(0.0, 0.0), sf::Vector2f(settings.getWindowWidth(), settings.getWindowHeight()));

	VisualController visual_ctrl;
	GUIVisualController gui_visual_ctrl;
	Map game_map1("maps/" + map_name + ".map");

	if (settings.isSpriteDebugOutputEnabled() && settings.isRedactorMode()) {
		std::cout << "Redactor mode objects upload" << std::endl;
	}

	Map object_presets("redactor_resources/object_templates.map");
	GUIManager gui_manager(object_presets.getObjectsBuffer());

	if (settings.isSpriteDebugOutputEnabled() && settings.isRedactorMode()) {
		std::cout << "Redactor mode objects upload completed" << std::endl;
	}

	window.setView(view1);

	bool is_game_cycle = true;
	int frame_num = 0;


	while (window.isOpen())
	{
		//frame_start = std::chrono::system_clock::now();
		frame_num++;

		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		timer.processTime();

		Point viewport_pos = Point(view1.getCenter().x, view1.getCenter().y);

		// game cycle

		if (is_game_cycle) {
			window.clear(sf::Color::Black);
			is_game_cycle = visual_ctrl.processFrame(&window, game_map1.getObjectsBuffer());
			is_game_cycle = gui_visual_ctrl.processFrame(&window, gui_manager.getObjectsBuffer(), viewport_pos) && is_game_cycle;
			
			if (settings.isCollisionDebugMode()) {

				for (int x = 0; x < window.getSize().x; x+=3) {
					for (int y = 0; y < window.getSize().y; y+=3) {
						if (game_map1.isClickable(Point(x, y) + viewport_pos - Point(settings.getWindowWidth() / 2, settings.getWindowHeight() / 2))) {
							sf::Vertex line[] =
							{
								sf::Vertex(sf::Vector2f(x + viewport_pos.x - settings.getWindowWidth() / 2, y + viewport_pos.y - settings.getWindowHeight() / 2)),
								sf::Vertex(sf::Vector2f(x + viewport_pos.x - settings.getWindowWidth() / 2, y + viewport_pos.y + 1 - settings.getWindowHeight() / 2))
							};
							line->color = sf::Color::Red;

							window.draw(line, 2, sf::Lines);
						}
					}
				}
			}

			if (settings.isNavigationGridMode()) {
				// render navigation grid
				Point grid_offset = game_map1.getNavigationGridOffset();
				double grid_size = game_map1.getNavigationGridSize();

				for (int x = 0; x < window.getSize().x; x += 3) {
					for (int y = 0; y < window.getSize().y; y += 3) {
						Point grid_point = (Point(x, y) + viewport_pos - Point(settings.getWindowWidth() / 2, settings.getWindowHeight() / 2));
						if (game_map1.getNavigationGridElem((grid_point.x - grid_offset.x) / grid_size, (grid_point.y - grid_offset.y) / grid_size)) {
							sf::Vertex line[] =
							{
								sf::Vertex(sf::Vector2f(grid_point.x, grid_point.y)),
								sf::Vertex(sf::Vector2f(grid_point.x, grid_point.y + 1))
							};
							line->color = sf::Color::Green;

							window.draw(line, 2, sf::Lines);
						}
					}
				}

				// render navigation paths
				std::vector<std::vector<Point>> navigation_paths = game_map1.getNavigationPaths();
				for (int path_num = 0; path_num < navigation_paths.size(); path_num++) {
					for (int i = 1; i < navigation_paths[path_num].size(); i++) {
						sf::Vertex line[] =
						{
							sf::Vertex(sf::Vector2f(navigation_paths[path_num][i - 1].x/* + viewport_pos.x - settings.getWindowWidth() / 2*/, navigation_paths[path_num][i - 1].y/* + viewport_pos.y - settings.getWindowHeight() / 2*/)),
							sf::Vertex(sf::Vector2f(navigation_paths[path_num][i].x/* + viewport_pos.x - settings.getWindowWidth() / 2*/, navigation_paths[path_num][i].y/* + viewport_pos.y - settings.getWindowHeight() / 2*/))
						};
						line->color = sf::Color::Blue;

						window.draw(line, 2, sf::Lines);
					}
				}
			}
			
		}
		// input handling

		Point cursor_pos;
		sf::Vector2i mouse_pos = sf::Mouse::getPosition();
		sf::Vector2i window_pos = window.getPosition();
		cursor_pos = Point(mouse_pos.x - settings.getWindowWidth() / 2 - window.getPosition().x, mouse_pos.y - settings.getWindowHeight() / 2 - window.getPosition().y);

		if (settings.isRedactorMode() && is_game_cycle) {
			if (gui_manager.getSelectedObject() != nullptr) {

				Object * object = new Object(*gui_manager.getSelectedObject());
				object->setPosition(cursor_pos + viewport_pos);
				visual_ctrl.drawObject(object, &window);
				delete object;
			}
		}
		

		if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
			if (!gui_manager.processFrame(cursor_pos, viewport_pos)) {
				game_map1.processFrame(cursor_pos + viewport_pos, frame_num % 100 == 1);
			}
		}
		else {
			if (!gui_manager.processFrame(Point(10000000, 100000000), viewport_pos)) {
				game_map1.processFrame(Point(10000000, 100000000), frame_num % 100 == 1);
			}
		}

		Event gui_event = gui_manager.getEvent();
		if (gui_event.getFirstObject() != nullptr) {
			switch (gui_event.getEventType()) {
			case create_new:
				game_map1.addObject(gui_event.getFirstObject(), 0);
			default:
				break;
			}
		}

		double hero_speed = consts.getDefaultHeroSpeed();
		Object * hero_object = game_map1.getHero();
		if (hero_object != nullptr) {
			if (hero_object != nullptr) {
				hero_object->setSpeed(Point(0, 0));
				hero_object->setAnimationType(hold_anim);
			}

		}
		
		
		if (settings.isRedactorMode()) {
			const double viewport_speed = 0.1;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
				view1.move(0, -viewport_speed);
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
				view1.move(-viewport_speed, 0);
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
				view1.move(0, viewport_speed);
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
				view1.move(viewport_speed, 0);
			}
			double scroll_bar_speed = 0.1;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
				gui_manager.scrollRedactor(-scroll_bar_speed);
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
				gui_manager.scrollRedactor(scroll_bar_speed);
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::K)) {
				saveMap("maps/" + map_name + ".map", game_map1.getObjectsBuffer());
				return;
			}
		}
		else {
			if (hero_object != nullptr) {
				if (sf::Joystick::isConnected(0)) {         // gamepad input

					hero_object->setSpeed(Point(
						sf::Joystick::getAxisPosition(0, sf::Joystick::X),
						sf::Joystick::getAxisPosition(0, sf::Joystick::Y)));
					if (abs(hero_object->getSpeed().x) < 1 && abs(hero_object->getSpeed().y) < 1) {
						hero_object->setSpeed(Point());
					}
					else {
						hero_object->setAnimationType(move_anim);
					}

					int buttons_count = sf::Joystick::getButtonCount(0);
					for (int i = 0; i < buttons_count; i++) {
						if (sf::Joystick::isButtonPressed(0, i)) {
							if (settings.isGamepadDebugEnabled()) {
								std::cout << "Gamepad button number " << i << " is pressed" << std::endl;
							}
						}
					}
				}
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
					hero_object->changeSpeed(Point(0, -1));
					hero_object->setAnimationType(move_anim);
				}
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
					hero_object->changeSpeed(Point(-1, 0));
					hero_object->setAnimationType(move_anim);
				}
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
					hero_object->changeSpeed(Point(0, 1));
					hero_object->setAnimationType(move_anim);
				}
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
					hero_object->changeSpeed(Point(1, 0));
					hero_object->setAnimationType(move_anim);
				}

				if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)) {
					settings.switchNavigationGridMode();
					Sleep(100);
				}

				if (hero_object->getSpeed().getLength() != 0) {
					hero_object->setAngle(atan2(hero_object->getSpeed().y, hero_object->getSpeed().x) / PI * 180);
					hero_object->setSpeed(hero_object->getSpeed().getNormal() * hero_speed);
				}
			}
		}
		

		// viewport positioning
		if (settings.isRedactorMode()) {

		}
		else {
			if (hero_object != nullptr) {
				const double view_speed_coef = 0.006;    // must be from 0 to 1, where 0 for static camera and 1 for camera istantly over hero
				Point hero_position = hero_object->getPosition();
				Point diff = (hero_position - Point(view1.getCenter())) * view_speed_coef;
				view1.setCenter(view1.getCenter() + sf::Vector2f(diff.x, diff.y));
			}
		}
		window.setView(view1);

		window.display();
	}
}

int main() {
	std::cout << "Type g and press Enter for game mode and type r for redactor mode" << std::endl;

	std::string input = "kek";
	while (input != "r" && input != "g") {
		std::cin >> input;
	}
	if (input == "g") {
		settings.setRedactorMode(false);
	}
	else {
		settings.setRedactorMode(true);
	}
	std::cout << "Enter map name" << std::endl;
	std::cin >> input; // path to game map
	gameCycle(input);

	return 0;
}