#include <MOMOS/sprite_renderer.h>
#include <GL/glew.h>
#include <algorithm>

SpriteRenderer::SpriteRenderer(Shader &shader)
{
	this->shader = shader;
	this->batching_enabled_ = false;
	this->quadVAO = 0;
	this->quadVBO = 0;
	this->batchVAO = 0;
	this->batchVBO = 0;
	this->instanceVBO = 0;
	this->initRenderData();
	this->initBatchRenderData();
}

SpriteRenderer::~SpriteRenderer()
{
	if (this->quadVAO != 0) {
		glDeleteVertexArrays(1, &this->quadVAO);
	}
	if (this->quadVBO != 0) {
		glDeleteBuffers(1, &this->quadVBO);
	}
	if (this->batchVAO != 0) {
		glDeleteVertexArrays(1, &this->batchVAO);
	}
	if (this->batchVBO != 0) {
		glDeleteBuffers(1, &this->batchVBO);
	}
	if (this->instanceVBO != 0) {
		glDeleteBuffers(1, &this->instanceVBO);
	}
}

void SpriteRenderer::DrawSprite(Texture2D &texture, glm::vec2 position, glm::vec2 size, GLfloat rotate, glm::vec3 color)
{
	glm::vec4 defaultUV(0.0f, 0.0f, 1.0f, 1.0f);
	
	if (batching_enabled_) {
		AddSpriteToBatch(texture, position, size, defaultUV, rotate, color);
	} else {
		renderImmediate(texture, position, size, rotate, color, defaultUV);
	}
}

void SpriteRenderer::DrawSpriteWithUV(Texture2D &texture, glm::vec2 position,
    glm::vec2 size, glm::vec4 uvCoords, GLfloat rotate, glm::vec3 color)
{
	if (batching_enabled_) {
		AddSpriteToBatch(texture, position, size, uvCoords, rotate, color);
	} else {
		renderImmediate(texture, position, size, rotate, color, uvCoords);
	}
}

void SpriteRenderer::BeginBatch()
{
	batching_enabled_ = true;
	sprite_batches_.clear();
}

void SpriteRenderer::EndBatch()
{
	Flush();
	batching_enabled_ = false;
}

void SpriteRenderer::Flush()
{
	if (!batching_enabled_ || sprite_batches_.empty()) {
		return;
	}
	
	this->shader.Use();
	
	// Render each texture batch
	for (auto& batch : sprite_batches_) {
		renderBatch(batch.first, batch.second);
	}
	
	sprite_batches_.clear();
}

void SpriteRenderer::AddSpriteToBatch(Texture2D &texture, glm::vec2 position,
    glm::vec2 size, glm::vec4 uvCoords, GLfloat rotate, glm::vec3 color)
{
	if (!batching_enabled_) {
		// If batching not enabled, render immediately
		renderImmediate(texture, position, size, rotate, color, uvCoords);
		return;
	}
	
	GLuint textureID = texture.ID;
	
	// Check if we need to flush due to batch size limit
	if (sprite_batches_.find(textureID) != sprite_batches_.end()) {
		if (sprite_batches_[textureID].size() >= MAX_BATCH_SIZE) {
			// Flush this texture's batch
			renderBatch(textureID, sprite_batches_[textureID]);
			sprite_batches_[textureID].clear();
		}
	}
	
	// Add sprite to batch
	SpriteBatchData spriteData;
	spriteData.position = position;
	spriteData.size = size;
	spriteData.rotate = rotate;
	spriteData.color = color;
	spriteData.uvCoords = uvCoords;
	spriteData.textureID = textureID;
	
	sprite_batches_[textureID].push_back(spriteData);
}

