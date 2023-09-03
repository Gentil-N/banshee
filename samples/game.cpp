#include "game.hpp"

#include <chrono>

struct
{
       glm::mat4 projection;
       glm::mat4 view;
} camera;
std::chrono::system_clock::time_point last;
const float _time_per_frame = 1.0f / 60.0f;

namespace bsh
{
       Stage stage;
       sch::Schedule schedule;

       void init()
       {

              dsk::init();
              WINDOW.create(WIN_WIDTH, WIN_HEIGHT, "haha", false);

              grx::InitInfo init_info = {"Sample", 1, 0, 0, (uint32_t)WIN_WIDTH, (uint32_t)WIN_HEIGHT, dsk::get_x11_display(), WINDOW.get_x11_window()};
              grx::init(init_info, true);
              std::vector<char> shadvert = ast::read_file("./data/shaders/main.vert.spv"), shadfrag = ast::read_file("./data/shaders/main.frag.spv"),
                                pshadvert = ast::read_file("./data/shaders/particle.vert.spv"), pshadfrag = ast::read_file("./data/shaders/particle.frag.spv");
              grx::engine_3d::InitInfo eng_init_info =
                  {1000, 50, 50,
                   (uint32_t)shadvert.size(), shadvert.data(), (uint32_t)shadfrag.size(), shadfrag.data(),
                   (uint32_t)pshadvert.size(), pshadvert.data(), (uint32_t)pshadfrag.size(), pshadfrag.data()};
              grx::engine_3d::init(eng_init_info);

              snd::init();

              camera.projection = glm::ortho(WIN_LEFT, WIN_RIGHT, WIN_BOTTOM, WIN_TOP, 0.1f, 100.0f);
              camera.projection[1][1] *= -1.0f;
              camera.view = glm::identity<glm::mat4>();
              camera.view = glm::translate(camera.view, glm::vec3(0.0f, 0.0f, -10.0f));
              camera.view = glm::rotate(camera.view, glm::pi<float>() * 0.5f, glm::vec3(1.0f, 0.0f, 0.0f));
              grx::engine_3d::update_scene_projection(&camera.projection[0]);
              grx::engine_3d::update_scene_view(&camera.view[0]);

              sch::Task task_0 = {};
              task_0.waiting_type = sch::Task::WAIT_ELAPSED_TIME;
              task_0.waiting.elapsed_time = 4.0f;
              sch::Event summon_enemy_1_event = {};
              summon_enemy_1_event.event_type = sch::EV_SUMMON_PLANE;
              summon_enemy_1_event.data.summon_plane.faction = sch::FT_ENEMY;
              summon_enemy_1_event.data.summon_plane.position = glm::vec3(WIN_LEFT / 2.0f, 0, -WIN_TOP * 1.2f);
              summon_enemy_1_event.data.summon_plane.speed = glm::vec3(0.1f, 0, 0.03f);
              summon_enemy_1_event.data.summon_plane.gun_rate = 0.7f;
              summon_enemy_1_event.data.summon_plane.gun_power = 0.1f;
              summon_enemy_1_event.data.summon_plane.ai_kamikaze = false;
              summon_enemy_1_event.data.summon_plane.life = 200;
              summon_enemy_1_event.data.summon_plane.damages = 400;
              summon_enemy_1_event.data.summon_plane.bullet_life = 1;
              summon_enemy_1_event.data.summon_plane.bullet_damages = 100;
              sch::Event summon_enemy_2_event = {};
              summon_enemy_2_event.event_type = sch::EV_SUMMON_PLANE;
              summon_enemy_2_event.data.summon_plane.faction = sch::FT_ENEMY;
              summon_enemy_2_event.data.summon_plane.move = comp::MV_NOTHING;
              summon_enemy_2_event.data.summon_plane.position = glm::vec3(WIN_RIGHT / 2.0f, 0, -WIN_TOP * 1.2f);
              summon_enemy_2_event.data.summon_plane.speed = glm::vec3(0.1f, 0, 0.03f);
              summon_enemy_2_event.data.summon_plane.gun_rate = 1.0f;
              summon_enemy_2_event.data.summon_plane.gun_power = 0.2f;
              summon_enemy_2_event.data.summon_plane.ai_kamikaze = false;
              summon_enemy_2_event.data.summon_plane.life = 200;
              summon_enemy_2_event.data.summon_plane.damages = 400;
              summon_enemy_2_event.data.summon_plane.bullet_life = 1;
              summon_enemy_2_event.data.summon_plane.bullet_damages = 100;
              sch::Event summon_enemy_3_event = {};
              summon_enemy_3_event.event_type = sch::EV_SUMMON_PLANE;
              summon_enemy_3_event.data.summon_plane.faction = sch::FT_ENEMY;
              summon_enemy_3_event.data.summon_plane.position = glm::vec3(0, 0, -WIN_TOP * 1.4f);
              summon_enemy_3_event.data.summon_plane.speed = glm::vec3(0.05f, 0, 0.05f);
              summon_enemy_3_event.data.summon_plane.gun_rate = 0.5f;
              summon_enemy_3_event.data.summon_plane.gun_power = 0.1f;
              summon_enemy_3_event.data.summon_plane.ai_kamikaze = true;
              summon_enemy_3_event.data.summon_plane.life = 200;
              summon_enemy_3_event.data.summon_plane.damages = 400;
              summon_enemy_3_event.data.summon_plane.bullet_life = 1;
              summon_enemy_3_event.data.summon_plane.bullet_damages = 200;

              task_0.events.push_back(summon_enemy_1_event);
              task_0.events.push_back(summon_enemy_2_event);
              task_0.events.push_back(summon_enemy_3_event);

              for (size_t i = 0; i < 100; ++i)
              {
                     schedule.tasks.push_back(task_0);
              }

              sch::MapConfiguration map_config = {sch::DTT_MORNING, 0.04f};
              load_resources_stage(stage, map_config);

              GAME_SHOULD_BE_ENDED = false;
              WINDOW.show();

              last = std::chrono::high_resolution_clock::now();
       }

       void end()
       {
              free_resources_stage(stage);

              snd::end();

              grx::engine_3d::end();
              grx::end();

              WINDOW.destroy();
              dsk::end();
       }

       void loop()
       {
              std::chrono::system_clock::time_point current = std::chrono::high_resolution_clock::now();
              GAME_DELTA = (float)std::chrono::duration_cast<std::chrono::milliseconds>(current - last).count() / 1000.0f;
              GAME_ADVANCE = GAME_DELTA / _time_per_frame;
              last = current;
              dsk::refresh();
              grx::render_next_frame();
              step_stage(stage, schedule);
              GAME_SHOULD_BE_ENDED = WINDOW.should_be_destroyed();
       }

       void run_the_game()
       {
              spdlog::info("initializing...");
              init();
              spdlog::info("running...");
              while (!GAME_SHOULD_BE_ENDED)
              {
                     loop();
              }
              spdlog::info("ending...");
              end();
       }
}

int main()
{
       try
       {
              bsh::run_the_game();
       }
       catch (const std::exception &e)
       {
              spdlog::critical(e.what());
              return 777;
       }
       return 0;
}