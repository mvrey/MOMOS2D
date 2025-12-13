#ifndef SPRITE_RENDERER_H
#define SPRITE_RENDERER_H 1

#include <MOMOS/sprite.h>
#include <MOMOS/texture.h>
#include <MOMOS/shader.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <map>

// Structure to hold sprite data for batching
struct SpriteBatchData {
    glm::vec2 position;
    glm::vec2 size;
    GLfloat rotate;
    glm::vec3 color;
    glm::vec4 uvCoords;  // (u_min, v_min, u_max, v_max)
    GLuint textureID;
    
    SpriteBatchData() : position(0.0f), size(1.0f), rotate(0.0f), 
                        color(1.0f), uvCoords(0.0f, 0.0f, 1.0f, 1.0f), textureID(0) {}
};

class SpriteRenderer
{
    public:
        SpriteRenderer(Shader &shader);
        ~SpriteRenderer();

        // Immediate rendering (for backward compatibility, auto-flushes if batching is active)
        void DrawSprite(Texture2D &texture, glm::vec2 position, 
            glm::vec2 size = glm::vec2(10, 10), GLfloat rotate = 0.0f, 
            glm::vec3 color = glm::vec3(1.0f));
        
        // Draw sprite with custom UV coordinates (for atlas support)
        void DrawSpriteWithUV(Texture2D &texture, glm::vec2 position,
            glm::vec2 size, glm::vec4 uvCoords, GLfloat rotate = 0.0f,
            glm::vec3 color = glm::vec3(1.0f));
        
        // Batching API
        void BeginBatch();
        void EndBatch();
        void Flush();
        
        // Add sprite to batch (doesn't render immediately)
        void AddSpriteToBatch(Texture2D &texture, glm::vec2 position,
            glm::vec2 size, glm::vec4 uvCoords, GLfloat rotate = 0.0f,
            glm::vec3 color = glm::vec3(1.0f));
        
        // Check if batching is enabled
        bool IsBatching() const { return batching_enabled_; }
        
    private:
        Shader shader; 
        GLuint quadVAO;
        GLuint quadVBO;  // Store VBO for dynamic updates
        GLuint batchVBO; // Large VBO for batched rendering (currently unused)
        GLuint batchVAO; // VAO for batched rendering
        GLuint instanceVBO; // VBO for instance data
        
        // Batching state
        bool batching_enabled_;
        std::map<GLuint, std::vector<SpriteBatchData>> sprite_batches_; // Grouped by texture ID
        
        // Maximum batch size (can be adjusted)
        static const size_t MAX_BATCH_SIZE = 10000; // Max sprites per batch

        void initRenderData();
        void initBatchRenderData();
        void cleanupBatchRenderData();
        void renderBatch(GLuint textureID, const std::vector<SpriteBatchData> &sprites);
        void renderImmediate(Texture2D &texture, glm::vec2 position, glm::vec2 size,
                           GLfloat rotate, glm::vec3 color, glm::vec4 uvCoords);
};
#endif