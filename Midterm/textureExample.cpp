#include "cg.h"
#include "vboplane.h"
#include "vboteapot.h"
#include "bmpreader.h"

void startup();
void render();
void shutdown();
extern void version();
extern GLuint compile_shaders(const char*  vsource, const char*  fsource);

struct scene {
	const int width = 800; const int height = 600;

	GLuint rendering_program;
	VBOPlane *plane;
	VBOTeapot *teapot;

	mat4 model;
	mat4 view;
	mat4 proj;
	float angle;

	GLint v_loc, m_loc, proj_loc;
	GLuint tex_object[3];
};
scene sc;

void generate_texture(GLubyte * data, int width, int height)
{
	int x, y;
	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			GLubyte c = (((x & 0x8) == 0) ^ ((y & 0x8) == 0))*255.0;
			data[(y * width + x) * 4 + 0] = c;
			data[(y * width + x) * 4 + 1] = c;
			data[(y * width + x) * 4 + 2] = c;
			data[(y * width + x) * 4 + 3] = 1.0f;
		}
	}
}

void startup()
{
	/******* OpenGL Initialization */
	glEnable(GL_DEPTH_TEST); glDepthFunc(GL_LEQUAL); //Passes if the incoming depth value is less than or
	glEnable(GL_SMOOTH);
	/**** Shaders as variables */
	sc.rendering_program = compile_shaders("tex_lighting_v.glsl", "tex_lighting_f.glsl");
	glUseProgram(sc.rendering_program);

	// modeling
	sc.plane = new VBOPlane(80.0f, 80.0f, 20, 20);
	sc.teapot = new VBOTeapot(14, mat4(1.0f));

	// viewing
	sc.angle = 0.957283f;
	sc.model = mat4(1.0);

	sc.view = lookAt(vec3(0.0f, 5.0f, 20.0f), vec3(0.0f, 0.75f, 0.0f), vec3(0.0f, 1.0f, 0.0f));

	float cameraZ = sc.height * 0.5 / tan(radians(25.0f));
	sc.proj = perspective(radians(70.0f), (float)sc.width / (float)sc.height, (float)0.001*cameraZ, (float)10.0*cameraZ);

	sc.m_loc = glGetUniformLocation(sc.rendering_program, "m_matrix");
	sc.v_loc = glGetUniformLocation(sc.rendering_program, "v_matrix");
	sc.proj_loc = glGetUniformLocation(sc.rendering_program, "proj_matrix");
	
	glUniformMatrix4fv(sc.m_loc, 1, GL_FALSE, value_ptr(mat4(1.0)));
	glUniformMatrix4fv(sc.v_loc, 1, GL_FALSE, value_ptr(sc.view));
	glUniformMatrix4fv(sc.proj_loc, 1, GL_TRUE, value_ptr(sc.proj));

	// Lighting
	glUniform3f(glGetUniformLocation(sc.rendering_program, "Light.Intensity"), 0.8, 0.8, 0.8);

	// Load texture file
	glActiveTexture(GL_TEXTURE0);
	sc.tex_object[0] = BMPReader::loadTex("brick1.bmp");
	glActiveTexture(GL_TEXTURE1);
	sc.tex_object[1] = BMPReader::loadTex("diffuse.bmp");

	// create the checkerboard pattern

	// Define some data to upload into the texture
	GLubyte* data = new GLubyte[64 * 64 * 4];
	generate_texture(data, 64, 64);

	glActiveTexture(GL_TEXTURE2);
	// Generate a name for the texture
	glGenTextures(1, &sc.tex_object[2]);
	// Now bind it to the context using the GL_TEXTURE_2D binding point
	glBindTexture(GL_TEXTURE_2D, sc.tex_object[2]);

	// Assume the texture is already bound to the GL_TEXTURE_2D target
	glTexImage2D(GL_TEXTURE_2D,  // 2D texture
		0,              // Level 0
		GL_RGB,          // Offset 0, 0
		64, 64, 0,       // 64 x 64 texels, replace entire image
		GL_RGBA,        // Four channel data
		GL_UNSIGNED_BYTE,       // Floating point data
		data);          // Pointer to data
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	delete[] data;
}

void idle() {
	sc.angle += 0.0001f;
	if (sc.angle > 2.0*pi<float>()) sc.angle -= 2.0*pi<float>();
	glutPostRedisplay();
}

void render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.6f, 0.6f, 0.6f, 1.0f);
	glUseProgram(sc.rendering_program);

	// Lighting
	glUniform4fv(glGetUniformLocation(sc.rendering_program, "Light.Position"),
		1, value_ptr(sc.view*vec4(10.0f*cos(sc.angle), 3.0, 10.0f*sin(sc.angle), 1.0)));

	/********Object  */
	// Teapot Material & texture
	glUniform3f(glGetUniformLocation(sc.rendering_program, "Material.Ka"), 0.9f, 0.5f, 0.3f);
	glUniform3f(glGetUniformLocation(sc.rendering_program, "Material.Kd"), 0.9f, 0.96f, 0.95f);
	glUniform3f(glGetUniformLocation(sc.rendering_program, "Material.Ks"), 0.1f, 0.1f, 0.1f);
	glUniform1f(glGetUniformLocation(sc.rendering_program, "Material.Shininess"), 100.0f);
	glUniform1i(glGetUniformLocation(sc.rendering_program, "Tex1"), 0);

	// Teapot Matrix
	sc.model = rotate(mat4(1.0f), (float)radians(-90.0), vec3(1.0, 0.0, 0.0));
	glUniformMatrix4fv(sc.m_loc, 1, GL_FALSE, value_ptr(sc.model));
	sc.teapot->render();

	// Teapot Texture & Matrix, (same material)
	glUniform1i(glGetUniformLocation(sc.rendering_program, "Tex1"), 1);
	sc.model = translate(mat4(1.0), vec3(6.0, 0.0, 0.0));
	sc.model = rotate(sc.model, (float)radians(-90.0), vec3(1.0, 0.0, 0.0));
	glUniformMatrix4fv(sc.m_loc, 1, GL_FALSE, value_ptr(sc.model));
	sc.teapot->render();

	// Plane 
	glUniform3f(glGetUniformLocation(sc.rendering_program, "Material.Ka"), 0.9f, 0.9f, 0.9f);
	glUniform3f(glGetUniformLocation(sc.rendering_program, "Material.Kd"), 0.7f, 0.7f, 0.7f);
	glUniform3f(glGetUniformLocation(sc.rendering_program, "Material.Ks"), 0.1f, 0.1f, 0.1f);
	glUniform1f(glGetUniformLocation(sc.rendering_program, "Material.Shininess"), 180.0f);
	glUniform1i(glGetUniformLocation(sc.rendering_program, "Tex1"), 2);
	glUniformMatrix4fv(sc.m_loc, 1, GL_FALSE, value_ptr(mat4(1.0)));
	sc.plane->render();

	glutSwapBuffers();
}

void shutdown()
{
	glDeleteProgram(sc.rendering_program);
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(sc.width, sc.height);
	glutCreateWindow("Phong Shading");
	glewInit();

	version();
	startup();
	glutDisplayFunc(render);
	glutIdleFunc(idle);

	glutMainLoop();
	shutdown();
	return 0;
}
