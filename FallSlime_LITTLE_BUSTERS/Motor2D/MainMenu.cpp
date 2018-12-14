#include "p2Defs.h"
#include "p2Log.h"
#include "MainMenu.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Map.h"
#include "j1Gui.h"
#include "j1Fonts.h"
#include "Brofiler/Brofiler.h"


#include "Label.h"
#include "Checkbox.h"
#include "Image.h"
#include "Button_Input.h"
#include "Slider.h"



MainMenu::MainMenu() : j1Scene()
{
	name.create("main_menu");
}


bool MainMenu::Update(float dt)
{
	BROFILER_CATEGORY("MainMenu Update", Profiler::Color::MediumBlue);

	return true;
}

bool MainMenu::PostUpdate()
{
	BROFILER_CATEGORY("MainMenu PostUpdate", Profiler::Color::MediumBlue);

	// Blit background--------------------------------------
	for (uint i = 0; i < max_background_layers; i++)
		App->render->Blit(paralax_tex_1, 0, background_startpos, &parallax1[i].rect_parallax, false, parallax_speed_1 /** dt*/);


	for (uint i = 0; i < max_background_layers; i++)
	{

		if (i == 0)
			App->render->Blit(paralax_tex_2, 0, background_startpos, &parallax2[i].rect_parallax, false, parallax_speed_3 /** dt*/);
		else if (i > 0)
			App->render->Blit(paralax_tex_2, 0, background_startpos, &parallax2[i].rect_parallax, false, parallax_speed_2/** dt*/);
	}

	for (uint i = 0; i < max_background_layers; i++)
		App->render->Blit(paralax_tex_3, 0, background_startpos, &parallax3[i].rect_parallax, false, parallax_speed_2 /** dt*/);


	App->map->Draw();

	return true;
}

