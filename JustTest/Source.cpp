#include "VisualController.h"
#include "GUIVisualController.h"
#include "GUIManager.h"

enum XBOXGamepadButtons {
	A,B,X,Y,LB,RB,BACK,START,LSTICK,RSTICK
};

void gameCycle() {
	collision_type_init();
	object_type_init();
	event_type_init();
	sprite_type_init();
	animation_type_init();

	if (!settings.isLoaded()) {
		std::cout << "Settings file error, enabling default settings" << std::endl;
		settings.setDefaults();
	}

	sf::RenderWindow window(sf::VideoMode(settings.getWindowWidth(), settings.getWindowHeight()), "JustTest", sf::Style::None);
	sf::View view1(sf::Vector2f(0.0, 0.0), sf::Vector2f(settings.getWindowWidth(), settings.getWindowHeight()));

	VisualController visual_ctrl;
	GUIVisualController gui_visual_ctrl;
	Map game_map1;
	GUIManager gui_manager;


	window.setView(view1);

	bool is_game_cycle = true;


	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
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

		// game cycle
		if (is_game_cycle) {
			is_game_cycle = visual_ctrl.processFrame(&window, game_map1.getObjectsBuffer());
			is_game_cycle = gui_visual_ctrl.processFrame(&window, gui_manager.getObjectsBuffer()) || is_game_cycle;
		}
		// input handling

		Point cursor_pos;
		sf::Vector2i mouse_pos = sf::Mouse::getPosition();
		sf::Vector2i window_pos = window.getPosition();
		cursor_pos = Point(mouse_pos.x - settings.getWindowWidth() / 2 - window.getPosition().x, mouse_pos.y - settings.getWindowHeight() / 2 - window.getPosition().y);


		if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
			if (!gui_manager.processFrame(cursor_pos)) {
				game_map1.processFrame(cursor_pos);
			}
		}
		else {
			if (!gui_manager.processFrame(Point(10000000, 100000000))) {
				game_map1.processFrame(Point(10000000, 100000000));
			}
		}

		double hero_speed = consts.getDefualtHeroSpeed();
		Object * hero_object = game_map1.getHero();
		hero_object->setSpeed(Point(0, 0));
		hero_object->setAnimationType(hold_anim);
		hero_object->setAngle(0);

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
		else {
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
				hero_object->changeSpeed(Point(0, -1));
				hero_object->setAnimationType(move_anim);
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
				hero_object->changeSpeed(Point(-1, 0));
				hero_object->setAnimationType(move_anim);
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
				hero_object->changeSpeed(Point(0, 1));
				hero_object->setAnimationType(move_anim);
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
				hero_object->changeSpeed(Point(1, 0));
				hero_object->setAnimationType(move_anim);
			}
		}
		
		hero_object->setAngle(atan2(hero_object->getSpeed().y, hero_object->getSpeed().x) / PI * 180);
		hero_object->setSpeed(hero_object->getSpeed().getNormal() * hero_speed);

		// viewport positioning
		const double view_speed_coef = 0.006;    // must be from 0 to 1, where 0 for static camera and 1 for camera istantly over hero
		Point hero_position = hero_object->getPosition();
		Point diff = (hero_position - Point(view1.getCenter())) * view_speed_coef;
		view1.setCenter(view1.getCenter() + sf::Vector2f(diff.x, diff.y));
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
	gameCycle();

	return 0;
}