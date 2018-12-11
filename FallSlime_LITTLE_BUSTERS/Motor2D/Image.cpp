#include "Image.h"
#include "j1Render.h"
#include "j1Gui.h"

Image::Image(iPoint position, Animation animation, SDL_Texture * texture, Gui_Listener* listener): Object(position, listener)
{
	this->animation = animation;
	this->texture = texture;
}


Image::~Image()
{

}

bool Image::Draw()
{
	SDL_Rect anim_rect = animation.GetCurrentFrame();

	section.w = anim_rect.w;
	section.h = anim_rect.h;
	App->render->Blit( texture ,  position.x - section.w/2 , position.y - section.h / 2, &anim_rect, 0.0f);

	return true;
}