bool MainMenu::LoadScene(pugi::xml_node & node)
{
	BROFILER_CATEGORY("MainMenu Load", Profiler::Color::Maroon);

	LOG("Loading MainMenu");

	// Paralax ========================================================
	music_path = node.child("music").attribute("path").as_string("");
	App->audio->PlayMusic(music_path.GetString());

	paralax_tex_1 = App->tex->Load(node.child("background1").attribute("path").as_string(""));
	paralax_tex_2 = App->tex->Load(node.child("background2").attribute("path").as_string(""));
	paralax_tex_3 = App->tex->Load(node.child("background3").attribute("path").as_string(""));

	background_width = node.child("background_dimension").attribute("width").as_uint(0u);
	background_high = node.child("background_dimension").attribute("high").as_uint(0u);
	max_background_layers = node.child("max_background_layers").attribute("value").as_uint(0u);
	background_startpos = node.child("background_startpos").attribute("value").as_uint(0u);
	parallax_speed_1 = node.child("parallax_speed").attribute("low").as_float(0.0f);
	parallax_speed_2 = node.child("parallax_speed").attribute("medium").as_float(0.0f);
	parallax_speed_3 = node.child("parallax_speed").attribute("high").as_float(0.0f);

	for (uint i = 0; i < 4; i++)
	{
		parallax1[i].rect_parallax.x = background_width * i;
		parallax1[i].rect_parallax.y = 0;
		parallax1[i].rect_parallax.w = background_width;
		parallax1[i].rect_parallax.h = background_high;

		parallax2[i].rect_parallax.x = background_width * i;
		parallax2[i].rect_parallax.y = 0;
		parallax2[i].rect_parallax.w = background_width;
		parallax2[i].rect_parallax.h = background_high;

		parallax3[i].rect_parallax.x = background_width * i;
		parallax3[i].rect_parallax.y = 0;
		parallax3[i].rect_parallax.w = background_width;
		parallax3[i].rect_parallax.h = background_high;
	}

	// Images ============================================
	//Animation logo_anim;
	//logo_anim.PushBack({ 0, 0, 219, 94 });
	//logo = App->gui->CreateImage(iPoint(326, 84), logo_anim, this);
	//logo->IsDraggable(true);

	Animation panel_anim;
	panel_anim.PushBack({ 387, 0, 389, 293 });
	panel = App->gui->CreateImage(iPoint(320, 182), panel_anim, this);
	panel->IsDraggable(true);

	// Labels ============================================
	karma_font = App->font->Load("fonts/KarmaSuture.ttf", 24);
	SDL_Color color = {231,94,152,255};

	music_volume_label = App->gui->CreateLabel(iPoint(229, 130), "Music volume", karma_font, this, color);
	music_volume_label->SetAnchor(panel);
	sfx_volume_label = App->gui->CreateLabel(iPoint(218, 173), "Sfx volume", karma_font, this, color);
	sfx_volume_label->SetAnchor(panel);
	mute_label = App->gui->CreateLabel(iPoint(185, 216), "Mute", karma_font, this, color);
	mute_label->SetAnchor(panel);
	limitate_fps_label = App->gui->CreateLabel(iPoint(225, 255), "Limitate FPS", karma_font, this, color);
	limitate_fps_label->SetAnchor(panel);

	// Buttons ============================================
	Button_Definition button_rectangle({ 219 , 0, 122, 36 }, { 219, 36, 122, 36 }, { 219, 72, 122, 36 });

	button_play = App->gui->CreateButton(iPoint(0, 0), button_rectangle, this);
	button_play->IsDraggable(true);

	//Button_Definition button_quad({ 343 ,0, 42,45 }, { 343 ,45, 42,45 }, { 343 ,90, 42,45 });

	//button_settings = App->gui->CreateButton(iPoint(40, 40), button_quad, this);
	//button_settings->IsDraggable(true);

	// Sliders ============================================
	Slider_Definition slider_def;
	slider_def.distance = 120;
	slider_def.default_value = 0;
	slider_def.thumb_definition.idle_rect = { 302 ,111, 13,24 };
	slider_def.thumb_definition.hover_rect = { 315 ,111, 13,24 };
	slider_def.thumb_definition.pushed_rect = { 328 ,111, 13,24 };
	slider_def.rail_draw_rect = { 248 ,136, 136,7 };

	slider_music_volume = App->gui->CreateSlider(iPoint(399, 134), slider_def, this);
	slider_music_volume->SetAnchor(panel);

	slider_sfx_volume = App->gui->CreateSlider(iPoint(399, 178), slider_def, this);
	slider_sfx_volume->SetAnchor(panel);

	// Checkboxes ========================================
	Checkbox_Definition checkbox_def;
	checkbox_def.check_off_button.idle_rect = { 248, 144 , 19, 21};
	checkbox_def.check_off_button.hover_rect = { 267, 144 , 19, 21 };
	checkbox_def.check_off_button.pushed_rect = { 286, 144 , 19, 21 };

	checkbox_def.check_on_button.idle_rect = { 305, 144 , 19, 21 };
	checkbox_def.check_on_button.hover_rect = { 324, 144 , 19, 21 };
	checkbox_def.check_on_button.pushed_rect = { 343, 144 , 19, 21 };

	checkbox_mute = App->gui->CreateCheckbox(iPoint(398, 219), checkbox_def, this);
	checkbox_mute->SetAnchor(panel);

	checkbox_limitate_fps = App->gui->CreateCheckbox(iPoint(398, 257), checkbox_def, this);
	checkbox_limitate_fps->SetAnchor(panel);

	App->gui->SetStateToBranch(ObjectState::hidden, panel);

	return true;
}

bool MainMenu::UnloadScene()
{
	BROFILER_CATEGORY("MainMenu Unload", Profiler::Color::Maroon);
	App->font->Unload(karma_font);

	App->gui->DeleteObject(music_volume_label);
	App->gui->DeleteObject(sfx_volume_label);
	App->gui->DeleteObject(limitate_fps_label);
	App->gui->DeleteObject(mute_label);

	App->gui->DeleteObject(logo);
	App->gui->DeleteObject(panel);
	App->gui->DeleteObject(button_play);
	App->gui->DeleteObject(button_continue);
	App->gui->DeleteObject(button_exit);
	App->gui->DeleteObject(button_credits);
	App->gui->DeleteObject(button_settings);

	App->gui->DeleteObject(slider_music_volume);
	App->gui->DeleteObject(slider_sfx_volume);

	App->gui->DeleteObject(checkbox_mute);
	App->gui->DeleteObject(checkbox_limitate_fps);

	App->tex->UnLoad(paralax_tex_1);
	App->tex->UnLoad(paralax_tex_2);
	App->tex->UnLoad(paralax_tex_3);
	return true;
}

bool MainMenu::OutClick(Object * object)
{
	if (object == button_play)
	{
		App->gui->SetStateToBranch(ObjectState::visible, panel);
	}

	return true;
}


