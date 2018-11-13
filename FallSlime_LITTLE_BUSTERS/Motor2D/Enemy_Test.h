#ifndef __Enemy_Test_H__
#define __Enemy_Test_H__

#include "Enemy.h"

struct SDL_Texture;
struct Collider;

class Enemy_Test: public Enemy
{
public:
	Enemy_Test(fPoint position, SDL_Rect collider_rect);

	bool Move(float dt) ;
	bool Draw(SDL_Texture* sprites) ;
	bool OnCollision(Collider* collider) ;
};

#endif // __Enemy_Test_H__
