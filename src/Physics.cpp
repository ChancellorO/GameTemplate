#include "Physics.hpp"

#include <iostream>

void g::Physics::Start() {
	auto gravity = b2Vec2{0, 10};
	world = std::make_shared<b2World>(gravity);
	g.reg.ctx().emplace<std::shared_ptr<b2World>>(world);

	g.reg.on_construct<g::PhysicsObject>().connect<&Physics::OnPhysicsObjectConstruct>(*this);
}

void g::Physics::Update(float dt) {
	// syncing physics with transform
	g.reg.view<b2Body*, g::Transform>().each([](auto entity, b2Body*& body, g::Transform& transform) {
		body->SetTransform(transform.p, transform.angle);
	});

	// applying drag
	g.reg.view<b2Body*, g::PhysicsObject>().each([](auto entity, b2Body*& body, g::PhysicsObject& po) {
		b2Vec2 drag = body->GetLinearVelocity();
		drag *= -po.drag * 1000.0f;
		// std::cout << drag.x << ", " << drag.y << std::endl;
		body->ApplyForceToCenter(drag, false);
	});

	world->Step(dt, 8, 3);

	// syncing transform with physics
	g.reg.view<b2Body*, g::Transform>().each([](auto entity, b2Body*& body, g::Transform& transform) {
		transform.p = body->GetPosition();
		transform.angle = body->GetAngle();
	});
}

void g::Physics::OnPhysicsObjectConstruct(entt::registry&, entt::entity entity) {
	auto& transform = g.reg.get_or_emplace<g::Transform>(entity, g::Transform{});

	auto& physicsObject = g.reg.get<g::PhysicsObject>(entity);

	b2BodyDef bodyDef;
	bodyDef.type = physicsObject.dynamic ? b2_dynamicBody : b2_staticBody;
	bodyDef.position = transform.p;
	bodyDef.angle = transform.angle;
	bodyDef.gravityScale = physicsObject.gravityScale;
	bodyDef.fixedRotation = physicsObject.fixedRotation;
	bodyDef.userData.pointer = static_cast<uintptr_t>(entity);

	b2Body* body = world->CreateBody(&bodyDef);

	b2PolygonShape shape;
	shape.SetAsBox(transform.halfExtents.x, transform.halfExtents.y);

	b2FixtureDef fixtureDef;
	fixtureDef.density = physicsObject.density;
	fixtureDef.friction = physicsObject.friction;
	fixtureDef.shape = &shape;

	body->CreateFixture(&fixtureDef);

	g.reg.emplace<b2Body*>(entity, body);
}