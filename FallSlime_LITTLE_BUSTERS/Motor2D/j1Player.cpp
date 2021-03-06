#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Render.h"
#include "SceneManager.h"
#include "j1Window.h"
#include "j1Collision.h"
#include "j1Player.h"
#include "Enemy_Bat.h"
#include "Enemy_Skeleton.h"
#include "j1Map.h"
#include <math.h>
#include "j1FadeToBlack.h"
#include "EntityManager.h"
#include "Hud.h"
#include "j1Scene.h"
#include "Brofiler/Brofiler.h"



j1Player::j1Player(fPoint position, fPoint spawn_pos, Properties * properties) : Entity( position, spawn_pos, properties)
{
	name.create("player");

	// Add components ===================================================================
	Player_Properties* player_properties = (Player_Properties *)properties;
	collider_rect = player_properties->collider_rect;

	// Values ---------------------------------------------
	gravity = player_properties->gravity;
	speed_air = player_properties->speed_air;
	speed_ground = player_properties->speed_ground;
	speed_jump = player_properties->speed_jump;
	speed_gummy_jump = player_properties->speed_gummy_jump;
	speed_attack = player_properties->speed_attack;

	// Colliders ------------------------------------------
	if (god_mode)
	{
		main_collider = App->collision->AddCollider(collider_rect, COLLIDER_GOD, App->entity_manager);
	}
	else
	{
		main_collider = App->collision->AddCollider(collider_rect, COLLIDER_PLAYER, App->entity_manager);
	}

	// Textures ------------------------------------------

	tex_player = player_properties->player_tex;
	death_splash = player_properties->death_tex;
	attack_splash = player_properties->attack_tex;

	// Animations ----------------------------------------
	jumping_anim = player_properties->jumping_anim;
	death_anim = player_properties->death_anim;
	attack_anim = player_properties->attack_anim;

	// Sfx ----------------------------------------------
	fx_jump1 = player_properties->id_jump_fx1;
	fx_jump2 = player_properties->id_jump_fx2;
	fx_jump3 = player_properties->id_jump_fx3;
	fx_jump4 = player_properties->id_jump_fx4;
	fx_jump5 = player_properties->id_jump_fx5;
	id_death_fx = player_properties->id_death_fx;
	fx_attack = player_properties->id_attack_fx;
}

j1Player::~j1Player()
{

}

bool j1Player::HandleInput()
{
	BROFILER_CATEGORY("Player HandleInput", Profiler::Color::LightCoral);

	if (current_state != Player_State::dead)
	{
		if (App->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN)
		{
			if (god_mode)
			{
				main_collider->type = COLLIDER_PLAYER;
				god_mode = false;
			}
			else
			{
				main_collider->type = COLLIDER_GOD;
				god_mode = true;
			}
		}
		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_D) == KEY_IDLE)
		{
			if (on_ground)
				velocity.x = -speed_ground;
			else
				velocity.x = -speed_air;
			flip_x = true;
		}
		else if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_A) == KEY_IDLE)
		{
			if (on_ground)
				velocity.x = +speed_ground;
			else
				velocity.x = +speed_air;
			flip_x = false;
		}
		else
			velocity.x = 0;
	}

	//Only if player is dead
	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && current_state == Player_State::dead)
	{
		App->scene_manager->ResetScene();
	}

	// Only if player is on ground 
	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && current_state == Player_State::boucing && gummy_jump ==false)
	{
		gummy_jump = true;
		App->audio->PlayFx(fx_jump5);
	}

	//Only if player is jumping
	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_DOWN && current_state == Player_State::jumping && attack == false)
	{
		attack = true;
		current_state = Player_State::attack;
		main_collider->type = COLLIDER_ATTACK;
		App->audio->PlayFx(fx_attack);

	}
	//Physics applied

	if (apply_attack)
	{
		if(is_gummy_jumping && attack)
			velocity.y += ((float)attack * speed_attack) + speed_gummy_jump;

		else
			velocity.y += (float)attack * speed_attack;
		attack = false;
		apply_attack = false;		
	}

	if (apply_jump_speed)
	{
		velocity.y = -speed_jump - (float)gummy_jump * speed_gummy_jump;


		if (gummy_jump)
			is_gummy_jumping = true;

		on_ground = false;

		check_fall = false;
		apply_jump_speed = false;
		gummy_jump = false;
	}


	//if (apply_invulnerability && Invulnerability(1.F))
	//	apply_invulnerability = false;

	//Random Jump Fx
	if (on_ground && current_state == Player_State::jumping  )
	{
		current_state = Player_State::boucing;

		if (App->scene_manager->GetCurrentScene()->is_pausable == true)
		{
			uint random_jump = rand() % 4 + 1;

			switch (random_jump)
			{
			case 1:
				App->audio->PlayFx(fx_jump1);
				break;
			case 2:
				App->audio->PlayFx(fx_jump2);
				break;
			case 3:
				App->audio->PlayFx(fx_jump3);
				break;
			case 4:
				App->audio->PlayFx(fx_jump4);
				break;
			default:
				App->audio->PlayFx(fx_jump1);
				break;
			}
		}
	}

	return true;
}

