#include "cg.h"
#include "vboplane.h"
#include "vboteapot.h"
#include "vbocube.h"
#include "vbosphere.h"
#include "vbotorus.h"
#include "vbotrianglebar.h"
#include "vbomesh.h"

void startup();
void render();
void shutdown();
extern void version();

float myMaterial[20][4][3] = {
	//00 1:jade
	0.135,	0.2225,	0.1575,
	0.54,		0.89,		0.63,
	0.316228,	0.316228,	0.316228,
	12.8,		0,			0,
	//01 7:bronze
	0.2125,		0.1275,		0.054,
	0.714,		0.4284,		0.18144,
	0.393548,	0.271906,	0.166721,
	25.6,		0,			0,
	//02 13:Plastic(Cyan)
	0.0,	0.1,	0.06,
	0.0,	0.50980392,	0.50980392,
	0.50196078,	0.50196078,	0.50196078,
	32.0,	0,	0,
	//03	cyan rubber
	0.0,	0.05,	0.05,
	0.4,	0.5,	0.5,
	0.04,	0.7,	0.7,
	0.078125 * 128,	0,	0,
	//04	Obsidian
	0.05375,	0.05,	0.06625,
	0.18275,	0.17,	0.22525,
	0.332741,	0.328634,	0.346435,
	0.3 * 128,	0,	0,
	//05	silver
	0.19225,	0.19225,	0.19225,
	0.50754,	0.50754,	0.50754,
	0.508273,	0.508273,	0.508273,
	0.4 * 128,	0,	0,
	//06	green plastic
	0.0,	0.0,	0.0,
	0.1,	0.35,	0.1,
	0.45,	0.55,	0.45,
	0.25 * 128,	0,	0,
	//07	green rubber
	0.0,	0.05,	0.0,
	0.4,	0.5,	0.4,
	0.04,	0.7,	0.04,
	0.078125 * 128,	0,	0,
	//08	Pearl
	0.25,	0.20725,	0.20725,
	1,	0.829,	0.829,
	0.296648,	0.296648,	0.296648,
	0.088 * 128,	0,	0,
	//09	Copper
	0.19125,	0.0735,		0.0225,
	0.7038,		0.27048,	0.0828,
	0.256777,	0.137622,	0.086014,
	0.1 * 128,	0,	0,
	//10	red plastic
	0.0,	0.0,	0.0,
	0.5,	0.0,	0.0,
	0.7,	0.6,	0.6,
	0.25 * 128,	0,	0,
	//11	red rubber	
	0.05,	0.0,	0.0,
	0.5,	0.4,	0.4,
	0.7,	0.04,	0.04,
	0.078125 * 128,	0,	0,
	//12	Ruby
	0.1745,		0.01175,	0.01175,
	0.61424,	0.04136,	0.04136,
	0.727811,	0.626959,	0.626959,
	0.6 * 128,	0,	0,
	//13	Gold
	0.24725,	0.1995,		0.0745,
	0.75164,	0.60648,	0.22648,
	0.628281,	0.555802,	0.366065,
	0.4 * 128,	0,	0,
	//14	white plastic
	0.0,	0.0,	0.0,
	0.55,	0.55,	0.55,
	0.70,	0.70,	0.70,
	0.25 * 128,	0,	0,
	//15	white rubber
	0.05,	0.05,	0.05,
	0.5,	0.5,	0.5,
	0.7,	0.7,	0.7,
	0.078125 * 128,	0,	0,
	//16	Turquoise
	0.1,	0.18725,	0.1745,
	0.396,	0.74151,	0.69102,
	0.297254,	0.30829,	0.306678,
	0.1 * 128,	0,	0,
	//17	Silver
	0.19225,	0.19225,	0.19225,
	0.50754,	0.50754,	0.50754,
	0.508273,	0.508273,	0.508273,
	0.4 * 128,	0,	0,
	//18	yellow plastic
	0.0,	0.0,	0.0,
	0.5,	0.5,	0.0,
	0.60,	0.60,	0.50,
	0.25 * 128, 0, 0,
	//19	yellow rubber
	0.05,	0.05,	0.0,
	0.5,	0.5,	0.4,
	0.7,	0.7,	0.04,
	0.078125 * 128,	0,	0 
};
struct scene {
	int width = 800, height = 600;
	GLuint rendering_program;
	GLuint vertex_array_object;

	// �𵨸� ����
	VBOCube* cube;
	VBOPlane* plane;
	VBOSphere* sphere;
	VBOTorus* torus;
	VBOTriangleBar* trianglebar;
	VBOTeapot* teapot;
	VBOMesh* mesh;


