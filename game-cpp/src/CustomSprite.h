#pragma once

#include <godot_cpp/classes/sprite2d.hpp>


class CustomSprite : public godot::Sprite2D
{
	GDCLASS(CustomSprite, Sprite2D)

public:
	void _process(double delta) override;

protected:
	static void _bind_methods();
	double time_passed = 0.0;
};