bool j1Player::Update(float dt)
{
	BROFILER_CATEGORY("Player Update", Profiler::Color::LightCyan);

	if (current_state != Player_State::jumping && current_state != Player_State::attack)
	{
		return true;
	}
	
	// God Mode movement =======================================
	int speed_god_mode = 600;
	if (god_mode)
	{
		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
			position.y -= speed_god_mode * dt;
		if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
			position.y += speed_god_mode *dt;
		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
			position.x -= speed_god_mode * dt;
		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
			position.x += speed_god_mode *dt;

		velocity.x = velocity.y = 0;
		return true;
	}
	
	// Normal movement =======================================+
	if (on_ground == false)
	{
		acceleration.y = gravity;
		check_fall = false;
	}
	else
	{
		acceleration.y = 0;
	}
	
	
	velocity += {acceleration.x *dt, acceleration.y *dt};
	
	if (velocity.y > 450)
		velocity.y = 450;

	position += {velocity.x *dt, velocity.y *dt};

	main_collider->SetPos(position.x - collider_rect.w / 2, position.y - collider_rect.h / 2);
	on_ground = false;

	p2List<Direction> directions;
	App->collision->CheckOverlap(directions, main_collider, COLLIDER_WALL, position, velocity);

	if (directions.count() > 0)
	{
		for (p2List_item<Direction> * item = directions.start; item; item = item->next)
		{
			switch (item->data)
			{
			case Direction::down:
				check_fall = true;
				on_ground = true;
				main_collider->type = COLLIDER_PLAYER;
				break;
			case Direction::up:
				velocity.y = 0;
				break;
			}
		}
		main_collider->SetPos(position.x - collider_rect.w / 2, position.y - collider_rect.h / 2);
	}
	return true;
}

// Called each loop iteration

bool j1Player::Draw()
{
	BROFILER_CATEGORY("Player Draw", Profiler::Color::LightGoldenRodYellow);

	SDL_Rect frame; 
	SDL_Texture* texture = nullptr;
	jumping_anim.speed = 43.75F;


	switch ((Player_State)current_state)
	{
	case Player_State::jumping:
		frame = jumping_anim.GetLastFrame();
		texture = tex_player;
		apply_attack = true;   

		break;

	case Player_State::boucing:
		if (jumping_anim.GetFrameValue() > 9)
		{
			current_state = Player_State::jumping;
			apply_jump_speed = true;
			is_gummy_jumping = false;
			frame = jumping_anim.GetCurrentFrame();
			jumping_anim.Reset();
			texture = tex_player;
			break;
		}
		frame = jumping_anim.GetCurrentFrame();
		texture = tex_player;
		break;

	case Player_State::dead:
		frame = death_anim.GetLastFrame();
		texture = death_splash;
		flip_x = false;

		if (god_mode)
		{
			main_collider->type = COLLIDER_PLAYER;
			god_mode = false;
		}
			
		if (!dead_enable)
		{
			App->audio->PlayFx(id_death_fx);
			dead_enable = true;
			App->hud->SetLifes(App->hud->GetLifes() - 1);
		}
		break;
	case Player_State::attack:
		frame = attack_anim.GetCurrentFrame();
		texture = attack_splash;
		
		if (on_ground)
			current_state = Player_State::jumping;
		break;
	default:
		break;
	}
	
	App->render->Blit(texture, (int)position.x - frame.w/2 , (int)position.y - frame.h / 2, &frame  , flip_x );

	return true;
}

bool j1Player::Reset()
{
	BROFILER_CATEGORY("Player Reset", Profiler::Color::LightGray);

	main_collider->SetPos(spawn_pos.x - main_collider->rect.w * 0.5f , spawn_pos.y - main_collider->rect.h * 0.5f);
	velocity.x = 0;
	velocity.y = 0;
	acceleration.x = 0;
	acceleration.y = 0;
	dead_enable = false;
	current_state = Player_State::jumping;
	jumping_anim.Reset();

	if (main_collider != nullptr && main_collider->type != COLLIDER_GOD)
	{
		main_collider->type = COLLIDER_PLAYER;
	}
	return true;
}


