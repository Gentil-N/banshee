#include "game.hpp"

namespace bsh
{
       /*void create_terrain(Stage &stage, const sch::MapConfiguration map_config)
       {
              entt::entity terrain = stage.reg.create();
       }*/

       template <typename type>
       bool contains(const std::vector<type> &vec, const type &t)
       {
              for (const auto &elem : vec)
              {
                     if (elem == t)
                     {
                            return true;
                     }
              }
              return false;
       }

       snd::Sound add_comp_speaker(Stage &stage, entt::entity entity, const snd::Sample &sample, glm::vec3 position)
       {
              snd::Sound sound;
              sound.create();
              sound.set_sample(sample);
              sound.set_position(position.x, position.y, position.z);
              stage.reg.emplace<comp::Speaker>(entity, sound);
              return sound;
       }

       glm::vec2 get_tex_index_ee(comp::ExplosionEmitter &emit, size_t i)
       {
              return glm::vec2(i % (size_t)emit.dim_atlas.x, (i - (i % (size_t)emit.dim_atlas.x)) / (size_t)emit.dim_atlas.x);
       }

       void update_draw_ee(comp::ExplosionEmitter &emit)
       {
              struct
              {
                     glm::vec3 position;
                     glm::vec2 tex_index;
                     float scale;
              } data; //to not override virtual data's struct
              for (size_t i = 0; i < emit.particles.size(); ++i)
              {
                     data.position = emit.particles[i].position;
                     data.tex_index = emit.particles[i].tex_index;
                     data.scale = emit.particles[i].scale;
                     grx::engine_3d::update_particle(emit.particle_system, i, &data);
              }
              data.position = glm::vec3(0, 0, 0);
              data.tex_index = glm::vec2(0, 0);
              data.scale = 0.0f;
              for (size_t i = emit.particles.size(); i < emit.nump; ++i)
              {
                     grx::engine_3d::update_particle(emit.particle_system, i, &data);
              }
       }

       bool update_particles_ee(Stage &stage, comp::ExplosionEmitter &emit)
       {
              const float max_life = 1.2f;
              if (emit.life == 0.0f)
              {
                     for (size_t i = 0; i < emit.nump; ++i)
                     {
                            //velocity = glm::rotateY(velocity, i * 2 * glm::pi<float>() / emit.nump);
                            comp::ExplosionParticle particle;
                            particle.position = emit.position;
                            particle.tex_index = glm::vec2(0, 0);
                            particle.scale = 2.0f;
                            particle.life = 0.0f;
                            particle.velocity = glm::vec3(0, 0, 0);
                            emit.particles.push_back(particle);
                     }
              }
              const float step = max_life / (emit.dim_atlas.x * emit.dim_atlas.y);
              for (size_t i = 0; i < emit.particles.size(); ++i)
              {
                     emit.particles[i].tex_index = get_tex_index_ee(emit, (size_t)(emit.particles[i].life / step));
                     emit.particles[i].position += GAME_ADVANCE * emit.particles[i].velocity + glm::vec3(0, 0, GAME_ADVANCE * stage.map_config.map_flow);
                     emit.particles[i].life += GAME_DELTA;
              }
              emit.life += GAME_DELTA;
              if (emit.life >= max_life)
              {
                     return true;
              }
              return false;
       }

       glm::vec2 get_tex_index_se(comp::SmokeEmitter &emit, size_t i)
       {
              return glm::vec2(i % (size_t)emit.dim_atlas.x, (i - (i % (size_t)emit.dim_atlas.x)) / (size_t)emit.dim_atlas.x);
       }

       void update_draw_se(comp::SmokeEmitter &emit)
       {
              struct
              {
                     glm::vec3 position;
                     glm::vec2 tex_index;
                     float scale;
              } data; //to not override virtual data's struct
              for (size_t i = 0; i < emit.particles.size(); ++i)
              {
                     data.position = emit.particles[i].position;
                     data.tex_index = emit.particles[i].tex_index;
                     data.scale = emit.particles[i].scale;
                     grx::engine_3d::update_particle(emit.particle_system, i, &data);
              }
              data.position = glm::vec3(0, 0, 0);
              data.tex_index = glm::vec2(0, 0);
              data.scale = 0.0f;
              for (size_t i = emit.particles.size(); i < emit.nump; ++i)
              {
                     grx::engine_3d::update_particle(emit.particle_system, i, &data);
              }
       }

       bool update_particles_se(Stage &stage, comp::SmokeEmitter &emit)
       {
              const float loop_life = 3.0f;
              if (emit.particles.size() < emit.nump)
              {
                     const float emit_time = loop_life / (float)emit.nump;
                     emit.emit_elapsed_life += GAME_DELTA;
                     if (emit.emit_elapsed_life > emit_time)
                     {
                            emit.emit_elapsed_life = 0;

                            comp::SmokeParticle particle;
                            particle.position = emit.position;
                            particle.tex_index = glm::vec2(0, 0);
                            particle.scale = 1.0f;
                            particle.life = 0.0f;
                            emit.particles.push_back(particle);
                     }
              }
              const float step = loop_life / (emit.dim_atlas.x * emit.dim_atlas.y);
              for (size_t i = 0; i < emit.particles.size(); ++i)
              {
                     emit.particles[i].tex_index = get_tex_index_se(emit, (size_t)(emit.particles[i].life / step));
                     emit.particles[i].position += glm::vec3(0, 0, GAME_ADVANCE * stage.map_config.map_flow);
                     emit.particles[i].life += GAME_DELTA;
                     if (emit.particles[i].life >= loop_life)
                     {
                            emit.particles[i].position = emit.position;
                            emit.particles[i].tex_index = glm::vec2(0, 0);
                            emit.particles[i].life = 0.0f;
                     }
              }
              return false;
       }

