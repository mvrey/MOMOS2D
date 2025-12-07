#version 330 core
layout (location = 0) in vec4 vertex; // <vec2 position, vec2 texCoords>

// Instanced attributes for batching (only used when useInstancing is true)
layout (location = 1) in vec2 instancePosition;
layout (location = 2) in vec2 instanceSize;
layout (location = 3) in float instanceRotate;
layout (location = 4) in vec4 instanceUV;

out vec2 TexCoords;

uniform mat4 projection;
uniform mat4 model; // Used for legacy rendering
uniform int useInstancing; // 1 = instanced rendering, 0 = legacy (backward compatibility)

void main()
{
    if (useInstancing == 1) {
        // Instanced rendering path for batching
        TexCoords = mix(instanceUV.xy, instanceUV.zw, vertex.zw);
        
        // Transform vertex position with instance data
        // Match the immediate rendering transformation:
        // 1. Scale first (vertex.xy is 0-1, scale by instanceSize)
        vec2 pos = vertex.xy * instanceSize;
        // 2. Translate to center for rotation
        pos -= instanceSize * 0.5;
        // 3. Rotate around center
        float cosR = cos(instanceRotate);
        float sinR = sin(instanceRotate);
        vec2 rotatedPos = vec2(
            pos.x * cosR - pos.y * sinR,
            pos.x * sinR + pos.y * cosR
        );
        // 4. Translate back and add position
        rotatedPos += instancePosition + instanceSize * 0.5;
        
        gl_Position = projection * vec4(rotatedPos, 0.0, 1.0);
    } else {
        // Legacy path (for backward compatibility)
        TexCoords = vertex.zw;
        gl_Position = projection * model * vec4(vertex.xy, 0.0, 1.0);
    }
}