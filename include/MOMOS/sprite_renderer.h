#ifndef SPRITE_RENDERER_H
#define SPRITE_RENDERER_H 1

#include <MOMOS/sprite.h>
#include <MOMOS/texture.h>
#include <MOMOS/shader.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class SpriteRenderer
{
    public:
        SpriteRenderer(Shader &shader);
        ~SpriteRenderer();

        void DrawSprite(Texture2D &texture, glm::vec2 position, 
            glm::vec2 size = glm::vec2(10, 10), GLfloat rotate = 0.0f, 
            glm::vec3 color = glm::vec3(1.0f));
        
        // Draw sprite with custom UV coordinates (for atlas support)
        void DrawSpriteWithUV(Texture2D &texture, glm::vec2 position,
            glm::vec2 size, glm::vec4 uvCoords, GLfloat rotate = 0.0f,
            glm::vec3 color = glm::vec3(1.0f));
    private:
        Shader shader; 
        GLuint quadVAO;
        GLuint quadVBO;  // Store VBO for dynamic updates

        void initRenderData();
};
#endif