//Save and Load
bool j1Player::Load(pugi::xml_node& node) 
{
	bool ret = true;

	velocity.x = node.child("velocity").attribute("x").as_float(0);
	velocity.y = node.child("velocity").attribute("y").as_float(0);
	acceleration.x = node.child("acceleration").attribute("x").as_float(0);
	acceleration.y = node.child("acceleration").attribute("y").as_float(0);

	apply_jump_speed = node.child("conditions").attribute("apply_jump_speed").as_bool(false);
	on_ground = node.child("conditions").attribute("on_ground").as_bool(false);
	check_fall = node.child("conditions").attribute("check_fall").as_bool(false);
	flip_x = node.child("conditions").attribute("flip_x").as_bool(false);
	gummy_jump = node.child("conditions").attribute("gummy_jump").as_bool(false);

	p2SString state_string(node.child("state").attribute("current_state").as_string(""));

	if (state_string == "jumping")
	{
		current_state = Player_State::jumping;
	}
	else if  (state_string == "boucing")
	{
		current_state = Player_State::boucing;
	}
	else if (state_string == "dying")
	{
		current_state = Player_State::dead;
	}
	else if (state_string == "attack")
	{
		current_state = Player_State::attack;
	}

	p2SString collider_string(node.child("state").attribute("collider_type").as_string(""));

	if (collider_string == "collider_player")
	{
		main_collider->type = COLLIDER_TYPE::COLLIDER_PLAYER;
	}
	else if (collider_string == "collider_none")
	{
		main_collider->type = COLLIDER_TYPE::COLLIDER_NONE;
	}
	else if (collider_string == "collider_god")
	{
		main_collider->type = COLLIDER_TYPE::COLLIDER_GOD;
	}
	else if (collider_string == "collider_attack")
	{
		main_collider->type = COLLIDER_TYPE::COLLIDER_ATTACK;
	}

	return ret;
}

bool j1Player::Save(pugi::xml_node& node) const
{
	bool ret = true;

	pugi::xml_node vel = node.append_child("velocity");

	vel.append_attribute("x") = velocity.x;
	vel.append_attribute("y") = velocity.y;

	pugi::xml_node conditions = node.append_child("conditions");

	conditions.append_attribute("apply_jump_speed") = apply_jump_speed;
	conditions.append_attribute("on_ground") = on_ground;
	conditions.append_attribute("check_fall") = check_fall;
	conditions.append_attribute("flip_x") = flip_x;
	conditions.append_attribute("gummy_jump") = gummy_jump;

	pugi::xml_node state_node = node.append_child("state");

	p2SString state_string;
	switch ((Player_State)current_state)
	{
	case Player_State::jumping:
		state_string.create("jumping");
		break;
	case Player_State::boucing:
		state_string.create("boucing");
		break;
	case Player_State::dead:
		state_string.create("dying");
		break;
	case Player_State::attack:
		state_string.create("attack");
		break;
	default:
		break;
	}
	
	state_node.append_attribute("current_state") = state_string.GetString();

	p2SString collider_string;
	switch (main_collider->type)
	{
	case COLLIDER_TYPE::COLLIDER_PLAYER:
		collider_string.create("collider_player");
		break;
	case COLLIDER_TYPE::COLLIDER_NONE:
		collider_string.create("collider_none");
		break;
	case COLLIDER_TYPE::COLLIDER_GOD:
		collider_string.create("collider_god");
		break;
	case COLLIDER_TYPE::COLLIDER_ATTACK:
		collider_string.create("collider_attack");
		break;
	default:
		break;
	}

	state_node.append_attribute("collider_type") = collider_string.GetString();

	return ret;
}

// Remove Colliders Overlap
bool j1Player::OnCollision(Collider* c1, Collider* c2)
{
	BROFILER_CATEGORY("Player OnCollision", Profiler::Color::LightGreen);

	bool ret = true;

	switch (c2->type)
	{
	case COLLIDER_DEATH:
		current_state = Player_State::dead;
		main_collider->type = COLLIDER_NONE;
		break;
	case COLLIDER_NEXT_LEVEL:
		App->scene_manager->ChangeToNextPhase();
		break;
	case COLLIDER_ENEMY:

		if (current_state == Player_State::attack)
		{
			App->collision->CheckOverlap( p2List<Direction>() ,main_collider, COLLIDER_ENEMY, position, velocity);
			on_ground = true;
	/*		apply_invulnerability = true;*/
			attack_tremble = true;
			break;
		}
		
		current_state = Player_State::dead;
		main_collider->type = COLLIDER_NONE;
		break;
	}
	return ret;
}

bool j1Player::Invulnerability(float time)
{
	// Reset
	if (apply_timer)
		timer_invulnerability.Start();
	
	// Timer
	if (timer_invulnerability.ReadSec() > time)
	{
		main_collider->type = COLLIDER_PLAYER;
		apply_timer = true;
		return true;
	}
	else
		main_collider->type = COLLIDER_GOD;
	
	return apply_timer = false;
}