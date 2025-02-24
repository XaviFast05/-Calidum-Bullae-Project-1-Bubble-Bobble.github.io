
#include "Enemy.h"
#include "Sprite.h"
#include "TileMap.h"
#include "Globals.h"
#include <raymath.h>
#include "Player.h"



Enemy::Enemy(const Point& p, E_State s, E_Look view, E_Type t) :
	Entity(p, ENEMY_PHYSICAL_WIDTH, ENEMY_PHYSICAL_HEIGHT, ENEMY_FRAME_SIZE, ENEMY_FRAME_SIZE)
{
	type = t;
	state = s;
	look = view;
	jump_delay = ENEMY_JUMP_DELAY;
	map = nullptr;
	score = 0;
	lifes = 3;
	player = nullptr;
	jumptime = 0;
	inShoot = true;
	stages = 1;
	logPosXL = pos.x - 65;
	logPosXR = pos.x + 65;

	GettingTime = true;
	TimeToGo = 0;
}
Enemy::~Enemy()
{
}
AppStatus Enemy::Initialise()
{
	int i;
	const int n = ENEMY_FRAME_SIZE;

	ResourceManager& data = ResourceManager::Instance();
	
	if (data.LoadTexture(Resource::IMG_BUSTER, "images/BubbleBusterSprite.png") != AppStatus::OK)
	{
		return AppStatus::ERROR;
	}

	if (data.LoadTexture(Resource::IMG_SKELMON, "images/SkelMonstaSprite.png") != AppStatus::OK)
	{
		return AppStatus::ERROR;
	}
	switch (type)
	{
	case E_Type::BUSTER:render = new Sprite(data.GetTexture(Resource::IMG_BUSTER)); break;
	case E_Type::SKELMON:render = new Sprite(data.GetTexture(Resource::IMG_SKELMON)); break;

		default: LOG("Internal error: enemy creation of invalid type");
	}
	if (render == nullptr)
	{
		LOG("Failed to allocate memory for enemy sprite");
		return AppStatus::ERROR;
	}

	Sprite* sprite = dynamic_cast<Sprite*>(render);
	sprite->SetNumberAnimations((int)EnemyAnim::NUM_ANIMATIONS);

	sprite->SetAnimationDelay((int)EnemyAnim::IDLE_RIGHT, ANIM_DELAY);
	
		sprite->AddKeyFrame((int)EnemyAnim::IDLE_RIGHT, { 0, 0, -n, n });
		sprite->AddKeyFrame((int)EnemyAnim::IDLE_RIGHT, { 0, 0, -n, n });
		

	sprite->SetAnimationDelay((int)EnemyAnim::IDLE_LEFT, ANIM_DELAY);
	
		sprite->AddKeyFrame((int)EnemyAnim::IDLE_LEFT, { 0, 0, n, n });
		sprite->AddKeyFrame((int)EnemyAnim::IDLE_LEFT, { 0, 0, n, n });
		
	

	sprite->SetAnimationDelay((int)EnemyAnim::WALKING_RIGHT, ANIM_DELAY);
	for (i = 1; i < 4; ++i)
	{
		sprite->AddKeyFrame((int)EnemyAnim::WALKING_RIGHT, { 0 * n, 0 * n, -n, n });
		sprite->AddKeyFrame((int)EnemyAnim::WALKING_RIGHT, { 1 * n, 0 * n, -n, n });
		sprite->AddKeyFrame((int)EnemyAnim::WALKING_RIGHT, { 2 * n, 0 * n, -n, n });
		sprite->AddKeyFrame((int)EnemyAnim::WALKING_RIGHT, { 3 * n, 0 * n, -n, n });
	}
	sprite->SetAnimationDelay((int)EnemyAnim::WALKING_LEFT, ANIM_DELAY);
	for (i = 1; i < 4; ++i)
	{
		sprite->AddKeyFrame((int)EnemyAnim::WALKING_LEFT, { 0 * n, 0 * n, n, n });
		sprite->AddKeyFrame((int)EnemyAnim::WALKING_LEFT, { 1 * n, 0 * n, n, n });
		sprite->AddKeyFrame((int)EnemyAnim::WALKING_LEFT, { 2 * n, 0 * n, n, n });
		sprite->AddKeyFrame((int)EnemyAnim::WALKING_LEFT, { 3 * n, 0 * n, n, n });
	}

	sprite->SetAnimationDelay((int)EnemyAnim::FALLING_RIGHT, ANIM_DELAY);
	for (i = 0; i < 4; ++i)
		sprite->AddKeyFrame((int)EnemyAnim::FALLING_RIGHT, { (float)i * n, 0 * n, -n, n });

	sprite->SetAnimationDelay((int)EnemyAnim::FALLING_LEFT, ANIM_DELAY);
	for (i = 0; i < 4; ++i)
		sprite->AddKeyFrame((int)EnemyAnim::FALLING_LEFT, { (float)i * n, 0 * n, n, n });


	sprite->SetAnimationDelay((int)EnemyAnim::JUMPING_RIGHT, ANIM_DELAY);
	for (i = 0; i < 4; ++i)
		sprite->AddKeyFrame((int)EnemyAnim::JUMPING_RIGHT, { (float)i * n, 0 * n, -n, n });

	sprite->SetAnimationDelay((int)EnemyAnim::JUMPING_LEFT, ANIM_DELAY);
	for (i = 0; i < 4; ++i)
		sprite->AddKeyFrame((int)EnemyAnim::JUMPING_LEFT, { (float)i * n, 0 * n, n, n });

	sprite->SetAnimationDelay((int)EnemyAnim::LEVITATING_RIGHT, ANIM_JUMP_DELAY);
	for (i = 0; i < 4; ++i)
		sprite->AddKeyFrame((int)EnemyAnim::LEVITATING_RIGHT, { (float)i * n, 0 * n, -n, n });
	sprite->SetAnimationDelay((int)EnemyAnim::LEVITATING_LEFT, ANIM_JUMP_DELAY);
	for (i = 0; i < 4; ++i)
		sprite->AddKeyFrame((int)EnemyAnim::LEVITATING_LEFT, { (float)i * n, 0 * n, n, n });

	sprite->SetAnimationDelay((int)EnemyAnim::CLIMBING, ANIM_JUMP_DELAY);
	for (i = 0; i < 3; ++i)
		sprite->AddKeyFrame((int)EnemyAnim::CLIMBING, { (float)i * n, 3 * n, n, n });

	sprite->SetAnimation((int)EnemyAnim::IDLE_RIGHT);

	return AppStatus::OK;
}
void Enemy::SetTileMap(TileMap* tilemap)
{
	map = tilemap;
}
void Enemy::SetPlayer(Player* play)
{
	player = play;
}
E_State Enemy::GetState()
{
	return state;
}
void Enemy::Bubbler()
{
	TimeToGo = GetTime() + 10;
	state = E_State::BUBBLED;
	SetAnimation((int)EnemyAnim::CLIMBING);
}
bool Enemy::IsLookingRight() const
{
	return look == E_Look::RIGHT;
}
bool Enemy::IsLookingLeft() const
{
	return look == E_Look::LEFT;
}
bool Enemy::IsAscending() const
{
	return dir.y < -ENEMY_LEVITATING_SPEED;
}
bool Enemy::IsLevitating() const
{
	return abs(dir.y) <= ENEMY_LEVITATING_SPEED;
}
bool Enemy::IsDescending() const
{
	return dir.y > ENEMY_LEVITATING_SPEED;
}
bool Enemy::IsInFirstHalfTile() const
{
	return pos.y % TILE_SIZE < TILE_SIZE / 2;
}
bool Enemy::IsInSecondHalfTile() const
{
	return pos.y % TILE_SIZE >= TILE_SIZE / 2;
}
void Enemy::SetAnimation(int id)
{
	Sprite* sprite = dynamic_cast<Sprite*>(render);
	sprite->SetAnimation(id);
}
EnemyAnim Enemy::GetAnimation()
{
	Sprite* sprite = dynamic_cast<Sprite*>(render);
	return (EnemyAnim)sprite->GetAnimation();
}
void Enemy::Stop()
{
	dir = { 0,0 };
	state = E_State::IDLE;
	if (IsLookingRight())	SetAnimation((int)EnemyAnim::IDLE_RIGHT);
	else					SetAnimation((int)EnemyAnim::IDLE_LEFT);
}
void Enemy::StartWalkingLeft()
{
	state = E_State::WALKING;
	look = E_Look::LEFT;
	SetAnimation((int)EnemyAnim::WALKING_LEFT);
}
void Enemy::StartWalkingRight()
{
	state = E_State::WALKING;
	look = E_Look::RIGHT;
	SetAnimation((int)EnemyAnim::WALKING_RIGHT);
}
void Enemy::StartFalling()
{
	dir.y = ENEMY_SPEED;
	state = E_State::FALLING;
	if (IsLookingRight())	SetAnimation((int)EnemyAnim::FALLING_RIGHT);
	else					SetAnimation((int)EnemyAnim::FALLING_LEFT);
}
void Enemy::StartJumping()
{
	dir.y = -ENEMY_JUMP_FORCE;
	state = E_State::JUMPING;
	if (IsLookingRight())	SetAnimation((int)EnemyAnim::JUMPING_RIGHT);
	else					SetAnimation((int)EnemyAnim::JUMPING_LEFT);
	jump_delay = ENEMY_JUMP_DELAY;
}
void Enemy::StartAttacking()
{

	state = E_State::ATTACKING;
	if (IsLookingRight())	SetAnimation((int)EnemyAnim::ATTACK_RIGHT);
	else					SetAnimation((int)EnemyAnim::ATTACK_LEFT);
}
void Enemy::ChangeAnimRight()
{
	look = E_Look::RIGHT;
	switch (state)
	{
	case E_State::IDLE:	 SetAnimation((int)EnemyAnim::IDLE_RIGHT);    break;
	case E_State::WALKING: SetAnimation((int)EnemyAnim::WALKING_RIGHT); break;
	case E_State::JUMPING: SetAnimation((int)EnemyAnim::JUMPING_RIGHT); break;
	case E_State::FALLING: SetAnimation((int)EnemyAnim::FALLING_RIGHT); break;
	case E_State::ATTACKING: SetAnimation((int)EnemyAnim::ATTACK_RIGHT); break;
	}
}
void Enemy::ChangeAnimLeft()
{
	look = E_Look::LEFT;
	switch (state)
	{
	case E_State::IDLE:	 SetAnimation((int)EnemyAnim::IDLE_LEFT);    break;
	case E_State::WALKING: SetAnimation((int)EnemyAnim::WALKING_LEFT); break;
	case E_State::JUMPING: SetAnimation((int)EnemyAnim::JUMPING_LEFT); break;
	case E_State::FALLING: SetAnimation((int)EnemyAnim::FALLING_LEFT); break;
	case E_State::ATTACKING: SetAnimation((int)EnemyAnim::ATTACK_LEFT); break;
	}
}

