#pragma once

class Constants {

	const double speed_damage_threshold = 100;
	const double speed_damage_coef = 1;
	const double default_hero_speed = 0.1;

public:

	const double getSpeedDamageThreshold() {
		return speed_damage_threshold;
	}

	const double getSpeedDamageCoef() {
		return speed_damage_coef;
	}
	
	const double getDefualtHeroSpeed() {
		return default_hero_speed;
	}
};

Constants consts;