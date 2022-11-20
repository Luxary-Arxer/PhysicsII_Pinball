#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleSceneIntro.h"
#include "ModuleInput.h"
#include "ModuleTextures.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"

ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{

	// Initialise all the internal class variables, at least to NULL pointer
	circle = box = rick = NULL;
	ray_on = false;
	sensed = false;

	// Animations

	// Plunger idle animation
	plungerIdle.PushBack({ 44, 0, 40, 80 });
	plungerIdle.PushBack({ 0, 0, 40, 80 });
	plungerIdle.speed = 0.025f;

	// Plunger charging animation
	plungerCharging.PushBack({ 88, 0, 40, 80 });
	plungerCharging.PushBack({ 132, 0, 40, 80 });
	plungerCharging.PushBack({ 176, 0, 40, 80 });
	plungerCharging.PushBack({ 220, 0, 40, 80 });
	plungerCharging.PushBack({ 264, 0, 40, 80 });
	plungerCharging.speed = 0.1f;

	// Plunger maximum charge animation
	plungerMaxCharged.PushBack({ 220, 0, 40, 80 });
	plungerMaxCharged.PushBack({ 264, 0, 40, 80 });
	plungerMaxCharged.speed = 0.1f;

	// Pokemon center animation
	centerIdle.PushBack({ 0*192, 0, 192, 192 });
	centerIdle.PushBack({ 1 * 192, 0, 192, 192 });
	centerIdle.PushBack({ 2 * 192, 0, 192, 192 });
	centerIdle.PushBack({ 3 * 192, 0, 192, 192 });
	centerIdle.PushBack({ 4 * 192, 0, 192, 192 });
	centerIdle.PushBack({ 5 * 192, 0, 192, 192 });
	centerIdle.PushBack({ 6 * 192, 0, 192, 192 });
	centerIdle.PushBack({ 7 * 192, 0, 192, 192 });
	centerIdle.PushBack({ 8 * 192, 0, 192, 192 });
	centerIdle.PushBack({ 9 * 192, 0, 192, 192 });
	centerIdle.PushBack({ 10 * 192, 0, 192, 192 });
	centerIdle.PushBack({ 11 * 192, 0, 192, 192 });
	centerIdle.speed = 0.08;

	// Shroomish animation
	ShroomishIdle.PushBack({ 0 * 58, 0, 58, 68 });
	ShroomishIdle.PushBack({ 1 * 58, 0, 58, 68 });
	ShroomishIdle.speed = 0.02;

	ShroomishHit.PushBack({ 2 * 58, 0, 58, 68 });
	ShroomishHit.PushBack({ 3 * 58, 0, 58, 68 });
	ShroomishHit.loop = false;
	ShroomishHit.speed = 0.02;

	// Pikachu animation
	pikachuAnim.PushBack({ 0, 0, 53, 47 });
	pikachuAnim.PushBack({ 53, 0, 53, 47 });
	pikachuAnim.speed = 0.025f;

}

ModuleSceneIntro::~ModuleSceneIntro()
{
	// You should do some memory cleaning here, if required
}

bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	// Set camera position
	App->renderer->camera.x = App->renderer->camera.y = 0;

	// Load textures
	circle = App->textures->Load("pinball/wheel.png"); 
	box = App->textures->Load("pinball/crate.png");
	rick = App->textures->Load("pinball/rick_head.png");
	flipper = App->textures->Load("pinball/rick_flipper");
	bonus_fx = App->audio->LoadFx("pinball/bonus.wav");
	background = App->textures->Load("pinball/background.png");
	spoink = App->textures->Load("pinball/spoink.png");
	font = App->textures->Load("pinball/Font.png");
	pokeball = App->textures->Load("pinball/pokeball.png");
	superball = App->textures->Load("pinball/superball.png");
	ultraball = App->textures->Load("pinball/ultraball.png");
	flipperLeftTex = App->textures->Load("pinball/flipperLeft.png");
	flipperRightTex = App->textures->Load("pinball/flipperRight.png");
	pokemoncenter = App->textures->Load("pinball/pokemoncenter.png");
	pokemonentrance = App->textures->Load("pinball/pokemonentrance.png");
	Shroomish = App->textures->Load("pinball/Pokemon_Shroomish.png");
	pikachu = App->textures->Load("pinball/pikachu.png");
	gameOver = App->textures->Load("pinball/game_over.png");

	hitbox.add(App->physics->CreateChain(0, 0, hitbox2, 154));//
	hitboxa.add(App->physics->CreateChain(0, 0, background4, 24));//
	hitboxb.add(App->physics->CreateChain(0, 0, background3, 22));//
	hitboxc.add(App->physics->CreateChain(0, 0, background2, 94));//
	hitboxd.add(App->physics->CreateChain(0, 0, background1, 26));//
	hitboxe.add(App->physics->CreateChain(0, 0, hitbox7, 14));//
	hitboxf.add(App->physics->CreateChain(0, 0, hitbox8, 12));//
	hitboxg.add(App->physics->CreateChain(0, 0, triangleLeft, 6));//
	hitboxh.add(App->physics->CreateChain(0, 0, triangleRight, 6));//

	// Create a big red sensor on the bottom of the screen.
	// This sensor will not make other objects collide with it, but it can tell if it is "colliding" with something else
	lower_ground_sensor = App->physics->CreateRectangleSensor(SCREEN_WIDTH / 2, SCREEN_HEIGHT, SCREEN_WIDTH, 50);

	// Add this module (ModuleSceneIntro) as a listener for collisions with the sensor.
	// In ModulePhysics::PreUpdate(), we iterate over all sensors and (if colliding) we call the function ModuleSceneIntro::OnCollision()
	lower_ground_sensor->listener = this;

	py = App->physics->plunger->body->GetPosition().y;

	// First ball creation
	ball = App->physics->CreateCircle(484, 720, 13);
	ball->listener = this;

	// Flippers creation
	App->physics->flippers[0] = App->physics->CreateFlipper(202, 800, 50, 20, true);
	App->physics->flippers[0]->body->SetTransform(App->physics->flippers[0]->body->GetPosition(), 0.6);
	App->physics->flippers[1] = App->physics->CreateFlipper(282, 800, 50, 20, false);
	App->physics->flippers[1]->body->SetTransform(App->physics->flippers[0]->body->GetPosition(), -0.6);

	//Bumpers creation



	bumper1 = App->physics->CreateBumperCircle(266, 278, 12, 1);
	bumper2 = App->physics->CreateBumperCircle(188, 285, 12, 1);
	bumper3 = App->physics->CreateBumperCircle(235, 336, 12, 1);
	bumper1->listener = this;
	bumper2->listener = this;
	bumper3->listener = this;

	//sensor1 = App->physics->CreateCircleSensor(266, 278, 30);
	//sensor2 = App->physics->CreateCircleSensor(188, 285, 30);
	//sensor3 = App->physics->CreateCircleSensor(235, 336, 30);
	//sensor1->listener = this;
	//sensor2->listener = this;
	//sensor3->listener = this;


	return ret;
}

bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");

	return true;
}