	// ���� ����
	mat4 model, view, proj;
	GLint m_loc, v_loc, proj_loc, color_loc;
	float angle, val;
};
scene sc;

void startup()
{
	/******* OpenGL Initialization */
	glEnable(GL_DEPTH_TEST); glDepthFunc(GL_LEQUAL); //Passes if the incoming depth value is less than or
	/**** Shaders as variables */
	sc.rendering_program = compile_shaders("perfrag.glsl", "perfragf.glsl");
	glUseProgram(sc.rendering_program);

	// modeling
	sc.cube = new VBOCube();
	sc.sphere = new VBOSphere();
	sc.plane = new VBOPlane(50.0f, 50.0f, 1, 1);
	sc.teapot = new VBOTeapot(14, mat4(1.0f));

	// viewing
	sc.angle = 0.957283f;
	sc.val = 0.0f;
	sc.model = mat4(1.0);

	sc.view = lookAt(vec3(0.0f, 2.0f, 8.0f), vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));

	float cameraZ = sc.height * 0.5 / tan(radians(25.0f));
	sc.proj = perspective(radians(50.0f), (float)sc.width / (float)sc.height, (float)0.001 * cameraZ, (float)10.0 * cameraZ);

	//ortho
	sc.m_loc = glGetUniformLocation(sc.rendering_program, "m_matrix");
	sc.v_loc = glGetUniformLocation(sc.rendering_program, "v_matrix");
	sc.proj_loc = glGetUniformLocation(sc.rendering_program, "proj_matrix");

	glUniformMatrix4fv(sc.m_loc, 1, GL_FALSE, value_ptr(mat4(1.0)));
	glUniformMatrix4fv(sc.v_loc, 1, GL_FALSE, value_ptr(sc.view));
	glUniformMatrix4fv(sc.proj_loc, 1, GL_FALSE, value_ptr(sc.proj));

	// Lighting
	glUniform3f(glGetUniformLocation(sc.rendering_program, "Light.Intensity"), 0.8, 0.8, 0.8);
}

