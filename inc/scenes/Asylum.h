#pragma once

#include <memory>

#include "vel/InputState.h"
#include "vel/scene/Scene.h"
#include "vel/scene/stage/Actor.h"

#include "Client.h"


class Asylum : public vel::scene::Scene
{

public:
	Asylum() : vel::scene::Scene() {};
	void    load();
	void	innerLoop(float deltaTime);
	void	outerLoop(float frameTime, float renderLerpInterval);
	void	postPhysics(float deltaTime);

	void	addDynamicCollidableVisibleObjects(vel::scene::stage::Stage& stage);
	void	createBaseMap(vel::scene::stage::Stage& stage);
	void	addStaticCollidableHiddenObjects(vel::scene::stage::Stage& stage);
	void	addStaticNonCollidableVisibleObjects(vel::scene::stage::Stage& stage);



	std::unique_ptr<Client>			client;
	btRigidBody*					mapBody;

	bool	swappingScenes = false;



};