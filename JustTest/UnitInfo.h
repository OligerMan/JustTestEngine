#pragma once
#include <algorithm>

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

	double default_speed = 0.1;
	double speed_coef = 1;

	int faction = 0;              // faction index: 0 for non-unit objects, 1 for hero and his allies, from 2 to infinity(2^31) for other factions

	bool is_effects_allowed = true;

public:

	bool dealDamage(double damage) {
		if (!inf_hp && damage > 0) {
			hp = std::max(EPS, hp - damage);
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

	double getEndur() {
		return endur;
	}

	double getMaxEndur() {
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

};