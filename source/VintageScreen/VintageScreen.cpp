#define _CRT_SECURE_NO_WARNINGS

#include <iostream>

#include <VintageEffectModule\VintageEffectModule.hpp>
#include <MediaModule\MediaModule.hpp>
#include <Misc\PixelData\PixelData.hpp>

int main(int argc, char *argv[]) {
	PixelData pb(768, 576);
	
	MediaModule mm(pb);
	VintageEffectModule vem(pb);
	vem.start();

	return 0;
}

/*
#include <gl/glew.h>
#include <gl/GL.h>
#include <gl/glut.h>

#include <math.h>

#include <MediaModule\MediaModule.hpp>
#include <Misc\PixelData\PixelData.hpp>

const int WIDTH = 768;
const int HEIGHT = 576;

static char *shaderLoadSource(const char *filePath){
	const size_t blockSize = 512;
	FILE *fp;
	char buf[blockSize];
	char *source = NULL;
	size_t tmp, sourceLength = 0;

	// open file
	fp = fopen(filePath, "r");
	if(!fp) {
		fprintf(stderr, "shaderLoadSource(): Unable to open %s for reading\n", filePath);
		return NULL;
	}

	// read the entire file into a string
	while((tmp = fread(buf, 1, blockSize, fp)) > 0) {
		char *newSource = (char *)malloc(sourceLength + tmp + 1);
		if(!newSource) {
			fprintf(stderr, "shaderLoadSource(): malloc failed\n");
			if(source)
				free(source);
			return NULL;
		}

		if(source) {
			memcpy(newSource, source, sourceLength);
			free(source);
		}
		memcpy(newSource + sourceLength, buf, tmp);

		source = newSource;
		sourceLength += tmp;
	}

	// close the file and null terminate the string
	fclose(fp);
	if(source)
		source[sourceLength] = '\0';

	return source;
}

static GLuint shaderCompileFromFile(GLenum type, const char *filePath)
{
	char *source;
	GLuint shader;
	GLint length, result;

	// get shader source
	source = shaderLoadSource(filePath);
	if(!source)
		return 0;

	// create shader object, set the source, and compile
	shader = glCreateShader(type);
	length = strlen(source);
	glShaderSource(shader, 1, (const char **)&source, &length);
	glCompileShader(shader);
	free(source);

	// make sure the compilation was successful
	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
	if(result == GL_FALSE) {
		char *log;

		// get the shader info log
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
		log = (char *)malloc(length);
		glGetShaderInfoLog(shader, length, &result, log);

		// print an error message and the info log
		fprintf(stderr, "shaderCompileFromFile(): Unable to compile %s: %s\n", filePath, log);
		free(log);

		glDeleteShader(shader);
		return 0;
	}

	return shader;
}

void shaderAttachFromFile(GLuint program, GLenum type, const char *filePath){
	// compile the shader
	GLuint shader = shaderCompileFromFile(type, filePath);
	if(shader != 0) {
		// attach the shader to the program
		glAttachShader(program, shader);

		//delete the shader - it won't actually be
		//destroyed until the program that it's attached
		//to has been destroyed
		glDeleteShader(shader);
	}
}

static GLuint g_program;
static GLuint g_texture;
static GLuint g_time;

void sceneInit(void)
{
	GLint result;

	// create program object and attach shaders
	g_program = glCreateProgram();
	//shaderAttachFromFile(g_program, GL_VERTEX_SHADER, "shader.vp");
	shaderAttachFromFile(g_program, GL_FRAGMENT_SHADER, "shader.fp");

	// link the program and make sure that there were no errors
	glLinkProgram(g_program);
	glGetProgramiv(g_program, GL_LINK_STATUS, &result);
	if(result == GL_FALSE) {
		GLint length;
		char *log;

		// get the program info log
		glGetProgramiv(g_program, GL_INFO_LOG_LENGTH, &length);
		log = (char *)malloc(length);
		glGetProgramInfoLog(g_program, length, &result, log);

		// print an error message and the info log
		fprintf(stderr, "sceneInit(): Program linking failed: %s\n", log);
		free(log);

		// delete the program
		glDeleteProgram(g_program);
		g_program = 0;
	}

	g_texture = glGetUniformLocation(g_program, "texture");
	g_time = glGetUniformLocation(g_program, "time");

}

void init() {
	//
	// TEXTURE
	// 
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	
	PixelData pb(WIDTH, HEIGHT);
	MediaModule mm(pb);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, pb.width, pb.height, 0, GL_RGB, GL_FLOAT, static_cast<GLvoid*>(&(pb[0][0])));
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

	//
	// SHADER
	// 
	glewInit();

	printf ("Vendor: %s\n", glGetString (GL_VENDOR));
	printf ("Renderer: %s\n", glGetString (GL_RENDERER));
	printf ("Version: %s\n", glGetString (GL_VERSION));
	printf ("GLSL: %s\n", glGetString (GL_SHADING_LANGUAGE_VERSION));

	sceneInit();

}

void drawQuad() {
	float zoom = 0.9f;

	glBegin(GL_QUADS); {
		glNormal3f(0, 0, 1);

		glTexCoord2f(0, 1);
		glVertex2f(-zoom, zoom);

		glTexCoord2f(1, 1);
		glVertex2f( zoom,  zoom);

		glTexCoord2f(1, 0);
		glVertex2f( zoom, -zoom);

		glTexCoord2f(0, 0);
		glVertex2f(-zoom, -zoom);
	} glEnd();
}

void display() {
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(g_program);
	glUniform1i(g_texture, 0);
	glUniform1f(g_time, (float)glutGet(GLUT_ELAPSED_TIME)/1000.f);
	
	glEnable(GL_TEXTURE_2D);
	drawQuad();
	glDisable(GL_TEXTURE_2D);

	glUseProgram(0);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	drawQuad();
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL);


	glutSwapBuffers();
}

void idle() {
	glutPostRedisplay();
}

int main(int argc, char *argv[]) {
	glutInit(&argc, argv);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutInitWindowPosition(100, 100);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow("Waving flag");

	init();

	glutDisplayFunc(display);
	glutIdleFunc(idle);

	glutMainLoop();
	return 0;
}*/
