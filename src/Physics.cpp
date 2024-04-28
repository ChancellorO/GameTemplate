#include "Physics.hpp"

#include <iostream>

void g::Physics::Start() {
	auto gravity = b2Vec2{0, kGravity};
	world = std::make_shared<b2World>(gravity);
	g.reg.ctx().emplace<std::shared_ptr<b2World>>(world);

	g.reg.on_construct<g::PhysicsObject>().connect<&Physics::OnPhysicsObjectConstruct>(*this);
	g.reg.on_destroy<b2Body*>().connect<&Physics::OnPhysicsObjectDestruct>(*this);
}

void g::Physics::Update(float dt) {
	// syncing physics with transform
	g.reg.view<const g::PhysicsObject, b2Body*, g::Transform>().each([](auto entity, const g::PhysicsObject& po, b2Body*& body, g::Transform& transform) {
		b2Vec2 newPos = transform.p;
		newPos += po.positionOffset;
		body->SetTransform(newPos, transform.angle);
	});

	// applying drag
	g.reg.view<b2Body*, g::PhysicsObject>().each([](auto entity, b2Body*& body, g::PhysicsObject& po) {
		b2Vec2 drag = body->GetLinearVelocity();
		drag *= -po.drag * 1000.0f;
		body->ApplyForceToCenter(drag, false);
	});

	world->Step(dt, 8, 3);

	// syncing transform with physics
	g.reg.view<const g::PhysicsObject, b2Body*, g::Transform>().each([](auto entity, const g::PhysicsObject& po, b2Body*& body, g::Transform& transform) {
		transform.p = body->GetPosition();
		transform.p -= po.positionOffset;
		transform.angle = body->GetAngle();
	});
}

void g::Physics::OnPhysicsObjectDestruct(entt::registry&, entt::entity entity) {
	auto b2b = g.reg.get<b2Body*>(entity);
	world->DestroyBody(b2b);
}

void g::Physics::OnPhysicsObjectConstruct(entt::registry&, entt::entity entity) {
	auto& transform = g.reg.get_or_emplace<g::Transform>(entity, g::Transform{});

	auto& physicsObject = g.reg.get<g::PhysicsObject>(entity);

	b2BodyDef bodyDef;
	bodyDef.type = physicsObject.dynamic ? b2_dynamicBody : b2_staticBody;
	bodyDef.position = transform.p;
	bodyDef.position += physicsObject.positionOffset;
	bodyDef.angle = transform.angle;
	bodyDef.gravityScale = physicsObject.gravityScale;
	bodyDef.fixedRotation = physicsObject.fixedRotation;
	bodyDef.userData.pointer = static_cast<uintptr_t>(entity);

	b2Body* body = world->CreateBody(&bodyDef);

	b2PolygonShape shape;
	b2Vec2 halfExtents = transform.halfExtents;
	halfExtents += physicsObject.halfExtentOffset;
	shape.SetAsBox(halfExtents.x, halfExtents.y);

	b2FixtureDef fixtureDef;
	fixtureDef.density = physicsObject.density;
	fixtureDef.friction = physicsObject.friction;
	fixtureDef.shape = &shape;
    fixtureDef.isSensor = physicsObject.isSensor;

	body->CreateFixture(&fixtureDef);

	g.reg.emplace<b2Body*>(entity, body);
}