update_status ModuleSceneIntro::Update()
{
	currentAnimation = &plungerIdle;

	currentpokemoncenter = &centerIdle;

	currentShroomish = &ShroomishIdle;

	currentPikachu = &pikachuAnim;

	// If user presses SPACE, charges the plunger

	int plungerPosX, plungerPosY;
	//bool plungerUP = true;

	App->physics->plunger->GetPosition(plungerPosX, plungerPosY);
	App->physics->plunger->body->ApplyForce({ 0,-10 }, { 0,0 }, true);

	/*
	// The plunger moves up and down

	if (plungerUP)
	{
		App->physics->plunger->body->ApplyForce({ 0, -10 }, { 0,0 }, true);
		plungerUP = false;
	}
	else
	{
		App->physics->plunger->body->ApplyForce({ 0, 0 }, { 0,0 }, true);
		plungerUP = true;
	}
	*/

	// Camera controller 
	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
	{
		App->renderer->camera.y = App->renderer->camera.y + 2;
	}

	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
	{
		App->renderer->camera.y = App->renderer->camera.y - 2;
	}

	if (App->input->GetKey(SDL_SCANCODE_P) == KEY_DOWN)
	{
		score += 10;
	}

	if (numballs != 0)
	{
		App->physics->PhysicsUpdate();

		// Plunger controller
		if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_DOWN)
		{
			currentAnimation = &plungerCharging;
		}

		if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		{
			currentAnimation = &plungerMaxCharged;
			App->physics->plunger->body->ApplyForce({ 0,10 }, { 0,0 }, true);
		}

		// If user releases SPACE, the plunger shoots
		float charge = App->physics->plunger->body->GetPosition().y - py;

		if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_UP || App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_UP)
		{
			if (currentAnimation != &plungerIdle)
			{
				plungerIdle.Reset();
				currentAnimation = &plungerIdle;
			}

			if (charge < 0.2f)
			{
				App->physics->plunger->body->ApplyForce({ 0, -300 }, { 0, 0 }, true);
			}
			else if (charge < 0.5f)
			{
				App->physics->plunger->body->ApplyForce({ 0, -450 }, { 0, 0 }, true);
			}
			else
			{
				App->physics->plunger->body->ApplyForce({ 0, -700 }, { 0, 0 }, true);
			}
		}

	}
	// If user presses S, enable RayCast
	if(App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN)
	{
		// Enable raycast mode
		ray_on = !ray_on;

		// Origin point of the raycast is be the mouse current position now (will not change)
		ray.x = App->input->GetMouseX();
		ray.y = App->input->GetMouseY();
	}

	// If user presses 1, create a new circle object
	if(App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
	{
		circles.add(App->physics->CreateCircle(App->input->GetMouseX(), App->input->GetMouseY(), 10));

		// Add this module (ModuleSceneIntro) as a "listener" interested in collisions with circles.
		// If Box2D detects a collision with this last generated circle, it will automatically callback the function ModulePhysics::BeginContact()
		circles.getLast()->data->listener = this;
	}

	// If user presses 2, create a new box object
	if(App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
	{
		boxes.add(App->physics->CreateRectangle(App->input->GetMouseX(), App->input->GetMouseY(), 100, 50));
	}


	if (App->input->GetKey(SDL_SCANCODE_4) == KEY_DOWN)
	{
		flippers.add(App->physics->CreateRectangle(App->input->GetMouseX(), App->input->GetMouseY(), 328, 90));
	}


	// If user presses 3, create a new RickHead object
	if(App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN)
	{
		// Pivot 0, 0
		int rick_head[64] = {
			14, 36,
			42, 40,
			40, 0,
			75, 30,
			88, 4,
			94, 39,
			111, 36,
			104, 58,
			107, 62,
			117, 67,
			109, 73,
			110, 85,
			106, 91,
			109, 99,
			103, 104,
			100, 115,
			106, 121,
			103, 125,
			98, 126,
			95, 137,
			83, 147,
			67, 147,
			53, 140,
			46, 132,
			34, 136,
			38, 126,
			23, 123,
			30, 114,
			10, 102,
			29, 90,
			0, 75,
			30, 62
		};

		ricks.add(App->physics->CreateChain(App->input->GetMouseX(), App->input->GetMouseY(), rick_head, 64));
	}


	if (colision == true) {
		colision = false ;
	}
	// Prepare for raycast ------------------------------------------------------
	
	// The target point of the raycast is the mouse current position (will change over game time)
	iPoint mouse;
	mouse.x = App->input->GetMouseX();
	mouse.y = App->input->GetMouseY();

	// Total distance of the raycast reference segment
	int ray_hit = ray.DistanceTo(mouse);

	// Declare a vector. We will draw the normal to the hit surface (if we hit something)
	fVector normal(0.0f, 0.0f);

	// All draw functions ------------------------------------------------------

	// Circles
	p2List_item<PhysBody*>* c = circles.getFirst();
	while(c != NULL)
	{
		int x, y;
		c->data->GetPosition(x, y);

		// If mouse is over this circle, paint the circle's texture
		if(c->data->Contains(App->input->GetMouseX(), App->input->GetMouseY()))
			App->renderer->Blit(circle, x, y, NULL, 1.0f, c->data->GetRotation());

		c = c->next;
	}

	// Boxes
	c = boxes.getFirst();
	while(c != NULL)
	{
		int x, y;
		c->data->GetPosition(x, y);

		// Always paint boxes texture
		App->renderer->Blit(box, x, y, NULL, 1.0f, c->data->GetRotation());

		// Are we hitting this box with the raycast?
		if(ray_on)
		{
			// Test raycast over the box, return fraction and normal vector
			int hit = c->data->RayCast(ray.x, ray.y, mouse.x, mouse.y, normal.x, normal.y);
			if(hit >= 0)
				ray_hit = hit;
		}
		c = c->next;
	}

	// Rick Heads
	c = ricks.getFirst();
	while(c != NULL)
	{
		int x, y;
		c->data->GetPosition(x, y);
		App->renderer->Blit(rick, x, y, NULL, 1.0f, c->data->GetRotation());
		c = c->next;
	}

	// Flipers Heads
	c = flippers.getFirst();
	while (c != NULL)
	{
		int x, y;
		c->data->GetPosition(x, y);

		// Always paint boxes texture
		App->renderer->Blit(flipper, x, y, NULL, 1.0f, c->data->GetRotation());

		// Are we hitting this box with the raycast?
		if (ray_on)
		{
			// Test raycast over the box, return fraction and normal vector
			int hit = c->data->RayCast(ray.x, ray.y, mouse.x, mouse.y, normal.x, normal.y);
			if (hit >= 0)
				ray_hit = hit;
		}
		c = c->next;
	}

	// Raycasts -----------------
	if(ray_on == true)
	{
		// Compute the vector from the raycast origin up to the contact point (if we're hitting anything; otherwise this is the reference length)
		fVector destination(mouse.x-ray.x, mouse.y-ray.y);
		destination.Normalize();
		destination *= ray_hit;

		// Draw a line from origin to the hit point (or reference length if we are not hitting anything)
		App->renderer->DrawLine(ray.x, ray.y, ray.x + destination.x, ray.y + destination.y, 255, 255, 255);

		// If we are hitting something with the raycast, draw the normal vector to the contact point
		if(normal.x != 0.0f)
			App->renderer->DrawLine(ray.x + destination.x, ray.y + destination.y, ray.x + destination.x + normal.x * 25.0f, ray.y + destination.y + normal.y * 25.0f, 100, 255, 100);
	}

	currentAnimation->Update();
	currentpokemoncenter->Update();
	currentShroomish->Update();
	currentPikachu->Update();

	// Background texture
	App->renderer->Blit(background, 0, 0, NULL, 1.0F);

	App->renderer->Blit(pokemonentrance, 458, 175, NULL, 1.0F);

	SDL_Rect rect = currentAnimation->GetCurrentFrame();
	SDL_Rect rect2 = currentpokemoncenter->GetCurrentFrame();
	SDL_Rect rect3 = currentShroomish->GetCurrentFrame();
	SDL_Rect rect4 = currentPikachu->GetCurrentFrame();

	// Plunger texture
	App->renderer->Blit(spoink, 466, 750, &rect);

	// Ball lives
	if (ball->body->GetPosition().y >= PIXEL_TO_METERS(1000))
	{
		if (numballs >= 2) {
			numballs--;
			ball = App->physics->CreateCircle(484, 720, 13);
		}
		else if (numballs == 1) {
			numballs--;
		}
	}

	// Ball textures
	if (numballs >= 3)
	{
		App->renderer->Blit(ultraball,
			METERS_TO_PIXELS(App->scene_intro->ball->body->GetPosition().x - 12),
			METERS_TO_PIXELS(App->scene_intro->ball->body->GetPosition().y - 12),
			NULL, 1.0F, (App->scene_intro->ball->GetRotation()));
	}
	else if (numballs == 2)
	{
		App->renderer->Blit(superball,
			METERS_TO_PIXELS(App->scene_intro->ball->body->GetPosition().x - 12),
			METERS_TO_PIXELS(App->scene_intro->ball->body->GetPosition().y - 12),
			NULL, 1.0F, (App->scene_intro->ball->GetRotation()));
	}
	else if (numballs == 1)
	{
		App->renderer->Blit(pokeball,
			METERS_TO_PIXELS(App->scene_intro->ball->body->GetPosition().x - 12),
			METERS_TO_PIXELS(App->scene_intro->ball->body->GetPosition().y - 12),
			NULL, 1.0F, (App->scene_intro->ball->GetRotation()));
	}

	if (score >= max_score) {
		max_score = score;
	}

	FontDrawScore(score, 4, posicioFont, posicioFontY, 20, 1);
	FontDrawLife(numballs, 1, posicioFont+280, posicioFontY, 20, 1);
	FontDrawMaxScore(max_score, 4, posicioMaxScoreX, posicioMaxScoreY, 20, 1);



	App->renderer->Blit(pokemoncenter, 321, 38, &rect2);

	App->renderer->Blit(Shroomish, 160, 242, &rect3);
	App->renderer->Blit(Shroomish, 238, 235, &rect3);
	App->renderer->Blit(Shroomish, 205, 293, &rect3);

	App->renderer->Blit(pikachu, 40, 760, &rect4);

	// Flippers textures
	App->renderer->Blit(App->scene_intro->flipperLeftTex,
		METERS_TO_PIXELS(App->physics->flippers[0]->body->GetPosition().x - 35),
		METERS_TO_PIXELS(App->physics->flippers[0]->body->GetPosition().y - 10),
		0, 1.0f, App->physics->flippers[0]->body->GetAngle()* RADTODEG);

	App->renderer->Blit(App->scene_intro->flipperRightTex,
		METERS_TO_PIXELS(App->physics->flippers[1]->body->GetPosition().x - 25),
		METERS_TO_PIXELS(App->physics->flippers[1]->body->GetPosition().y - 10),
		0, 1.0f, App->physics->flippers[1]->body->GetAngle()* RADTODEG);


	// Gives extra ball
	if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN)
		numballs = 3;
	
	// Game over screen
	if (numballs == 0)
	{
		App->renderer->Blit(gameOver, 0, 0, NULL, 1.0F);
	}

	// Keep playing
	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody* bodyA, PhysBody* bodyB)
{
	// Play Audio FX on every collision, regardless of who is colliding


	if (bodyA == ball && (bodyB == bumper1 || bodyB == bumper2 || bodyB == bumper3)) {
		score += 10;
		App->audio->PlayFx(bonus_fx);
		colision = true;
		if (currentShroomish != &ShroomishHit)
		{
			ShroomishHit.Reset();
			currentShroomish = &ShroomishHit;
		}
		if (currentShroomish != &ShroomishHit) {
			if (currentShroomish->HasFinished()) {
				//printf("_Death_");
				//pbody->body->SetTransform({ PIXEL_TO_METERS(150),PIXEL_TO_METERS(586) }, 0);
				currentShroomish = &ShroomishIdle;
			}
		}
	}


	// Do something else. You can also check which bodies are colliding (sensor? ball? player?)
}

