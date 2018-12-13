#ifndef __Coin_H__
#define __Coin_H__

#include "j1Module.h"
#include "p2List.h"
#include "p2Point.h"
#include "Animation.h"
#include "Entity.h"

struct SDL_Texture;
struct Collider;
struct Entity_Info;

enum class Coin_States
{
	enable,
	disable
};

class Coin : public Entity
{
public:

	Coin(fPoint position, fPoint spawn_pos, Properties *properties);

	~Coin();


	bool Update(float dt);

	bool Draw();

	bool Reset();

	bool OnCollision(Collider* c1, Collider* c2);

	bool Active();

	bool Desactive();

private:

	Coin_States		  current_state = Coin_States::enable;
	//-----------Vars-----------------------
	bool			  enable_fx = false;
	uint  			  coin_counter = NULL;
	Collider*         main_collider = nullptr;
	fPoint			  moving_pos;
	bool			  moving = true;
	//-----------Textures-------------------
	SDL_Texture      *tex_coin = nullptr;

	//----------Animations-----------------
	Animation         coin_anim;

	//------------Sfx----------------------
	uint		      fx_pick_up;
};


#endif // !__Coin_H__