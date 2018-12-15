#include "p2Defs.h"
#include "p2Log.h"
#include "Label.h"
#include "j1Render.h"
#include "j1Fonts.h"
#include "j1Textures.h"


Label::Label(const iPoint position, const p2SString text, _TTF_Font* font, SDL_Color color, Gui_Listener* listener): Object(position, listener)
{
	//Properties ==================================
	is_draggable = false;
	is_interactive = false;

	this->font = font;
	this->color = color;
	SetText(text);

}

Label::~Label()
{
	if (texture != nullptr)
	{
		App->tex->DeleteExternalTexture(texture);
	}
}

void Label::SetText(p2SString text)
{
	if (texture != nullptr)
	{
		App->tex->DeleteExternalTexture(texture);
	}

	text.create(text.GetString());
	App->font->CalcSize(text.GetString(), section.w, section.h, font);
	section.x = section.y = 0;
	texture = App->font->Print(text.GetString(), color, font);
}

bool Label::Draw()
{
	App->render->Blit(texture, position.x- section.w * 0.5f, position.y - section.h * 0.5f , &section, false, 0.0f);

	return false;
}