void Enemy::Update()
{
	//Player doesn't use the "Entity::Update() { pos += dir; }" default behaviour.
	//Instead, uses an independent behaviour for each axis.
	if (state == E_State::BUBBLED)
	{
		TimeInBubble();
		MoveY();
	}
	else 
	{
		MoveX();
		MoveY();
	}

	Sprite* sprite = dynamic_cast<Sprite*>(render);
	sprite->Update();
	connect();

}
void Enemy::MoveX()
{
	if (type == E_Type::BUSTER)
	{
		AABB box;
		int prev_x = pos.x;
		
		if (look == E_Look::LEFT && state != E_State::FALLING && state != E_State::JUMPING)
		{
			pos.x += -ENEMY_SPEED;
			if (state == E_State::IDLE || state == E_State::ATTACKING) StartWalkingLeft();
			else
			{
				if (IsLookingRight()) ChangeAnimLeft();
			}

			box = GetHitbox();
			if (map->TestCollisionWallLeft(box))
			{
				pos.x = prev_x;
				if (state == E_State::WALKING) ChangeAnimRight();
			}
		}
		else if (look == E_Look::RIGHT && state != E_State::FALLING && state != E_State::JUMPING)
		{
			pos.x += ENEMY_SPEED;
			if (state == E_State::IDLE || state == E_State::ATTACKING) StartWalkingRight();
			else
			{
				if (IsLookingLeft()) ChangeAnimRight();
			}

			box = GetHitbox();
			if (map->TestCollisionWallRight(box))
			{
				pos.x = prev_x;
				if (state == E_State::WALKING) ChangeAnimLeft();
			}
		}
		else
		{
			if (state == E_State::WALKING) Stop();
		}
	}
	else if (type == E_Type::SKELMON)
	{
		AABB box;
		int prev_x = pos.x;
		box = GetHitbox();
		if (look == E_Look::RIGHT)
		{
			pos.x += ENEMY_SPEED;
			if (map->TestCollisionWallRight(box))
			{
				pos.x = prev_x;
				look = E_Look::LEFT;
				SetAnimation((int)EnemyAnim::WALKING_LEFT);
			}
		}
		else if (look == E_Look::LEFT)
		{
			pos.x += -ENEMY_SPEED;
			if (map->TestCollisionWallLeft(box))
			{
				pos.x = prev_x;
				look = E_Look::RIGHT;
				SetAnimation((int)EnemyAnim::WALKING_RIGHT);
			}
		}
	}
}
void Enemy::MoveY()
{
	if (state == E_State::BUBBLED)
	{
		BubbleMovement();
	}
	else if (type == E_Type::BUSTER)
	{
		AABB box;

		jumptime += GetFrameTime();
		if (state == E_State::JUMPING)
		{
			LogicJumping();
		}
		else //idle, walking, falling
		{
			pos.y += ENEMY_SPEED;
			box = GetHitbox();
			Point playerpos = player->GetPos();
			if (map->TestCollisionGround(box, &pos.y))
			{
				if (state == E_State::FALLING) Stop();
				else if (playerpos.y < pos.y && jumptime>2)
				{
					StartJumping();
					jumptime = 0;
				}
			}
			else
			{
				if (state != E_State::FALLING && playerpos.y > pos.y) StartFalling();
				else if (state != E_State::FALLING && playerpos.y < pos.y)
				{
					StartJumping();
				}
			}
		}
	}
	else
	{
		AABB box;
		int prev_y = pos.y;
		box = GetHitbox();
		if (DIAG_MOVE_E == Diag_E::DIAG_UP_E)
		{
			pos.y += -ENEMY_SPEED;
			if (pos.y <= 30 or map->TestCollisionTop(box, &pos.y))
			{
				pos.y = prev_y;
				DIAG_MOVE_E = Diag_E::DIAG_DOWN_E;
			}
		}
		else if (DIAG_MOVE_E == Diag_E::DIAG_DOWN_E)
		{
			pos.y += ENEMY_SPEED;
			if (map->TestCollisionGround(box, &pos.y))
			{
				pos.y = prev_y;
				DIAG_MOVE_E = Diag_E::DIAG_UP_E;
			}
		}
	}
}

