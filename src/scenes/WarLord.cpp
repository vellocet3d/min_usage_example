#include <memory>
#include <iostream>

#include "glm/gtx/string_cast.hpp"
#include "btBulletDynamicsCommon.h"
#include "vel/helpers/functions.h"
#include "vel/App.h"
#include "vel/scene/CollisionDebugDrawer.h"
#include "vel/scene/stage/Transform.h"

#include "scenes/WarLord.h"
#include "scenes/Asylum.h"


void WarLord::load()
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

void WarLord::innerLoop(float deltaTime)
{
	this->client->fixedUpdate(deltaTime);	
	
}

void WarLord::postPhysics(float deltaTime)
{
	this->client->postPhysics(deltaTime);

}

void WarLord::outerLoop(float frameTime, float renderLerpInterval)
{
	this->client->immediateUpdate(frameTime, renderLerpInterval);



	if (vel::App::get().getInputState().keyEscape)
	{
		vel::App::get().close();
	}

	if (vel::App::get().getInputState().keyL && !this->swappingScenes)
	{
		this->swappingScenes = true;
		vel::App::get().loadNextScene(new Asylum());
	}
}

void WarLord::createBaseMap(vel::scene::stage::Stage& stage)
{
	// load the map
	stage.loadActors("data/models/maps/warlord/static_collidable_visible.fbx");

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

void WarLord::addStaticNonCollidableVisibleObjects(vel::scene::stage::Stage& stage)
{
	auto actorIndexes = stage.loadActors("data/models/maps/warlord/static_noncollidable_visible.fbx");

	/*
		For each actor that was just loaded:
			> do nothing as nothing is required!
	*/
}

void WarLord::addStaticCollidableHiddenObjects(vel::scene::stage::Stage& stage)
{
	auto actorIndexes = stage.loadActors("data/models/maps/warlord/static_collidable_hidden.fbx");

	/*
		For each actor that was just loaded:
			> set it's visible status to false
			> create a static collision rigidbody, adding to collision world, and adding
				pointer to actor
			> check if additional logic required by parsing actor name (for example, is this
				a sensor object and if so apply sensor logic)
	*/
	for (auto& ai : actorIndexes)
	{
		auto a = stage.getActor(ai);

		a->setVisible(false);

		auto avec = std::vector<Actor*>();
		avec.push_back(a);
		auto rb = stage.getCollisionWorld()->addStaticCollisionBody(a->getName(), avec);

		a->setRigidBody(rb);

		// check if this is a sensor
		if (a->getName().find("ladder1_sensor") != std::string::npos)
		{

			stage.getCollisionWorld()->addSensor(new vel::scene::stage::Sensor([this](btPersistentManifold* contactManifold,
				std::pair<btCollisionObject*, btCollisionObject*> contactPair) {

				this->client->setCapsuleContactWithLadder(true);

			}, rb, this->client->getCapsuleActor()->getGhostObject()));

		}
	}
}

void WarLord::addDynamicCollidableVisibleObjects(vel::scene::stage::Stage& stage)
{
	// load dynamic collidable visible objects
	stage.loadActors("data/models/maps/warlord/dynamic_collidable_visible.fbx", true);

	/////////////////////////////////
	// create all barrels
	/////////////////////////////////

	// create the collision shape to be used by all barrels
	btCollisionShape* barrel1 = new btCylinderShape(btVector3(0.32, 0.44, 0.32));
	stage.getCollisionWorld()->addCollisionShape("barrel1", barrel1);

	for (auto& a : stage.getActors())
	{
		if (a.getName().find("barrel1") != std::string::npos)
		{
			//a.getTransform().setRotation(-90.0f, glm::vec3(1.0f, 0.0f, 0.0f));

			btTransform startTransform;
			startTransform.setIdentity();

			btScalar mass(1.0f);
			btVector3 localInertia(0, 0, 0);
			barrel1->calculateLocalInertia(mass, localInertia);

			auto at = a.getTransform().getTranslation();
			startTransform.setOrigin(btVector3(at.x, at.y, at.z));
			startTransform.setRotation(vel::helpers::functions::glmToBulletQuat(a.getTransform().getRotation()));

			btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
			btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, barrel1, localInertia);
			auto barrelBody = new btRigidBody(rbInfo);

			a.setRigidBody(barrelBody);
			a.setManualTransform(false);

			stage.getCollisionWorld()->getDynamicsWorld()->addRigidBody(barrelBody);
		}
	}

	
}