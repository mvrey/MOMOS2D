#include <stdio.h>
#include <string.h>
#define FONTSTASH_IMPLEMENTATION

#include <fontstash/fontstash.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#define GLFONTSTASH_IMPLEMENTATION
#include <fontstash/glfontstash.h>

#include <MOMOS/draw.h>
#include <MOMOS/momos.h>



namespace MOMOS {


	FONScontext* fs = NULL;
	int font;

	void DrawBegin() {
		glPointSize(10);
		glLineWidth(2.5);
	}


	void DrawEnd() {
		glFlush();
	}


	void DrawSetStrokeColor(unsigned char R, unsigned char G, unsigned char B, unsigned char Alpha) {
		const float norm = 1.0f / 255.0f;
		glColor4f(R * norm, G * norm, B * norm, Alpha * norm);
	}


	void DrawSetFillColor(unsigned char R, unsigned char G, unsigned char B, unsigned char Alpha) {
		const float norm = 1.0f / 255.0f;
		glColor4f(R * norm, G * norm, B * norm, Alpha * norm);
		unsigned int color = glfonsRGBA(R, G, B, Alpha);
		fonsSetColor(fs, color);
	}


	void DrawClear(unsigned char R, unsigned char G, unsigned char B, unsigned char Alpha) {
		const float norm = 1.0f / 255.0f;
		glClearColor(R * norm, G * norm, B * norm, Alpha * norm);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}


	void DrawLine(float x1, float y1, float x2, float y2) {
		glBegin(GL_LINES);
		glVertex2f(x1, win_height - y1);
		glVertex2f(x2, win_height - y2);
		glEnd();
	}


	void DrawPath(float *pairs_of_points, int num_points) {
		glBegin(GL_LINES);
		for (int i = 0; i < num_points*2 - 2; i+=2) {
			glVertex2f(pairs_of_points[i], win_height - pairs_of_points[i + 1]);
			glVertex2f(pairs_of_points[i + 2], win_height - pairs_of_points[i + 3]);
		}
		glEnd();
	}


	void DrawSolidPath(float *pairs_of_points, int num_points, bool stroke) {
		glBegin(GL_POLYGON);
		for (int i = 0; i < num_points; i++) {
			glVertex2f(pairs_of_points[i*2], win_height - pairs_of_points[i*2+1]);
		}
		glEnd();
	}


	void DrawSetTextFont(const char *name) {
		glfonsDelete(fs);
		fs = glfonsCreate(512, 512, FONS_ZERO_TOPLEFT);
		if (fs == NULL) {
			printf("Could not create stash.\n");
		}

		font = FONS_INVALID;
		font = fonsAddFont(fs, "sans", name);
		if (font == FONS_INVALID) {
			printf("Could not add font normal.\n");
		}
		fonsSetFont(fs, font);
	}


	void DrawSetTextSize(float size) {
		fonsSetSize(fs, size);
	}


	void DrawSetTextBlur(float blur_radius) {
		fonsSetBlur(fs, blur_radius);
	}


	void DrawText(float x, float y, const char *text) {

		// Update and render
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_TEXTURE_2D);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0, win_width, win_height, 0, -1, 1);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glDisable(GL_DEPTH_TEST);
		//glEnable(GL_CULL_FACE);

		fonsDrawText(fs, x, y, text, NULL);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0, MOMOS::win_width, 0, MOMOS::win_height, 0, 1);
	}

}