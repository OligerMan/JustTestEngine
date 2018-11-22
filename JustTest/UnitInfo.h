#pragma once
#include <algorithm>
#include <string>
#include <fstream>

enum FactionList {
	null_faction,
	hero_faction,
	friendly_faction,
	neutral_faction,
	agressive_faction,

	FACTION_COUNT
};

std::vector<std::string> faction_type;


void faction_type_init() {
	faction_type.resize(FACTION_COUNT);

	faction_type[null_faction] = "null";

	faction_type[hero_faction] = "hero";
	faction_type[friendly_faction] = "friendly";
	faction_type[neutral_faction] = "neutral";
	faction_type[agressive_faction] = "agressive";
}

int is_faction_type_exists(std::string example) {
	for (int i = 0; i < FACTION_COUNT; i++) {
		if (faction_type[i] == example) {
			return i;
		}
	}
	return -1;
}

class UnitInfo {
	double EPS = 0.00001;

	double max_hp = 0;
	double hp = 0;        // health points
	bool inf_hp = true;
	double max_mana = 0;
	double mana = 0;      // mana points
	bool inf_mana = true;
	double max_endur = 0;
	double endur = 0;     // endurance
	bool inf_endur = true;

	double attack1_range = 150;
	double attack2_range = 0;
	double attack3_range = 0;

	double attack1_damage = 10;
	double attack2_damage = 0;
	double attack3_damage = 0;

	double default_speed = 0.07;
	double speed_coef = 1;

	int faction = 0;              // faction index: 0 for non-unit objects, 1 for hero and his allies, from 2 to infinity(2^31) for other factions

	bool is_effects_allowed = true;

	bool unit_info_correct = true;


	void parseUnitInfo(std::string path) {
		std::ifstream unit_input;
		unit_input.open(path);

		if (!unit_input.is_open()) {
			if (settings.isErrorOutputEnabled()) {
				std::cout << "Unit info file " << path << " not found" << std::endl;
			}
			return;
		}

		if (settings.isUnitInfoDebugEnabled()) {
			std::cout << "Unit file from " << path << " configuration" << std::endl;
		}

		std::string setting, value;
		while (true) {
			if (settings.isUnitInfoDebugEnabled()) {
				std::cout << "    ";
			}
			unit_input >> setting;
			unit_input >> value;
			if (setting == "") {
				unit_info_correct = false;
				if (settings.isUnitInfoDebugEnabled()) {
					std::cout << "Unit info file " << path << " error" << std::endl;
				}
				break;
			}
			if (setting == "max_hp") {
				max_hp = std::stoi(value);
				hp = max_hp;
				if (settings.isUnitInfoDebugEnabled()) {
					std::cout << "Max health for " << path << " stated to " << hp << std::endl;
				}
			}
			if (setting == "inf_hp") {
				inf_hp = std::stoi(value);
				if (settings.isUnitInfoDebugEnabled()) {
					std::cout << "Infinite health is set to " << inf_hp << std::endl;
				}
			}
			if (setting == "max_mana") {
				max_mana = std::stoi(value);
				mana = max_mana;
				if (settings.isUnitInfoDebugEnabled()) {
					std::cout << "Max mana for " << path << " stated to " << mana << std::endl;
				}
			}
			if (setting == "inf_mana") {
				inf_mana = std::stoi(value);
				if (settings.isUnitInfoDebugEnabled()) {
					std::cout << "Infinite mana is set to " << inf_mana << std::endl;
				}
			}
			if (setting == "max_endur") {
				max_endur = std::stoi(value);
				endur = max_endur;
				if (settings.isUnitInfoDebugEnabled()) {
					std::cout << "Max endurance for " << path << " stated to " << endur << std::endl;
				}
			}
			if (setting == "inf_endur") {
				inf_endur = std::stoi(value);
				if (settings.isUnitInfoDebugEnabled()) {
					std::cout << "Infinite endurance is set to " << inf_endur << std::endl;
				}
			}
			if (setting == "attack1_range") {
				attack1_range = std::stoi(value);
				if (settings.isUnitInfoDebugEnabled()) {
					std::cout << "Attack range 1 is set to " << attack1_range << std::endl;
				}
			}
			if (setting == "attack2_range") {
				attack2_range = std::stoi(value);
				if (settings.isUnitInfoDebugEnabled()) {
					std::cout << "Attack range 2 is set to " << attack2_range << std::endl;
				}
			}
			if (setting == "attack3_range") {
				attack3_range = std::stoi(value);
				if (settings.isUnitInfoDebugEnabled()) {
					std::cout << "Attack range 3 is set to " << attack3_range << std::endl;
				}
			}
			if (setting == "attack1_damage") {
				attack1_damage = std::stoi(value);
				if (settings.isUnitInfoDebugEnabled()) {
					std::cout << "Attack damage 1 is set to " << attack1_damage << std::endl;
				}
			}
			if (setting == "attack2_damage") {
				attack2_damage = std::stoi(value);
				if (settings.isUnitInfoDebugEnabled()) {
					std::cout << "Attack damage 2 is set to " << attack2_damage << std::endl;
				}
			}
			if (setting == "attack3_damage") {
				attack3_damage = std::stoi(value);
				if (settings.isUnitInfoDebugEnabled()) {
					std::cout << "Attack damage 3 is set to " << attack3_damage << std::endl;
				}
			}
			if (setting == "default_speed") {
				default_speed = std::stoi(value); 
				if (settings.isUnitInfoDebugEnabled()) {
					std::cout << "Default speed is set to " << default_speed << std::endl;
				}
			}
			if (setting == "speed_coef") {
				speed_coef = std::stoi(value);
				if (settings.isUnitInfoDebugEnabled()) {
					std::cout << "Speed coefficient is set to " << speed_coef << std::endl;
				}
			}
			if (setting == "is_effects_allowed") {
				is_effects_allowed = std::stoi(value);
				if (settings.isUnitInfoDebugEnabled()) {
					std::cout << "Effects vulnerability is set to " << is_effects_allowed << std::endl;
				}
			}
			if (setting == "faction") {
				faction = std::stoi(value);
				if (settings.isUnitInfoDebugEnabled()) {
					std::cout << "Faction is set to " << faction << std::endl;
				}
			}
			if (setting == "unit_info_end") {
				if (settings.isUnitInfoDebugEnabled()) {
					std::cout << "Unit info config end" << std::endl;
				}
				break;
			}
		}
	}

public:

