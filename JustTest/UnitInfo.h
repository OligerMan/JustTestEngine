#pragma once
#include <algorithm>

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

};