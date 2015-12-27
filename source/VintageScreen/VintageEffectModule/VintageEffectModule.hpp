#pragma once

#include <Misc\OpenGL\OpenGLBoilerplate.hpp>
#include <Misc\PixelData\PixelData.hpp>

class VintageEffectModule {

private:
	// the image we want to show
	const PixelData &inputBuffer;
	
	// opengl handler (sort of...)
	OpenGLBoilerplate openglHandle;

	// shader files (TODO: make it inline in file)
	const std::string fragmentFile = R"(D:\BME\vintage screen\source\VintageScreen\VintageEffectModule\Shaders\shader.frag)";
	const std::string vertexFile = R"(D:\BME\vintage screen\source\VintageScreen\VintageEffectModule\Shaders\shader.vert)";

	// shader variables
	GLuint g_program;
	GLuint g_texture;
	GLuint g_contextWidth;
	GLuint g_contextHeight;
	GLuint g_time;

	void drawSurface() {
		auto texturePoint = [](float u, float v) {
			glTexCoord2f(u, v);
			float zoom = 1;
			float flatness = 4;
			float x = (2.f*u - 1.f)*zoom;
			float y = (2.f*v - 1.f)*zoom;

			float ratio = ((2.f*zoom-(1.f/flatness)*powf(fabsf(x), 2)) + (2.f*zoom-(1.f/flatness)*powf(fabsf(y), 2)))/(4.f*zoom);

			glVertex2f(x*ratio, y*ratio);
		};

		glBegin(GL_QUADS); {
			glNormal3f(0, 0, 1);
			float def = 1.f/10.f;
			for (float u = 0; u < 1; u += def){
				for (float v = 0; v < 1; v += def) {
					texturePoint(u, v); 
					texturePoint(u + def, v); 
					texturePoint(u + def, v + def); 
					texturePoint(u, v + def); 
				}
			}
		} glEnd();
	}

	void displayCallback(){

		loadShader();{

			glBegin(GL_QUADS); {
				glColor3f(1,0,0);
				glNormal3f(0, 0, 1);
				glVertex2f(-1, -1);
				glVertex2f( 1, -1);
				glVertex2f( 1,  1);
				glVertex2f(-1,  1);
			} glEnd();

			glEnable(GL_TEXTURE_2D);{
				drawSurface();
			} glDisable(GL_TEXTURE_2D);

		} unloadShader();

		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); {
		//	drawSurface();
		//} glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	void idleCallback() {
		// itt kell implementálni a 60fps-t
	}

	void initCallback() {
		// TEXTURE
		unsigned int texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, inputBuffer.width, inputBuffer.height, 0, GL_RGB, GL_FLOAT, static_cast<const GLvoid*>(&(inputBuffer[0][0])));
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

		// SHADER
		glewInit();

		printf ("Vendor: %s\n", glGetString (GL_VENDOR));
		printf ("Renderer: %s\n", glGetString (GL_RENDERER));
		printf ("Version: %s\n", glGetString (GL_VERSION));
		printf ("GLSL: %s\n", glGetString (GL_SHADING_LANGUAGE_VERSION));

		GLint result;

		// create program object and attach shaders
		g_program = glCreateProgram();
		//shaderAttachFromFile(g_program, GL_VERTEX_SHADER, info.vertexFile.c_str());
		GLSLHelper::shaderAttachFromFile(g_program, GL_FRAGMENT_SHADER, fragmentFile.c_str());

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

		//
		// Shader variables
		//
		g_texture = glGetUniformLocation(g_program, "texture");
		g_time = glGetUniformLocation(g_program, "time");
	}

	void loadShader() {
		glUseProgram(g_program);
		glUniform1i(g_texture, 0);
		glUniform1f(g_time, (float)glutGet(GLUT_ELAPSED_TIME)/100.f);
	}

	void unloadShader() {
		glUseProgram(0);
	}

public:
	VintageEffectModule(const PixelData &tspb): inputBuffer(tspb)
	{
		// create OpenGLInfo
		OpenGLInfo oi;
		oi.windowWidth = inputBuffer.width;
		oi.windowHeight = inputBuffer.height;
		oi.windowTitle = "Test Window";
		oi.drawCallback = std::bind(&VintageEffectModule::displayCallback, this);
		oi.idleCallback = std::bind(&VintageEffectModule::idleCallback, this);
		oi.initCallback = std::bind(&VintageEffectModule::initCallback, this);
		openglHandle.loadGLInfo(oi);
	}

	void start() {
		openglHandle.start();
	}
	
};