       void add_comp_explosionparticle(Stage &stage, entt::entity entity, glm::vec3 position)
       {
              size_t nump = 1;
              glm::vec2 dim_atlas(8, 6);
              grx::engine_3d::ParticleSystem *sys = grx::engine_3d::create_particle_system(stage.res.particle_textures.explosion, nump, dim_atlas.x, dim_atlas.y);
              stage.reg.emplace<comp::ExplosionEmitter>(entity, nump, dim_atlas, position, 0.0f, std::vector<comp::ExplosionParticle>(), sys);
       }

       void create_explosion(Stage &stage, glm::vec3 position)
       {
              entt::entity explosion = stage.reg.create();
              add_comp_explosionparticle(stage, explosion, position);
              snd::Sound sound = add_comp_speaker(stage, explosion, stage.res.samples.explosion, position);
              sound.play();
       }

       void add_comp_smokeparticle(Stage &stage, entt::entity entity, glm::vec3 position)
       {
              size_t nump = 100;
              glm::vec2 dim_atlas(8, 8);
              grx::engine_3d::ParticleSystem *sys = grx::engine_3d::create_particle_system(stage.res.particle_textures.smoke, nump, dim_atlas.x, dim_atlas.y);
              stage.reg.emplace<comp::SmokeEmitter>(entity, nump, dim_atlas, position, 0.0f, std::vector<comp::SmokeParticle>(), sys);
       }

       void add_unique_comp_smokeparticle(Stage &stage, entt::entity entity, glm::vec3 position)
       {
              auto view_smoke = stage.reg.view<comp::SmokeEmitter>();
              for (auto test : view_smoke)
              {
                     if (entity == test)
                     {
                            return;
                     }
              }
              add_comp_smokeparticle(stage, entity, position);
       }

       void destroy_comp_smoke(Stage &stage, entt::entity entity)
       {
              auto view_smoke = stage.reg.view<comp::SmokeEmitter>();
              for (auto test : view_smoke)
              {
                     if (entity == test)
                     {
                            auto &comp_emit = view_smoke.get<comp::SmokeEmitter>(test);
                            grx::engine_3d::destroy_particle_system(comp_emit.particle_system);
                            return;
                     }
              }
       }

       void destroy_comp_gun_bullet(Stage &stage, entt::entity entity)
       {
              auto view_gun_bullet = stage.reg.view<comp::GunBullet>();
              for (auto test : view_gun_bullet)
              {
                     if (entity == test)
                     {
                            auto &comp_gun_bullet = view_gun_bullet.get<comp::GunBullet>(test);
                            comp_gun_bullet.sound.stop();
                            comp_gun_bullet.sound.destroy();
                            return;
                     }
              }
       }

       void destroy_comp_phbody(Stage &stage, entt::entity entity)
       {
              auto view_phbody = stage.reg.view<comp::PhBody>();
              for (auto test : view_phbody)
              {
                     if (entity == test)
                     {
                            auto &comp_phbody = view_phbody.get<comp::PhBody>(test);
                            stage.ph_world->destroyCollisionBody(comp_phbody.body);
                            return;
                     }
              }
       }

       void destroy_comp_plane(Stage &stage, entt::entity entity)
       {
              auto view_plane = stage.reg.view<comp::Plane>();
              for (auto test : view_plane)
              {
                     if (entity == test)
                     {
                            auto &comp_plane = view_plane.get<comp::Plane>(test);
                            comp_plane.sound.stop();
                            comp_plane.sound.destroy();
                            return;
                     }
              }
       }

       rp3d::CollisionBody *add_comp_phbody(Stage &stage, entt::entity entity, const rp3d::Transform &body_transform)
       {
              rp3d::CollisionBody *body = stage.ph_world->createCollisionBody(body_transform);
              stage.reg.emplace<comp::PhBody>(entity, body);
              return body;
       }

       void create_bullet(Stage &stage, sch::Faction faction, glm::vec3 position, glm::vec3 velocity, int life, int damages)
       {
              entt::entity bullet = stage.reg.create();
              //glm::vec3 vel = glm::normalize(dst - src) * power;
              grx::engine_3d::ObjectData *objd = grx::engine_3d::alloc_object_data(stage.res.bullet.batch);
              stage.reg.emplace<comp::Object>(bullet, position, velocity, glm::vec3(0, 0, 0), stage.res.bullet.batch, objd);
              rp3d::CollisionBody *body = add_comp_phbody(stage, bullet, rp3d::Transform::identity());
              stage.reg.emplace<comp::Life>(bullet, faction, life, damages, life);
              body->addCollider(stage.res.bullet.shape, stage.res.bullet.shape_transform);
       }

       void create_bullet_by_target(Stage &stage, sch::Faction faction, glm::vec3 position, glm::vec3 target, float power, int life, int damages)
       {
              glm::vec3 velocity = glm::normalize(target - position) * power;
              create_bullet(stage, faction, position, velocity, life, damages);
       }

       snd::Sound create_gun_bullet_sound(Stage &stage)
       {
              snd::Sound sound;
              sound.create();
              sound.set_sample(stage.res.samples.gun_bullet_fire);
              return sound;
       }

