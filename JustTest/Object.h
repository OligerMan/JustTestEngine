#pragma once

#include "Collision.h"
#include "ObjectTypes.h"
#include "VisualInfo.h"
#include "UnitInfo.h"

class Object {

	CollisionModel col_model;

	CollisionType object_col_type;
	VisualInfo vis_info;
	UnitInfo unit_info;
	ObjectType object_type;

public:

	Object() {}

	Object(Point point) {
		col_model.setPosition(point);
		col_model.addCircle(Point(), 0);
	}

	Object(Point point, Point origin) : Object(point) {
		col_model.setOrigin(origin);
	}

	Object(Point point, CollisionType obj_col_type, VisualInfo visual_info) : Object(point) {
		object_col_type = obj_col_type;
		vis_info = visual_info;
	}

	Object(Point point, Point origin, CollisionType obj_col_type, VisualInfo visual_info) : Object(point, origin) {
		object_col_type = obj_col_type;
		vis_info = visual_info;
	}

	Object(Point point, Point origin, ObjectType new_object_type, CollisionType obj_col_type, VisualInfo visual_info) : Object(point, origin, obj_col_type, visual_info) {
		object_type = new_object_type;

		col_model = *(new CollisionModel("collision/" + collision_type[new_object_type] + ".col"));
		col_model.setPosition(point);
		col_model.setOrigin(origin);
	}

	CollisionModel * getCollisionModel() {
		return &col_model;
	}

	CollisionType getObjectCollisionType() {
		return object_col_type;
	}

	SpriteType getObjectSpriteType() {
		return vis_info.object_spr_type;
	}

	AnimationType getObjectAnimationType() {
		return vis_info.animation_type;
	}

	int getFrameNumber() {
		return vis_info.frame_num / vis_info.frame_duration;
	}

	int getFrameDuration() {
		return vis_info.frame_duration;
	}

	void frameIncrement() {
		vis_info.frame_num++;
	}

	Point getSpeed() {
		return col_model.getSpeed();
	}

	void setSpeed(Point speed) {
		col_model.setSpeed(speed);
	}

	void changeSpeed(Point difference) {
		col_model.changeSpeed(difference);
	}

	Point getPosition() {
		return col_model.getPosition();
	}

	void setPosition(Point origin) {
		col_model.setPosition(origin);
	}

	void changePosition(Point difference) {
		col_model.changePosition(difference);
	}

	Point getOrigin() {
		return col_model.getOrigin();
	}

	void setOrigin(Point origin) {
		col_model.setOrigin(origin);
	}

	void changeOrigin(Point difference) {
		col_model.changeOrigin(difference);
	}

	void setAutoOrigin() {
		col_model.setAutoOrigin();
	}

	double getAngle() {
		return col_model.getAngle();
	}

	void setAngle(double angle) {
		col_model.setAngle(angle);
	}

	void changeAngle(double difference) {
		col_model.changeAngle(difference);
	}

	ObjectType getObjectType() {
		return object_type;
	}

	UnitInfo * getUnitInfo() {
		return &unit_info;
	}

	void setAnimationType(AnimationType animation_type) {
		vis_info.animation_type = animation_type;
	}
};

bool checkObjectCollision(Object * obj1, Object * obj2) {
	return checkModelCollision(obj1->getCollisionModel(), obj2->getCollisionModel());
}