update_status ModuleSceneIntro::PostUpdate()
{
	return update_status::UPDATE_CONTINUE;
}


void ModuleSceneIntro::FontDrawScore(int score, int n, int posX, int posY, int separacio, float scale) {
	int initialPosX = posX;
	int scoreCopia = score;
	int scoreArray[4];
	for (int j = 0; j < n; ++j) {
		scoreArray[j] = scoreCopia % 10;
		scoreCopia /= 10;
	}

	SDL_Rect rect0 = { 1 * 20, 0, 20, 32 };
	SDL_Rect rect1 = { 2 * 20, 0, 20, 32 };
	SDL_Rect rect2 = { 3 * 20, 0, 20, 32 };
	SDL_Rect rect3 = { 4 * 20, 0, 20, 32 };
	SDL_Rect rect4 = { 5 * 20, 0, 20, 32 };
	SDL_Rect rect5 = { 6 * 20, 0, 20, 32 };
	SDL_Rect rect6 = { 7 * 20, 0, 20, 32 };
	SDL_Rect rect7 = { 8 * 20, 0, 20, 32 };
	SDL_Rect rect8 = { 9 * 20, 0, 20, 32 };
	SDL_Rect rect9 = { 10 * 20, 0, 20, 32 };

	for (int k = 0; k < n; ++k) {

		switch (scoreArray[k]) {
		case 0:
			App->renderer->Blit(font, posX, posY, &rect0, scale);
			break;
		case 1:
			App->renderer->Blit(font, posX, posY, &rect1, scale);
			break;
		case 2:
			App->renderer->Blit(font, posX, posY, &rect2, scale);
			break;
		case 3:
			App->renderer->Blit(font, posX, posY, &rect3, scale);
			break;
		case 4:
			App->renderer->Blit(font, posX, posY, &rect4, scale);
			break;
		case 5:
			App->renderer->Blit(font, posX, posY, &rect5, scale);
			break;
		case 6:
			App->renderer->Blit(font, posX, posY, &rect6, scale);
			break;
		case 7:
			App->renderer->Blit(font, posX, posY, &rect7, scale);
			break;
		case 8:
			App->renderer->Blit(font, posX, posY, &rect8, scale);
			break;
		case 9:
			App->renderer->Blit(font, posX, posY, &rect9, scale);
			break;
		}

		posX -= separacio; 
	}
	posX = initialPosX;
}

