#ifndef __Enemy_Skeleton_H__
#define __Enemy_Skeleton_H__

#include "j1Module.h"
#include "Animation.h"
#include "j1Player.h"
#include "Entity.h"
#include "Enemy.h"

struct SDL_Texture;
struct Collider;
struct Entity_Info;

enum class Enemy_Skeleton_State
{
	walking,
	idle,
	dead
};


class Enemy_Skeleton : public Enemy
{
public:
	Enemy_Skeleton(fPoint position, fPoint spawn_pos, Properties *properties);

	~Enemy_Skeleton();

	bool Update(float dt);

	bool Draw();

	bool Reset();

	bool OnCollision(Collider* c1, Collider* c2);

	bool FollowPath(float dt);

public:

	// States ================================
	Enemy_Skeleton_State	current_state = Enemy_Skeleton_State::walking;

private:

	//Bool ===================================
	bool				    flip_x = false;
	bool				    dead_fx = false;
	//Vars ===================================
	int						margin_flip;
	fPoint					skeleton_pos;
	//-----------Textures-------------------
	SDL_Texture				*tex_skeleton = nullptr;

	//----------Animations-----------------
	Animation         skeleton_attack_anim;
	Animation         skeleton_dead_anim;
	Animation         skeleton_walking_anim;
	Animation         skeleton_idle_anim;

	//------------Sfx----------------------
	uint			  fx_skaleton_death;


};

#endif // __Enemy_Skeleton_H__