void SpriteRenderer::renderImmediate(Texture2D &texture, glm::vec2 position, glm::vec2 size,
                           GLfloat rotate, glm::vec3 color, glm::vec4 uvCoords)
{
	// Prepare transformations
	this->shader.Use();
	this->shader.SetInteger("useInstancing", 0); // Use legacy path
	
	glm::mat4 model;
	model = glm::translate(model, glm::vec3(position, 0.0f));

	model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f));
	model = glm::rotate(model, rotate, glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));

	model = glm::scale(model, glm::vec3(size, 1.0f));

	this->shader.SetMatrix4("model", model);
	this->shader.SetVector3f("spriteColor", color);

	// Update VBO with UV coordinates
	GLfloat vertices[] = {
		// Pos      // Tex
		0.0f, 1.0f, uvCoords.x, uvCoords.w,  // Top-left
		1.0f, 0.0f, uvCoords.z, uvCoords.y,  // Bottom-right
		0.0f, 0.0f, uvCoords.x, uvCoords.y,  // Bottom-left

		0.0f, 1.0f, uvCoords.x, uvCoords.w,  // Top-left
		1.0f, 1.0f, uvCoords.z, uvCoords.w,  // Top-right
		1.0f, 0.0f, uvCoords.z, uvCoords.y   // Bottom-right
	};

	glBindBuffer(GL_ARRAY_BUFFER, this->quadVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

	glActiveTexture(GL_TEXTURE0);
	texture.Bind();

	glBindVertexArray(this->quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Set back default program
	glUseProgram(0);
}

void SpriteRenderer::renderBatch(GLuint textureID, const std::vector<SpriteBatchData> &sprites)
{
	if (sprites.empty()) {
		return;
	}
	
	this->shader.Use();
	this->shader.SetInteger("useInstancing", 1); // Use instanced rendering
	
	// Bind texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);
	
	// Base quad vertices (in local space: 0,0 to 1,1)
	// These are the same for all sprites
	GLfloat baseVertices[] = {
		// Pos      // Tex (used as interpolation factor)
		0.0f, 1.0f, 0.0f, 1.0f,  // Top-left
		1.0f, 0.0f, 1.0f, 0.0f,  // Bottom-right
		0.0f, 0.0f, 0.0f, 0.0f,  // Bottom-left
		0.0f, 1.0f, 0.0f, 1.0f,  // Top-left
		1.0f, 1.0f, 1.0f, 1.0f,  // Top-right
		1.0f, 0.0f, 1.0f, 0.0f   // Bottom-right
	};
	
	// Upload base quad vertices
	glBindBuffer(GL_ARRAY_BUFFER, this->quadVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(baseVertices), baseVertices);
	
	// Build instance data
	std::vector<GLfloat> instanceData;
	instanceData.reserve(sprites.size() * 9); // position(2) + size(2) + rotate(1) + uv(4) = 9 floats per instance
	
	for (const auto& sprite : sprites) {
		instanceData.push_back(sprite.position.x);
		instanceData.push_back(sprite.position.y);
		instanceData.push_back(sprite.size.x);
		instanceData.push_back(sprite.size.y);
		instanceData.push_back(sprite.rotate);
		instanceData.push_back(sprite.uvCoords.x); // u_min
		instanceData.push_back(sprite.uvCoords.y); // v_min
		instanceData.push_back(sprite.uvCoords.z); // u_max
		instanceData.push_back(sprite.uvCoords.w); // v_max
	}
	
	// Create or update instance VBO
	if (this->instanceVBO == 0) {
		glGenBuffers(1, &this->instanceVBO);
	}
	
	glBindBuffer(GL_ARRAY_BUFFER, this->instanceVBO);
	glBufferData(GL_ARRAY_BUFFER, instanceData.size() * sizeof(GLfloat), instanceData.data(), GL_DYNAMIC_DRAW);
	
	// Set up instanced attributes
	glBindVertexArray(this->batchVAO);
	
	// Base quad attributes (location 0)
	glBindBuffer(GL_ARRAY_BUFFER, this->quadVBO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
	
	// Instance attributes
	glBindBuffer(GL_ARRAY_BUFFER, this->instanceVBO);
	
	// Instance position (location 1)
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)0);
	glVertexAttribDivisor(1, 1);
	
	// Instance size (location 2)
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
	glVertexAttribDivisor(2, 1);
	
	// Instance rotation (location 3)
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(4 * sizeof(GLfloat)));
	glVertexAttribDivisor(3, 1);
	
	// Instance UV (location 4)
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(5 * sizeof(GLfloat)));
	glVertexAttribDivisor(4, 1);
	
	// Set color uniform (use first sprite's color)
	// Note: For per-sprite colors, we'd need to add color as an instanced attribute
	if (!sprites.empty()) {
		this->shader.SetVector3f("spriteColor", sprites[0].color);
	}
	
	// Draw all sprites using instanced rendering
	glDrawArraysInstanced(GL_TRIANGLES, 0, 6, static_cast<GLsizei>(sprites.size()));
	
	// Clean up
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	// Disable instanced attributes
	glBindVertexArray(this->batchVAO);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);
	glDisableVertexAttribArray(4);
	glBindVertexArray(0);
	
	//Set back default program
	glUseProgram(0);
}

void SpriteRenderer::initRenderData()
{
	// Configure VAO/VBO for immediate rendering
	GLfloat vertices[] = {
		// Pos      // Tex
		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,

		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f
	};

	glGenVertexArrays(1, &this->quadVAO);
	glGenBuffers(1, &this->quadVBO);

	glBindBuffer(GL_ARRAY_BUFFER, this->quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

	glBindVertexArray(this->quadVAO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void SpriteRenderer::initBatchRenderData()
{
	// Configure VAO for batched rendering (instanced rendering)
	glGenVertexArrays(1, &this->batchVAO);
	
	// The batchVAO will use the same base quad VBO as immediate rendering
	// Instance data will be provided via separate VBO in renderBatch()
	
	// Note: batchVBO is not used in the instanced rendering approach
	// but kept for potential future use
	this->batchVBO = 0;
	this->instanceVBO = 0;
}