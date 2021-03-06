#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Gui.h"
#include "Hud.h"
#include "Brofiler/Brofiler.h"
#include "j1Input.h"
#include "j1Render.h"
#include "EntityManager.h"
#include "j1Textures.h"
#include "Image.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Scene.h"
#include "j1Timer.h"

Hud::Hud() :j1Module()
{
	name.create("hud");
}

bool Hud::Awake(pugi::xml_node &node)
{
	lifes_counter = MAX_LIFES; //MAGIC NUMBER to xml 
	coin_counter = 0; //MAGIC NUMBER to xml 
	return true;
}

bool Hud::Start()
{
	hud_object = App->gui->CreateObject(iPoint( 0, 0), this);
	
	Animation anim_lives;
	anim_lives.PushBack({ 0, 96, 17, 14 });
	anim_lives.PushBack({ 50, 165, 29, 32 });

	for (int i = 0 ; i < MAX_LIFES; ++i)
	{
		Image* live = App->gui->CreateImage(iPoint(25 * i + 40, 30), anim_lives, this);
		lifes_list.add(live);
		live->SetAnchor(hud_object);
	}

	Animation anim_score;
	anim_score.PushBack({ 33, 96, 12, 18 });
	anim_score.PushBack({ 19, 96, 12, 18 });

	for (int i = 0; i < MAX_COINS; ++i)
	{
		Image* coin = App->gui->CreateImage(iPoint(25 * i + 550, 30), anim_score, this);
		coins_list.add(coin);
		coin->SetAnchor(hud_object);
	}

	return true;
}

bool Hud::PreUpdate()
{	
	return true;
}

bool Hud::Update(float dt)
{
	
	return true;
}

bool Hud::PostUpdate()
{
	timer_game = timer_game + timer.Read() / 1000;
	return true;
}

bool Hud::CleanUp()
{


	for (p2List_item<Image*>* item = lifes_list.start; item; item = item->next)
	{
		App->gui->DeleteObject(item->data);

	}

	lifes_list.clear();


	for (p2List_item<Image*>* item = coins_list.start; item; item = item->next)
	{

		App->gui->DeleteObject(item->data);
	}

	coins_list.clear();

	App->gui->DeleteObject(hud_object);
	App->gui->DeleteObject(panel_stages);

	return true;
}

bool Hud::Reset()
{
	BROFILER_CATEGORY("Hud Reset", Profiler::Color::LightGray);
	lifes_counter = 5;
	coin_counter = 0;

	for (uint i = 0; i < MAX_COINS; ++i)
	{
		coins_list[i]->SetAnimationFrame(0);
	}

	for (uint i = 0; i < MAX_LIFES; ++i)
	{
		lifes_list[i]->SetAnimationFrame(0);
	}

	return true;
}

bool Hud::Load(pugi::xml_node &node)
{

	SetLifes(node.child("counter").attribute("lifes").as_int(0));
	SetCoins(node.child("counter").attribute("coins").as_int(0));
	SetTimer(node.child("counter").attribute("timer").as_int(0));
	

	return true;
}

bool Hud::Save(pugi::xml_node &node) const
{
	pugi::xml_node counter = node.append_child("counter");

	counter.append_attribute("coins") = coin_counter;
	counter.append_attribute("lifes") = lifes_counter;
	counter.append_attribute("timer") = timer_game;

	return true;
}

bool Hud::HideHud()
{
	if (hud_object == nullptr)
	{
		return false;
	}

	App->gui->SetStateToBranch(ObjectState::hidden, hud_object);

	return true;
}

bool Hud::ShowHud()
{
	if (hud_object == nullptr)
	{
		return false;
	}

	App->gui->SetStateToBranch(ObjectState::visible, hud_object);
	return true;
}

int Hud::Getlife()
{
	return lifes_counter;
}


bool Hud::SetLifes(int lifes)
{
	 if (lifes <= 0)
	 {
		 lifes_counter = 0;
		 App->scene_manager->GetCurrentScene()->SetGameOver();
	 }
	 else if (lifes > MAX_LIFES)
	 { 
		 lifes_counter = MAX_LIFES;
	 }
	 else
	 {
		 lifes_counter = lifes;
	 }

	 for (uint i= 0 ; i < MAX_LIFES; ++i)
	 {
		 if (i >= lifes_counter)
		 {
			 lifes_list[i]->SetAnimationFrame(1);
		 }
		 else
		 {
			 lifes_list[i]->SetAnimationFrame(0);
		 }
	 }

	return true;
}

bool Hud::SetCoins(int coins)
{
	if (coins <= 0)
	{
		coin_counter = 0;
	}
	else if (coins > MAX_LIFES)
	{
		coin_counter = MAX_LIFES;
	}
	else
	{
		coin_counter = coins;
	}

	for (uint i = 0; i < MAX_COINS; ++i)
	{
		if (i >= coin_counter)
		{
			coins_list[i]->SetAnimationFrame(0);
		}
		else
		{
			coins_list[i]->SetAnimationFrame(1);
		}
	}

	return true;
}

bool Hud::SetTimer(int timer)
{

	timer_game = timer;
	return true;
}


int Hud::GetLifes() const
{
	return lifes_counter;
}

int Hud::GetCoins() const
{
	return coin_counter;
}

int Hud::GetTimer() const
{
	return timer_game;
}

