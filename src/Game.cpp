#include "Game.hpp"
#include "Physics.hpp"

#include <box2d/b2_distance.h>

using namespace entt::literals;

// helper function
entt::entity CreateSprite(entt::registry& reg, g::Transform trans, g::PhysicsObject po, g::Sprite s) {
    entt::entity entity = reg.create();

    // Creating a Transform Component for the current entity, with the Transform object
    // returns a reference to the newly created component
    reg.emplace<g::Transform>(entity, trans);
    reg.emplace<g::PhysicsObject>(entity, po);    
    reg.emplace<g::Sprite>(entity, s);

    return entity;
}

void Game::Start() {

    SetConfigFlags(0);

    InitWindow(kWindowWidth, kWindowHeight, kWindowTitle);

    reg.ctx().emplace_as<Camera2D>("camera"_hs, Camera2D {
        .offset = { 0 },
        .target = { 0 },
        .rotation = { 0 },
        .zoom = kZoom,
    });

    systems = std::vector<std::shared_ptr<System>> {
        std::make_shared<g::Physics>(g::Physics { *this })
    };

    // auto& is getting the reference, guaranteeing that it is being shared somewhere else
    for(auto& sys: systems) {
        sys->Start();
    };

    // creates new entity
    auto entity = reg.create();

    // loads cat texture
    textureCache.load("cat"_hs, "resources/cat.jpg");

    // CreateSprite returns an entt::entity
    auto player = CreateSprite(
        reg,
        g::Transform {
            .p=b2Vec2 { 10, 10 },
            .halfExtents = b2Vec2 { 10, 10},
        },
        g::PhysicsObject {
            .dynamic = true,
            .density = 1,
            .fixedRotation = true,
        },
        g::Sprite { textureCache["cat"_hs] }
    );

    // ask for clarification (pretty sure this is creating a component for the player entity, which is the tag player)
    reg.emplace<entt::tag<"player"_hs>>(player);

    // random character example
    auto character = CreateSprite(
        reg,
        g::Transform {
            .p=b2Vec2 { 50, 10 },
            .halfExtents = b2Vec2 { 10, 10},
        },
        g::PhysicsObject {
            .dynamic = true,
            .density = 1,
            .fixedRotation = true,
        },
        g::Sprite { textureCache["cat"_hs] }
    );

    reg.emplace<entt::tag<"character"_hs>>(character);

    // ground
    auto floor = CreateSprite(
        reg,
        g::Transform {
            .p=b2Vec2 {  kWorldWidth / 2, kWorldHeight - 5 },
            .halfExtents = b2Vec2 {  kWorldWidth / 2, 5 }, // 5 down and up?
        },
        g::PhysicsObject {
            .dynamic = false, // therefore static so doesn't move
            .density = 1,
            .fixedRotation = true,
        },
        g::Sprite { textureCache["cat"_hs] }
    );
}

void Game::Update() {
    BeginDrawing();
    BeginMode2D(reg.ctx().get<Camera2D>("camera"_hs));

    // into the registry, grabs every component that has b2Body and tag player
    // ask about b2Body and rigid body
    reg.view<b2Body*, entt::tag<"player"_hs>>().each([&](auto entity, b2Body*& body) {
        b2Vec2 inputDir {
            IsKeyDown(KEY_D) * 1.0f + IsKeyDown(KEY_A) * -1.0f,
            IsKeyDown(KEY_S) * 1.0f + IsKeyDown(KEY_W) * -1.0f,            
        };

        inputDir.Normalize();
        // scalar for velocity
        inputDir *= 100.0f;

        body->SetLinearVelocity(inputDir);
    });

    //logic for checking collision
    reg.view<Transform, b2Body*, entt::tag<"player"_hs>>().each([&](auto entity,Transform& transform, b2Body*& body) {
        // grabs the world component
        auto world = reg.ctx().get<std::shared_ptr<b2World>>();
        
        auto contact_edge = body->GetContactList();
        while(contact_edge) {
            auto otherEntity = static_cast<entt::entity>(contact_edge->other->GetUserData().pointer);
            if(reg.storage<entt::tag<"character"_hs>>().contains(otherEntity)) {

            }
            contact_edge = contact_edge->next;
            // reg.view<b2Body*, entt::tag<"character"_hs>().each([&](auto otherEnt, b2Body *& otherBody) {
            //     if(otherbody == contactEdge.other) {

            //     }                
            // });
            // contact_edge = contact_edge->next;
        }

        // reg.view<b2Body*, entt::tag<"character"_hs>>().each([&](auto ch, b2Body*& chBody) {
        //     b2DistanceProxy proxyA, proxyB;
        //     proxyA.Set(body->fix)
        //})
    });        

    ClearBackground(kBackgroundColor);

    // ask about [&]
    reg.view<g::Transform, g::Sprite>().each([&](auto entity, g::Transform& transform, auto& sprite){
        DrawTexturePro(
            sprite.tex,
            Rectangle {
                0, 0,
                static_cast<float>(sprite.tex->width), 
                static_cast<float>(sprite.tex->height),
            },
            Rectangle {
                transform.p.x,
                transform.p.y,
                2 * transform.halfExtents.x,
                2 * transform.halfExtents.y,                
            },
            Vector2 { transform.halfExtents.x, transform.halfExtents.y },
            transform.angle * RAD2DEG,
            RAYWHITE
            );
    });

    EndMode2D();
    EndDrawing();
    
    float dt = GetFrameTime();

    for(auto& sys : systems) {
        sys->Update(dt);
    }

    if(dt < 1/60.0f) {
        WaitTime(1/60.0f - dt);
    }

}

void Game::Destroy() {
    for(auto& sys : systems) {
        sys->Destroy();
    }
    textureCache.clear();
    CloseWindow();
}