void shade(int k) {
	glUniform3f(glGetUniformLocation(sc.rendering_program, "Material.Ka"), myMaterial[k][0][0], myMaterial[k][0][1], myMaterial[k][0][2]);
	glUniform3f(glGetUniformLocation(sc.rendering_program, "Material.Kd"), myMaterial[k][1][0], myMaterial[k][1][1], myMaterial[k][1][2]);
	glUniform3f(glGetUniformLocation(sc.rendering_program, "Material.Ks"), myMaterial[k][2][0], myMaterial[k][2][1], myMaterial[k][2][2]);
	glUniform1f(glGetUniformLocation(sc.rendering_program, "Material.Shininess"), myMaterial[k][3][0]);
}
float i = 0;
void idle() {
	sc.angle += 0.001f;
	if (sc.angle > 2.0 * 3.141592) sc.angle -= 2.0 * 3.141592;
	
	if(i==0)
		sc.val += 0.001;
	if(i==1)
		sc.val -= 0.001;
	if (sc.val > 5.0)
		i = 1;
	if (sc.val < 0.0)
		i = 0;
	glutPostRedisplay();
}
void render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.6f, 0.6f, 0.6f, 1.0f);

	glUseProgram(sc.rendering_program);

	// View
	sc.view = lookAt(vec3(sc.val, 2.0f+sc.val, 8.0f), vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));

	// Lighting 
	vec4 pos = sc.view * vec4(10.0f * cos(sc.angle), 6.0f, 10.0f * sin(sc.angle), 1.0f);
	glUniform4fv(glGetUniformLocation(sc.rendering_program, "Light.Position"),
		1, value_ptr(pos));


	// Models


	//bed frame
	shade(1);
	mat4 model_base;
	model_base = translate(mat4(1.0), vec3(0.0,0.5,0.0));
	model_base = rotate(model_base, (float)radians(30.0), vec3(0,1, 0));
	model_base = scale(model_base, vec3(5, 0.7, 4));
	glUniformMatrix4fv(sc.m_loc, 1, GL_FALSE, value_ptr(model_base));
	glUniformMatrix4fv(sc.v_loc, 1, GL_FALSE, value_ptr(sc.view));
	sc.cube->render();

	shade(17);
	model_base = translate(mat4(1.0), vec3(0.0, 1.2, 0.0));
	model_base = rotate(model_base, (float)radians(30.0), vec3(0, 1, 0));
	model_base = scale(model_base, vec3(4.8, 0.6, 3.7));
	glUniformMatrix4fv(sc.m_loc, 1, GL_FALSE, value_ptr(model_base));
	glUniformMatrix4fv(sc.v_loc, 1, GL_FALSE, value_ptr(sc.view));
	sc.cube->render();

	shade(1);
	model_base = rotate(mat4(1.0), (float)radians(30.0), vec3(0, 1, 0));
	model_base = translate(model_base, vec3(2.5, 0.5, 0.0));
	model_base = scale(model_base, vec3(0.2, 3.5, 4));
	glUniformMatrix4fv(sc.m_loc, 1, GL_FALSE, value_ptr(model_base));
	glUniformMatrix4fv(sc.v_loc, 1, GL_FALSE, value_ptr(sc.view));
	sc.cube->render();

	shade(3);
	model_base = rotate(mat4(1.0), (float)radians(30.0), vec3(0, 1, 0));
	model_base = translate(model_base, vec3(1.5, 1.5, 0.0));
	model_base = scale(model_base, vec3(1, 0.5, 1.5));
	glUniformMatrix4fv(sc.m_loc, 1, GL_FALSE, value_ptr(model_base));
	glUniformMatrix4fv(sc.v_loc, 1, GL_FALSE, value_ptr(sc.view));
	sc.cube->render();


	//table
	shade(3);
	model_base = translate(mat4(1.0), vec3(2, 0.5, 3));
	model_base = scale(model_base, vec3(0.1, 0.5, 0.1));
	glUniformMatrix4fv(sc.m_loc, 1, GL_FALSE, value_ptr(model_base));
	glUniformMatrix4fv(sc.v_loc, 1, GL_FALSE, value_ptr(sc.view));
	sc.cube->render();
	shade(3);
	model_base = translate(mat4(1.0), vec3(2 - 0.7, 0.5, 3));
	model_base = scale(model_base, vec3(0.1, 0.5, 0.1));
	glUniformMatrix4fv(sc.m_loc, 1, GL_FALSE, value_ptr(model_base));
	glUniformMatrix4fv(sc.v_loc, 1, GL_FALSE, value_ptr(sc.view));
	sc.cube->render();
	shade(3);
	model_base = translate(mat4(1.0), vec3(2, 0.5, 3 + 0.4));
	model_base = scale(model_base, vec3(0.1, 0.5, 0.1));
	glUniformMatrix4fv(sc.m_loc, 1, GL_FALSE, value_ptr(model_base));
	glUniformMatrix4fv(sc.v_loc, 1, GL_FALSE, value_ptr(sc.view));
	sc.cube->render();
	shade(3);
	model_base = translate(mat4(1.0), vec3(2 - 0.7, 0.5, 3 + 0.4));
	model_base = scale(model_base, vec3(0.1, 0.5, 0.1));
	glUniformMatrix4fv(sc.m_loc, 1, GL_FALSE, value_ptr(model_base));
	glUniformMatrix4fv(sc.v_loc, 1, GL_FALSE, value_ptr(sc.view));
	sc.cube->render();
	shade(3);
	model_base = translate(mat4(1.0), vec3(1.65 , 0.75, 3.2 ));
	model_base = scale(model_base, vec3(0.8, 0.1, 0.5));
	glUniformMatrix4fv(sc.m_loc, 1, GL_FALSE, value_ptr(model_base));
	glUniformMatrix4fv(sc.v_loc, 1, GL_FALSE, value_ptr(sc.view));
	sc.cube->render();


	shade(13);
	model_base = translate(mat4(1.0), vec3(1.6, 0.8, 3.2));
	model_base = rotate(model_base, (float)radians(-90.0), vec3(1.0, 0.0, 0.0));
	model_base = scale(model_base, vec3(0.06, 0.06, 0.06));
	glUniformMatrix4fv(sc.m_loc, 1, GL_FALSE, value_ptr(model_base));
	glUniformMatrix4fv(sc.v_loc, 1, GL_FALSE, value_ptr(sc.view));
	sc.teapot->render();

	shade(0);
	// floor
	glUniformMatrix4fv(sc.m_loc, 1, GL_FALSE, value_ptr(mat4(1.0)));
	sc.plane->render();

	glutSwapBuffers();
}

void shutdown()
{
	glDeleteProgram(sc.rendering_program);
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(sc.width, sc.height);
	glutCreateWindow("20160392");
	glewInit();

	version();
	startup();
	glutDisplayFunc(render);
	glutIdleFunc(idle);

	glutMainLoop();
	shutdown();
	return 0;
}