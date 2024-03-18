#include "CustomSprite.h"

using namespace godot;


void CustomSprite::_process(double delta)
{
	time_passed += delta;
	Vector2 new_position = Vector2
	(
		10.0 + 10.0 * sin(time_passed * 2.0),
		10.0 + 10.0 * cos(time_passed * 2.0)
	);

	set_position(new_position);
}

void CustomSprite::_bind_methods()
{
}