void Enemy::BubbleMovement()
{
	if (pos.y > 40)
	{
		pos.y += -ENEMY_SPEED;
	}
	else
	{
		if (pos.x <= GetRandomValue(110, WINDOW_WIDTH / 2))
		{
			pos.x += ENEMY_SPEED;
		}
		else if (pos.x > GetRandomValue(WINDOW_WIDTH / 2, 140))
		{
			pos.x += -ENEMY_SPEED;
		}
	}
}
void Enemy::TimeInBubble()
{
	if (TimeToGo <= GetTime())
	{
		state = E_State::IDLE;
	}
}
void Enemy::LogicJumping()
{
	AABB box, prev_box;
	int prev_y;

	jump_delay--;
	if (jump_delay == 0)
	{
		prev_y = pos.y;
		prev_box = GetHitbox();

		pos.y += dir.y;
		dir.y += ENEMY_FORCE;
		jump_delay = ENEMY_JUMP_DELAY;

		//Is the jump finished?
		if (dir.y > ENEMY_JUMP_FORCE)
		{
			dir.y = ENEMY_SPEED;
			StartFalling();
		}
		else
		{
			//Jumping is represented with 3 different states
			if (IsAscending())
			{
				if (IsLookingRight())	SetAnimation((int)EnemyAnim::JUMPING_RIGHT);
				else					SetAnimation((int)EnemyAnim::JUMPING_LEFT);
			}
			else if (IsLevitating())
			{
				if (IsLookingRight())	SetAnimation((int)EnemyAnim::LEVITATING_RIGHT);
				else					SetAnimation((int)EnemyAnim::LEVITATING_LEFT);
			}
			else if (IsDescending())
			{
				if (IsLookingRight())	SetAnimation((int)EnemyAnim::FALLING_RIGHT);
				else					SetAnimation((int)EnemyAnim::FALLING_LEFT);
			}
		}
		//We check ground collision when jumping down
		if (dir.y >= 0)
		{
			box = GetHitbox();

			//A ground collision occurs if we were not in a collision state previously.
			//This prevents scenarios where, after levitating due to a previous jump, we found
			//ourselves inside a tile, and the entity would otherwise be placed above the tile,
			//crossing it.
			if (!map->TestCollisionGround(prev_box, &prev_y) &&
				map->TestCollisionGround(box, &pos.y)&& state != E_State::BUBBLED)
			{
				Stop();
			}
		}
	}
}

void Enemy::DrawDebug(const Color& col) const
{
	Entity::DrawHitbox(pos.x, pos.y, width, height, col);

	/*DrawText(TextFormat("Position: (%d,%d)\nSize: %dx%d\nFrame: %dx%d", pos.x, pos.y, width, height, frame_width, frame_height), 10 * 10, 0, 8, LIGHTGRAY);
	DrawPixel(pos.x, pos.y, WHITE);*/
}
void Enemy::Release()
{
	ResourceManager& data = ResourceManager::Instance();
	data.ReleaseTexture(Resource::IMG_BUSTER);
	data.ReleaseTexture(Resource::IMG_SKELMON);

	render->Release();
}