       void fire_gun_bullet(Stage &stage, comp::GunBullet &comp_gun, glm::vec3 position, glm::vec3 target)
       {
              if (comp_gun.gun_elapsed_time >= comp_gun.gun_rate)
              {
                     create_bullet_by_target(stage, comp_gun.faction, position, target, comp_gun.power, comp_gun.bullet_life, comp_gun.bullet_damages);
                     comp_gun.gun_elapsed_time = 0.0f;
                     comp_gun.sound.set_position(position.x, position.y, position.z);
                     comp_gun.sound.play();
              }
              else
              {
                     comp_gun.gun_elapsed_time += GAME_DELTA;
              }
       }

       void add_player(
           Stage &stage, grx::engine_3d::Batch *batch, const std::string &name, glm::vec3 position, glm::vec3 speed,
           float gun_rate, float gun_power, int life, int damages, int bullet_life, int bullet_damages, dsk::KeyCode key_up, dsk::KeyCode key_down, dsk::KeyCode key_left,
           dsk::KeyCode key_right, dsk::KeyCode key_fire)
       {
              entt::entity player = stage.reg.create();
              stage.reg.emplace<comp::Player>(player, name, key_up, key_down, key_left, key_right, key_fire);
              stage.reg.emplace<comp::Object>(player, position, glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), batch, grx::engine_3d::alloc_object_data(batch));
              snd::Sound sound;
              sound.create();
              sound.set_sample(stage.res.samples.plane_player);
              sound.set_looping(true);
              sound.play();
              stage.reg.emplace<comp::Plane>(player, comp::MV_NOTHING, speed, sound);
              stage.reg.emplace<comp::GunBullet>(player, sch::FT_FRIEND, gun_rate, 0.0f, gun_power, bullet_life, bullet_damages, create_gun_bullet_sound(stage));
              stage.reg.emplace<comp::Life>(player, sch::FT_FRIEND, life, damages, life);
              rp3d::CollisionBody *body = add_comp_phbody(stage, player, rp3d::Transform::identity());
              body->addCollider(stage.res.plane.wings_shape, stage.res.plane.wings_shape_transform);
              body->addCollider(stage.res.plane.body_shape, stage.res.plane.body_shape_transform);
       }

       inline void add_player_1(
           Stage &stage, glm::vec3 position, glm::vec3 speed, float gun_rate, float gun_power, int life, int damages, int bullet_life, int bullet_damages)
       {
              add_player(
                  stage, stage.res.plane.r_player_1.batch, "player-1", position, speed, gun_rate, gun_power, life, damages, bullet_life, bullet_damages,
                  dsk::KEY_W, dsk::KEY_S, dsk::KEY_A, dsk::KEY_D, dsk::KEY_SPACE);
       }

       inline void add_player_2(
           Stage &stage, glm::vec3 position, glm::vec3 speed, float gun_rate, float gun_power, int life, int damages, int bullet_life, int bullet_damages)
       {
              add_player(
                  stage, stage.res.plane.r_player_2.batch, "player-2", position, speed, gun_rate, gun_power, life, damages, bullet_life, bullet_damages,
                  dsk::KEY_UP, dsk::KEY_DOWN, dsk::KEY_LEFT, dsk::KEY_RIGHT, dsk::KEY_ENTER);
       }

       void add_comp_plane(
           Stage &stage, entt::entity plane, sch::Faction faction, glm::vec3 position, int move, glm::vec3 speed, float gun_rate, float gun_power, bool ai_kamikaze,
           int life, int damages, int bullet_life, int bullet_damages)
       {
              grx::engine_3d::ObjectData *objd;
              grx::engine_3d::Batch *bch;
              glm::vec3 rotation = glm::vec3(0, 0, 0);
              float rot_up = 0.0f, rot_down = glm::pi<float>();
              sch::Faction effective_faction = sch::FT_FRIEND;
              switch (faction)
              {
              case sch::FT_ENEMY:
              {
                     objd = grx::engine_3d::alloc_object_data(stage.res.plane.r_enemy.batch);
                     bch = stage.res.plane.r_enemy.batch;
                     move = move | comp::MV_DOWN;
                     rotation.y = rot_down;
                     stage.reg.emplace<comp::EnemyAI>(plane, ai_kamikaze);
                     effective_faction = sch::FT_ENEMY;
                     break;
              }
              case sch::FT_FRIEND:
              {
                     objd = grx::engine_3d::alloc_object_data(stage.res.plane.r_friend.batch);
                     bch = stage.res.plane.r_friend.batch;
                     rotation.y = rot_up;
                     break;
              }
              default:
                     throw std::runtime_error("faction type is not valid");
              }
              stage.reg.emplace<comp::Object>(plane, position, glm::vec3(0, 0, 0), rotation, bch, objd);
              snd::Sound sound;
              sound.create();
              sound.set_sample(stage.res.samples.plane_other);
              sound.set_looping(true);
              sound.play();
              stage.reg.emplace<comp::Plane>(plane, move, speed, sound);
              stage.reg.emplace<comp::GunBullet>(plane, effective_faction, gun_rate, 0.0f, gun_power, bullet_life, bullet_damages, create_gun_bullet_sound(stage));
              stage.reg.emplace<comp::Life>(plane, effective_faction, life, damages, life);
              rp3d::CollisionBody *body = add_comp_phbody(stage, plane, rp3d::Transform::identity());
              body->addCollider(stage.res.plane.wings_shape, stage.res.plane.wings_shape_transform);
              body->addCollider(stage.res.plane.body_shape, stage.res.plane.body_shape_transform);
       }

       void procsys_players(Stage &stage)
       {
              auto view_player = stage.reg.view<comp::Player, comp::GunBullet, comp::Plane, comp::Object>();
              for (auto player : view_player)
              {
                     auto &comp_player = view_player.get<comp::Player>(player);
                     auto &comp_plane = view_player.get<comp::Plane>(player);
                     auto &comp_object = view_player.get<comp::Object>(player);
                     comp_plane.current_move = (int)comp::MV_NOTHING;
                     if (WINDOW.is_pressed(comp_player.key_up) && comp_object.position.z >= -WIN_TOP)
                     {
                            comp_plane.current_move = comp_plane.current_move | (int)comp::MV_UP;
                     }
                     else if (WINDOW.is_pressed(comp_player.key_down) && comp_object.position.z <= -WIN_BOTTOM)
                     {
                            comp_plane.current_move = comp_plane.current_move | (int)comp::MV_DOWN;
                     }
                     if (WINDOW.is_pressed(comp_player.key_left) && comp_object.position.x >= WIN_LEFT)
                     {
                            comp_plane.current_move = comp_plane.current_move | (int)comp::MV_LEFT;
                     }
                     else if (WINDOW.is_pressed(comp_player.key_right) && comp_object.position.x <= WIN_RIGHT)
                     {
                            comp_plane.current_move = comp_plane.current_move | (int)comp::MV_RIGHT;
                     }
                     if (WINDOW.is_pressed(comp_player.key_fire))
                     {
                            auto &comp_gun = view_player.get<comp::GunBullet>(player);
                            fire_gun_bullet(stage, comp_gun, comp_object.position, comp_object.position + glm::vec3(0, 0, -1.0f));
                     }
              }
       }

       void procsys_enemyais(Stage &stage)
       {
              auto view_enemies = stage.reg.view<comp::EnemyAI, comp::Plane, comp::GunBullet, comp::Object>();
              auto view_player = stage.reg.view<comp::Player, comp::Object>();
              for (auto enemy : view_enemies)
              {
                     auto &comp_ai = view_enemies.get<comp::EnemyAI>(enemy);
                     auto &comp_plane = view_enemies.get<comp::Plane>(enemy);
                     auto &comp_gun = view_enemies.get<comp::GunBullet>(enemy);
                     auto &comp_obj = view_enemies.get<comp::Object>(enemy);
                     if (view_player.front() == entt::null)
                     {
                            continue;
                     }
                     glm::vec3 friend_pos(10000, 10000, 10000); // very far...
                     float dist = glm::distance(comp_obj.position, friend_pos);
                     for (auto player : view_player)
                     {
                            auto &player_comp_obj = view_player.get<comp::Object>(player);
                            float new_dist = glm::distance(comp_obj.position, player_comp_obj.position);
                            if (new_dist < dist)
                            {
                                   friend_pos = player_comp_obj.position;
                            }
                     }
                     fire_gun_bullet(stage, comp_gun, comp_obj.position, friend_pos);
                     if (comp_ai.is_kamikaze)
                     {
                            comp_plane.current_move = (int)comp::MV_DOWN;
                            if (comp_obj.position.z - friend_pos.z < 0.0f)
                            {
                                   float diff = comp_obj.position.x - friend_pos.x;
                                   if (diff < -0.5f)
                                   {
                                          comp_plane.current_move = comp_plane.current_move | comp::MV_RIGHT;
                                   }
                                   else if (diff > 0.5f)
                                   {
                                          comp_plane.current_move = comp_plane.current_move | comp::MV_LEFT;
                                   }
                            }
                     }
              }
       }

       void procsys_planes(Stage &stage)
       {
              auto view_plane = stage.reg.view<comp::Plane, comp::Object>();
              for (auto plane : view_plane)
              {
                     auto &comp_plane = view_plane.get<comp::Plane>(plane);
                     auto &comp_obj = view_plane.get<comp::Object>(plane);
                     if (comp_plane.current_move & comp::MV_LEFT)
                     {
                            if (comp_obj.rotation.z < MAX_PLANE_ROLL)
                            {
                                   comp_obj.rotation.z += GAME_ADVANCE * comp_plane.speed.x / 2.0f;
                                   if (comp_obj.rotation.z > MAX_PLANE_ROLL)
                                   {
                                          comp_obj.rotation.z = MAX_PLANE_ROLL;
                                   }
                            }
                            comp_obj.velocity.x = -comp_plane.speed.x;
                     }
                     else if (comp_plane.current_move & comp::MV_RIGHT)
                     {
                            if (comp_obj.rotation.z > -MAX_PLANE_ROLL)
                            {
                                   comp_obj.rotation.z -= GAME_ADVANCE * comp_plane.speed.x / 2.0f;
                                   if (comp_obj.rotation.z < -MAX_PLANE_ROLL)
                                   {
                                          comp_obj.rotation.z = -MAX_PLANE_ROLL;
                                   }
                            }
                            comp_obj.velocity.x = comp_plane.speed.x;
                     }
                     else
                     {
                            if (comp_obj.rotation.z != 0.0f)
                            {
                                   float roll = -glm::sign<float>(comp_obj.rotation.z) * GAME_ADVANCE * comp_plane.speed.x / 2.0f;
                                   comp_obj.rotation.z = glm::abs<float>(comp_obj.rotation.z) < 0.2f ? 0.0f : comp_obj.rotation.z + roll;
                            }
                            comp_obj.velocity.x = 0;
                     }
                     if (comp_plane.current_move & comp::MV_UP)
                     {
                            comp_obj.velocity.z = -comp_plane.speed.z;
                     }
                     else if (comp_plane.current_move & comp::MV_DOWN)
                     {
                            comp_obj.velocity.z = comp_plane.speed.z;
                     }
                     else
                     {
                            comp_obj.velocity.z = 0;
                     }
                     comp_plane.sound.set_position(comp_obj.position.x, comp_obj.position.y, comp_obj.position.z);
                     //comp_plane.sound.set_velocity(comp_obj.velocity.x, comp_obj.velocity.y, comp_obj.velocity.z);
              }
       }

       void procsys_objects(Stage &stage)
       {
              auto view_obj = stage.reg.view<comp::Object>();
              auto view_player = stage.reg.view<comp::Player>();
              for (auto entity : view_obj)
              {
                     auto &obj = view_obj.get<comp::Object>(entity);
                     glm::mat4 final = glm::identity<glm::mat4>();
                     bool is_up = false;
                     for (auto player : view_player)
                     {
                            if (player == entity)
                            {
                                   is_up = true;
                            }
                     }
                     if (!is_up)
                     {
                            obj.position += glm::vec3(0, 0, GAME_ADVANCE * stage.map_config.map_flow);
                     }
                     obj.position += obj.velocity * GAME_ADVANCE;
                     final = glm::translate(final, obj.position);
                     final = glm::rotate(final, obj.rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
                     final = glm::rotate(final, obj.rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
                     final = glm::rotate(final, obj.rotation.y, glm::vec3(0.0f, -1.0f, 0.0f));
                     grx::engine_3d::update_object_data(obj.object_data, &final[0][0]);
              }
       }

       void procsys_bodies(Stage &stage)
       {
              auto view_ph = stage.reg.view<comp::PhBody, comp::Object>();
              for (auto entity : view_ph)
              {
                     auto &comp_phbody = view_ph.get<comp::PhBody>(entity);
                     auto &comp_obj = view_ph.get<comp::Object>(entity);
                     rp3d::Transform transform = rp3d::Transform::identity();
                     transform.setPosition(rp3d::Vector3(comp_obj.position.x, comp_obj.position.y, comp_obj.position.z));
                     transform.setOrientation(rp3d::Quaternion::fromEulerAngles(comp_obj.rotation.x, comp_obj.rotation.y, comp_obj.rotation.z));
                     comp_phbody.body->setTransform(transform);
              }
       }

       void procsys_collisions(Stage &stage)
       {
              auto view_lifes = stage.reg.view<comp::Life, comp::PhBody>();
              std::vector<entt::entity> tested;
              for (auto entity : view_lifes)
              {
                     tested.push_back(entity);
                     auto &comp_life = view_lifes.get<comp::Life>(entity);
                     auto &comp_phbody = view_lifes.get<comp::PhBody>(entity);
                     for (auto other : view_lifes) // Pas du tout optimisé !!!
                     {
                            if (other == entity || contains(tested, other)) // avoid auto test and already tested (double test collision)
                            {
                                   continue;
                            }
                            auto &other_comp_life = view_lifes.get<comp::Life>(other);
                            auto &other_comp_phbody = view_lifes.get<comp::PhBody>(other);
                            if (comp_life.faction != other_comp_life.faction && stage.ph_world->testOverlap(comp_phbody.body, other_comp_phbody.body))
                            {
                                   //spdlog::info("collision !");
                                   comp_life.life -= other_comp_life.damages;
                                   other_comp_life.life -= comp_life.damages;
                            }
                     }
              }
       }

       void procsys_deaths(Stage &stage)
       {
              auto view_objs = stage.reg.view<comp::Life, comp::PhBody, comp::Object>();
              for (auto entity : view_objs)
              {
                     auto &comp_life = view_objs.get<comp::Life>(entity);
                     if (comp_life.life <= 0)
                     {
                            auto &comp_obj = view_objs.get<comp::Object>(entity);
                            auto &comp_phbody = view_objs.get<comp::PhBody>(entity);
                            create_explosion(stage, comp_obj.position);
                            destroy_comp_plane(stage, entity);
                            destroy_comp_smoke(stage, entity);
                            destroy_comp_gun_bullet(stage, entity);
                            stage.ph_world->destroyCollisionBody(comp_phbody.body);
                            grx::engine_3d::free_object_data(comp_obj.batch, comp_obj.object_data);
                            stage.reg.destroy(entity);
                     }
                     else if (comp_life.life <= 0.5f * comp_life.max_life)
                     {
                            auto &comp_obj = view_objs.get<comp::Object>(entity);
                            add_unique_comp_smokeparticle(stage, entity, comp_obj.position);
                     }
              }
       }

       void procsys_explosions(Stage &stage)
       {
              auto view_explosions = stage.reg.view<comp::ExplosionEmitter, comp::Speaker>();
              for (auto entity : view_explosions)
              {
                     auto &comp_emit = view_explosions.get<comp::ExplosionEmitter>(entity);
                     auto &comp_speaker = view_explosions.get<comp::Speaker>(entity);
                     if (update_particles_ee(stage, comp_emit))
                     {
                            comp_speaker.sound.stop();
                            comp_speaker.sound.destroy();
                            grx::engine_3d::destroy_particle_system(comp_emit.particle_system);
                            stage.reg.destroy(entity);
                     }
                     else
                     {
                            update_draw_ee(comp_emit);
                     }
              }
       }

       void procsys_smokes(Stage &stage)
       {
              auto view_smokes = stage.reg.view<comp::SmokeEmitter, comp::Object>();
              for (auto entity : view_smokes)
              {
                     auto &comp_emit = view_smokes.get<comp::SmokeEmitter>(entity);
                     auto &comp_obj = view_smokes.get<comp::Object>(entity);
                     comp_emit.position = comp_obj.position;
                     update_particles_se(stage, comp_emit);
                     update_draw_se(comp_emit);
              }
       }

       void procsys_garbage(Stage &stage)
       {
              auto view_obj = stage.reg.view<comp::Object>();
              auto view_phbodies = stage.reg.view<comp::PhBody>();
              for (auto entity : view_obj)
              {
                     auto &comp_obj = view_obj.get<comp::Object>(entity);
                     if (comp_obj.position.x > WIN_RIGHT * 1.7f || comp_obj.position.x < WIN_LEFT * 1.7f || comp_obj.position.z > -WIN_BOTTOM * 1.7f || comp_obj.position.z < -WIN_TOP * 1.7f)
                     {
                            destroy_comp_plane(stage, entity);
                            destroy_comp_phbody(stage, entity);
                            destroy_comp_smoke(stage, entity);
                            grx::engine_3d::free_object_data(comp_obj.batch, comp_obj.object_data);
                            stage.reg.destroy(entity);
                     }
              }
       }

       void process_systems(Stage &stage)
       {
              if (WINDOW.is_pressed(dsk::KEY_BACKSPACE) && !stage.player_2_present)
              {
                     add_player_2(stage, glm::vec3(0, 0, 0), glm::vec3(0.4f, 0, 0.4f), 0.1f, 0.5f, 5000, 200, 1, 100);
                     stage.player_2_present = true;
              }
              procsys_players(stage);
              procsys_enemyais(stage);
              procsys_planes(stage);
              procsys_objects(stage);
              procsys_bodies(stage);
              procsys_collisions(stage);
              procsys_deaths(stage);
              procsys_explosions(stage);
              procsys_smokes(stage);
              procsys_garbage(stage);
       }

       void procev_summon_plane(Stage &stage, const sch::Event &event)
       {
              spdlog::info("ev : summon plane");
              entt::entity summon = stage.reg.create();
              add_comp_plane(
                  stage, summon, event.data.summon_plane.faction, event.data.summon_plane.position, event.data.summon_plane.move, event.data.summon_plane.speed,
                  event.data.summon_plane.gun_rate, event.data.summon_plane.gun_power, event.data.summon_plane.ai_kamikaze, event.data.summon_plane.life, event.data.summon_plane.damages,
                  event.data.summon_plane.bullet_life, event.data.summon_plane.bullet_damages);
       }

       void procev_summon_bullet(Stage &stage, const sch::Event &event)
       {
              //spdlog::info("ev : summon bullet");
              //entt::entity summon = stage.reg.create();
              //add_comp_bullet(stage, summon, event.data.summon_bullet.position, event.data.summon_bullet.velocity, 1, 100);
       }

       void process_events(Stage &stage, const std::vector<sch::Event> &events)
       {
              for (const sch::Event &event : events)
              {
                     switch (event.event_type)
                     {
                     case sch::EV_NOTHING:
                            spdlog::info("ev : nothing");
                            break;
                     case sch::EV_CHECKPOINT:
                            spdlog::info("ev : checkpoint");
                            break;
                     case sch::EV_SUMMON_PLANE:
                            procev_summon_plane(stage, event);
                            break;
                     case sch::EV_SUMMON_BULLET:
                            procev_summon_bullet(stage, event);
                            break;
                     default:
                            throw std::runtime_error("event type is not valid");
                     }
              }
       }

       void load_resources_stage(Stage &stage, const sch::MapConfiguration &map_config)
       {
              stage.map_config = map_config;
              /*GRAPHICS*/
              ast::Model model;
              std::array<ast::ModelOpt, 2> opts = {ast::MDL_OPT_TEXCOORDS, ast::MDL_OPT_NORMALS};
              ast::load_model(model, "./data/models/basic-plane/model.obj", opts.size(), opts.data());
              stage.res.plane.mesh = grx::engine_3d::create_mesh(model.vertices.data(), model.indices.data(), model.vertices.size() * sizeof(float), model.indices.size() * sizeof(uint32_t), model.indices.size());
              {
                     ast::Image image = ast::load_image("./data/models/basic-plane/texture-enemy.png", ast::IMG_CHL_RGB_ALPHA);
                     stage.res.plane.r_enemy.texture = grx::engine_3d::create_texture(image.width, image.height, image.pixels);
                     ast::free_image(image);
                     stage.res.plane.r_enemy.material = grx::engine_3d::create_material(stage.res.plane.r_enemy.texture);
                     stage.res.plane.r_enemy.batch = grx::engine_3d::create_batch(stage.res.plane.mesh, stage.res.plane.r_enemy.material);
              }
              {
                     ast::Image image = ast::load_image("./data/models/basic-plane/texture-friend.png", ast::IMG_CHL_RGB_ALPHA);
                     stage.res.plane.r_friend.texture = grx::engine_3d::create_texture(image.width, image.height, image.pixels);
                     ast::free_image(image);
                     stage.res.plane.r_friend.material = grx::engine_3d::create_material(stage.res.plane.r_friend.texture);
                     stage.res.plane.r_friend.batch = grx::engine_3d::create_batch(stage.res.plane.mesh, stage.res.plane.r_friend.material);
              }
              {
                     ast::Image image = ast::load_image("./data/models/basic-plane/texture-player-1.png", ast::IMG_CHL_RGB_ALPHA);
                     stage.res.plane.r_player_1.texture = grx::engine_3d::create_texture(image.width, image.height, image.pixels);
                     ast::free_image(image);
                     stage.res.plane.r_player_1.material = grx::engine_3d::create_material(stage.res.plane.r_player_1.texture);
                     stage.res.plane.r_player_1.batch = grx::engine_3d::create_batch(stage.res.plane.mesh, stage.res.plane.r_player_1.material);
              }
              {
                     ast::Image image = ast::load_image("./data/models/basic-plane/texture-player-2.png", ast::IMG_CHL_RGB_ALPHA);
                     stage.res.plane.r_player_2.texture = grx::engine_3d::create_texture(image.width, image.height, image.pixels);
                     ast::free_image(image);
                     stage.res.plane.r_player_2.material = grx::engine_3d::create_material(stage.res.plane.r_player_2.texture);
                     stage.res.plane.r_player_2.batch = grx::engine_3d::create_batch(stage.res.plane.mesh, stage.res.plane.r_player_2.material);
              }
              {
                     ast::load_model(model, "./data/models/bullet/model.obj", opts.size(), opts.data());
                     stage.res.bullet.mesh = grx::engine_3d::create_mesh(model.vertices.data(), model.indices.data(), model.vertices.size() * sizeof(float), model.indices.size() * sizeof(uint32_t), model.indices.size());
                     ast::Image image = ast::load_image("./data/models/bullet/texture.png", ast::IMG_CHL_RGB_ALPHA);
                     stage.res.bullet.texture = grx::engine_3d::create_texture(image.width, image.height, image.pixels);
                     ast::free_image(image);
                     stage.res.bullet.material = grx::engine_3d::create_material(stage.res.bullet.texture);
                     stage.res.bullet.batch = grx::engine_3d::create_batch(stage.res.bullet.mesh, stage.res.bullet.material);
              }
              {
                     ast::Image image = ast::load_image("./data/particles/explosion.png", ast::IMG_CHL_RGB_ALPHA);
                     stage.res.particle_textures.explosion = grx::engine_3d::create_texture(image.width, image.height, image.pixels);
                     ast::free_image(image);
              }
              {
                     ast::Image image = ast::load_image("./data/particles/smoke.png", ast::IMG_CHL_RGB_ALPHA);
                     stage.res.particle_textures.smoke = grx::engine_3d::create_texture(image.width, image.height, image.pixels);
                     ast::free_image(image);
              }
              glm::vec4 sun_dir = {1.0f, 0.7f, 0.4f, 1.0f};
              glm::vec4 sun_color = {1.0f, 1.0f, 1.0f, 1.0f};
              glm::vec4 ambient = {0.5f, 0.5f, 0.5f, 1.0f};
              grx::engine_3d::update_scene_sun_direction(&sun_dir.x);
              grx::engine_3d::update_scene_sun_color(&sun_color.x);
              grx::engine_3d::update_scene_ambient(&ambient.x);
              /*PHYSICS*/
              stage.ph_world = PHCMN.createPhysicsWorld();
              stage.res.bullet.shape = PHCMN.createSphereShape(0.3f);
              stage.res.bullet.shape_transform = rp3d::Transform::identity();
              stage.res.plane.wings_shape = PHCMN.createBoxShape(rp3d::Vector3(2.0f, 0.1f, 0.25f));
              stage.res.plane.body_shape = PHCMN.createBoxShape(rp3d::Vector3(0.15f, 0.3f, 1.25f));
              stage.res.plane.wings_shape_transform = rp3d::Transform::identity();
              stage.res.plane.wings_shape_transform.setPosition(rp3d::Vector3(0, 0, 0.15f));
              stage.res.plane.body_shape_transform = rp3d::Transform::identity();
              stage.res.plane.body_shape_transform.setPosition(rp3d::Vector3(0, 0, -0.2f));
              /*AUDIO*/
              {
                     ast::Mp3Sound mp3_sound = ast::load_mp3_sound("./data/sounds/pistol-1.mp3");
                     stage.res.samples.gun_bullet_fire.create(mp3_sound.buffer, mp3_sound.size, mp3_sound.channels, mp3_sound.freq);
                     ast::free_mp3_sound(mp3_sound);
              }
              {
                     ast::Mp3Sound mp3_sound = ast::load_mp3_sound("./data/sounds/explosion-1.mp3");
                     stage.res.samples.explosion.create(mp3_sound.buffer, mp3_sound.size, mp3_sound.channels, mp3_sound.freq);
                     ast::free_mp3_sound(mp3_sound);
              }
              {
                     ast::Mp3Sound mp3_sound = ast::load_mp3_sound("./data/sounds/propeller-flight-3.mp3");
                     stage.res.samples.plane_player.create(mp3_sound.buffer, mp3_sound.size, mp3_sound.channels, mp3_sound.freq);
                     ast::free_mp3_sound(mp3_sound);
              }
              {
                     ast::Mp3Sound mp3_sound = ast::load_mp3_sound("./data/sounds/propeller-flight-3.mp3");
                     stage.res.samples.plane_other.create(mp3_sound.buffer, mp3_sound.size, mp3_sound.channels, mp3_sound.freq);
                     ast::free_mp3_sound(mp3_sound);
              }
              /*AUTO ENTITIES*/
              add_player_1(stage, glm::vec3(0, 0, 0), glm::vec3(0.4f, 0, 0.4f), 0.1f, 0.5f, 5000, 200, 1, 100);
       }

       void step_stage(Stage &stage, const sch::Schedule &schedule)
       {
              if (schedule.tasks.empty() || stage.is_ended)
              {
                     stage.is_ended = true;
                     return;
              }

              bool wait = false;
              while (!wait && (stage.task_index < schedule.tasks.size()))
              {
                     const sch::Task &curr_task = schedule.tasks[stage.task_index];
                     switch (curr_task.waiting_type)
                     {
                     case sch::Task::WAIT_STATE:
                     {
                            if (curr_task.waiting.states & stage.states)
                            {
                                   process_events(stage, curr_task.events);
                                   ++stage.task_index;
                            }
                            else
                            {
                                   wait = true;
                            }
                            break;
                     }
                     case sch::Task::WAIT_ELAPSED_TIME:
                     {
                            if (curr_task.waiting.elapsed_time <= stage.elapsed_time)
                            {
                                   stage.elapsed_time = 0.0f;
                                   process_events(stage, curr_task.events);
                                   ++stage.task_index;
                            }
                            else
                            {
                                   stage.elapsed_time += GAME_DELTA;
                                   wait = true;
                            }
                            break;
                     }
                     default:
                            throw std::runtime_error("waiting type is not valid");
                     }
              }
              process_systems(stage);
       }

       void free_resources_stage(Stage &stage)
       {
              /*AUDIO*/
              auto view_gun_bullet = stage.reg.view<comp::GunBullet>();
              for (auto entity : view_gun_bullet)
              {
                     auto &comp_gun_bullet = view_gun_bullet.get<comp::GunBullet>(entity);
                     comp_gun_bullet.sound.stop();
                     comp_gun_bullet.sound.destroy();
              }
              stage.res.samples.explosion.destroy();
              stage.res.samples.gun_bullet_fire.destroy();
              stage.res.samples.plane_other.destroy();
              stage.res.samples.plane_player.destroy();
              /*PHYSICS*/
              auto view_phbodies = stage.reg.view<comp::PhBody>();
              for (auto entity : view_phbodies)
              {
                     auto &comp_phbody = view_phbodies.get<comp::PhBody>(entity);
                     stage.ph_world->destroyCollisionBody(comp_phbody.body);
              }
              PHCMN.destroyBoxShape(stage.res.plane.body_shape);
              PHCMN.destroyBoxShape(stage.res.plane.wings_shape);
              PHCMN.destroySphereShape(stage.res.bullet.shape);
              PHCMN.destroyPhysicsWorld(stage.ph_world);
              /*GRAPHICS*/
              auto view_smokes = stage.reg.view<comp::SmokeEmitter>();
              for (auto entity : view_smokes)
              {
                     auto &comp_emit = view_smokes.get<comp::SmokeEmitter>(entity);
                     grx::engine_3d::destroy_particle_system(comp_emit.particle_system);
              }
              auto view_explosions = stage.reg.view<comp::ExplosionEmitter, comp::Speaker>();
              for (auto entity : view_explosions)
              {
                     auto &comp_emit = view_explosions.get<comp::ExplosionEmitter>(entity);
                     auto &comp_speaker = view_explosions.get<comp::Speaker>(entity);
                     comp_speaker.sound.stop();
                     comp_speaker.sound.destroy();
                     grx::engine_3d::destroy_particle_system(comp_emit.particle_system);
              }
              auto view_plane = stage.reg.view<comp::Plane>();
              for (auto entity : view_plane)
              {
                     auto &comp_plane = view_plane.get<comp::Plane>(entity);
                     comp_plane.sound.stop();
                     comp_plane.sound.destroy();
              }
              auto view_obj = stage.reg.view<comp::Object>();
              for (auto entity : view_obj)
              {
                     auto &comp_obj = view_obj.get<comp::Object>(entity);
                     grx::engine_3d::free_object_data(comp_obj.batch, comp_obj.object_data);
              }
              {
                     grx::engine_3d::destroy_texture(stage.res.particle_textures.explosion);
              }
              {
                     grx::engine_3d::destroy_texture(stage.res.particle_textures.smoke);
              }
              {
                     grx::engine_3d::destroy_batch(stage.res.bullet.batch);
                     grx::engine_3d::destroy_material(stage.res.bullet.material);
                     grx::engine_3d::destroy_texture(stage.res.bullet.texture);
                     grx::engine_3d::destroy_mesh(stage.res.bullet.mesh);
              }
              {
                     grx::engine_3d::destroy_batch(stage.res.plane.r_player_2.batch);
                     grx::engine_3d::destroy_material(stage.res.plane.r_player_2.material);
                     grx::engine_3d::destroy_texture(stage.res.plane.r_player_2.texture);
              }
              {
                     grx::engine_3d::destroy_batch(stage.res.plane.r_player_1.batch);
                     grx::engine_3d::destroy_material(stage.res.plane.r_player_1.material);
                     grx::engine_3d::destroy_texture(stage.res.plane.r_player_1.texture);
              }
              {
                     grx::engine_3d::destroy_batch(stage.res.plane.r_friend.batch);
                     grx::engine_3d::destroy_material(stage.res.plane.r_friend.material);
                     grx::engine_3d::destroy_texture(stage.res.plane.r_friend.texture);
              }
              {
                     grx::engine_3d::destroy_batch(stage.res.plane.r_enemy.batch);
                     grx::engine_3d::destroy_material(stage.res.plane.r_enemy.material);
                     grx::engine_3d::destroy_texture(stage.res.plane.r_enemy.texture);
              }
              grx::engine_3d::destroy_mesh(stage.res.plane.mesh);
       }
}