	UnitInfo() {}

	UnitInfo(std::string path) {
		parseUnitInfo(path);
	}

	void setHealth(double new_hp) {
		hp = new_hp;
	}

	void setMana(double new_mana) {
		mana = new_mana;
	}

	void setEndurance(double new_endurance) {
		endur = new_endurance;
	}

	bool dealDamage(double damage) {
		if (!inf_hp && damage > 0) {
			hp = std::max(-EPS, hp - damage);
		}
		return (hp > 0);
	}

	void grantHeal(double heal) {
		if (!inf_hp && heal > 0) {
			hp = std::min(max_hp, hp + heal);
		}
	}

	bool dealManaBurn(double mana_burn) {
		if (!inf_mana && mana_burn > 0) {
			mana = std::max(EPS, mana - mana_burn);
		}
		return (mana > 0);
	}

	void grantManaRestore(double mana_restore) {
		if (!inf_mana && mana_restore > 0) {
			mana = std::min(max_mana, mana + mana_restore);
		}
	}

	bool dealEndurBurn(double endur_burn) {
		if (!inf_endur && endur_burn > 0) {
			endur = std::max(EPS, endur - endur_burn);
		}
		return (endur > 0);
	}

	void grantEndurRestore(double endur_restore) {
		if (!inf_endur && endur_restore > 0) {
			endur = std::min(max_endur, endur + endur_restore);
		}
	}

	double getHealth() {
		return hp;
	}

	double getMaxHealth() {
		return max_hp;
	}

	double getMana() {
		return mana;
	}

	double getMaxMana() {
		return max_mana;
	}

	double getEndurance() {
		return endur;
	}

	double getMaxEndurance() {
		return max_endur;
	}

	bool isDead() {
		return (!inf_hp && (hp < 0));
	}

	int getFaction() {
		return faction;
	}

	void setFaction(int new_faction) {
		faction = new_faction;
	}

	double getAttackRange1() {
		return attack1_range;
	}

	double getAttackRange2() {
		return attack2_range;
	}

	double getAttackRange3() {
		return attack3_range;
	}

	double getAttackDamage1() {
		return attack1_damage;
	}

	double getAttackDamage2() {
		return attack2_damage;
	}

	double getAttackDamage3() {
		return attack3_damage;
	}

};