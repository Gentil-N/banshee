#include "binding.hpp"

namespace grx
{
       Context _c;

       namespace engine_3d
       {
              Engine _e;

              float _quad_vertices[16] =
                  {-1.0f, -1.0f, 0.0f, 0.0f,
                   1.0f, -1.0f, 1.0f, 0.0f,
                   1.0f, 1.0f, 1.0f, 1.0f,
                   -1.0f, 1.0f, 0.0f, 1.0f};
              uint32_t _quad_indices[6] = {0, 1, 2, 2, 3, 0};
              size_t _quad_vertex_size = sizeof(float) * 16;
              size_t _quad_index_size = sizeof(uint32_t) * 6;
              uint32_t _quad_index_count = 6;
       }
}