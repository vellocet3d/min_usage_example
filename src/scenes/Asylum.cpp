#include <memory>
#include <iostream>

#include "glm/gtx/string_cast.hpp"
#include "btBulletDynamicsCommon.h"
#include "vel/helpers/functions.h"
#include "vel/App.h"
#include "vel/scene/CollisionDebugDrawer.h"
#include "vel/scene/stage/Transform.h"

#include "scenes/Asylum.h"
#include "scenes/WarLord.h"


void Asylum::load()
{

	auto& stage = this->addStage();
	stage.addPerspectiveCamera(false, 0.1f, 250.0f, 60.0f);

	// Static origin look
	stage.getCamera()->setPosition(-8.0f, 1.8f, 0.0f);
	stage.getCamera()->setLookAt(0.0f, 1.8f, 0.0f);

	// add a collision world to the stage
	stage.setCollisionWorld();

	// create a static perspective stage for user hands/weapon
	auto& stage3 = this->addStage();
	stage3.setClearDepthBuffer(true);
	stage3.addPerspectiveCamera(true, 0.1f, 25.0f, 60.0f);

	// ak47
	stage3.loadActors("data/models/weapons/ak47/002.fbx", true);
	auto akArms = stage3.getActor("hands");
	akArms->getTransform().setTranslation(glm::vec3(0.0f, -1.55f, -0.225f));
	akArms->getArmature()->playAnimation("idle");


	// create the base static collidable visible map using mesh data from all
	// actors in provided file
	this->createBaseMap(stage);

	// add the player capsule and create client object
	stage.loadActors("data/models/player_capsule/player_capsule.fbx", true);
	auto clientActor = stage.getActor("capsule");
	clientActor->getTransform().setTranslation(glm::vec3(0.0f, 1.0f, 0.0f));
	clientActor->setVisible(false); // uncomment this to hide capsule model (when using fps camera)


	this->client = std::make_unique<Client>(stage, clientActor, akArms);

	// add all objects that are static and collidable, but hidden
	this->addStaticCollidableHiddenObjects(stage);

	// add all objects that are static NON-collidable, and visible
	this->addStaticNonCollidableVisibleObjects(stage);

	// add all physics objects to stage
	this->addDynamicCollidableVisibleObjects(stage);


	// create an orthographic hud stage
	auto& stage2 = this->addStage();
	stage2.addOrthographicCamera(true, -0.1f, 250.0f, 0.665f);
	stage2.loadActors("data/models/cursor_ball/cursor_ball.fbx");

	auto cursorActor = stage2.getActor("cursor_ball");
	cursorActor->getTransform().setScale(glm::vec3(0.0045f, 0.0045f, 0.0045f));

}

void Asylum::innerLoop(float deltaTime)
{
	this->client->fixedUpdate(deltaTime);



}

void Asylum::postPhysics(float deltaTime)
{
	this->client->postPhysics(deltaTime);


}

void Asylum::outerLoop(float frameTime, float renderLerpInterval)
{
	this->client->immediateUpdate(frameTime, renderLerpInterval);

	if (vel::App::get().getInputState().keyEscape)
	{
		vel::App::get().close();
	}

	if (vel::App::get().getInputState().keyL && !this->swappingScenes)
	{
		this->swappingScenes = true;
		vel::App::get().loadNextScene(new WarLord());
	}
}

void Asylum::createBaseMap(vel::scene::stage::Stage& stage)
{
	// load the map
	stage.loadActors("data/models/maps/asylum/asylum.fbx");

	// for each static collidable visible object, add it to a vector to be used
	// to build the static collision mesh for the map, if the mesh should be it's
	// own referenceable entity, don't put it in static_collidable_visible.fbx
	std::vector<Actor*> staticActors;
	for (auto& a : stage.getActors())
	{
		staticActors.push_back(&a);
	}

	// pass the array of static actors to addStaticCollisionBody, which will generate
	// the static map rigid body
	this->mapBody = stage.getCollisionWorld()->addStaticCollisionBody("base_map", staticActors);
}

void Asylum::addStaticNonCollidableVisibleObjects(vel::scene::stage::Stage& stage)
{
	
}

void Asylum::addStaticCollidableHiddenObjects(vel::scene::stage::Stage& stage)
{
	
}

void Asylum::addDynamicCollidableVisibleObjects(vel::scene::stage::Stage& stage)
{

}