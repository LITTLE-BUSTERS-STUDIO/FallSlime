#include "Enemy.h"
#include "EntityManager.h"
#include "J1Player.h"
#include "j1PathFinding.h"
#include "j1Collision.h"
#include "j1Map.h"
#include "p2Log.h"
#include "j1Render.h"
#include "Brofiler/Brofiler.h"

#define MAX_DETECTION_RATIO 400

Enemy::Enemy(fPoint position, fPoint spawn_pos, Properties *properties) :Entity(position, spawn_pos, properties)
{
	Enemy_Properties* enemy_properties = (Enemy_Properties*)properties;

	iPoint pos = { (int)position.x, (int)position.y};
	current_point = App->map->WorldToMap(pos.x, pos.y);
	
	path_interval_time = enemy_properties->path_interval_time;
	detection_ratio = enemy_properties->detection_ratio;

	main_collider = App->collision->AddCollider(enemy_properties->collider_rect, COLLIDER_NONE, App->entity_manager);
	main_collider->SetPos(position.x - enemy_properties->collider_rect.w * 0.5F, position.y - enemy_properties->collider_rect.h* 0.5F);

}

Enemy::~Enemy()
{

}

bool Enemy::UpdateLogic() 
{
	if (  path_timer.Read() > path_interval_time ) 
	{
		fPoint target_pos(target->GetPosition());
		iPoint map_target (target_pos.x, target_pos.y);
		iPoint map_pos(position.x, position.y);

		map_target = App->map->WorldToMap(map_target.x, map_target.y);
		map_pos = App->map->WorldToMap(map_pos.x, map_pos.y);
		App->path_finding->CreatePath( map_pos, map_target, last_path);
		new_path = true;
		path_timer.Start();
	}
	return true;
}

bool  Enemy::FollowPath( float dt)
{
	if (!last_path.Count() > 0)
	{
		return false;
	}

	if (add_error_margin && !new_path)
	{
		current_point += error_margin;
		error_margin = { 0,0 };
		add_error_margin = false;
	}

	if (new_path)
	{
		last_path.Pop(current_point);
		last_path.Pop(current_point);
		new_path = false;
	}

	fPoint velocity_to_follow;
	iPoint node_in_world;

	node_in_world = App->map->MapToWorld(current_point.x, current_point.y);
	velocity_to_follow.x = (float)node_in_world.x  - position.x ;
	velocity_to_follow.y = (float)node_in_world.y - position.y ;

	velocity_to_follow.Normalize();

	previous_position = position;
	position.x += velocity_to_follow.x * 100.0f * dt; 
	position.y += velocity_to_follow.y * 100.0f * dt;
	main_collider->SetPos(position.x - main_collider->rect.w / 2, position.y - main_collider->rect.h / 2);

	p2List<Direction> directions;
	App->collision->CheckOverlap(directions, main_collider, COLLIDER_WALL, position , velocity_to_follow);
	

	if (position.x > node_in_world.x - 10 && position.x < node_in_world.x + 10 && position.y > node_in_world.y - 10 && position.y < node_in_world.y + 10)
	{
		if (last_path.Count())
		{
			last_path.Pop(current_point);
		}
	}
	
	if ((int)previous_position.x == (int)position.x  && (int)previous_position.y == (int)position.y)
	{
		if (node_in_world.x > position.x )
			error_margin.x += 3;
		else
			error_margin.x -= 3;

		if (node_in_world.y > position.y)
			error_margin.y += 3;
		else
			error_margin.y -= 3;

		add_error_margin = true;
	}
	return true;
}

bool Enemy::CheckTargetRatio()
{
	bool ret = false;

	if (detection_ratio < MAX_DETECTION_RATIO && target)
	{
		if (position.DistanceManhattan(App->entity_manager->GetPlayer()->GetPosition()) < detection_ratio)
		{
			target_detected = ret = true;
		}
	}
	return target_detected = ret;
}


bool  Enemy::Active()
{
	if (main_collider)
	{
		main_collider->type = COLLIDER_ENEMY;
	}

	active = true;

	return true;
}

bool  Enemy::Desactive()
{
	if (main_collider)
	{
		main_collider->type = COLLIDER_NONE;
	}

	active = false;
	return true;
}

bool Enemy::Reset()
{
	last_path.Clear();
	return true;
}

bool Enemy::DrawPath()
{
	if (App->path_finding->debug && active)
	{
		for (uint i = 0; i < last_path.Count(); ++i)
		{
			iPoint pos = App->map->MapToWorld(last_path.At(i)->x, last_path.At(i)->y);
			App->render->DrawQuad({ pos.x, pos.y ,16,16 }, 0, 0, 0, 200);
		}
	}

	return true;
}
