#pragma once

#include <fstream>
#include <string>


class Settings {
	int window_height;
	int window_width;
	bool sprite_loader_debug_output_enabled = true;
	bool console_enabled = true;
	bool error_output_enabled = true;
	bool redactor_mode = false;
	bool gamepad_debug_output = true;
	bool collision_debug_mode = false;
	bool navigation_debug_mode = false;

	bool is_settings_loaded = false;

public:

	void setDefaults() {
		window_height = 720;
		window_width = 1200;
	}

	Settings() {

		setDefaults();

		std::ifstream settings_input;
		settings_input.open("config.cfg");
		
		if (settings_input.is_open()) {
			std::string setting, value;
			is_settings_loaded = true;
			while (true) {
				settings_input >> setting;
				settings_input >> value;
				if (setting == "") {
					break;
				}
				if (setting == "error_output") {
					error_output_enabled = std::stoi(value);
				}
				if (setting == "sprt_debug") {
					sprite_loader_debug_output_enabled = std::stoi(value);
				}
				if (setting == "window_height") {
					window_height = std::stoi(value);
				}
				if (setting == "window_width") {
					window_width = std::stoi(value);
				}
				if (setting == "collision_debug") {
					collision_debug_mode = std::stoi(value);
				}
				if (setting == "gamepad_debug") {
					gamepad_debug_output = std::stoi(value);
				}
				if (setting == "navigation_debug") {
					navigation_debug_mode = std::stoi(value);
				}
				if (setting == "config_end") {
					is_settings_loaded = true;
					break;
				}
			}
		}
		else {
			is_settings_loaded = false;
		}
		
		settings_input.close();
	}

	bool isLoaded() {
		return is_settings_loaded;
	}

	int getWindowHeight() {
		return window_height;
	}

	int getWindowWidth() {
		return window_width;
	}

	bool isSpriteDebugOutputEnabled() {
		return sprite_loader_debug_output_enabled;
	}

	bool isErrorOutputEnabled() {
		return error_output_enabled;
	}

	bool isRedactorMode() {
		return redactor_mode;
	}

	void setRedactorMode(bool state) {
		redactor_mode = state;
	}

	bool isGamepadDebugEnabled() {
		return gamepad_debug_output;
	}

	bool isCollisionDebugMode() {
		return collision_debug_mode;
	}

	bool isNavigationGridMode() {
		return navigation_debug_mode;
	}
};

Settings settings;