void ModuleSceneIntro::FontDrawLife(int score, int n, int posX, int posY, int separacio, float scale) {
	int initialPosX = posX;
	int scoreCopia = score;
	int scoreArray[1];
	for (int j = 0; j < n; ++j) {
		scoreArray[j] = scoreCopia % 10;
		scoreCopia /= 10;
	}

	SDL_Rect rect0 = { 1 * 20, 2 * 34, 20, 34 };
	SDL_Rect rect1 = { 2 * 20, 1 * 34, 20, 34 };
	SDL_Rect rect2 = { 3 * 20, 1 * 34, 20, 34 };
	SDL_Rect rect3 = { 4 * 20, 0, 20, 34 };


	for (int k = 0; k < n; ++k) {

		switch (scoreArray[k]) {
		case 0:
			App->renderer->Blit(font, posX, posY, &rect0, scale);
			break;
		case 1:
			App->renderer->Blit(font, posX, posY, &rect1, scale);
			break;
		case 2:
			App->renderer->Blit(font, posX, posY, &rect2, scale);
			break;
		case 3:
			App->renderer->Blit(font, posX, posY, &rect3, scale);
			break;
		}
	}
	posX = initialPosX;
}

void ModuleSceneIntro::FontDrawMaxScore(int score, int n, int posX, int posY, int separacio, float scale) {
	int initialPosX = posX;
	int scoreCopia = score;
	int scoreArray[4];
	for (int j = 0; j < n; ++j) {
		scoreArray[j] = scoreCopia % 10;
		scoreCopia /= 10;
	}

	SDL_Rect rect0 = { 1 * 20, 34, 20, 32 };
	SDL_Rect rect1 = { 2 * 20, 34, 20, 32 };
	SDL_Rect rect2 = { 3 * 20, 34, 20, 32 };
	SDL_Rect rect3 = { 4 * 20, 34, 20, 32 };
	SDL_Rect rect4 = { 5 * 20, 34, 20, 32 };
	SDL_Rect rect5 = { 6 * 20, 34, 20, 32 };
	SDL_Rect rect6 = { 7 * 20, 34, 20, 32 };
	SDL_Rect rect7 = { 8 * 20, 34, 20, 32 };
	SDL_Rect rect8 = { 9 * 20, 34, 20, 32 };
	SDL_Rect rect9 = { 10 * 20, 34, 20, 32 };

	for (int k = 0; k < n; ++k) {

		switch (scoreArray[k]) {
		case 0:
			App->renderer->Blit(font, posX, posY, &rect0, scale);
			break;
		case 1:
			App->renderer->Blit(font, posX, posY, &rect1, scale);
			break;
		case 2:
			App->renderer->Blit(font, posX, posY, &rect2, scale);
			break;
		case 3:
			App->renderer->Blit(font, posX, posY, &rect3, scale);
			break;
		case 4:
			App->renderer->Blit(font, posX, posY, &rect4, scale);
			break;
		case 5:
			App->renderer->Blit(font, posX, posY, &rect5, scale);
			break;
		case 6:
			App->renderer->Blit(font, posX, posY, &rect6, scale);
			break;
		case 7:
			App->renderer->Blit(font, posX, posY, &rect7, scale);
			break;
		case 8:
			App->renderer->Blit(font, posX, posY, &rect8, scale);
			break;
		case 9:
			App->renderer->Blit(font, posX, posY, &rect9, scale);
			break;
		}

		posX -= separacio;
	}
	posX = initialPosX;
}