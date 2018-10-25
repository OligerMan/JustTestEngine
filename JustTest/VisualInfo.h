#pragma once

enum SpriteType {
	null_sprite,

	COUNT_SPRITE
};

std::vector<std::string> sprite_type;

void sprite_type_init() {
	sprite_type.resize(COUNT_SPRITE);

	sprite_type[null_sprite] = "null";
}

int is_sprite_type_exists(std::string example) {
	for (int i = 0; i < COUNT_SPRITE; i++) {
		if (sprite_type[i] == example) {
			return i;
		}
	}
	return -1;
}

enum AnimationType {
	null_animation,

	COUNT_ANIMATION
};

std::vector<std::string> animation_type;

void animation_type_init() {
	animation_type.resize(COUNT_ANIMATION);

	animation_type[null_animation] = "null";
}

int is_animation_type_exists(std::string example) {
	for (int i = 0; i < COUNT_ANIMATION; i++) {
		if (animation_type[i] == example) {
			return i;
		}
	}
	return -1;
}

struct VisualInfo {
	SpriteType object_spr_type = SpriteType::null_sprite;
	AnimationType animation_type = AnimationType::null_animation;
	int frame_num = 0;

	VisualInfo() {}

	VisualInfo(SpriteType sprite_type, AnimationType animation_type) : object_spr_type(sprite_type), animation_type(animation_type) {}
};