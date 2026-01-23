#ifndef MACROHEAD_H_
#define MACROHEAD_H_

#define GLM_ENABLE_EXPERIMENTAL

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/hash.hpp"

#ifndef SDelete
#define SDelete(ptr)                                                           \
    do {                                                                       \
        if (nullptr != ptr) {                                                  \
            delete ptr;                                                        \
            ptr = nullptr;                                                     \
        }                                                                      \
    } while (0)
#endif

#ifndef SDeleteArray
#define SDeleteArray(ptr)                                                      \
    do {                                                                       \
        if (nullptr != ptr) {                                                  \
            delete[] ptr;                                                      \
            ptr = nullptr;                                                     \
        }                                                                      \
    } while (0)
#endif

typedef glm::vec2 PTF_2D;
typedef glm::vec3 PTF_3D;
typedef glm::vec4 PTF_4D;

typedef glm::ivec2 PTI_2D;
typedef glm::ivec3 PTI_3D;
typedef glm::ivec4 PTI_4D;

typedef glm::mat2 MAT_2;
typedef glm::mat3 MAT_3;
typedef glm::mat4 MAT_4;

#endif // !MACROHEAD_H_
