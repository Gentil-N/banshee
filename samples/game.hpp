#pragma once
#ifndef __GAME_HPP__
#define __GAME_HPP__

#include "../kernel/kernel.hpp"

namespace bsh
{
       extern bool GAME_SHOULD_BE_ENDED;
       extern float GAME_DELTA;
       extern float GAME_ADVANCE;
       extern int WIN_WIDTH;
       extern int WIN_HEIGHT;
       extern float WIN_RATIO;
       extern float WIN_LEFT, WIN_RIGHT, WIN_BOTTOM, WIN_TOP;
       extern float MAX_PLANE_ROLL;
       extern dsk::Window WINDOW;
       extern rp3d::PhysicsCommon PHCMN;

       namespace comp
       {
              enum MoveType
              {
                     MV_NOTHING = 0x0,
                     MV_LEFT = 0x1,
                     MV_RIGHT = 0x2,
                     MV_DOWN = 0x4,
                     MV_UP = 0x8
              };
       }

       namespace sch
       {
              enum DayTimeType
              {
                     DTT_MORNING,
                     DTT_MIDDAY,
                     DTT_EVENING
              };

              struct MapConfiguration
              {
                     DayTimeType day_time = DTT_MORNING;
                     float map_flow = 0.0f;
              };

              enum State
              {
                     ST_NO_ENEMY = 0x1,
                     ST_NO_PLAYER = 0x2
              };

              enum EventType
              {
                     EV_NOTHING,
                     EV_CHECKPOINT,
                     EV_SUMMON_PLANE,
                     EV_SUMMON_BULLET
              };

              enum Faction
              {
                     FT_FRIEND,
                     FT_ENEMY
              };

              struct Event
              {
                     EventType event_type;
                     union
                     {
                            struct
                            {
                                   Faction faction;
                                   comp::MoveType move;
                                   glm::vec3 position, speed;
                                   float gun_rate, gun_power;
                                   bool ai_kamikaze;
                                   int life; 
                                   int damages; 
                                   int bullet_life; 
                                   int bullet_damages;
                            } summon_plane;
                            struct
                            {
                                   glm::vec3 position, velocity;
                            } summon_bullet;
                     } data;
              };

              struct Task
              {
                     union
                     {
                            float elapsed_time;
                            State states;
                     } waiting;
                     enum
                     {
                            WAIT_STATE,
                            WAIT_ELAPSED_TIME
                     } waiting_type;
                     std::vector<Event> events;
              };

              struct Schedule
              {
                     std::vector<Task> tasks;
              };
       }

       namespace comp
       {
              struct Object
              {
                     glm::vec3 position;
                     glm::vec3 velocity;
                     glm::vec3 rotation;
                     grx::engine_3d::Batch *batch;
                     grx::engine_3d::ObjectData *object_data;
              };

              struct ExplosionParticle
              {
                     glm::vec3 position;
                     glm::vec2 tex_index;
                     float scale;
                     float life = 0.0f;
                     glm::vec3 velocity;
              };

              struct ExplosionEmitter
              {
                     size_t nump;
                     glm::vec2 dim_atlas;
                     glm::vec3 position;
                     float life;
                     std::vector<ExplosionParticle> particles;
                     grx::engine_3d::ParticleSystem *particle_system;
              };

              struct SmokeParticle
              {
                     glm::vec3 position;
                     glm::vec2 tex_index;
                     float scale;
                     float life = 0.0f;
              };

              struct SmokeEmitter
              {
                     size_t nump;
                     glm::vec2 dim_atlas;
                     glm::vec3 position;
                     float emit_elapsed_life = 0.0f;
                     std::vector<SmokeParticle> particles;
                     grx::engine_3d::ParticleSystem *particle_system;
              };

              struct PhBody
              {
                     rp3d::CollisionBody *body;
              };

              struct Life
              {
                     sch::Faction faction;
                     int life;
                     int damages;
                     int max_life;
              };

              struct Plane
              {
                     int current_move;
                     glm::vec3 speed;
                     snd::Sound sound;
              };

              struct Player
              {
                     std::string name;
                     dsk::KeyCode key_up, key_down, key_left, key_right, key_fire;
              };

              struct GunBullet
              {
                     sch::Faction faction;
                     float gun_rate;
                     float gun_elapsed_time;
                     float power;
                     int bullet_life;
                     int bullet_damages;
                     snd::Sound sound;
              };

              struct Speaker
              {
                     snd::Sound sound;
              };

              struct EnemyAI
              {
                     bool is_kamikaze;
              };
       }

       struct Stage
       {
              entt::registry reg;
              bool is_ended;
              size_t task_index = 0;
              float elapsed_time = 0.0f;
              size_t states;
              bool player_2_present = false;
              sch::MapConfiguration map_config;
              rp3d::PhysicsWorld *ph_world;
              struct
              {
                     struct
                     {
                            grx::engine_3d::Mesh *mesh;
                            rp3d::BoxShape *wings_shape;
                            rp3d::BoxShape *body_shape;
                            rp3d::Transform wings_shape_transform;
                            rp3d::Transform body_shape_transform;
                            struct
                            {
                                   grx::engine_3d::Batch *batch;
                                   grx::engine_3d::Texture *texture;
                                   grx::engine_3d::Material *material;
                            } r_enemy, r_friend, r_player_1, r_player_2;

                     } plane;
                     struct
                     {
                            grx::engine_3d::Mesh *mesh;
                            grx::engine_3d::Batch *batch;
                            grx::engine_3d::Texture *texture;
                            grx::engine_3d::Material *material;
                            rp3d::SphereShape *shape;
                            rp3d::Transform shape_transform;
                     } bullet;
                     /*struct
                     {
                            grx::engine_3d::Mesh *mesh;
                            grx::engine_3d::Batch *batch;
                            grx::engine_3d::Texture *texture;
                            grx::engine_3d::Material *material;
                     } terrain;*/
                     struct
                     {
                            grx::engine_3d::Texture *explosion;
                            grx::engine_3d::Texture *smoke;
                     } particle_textures;
                     struct
                     {
                            snd::Sample gun_bullet_fire;
                            snd::Sample explosion;
                            snd::Sample plane_player;
                            snd::Sample plane_other;
                     } samples;
              } res;
       };

       void load_resources_stage(Stage &stage, const sch::MapConfiguration &map_config);
       void step_stage(Stage &stage, const sch::Schedule &schedule);
       void free_resources_stage(Stage &stage);
}

#endif //__GAME_HPP__