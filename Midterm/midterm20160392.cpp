#include "cg.h"
#include "vbocube.h"
#include "vboplane.h"
#include "vbosphere.h"
#include "vbotorus.h"
#include "vbotrianglebar.h"
#include "vboteapot.h"
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

	vec4 vertices[100];
	vec4 colors[100];
	unsigned int indices[100];
	int num, numIndex;

	// 모델링 변수
	VBOCube *cube;
	VBOPlane *plane;
	VBOSphere *sphere;
	VBOTorus *torus;
	VBOTriangleBar *trianglebar;
	VBOTeapot *teapot;
	VBOMesh *mesh;

	// 뷰잉 변수
	mat4 model, view, proj;
	GLint m_loc, v_loc, proj_loc, color_loc;
	float angle;
	float theta, val, val2, val3, val4, eyes, arms, arms2, nose, others, background;
};

scene sc;


// 맨 처음 한번 실행
void startup()
{
	//------------------------------------------------------------------------
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST); glDepthFunc(GL_LEQUAL);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glShadeModel(GL_SMOOTH);

	sc.rendering_program = compile_shaders("perfrag.glsl", "perfragf.glsl");
	glUseProgram(sc.rendering_program);

	glGenVertexArrays(1, &sc.vertex_array_object);
	glBindVertexArray(sc.vertex_array_object);

	sc.cube = new VBOCube();
	sc.plane = new VBOPlane(2.0f, 1.0f, 1, 1);
	sc.sphere = new VBOSphere();
	sc.torus = new VBOTorus(2.0f, 1.5f, 5, 40);
	sc.trianglebar = new VBOTriangleBar();
	sc.teapot = new VBOTeapot(5, mat4(1.0));

	sc.angle = 0.957283f;
	sc.model = mat4(1.0);
	sc.view = mat4(1.0);
	sc.proj = mat4(1.0);
	
	sc.view = lookAt(vec3(1.0f, 2.0f, 6.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 3.0f, 1.0f));
	float cameraZ = sc.height * 0.5 / tan(radians(0.5*50.0));
	sc.proj = perspective(50.0f, (float)sc.width / (float)sc.height, (float)0.001*cameraZ, (float)10.0*cameraZ);
	
	sc.m_loc = glGetUniformLocation(sc.rendering_program, "m_matrix");
	sc.v_loc = glGetUniformLocation(sc.rendering_program, "v_matrix");
	sc.proj_loc = glGetUniformLocation(sc.rendering_program, "proj_matrix");
	sc.color_loc = glGetUniformLocation(sc.rendering_program, "color");
	
	glUniformMatrix4fv(sc.m_loc, 1, GL_FALSE, value_ptr(mat4(1.0)));
	glUniformMatrix4fv(sc.v_loc, 1, GL_FALSE, value_ptr(sc.view));
	glUniformMatrix4fv(sc.proj_loc, 1, GL_FALSE, value_ptr(sc.proj));

	glUniform3f(glGetUniformLocation(sc.rendering_program, "Light.Intensity"), 0.8, 0.8, 0.8);

	sc.theta = 0.0;
	sc.val = 0.0;
	sc.val2 = 0.0;
	sc.val3 = 0.0;
	sc.val4 = 0.0;
	sc.eyes = 0.0;
	sc.arms = 0.0;
	sc.arms2 = 0.0;
	sc.nose = 0.0;
	sc.others = 0.0;
	sc.background = 0.0;
}

//  계속해서 실행됨
void idle()
{
	sc.angle += 0.001f;
	if (sc.angle > 2.0 * 3.141592) sc.angle -= 2.0 * 3.141592;
	if(sc.val<=1.5)
		sc.val += 0.0005;
	if (sc.val >= 1.5 && sc.val2<=1.0)
		sc.val2 += 0.0005;
	if (sc.val2 >= 1.0 && sc.val3<=sc.val*0.35)
		sc.val3 += 0.0003;
	if (sc.val3 >= sc.val * 0.2 && sc.val4<=0.5)
		sc.val4 += 0.0003;
	if (sc.val4 >= 0.5 && sc.eyes<=0.02)
		sc.eyes += 0.00005;
	if (sc.eyes >= 0.02 && sc.arms <= 0.3)
		sc.arms += 0.0005;
	if (sc.arms >= 0.2 && sc.arms2 <= 0.3)
		sc.arms2 += 0.0005;
	if (sc.arms2 >= 0.2 && sc.nose <= 0.1)
		sc.nose += 0.001;
	if (sc.nose >= 0.1) {
		if (sc.others <= 0.1)
			sc.others += 0.0003;
		else
			sc.background += 0.01;
	}
	
	
	glutPostRedisplay();
}

void shade(int k) {
	glUniform3f(glGetUniformLocation(sc.rendering_program, "Material.Ka"), myMaterial[k][0][0], myMaterial[k][0][1], myMaterial[k][0][2]);
	glUniform3f(glGetUniformLocation(sc.rendering_program, "Material.Kd"), myMaterial[k][1][0], myMaterial[k][1][1], myMaterial[k][1][2]);
	glUniform3f(glGetUniformLocation(sc.rendering_program, "Material.Ks"), myMaterial[k][2][0], myMaterial[k][2][1], myMaterial[k][2][2]);
	glUniform1f(glGetUniformLocation(sc.rendering_program, "Material.Shininess"), myMaterial[k][3][0]);
}
void render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.52, 0.804, 0.917, 1.0f);

	glUseProgram(sc.rendering_program);

	vec4 pos = sc.view * vec4(30.0f * cos(sc.angle), 100.0f, 1.0f * sin(sc.angle), 1.0f);
	glUniform4fv(glGetUniformLocation(sc.rendering_program, "Light.Position"),	1, value_ptr(pos));


	/******** Mesh  */


	// floor
	mat4 plane_base;
	shade(14);
	plane_base = translate(mat4(1.0), vec3(0.0, 0.7, 0.0));
	glUniformMatrix4fv(sc.m_loc, 1, GL_FALSE, value_ptr(plane_base));
	glUniformMatrix4fv(sc.v_loc, 1, GL_FALSE, value_ptr(sc.view));
	sc.plane->render();
	//background
	//plane_base = translate(mat4(1.0), vec3(-0.5, 0.1, -1));
	//plane_base = scale(plane_base, vec3(0.3*sc.background, 0.3*sc.background, 0.1*sc.others));
	//glUniformMatrix4fv(sc.m_loc, 1, GL_FALSE, value_ptr(plane_base));
	//glUniform4f(sc.color_loc, 0.10, 0.25, 0.34, 1.0);
	//sc.sphere->render();
	
	//roll a ball
	shade(8);
	mat4 model_base;
	model_base = translate(mat4(1.0), vec3(1.0-sc.val, 0.7-(sc.val*0.2), 0.0));
	model_base = scale(model_base, vec3(sc.val*0.2, sc.val*0.2, 0.2));
	glUniformMatrix4fv(sc.m_loc, 1, GL_FALSE, value_ptr(model_base));
	glUniformMatrix4fv(sc.v_loc, 1, GL_FALSE, value_ptr(sc.view));
	//glUniform4f(sc.color_loc, 0.9, 0.9, 0.9, 1.0);
	sc.sphere->render();

	if (sc.val >= 1.0)
	{
		shade(8);
		model_base = translate(mat4(1.0), vec3(1.0 - sc.val2-sc.val4, 0.7 - (sc.val2 * 0.2)-sc.val3, 0.0));
		model_base = scale(model_base, vec3(sc.val2 * 0.2, sc.val2 * 0.2, 0.2));
		glUniformMatrix4fv(sc.m_loc, 1, GL_FALSE, value_ptr(model_base));
		glUniformMatrix4fv(sc.v_loc, 1, GL_FALSE, value_ptr(sc.view));
		//glUniform4f(sc.color_loc, 0.9, 0.9, 0.9, 1.0);
		sc.sphere->render();
	}


	//build eyes
	shade(4);
	model_base = translate(mat4(1.0), vec3(-0.5 - 0.1, -0.1, 0.0));
	model_base = scale(model_base, vec3(sc.eyes, sc.eyes, sc.eyes));
	glUniformMatrix4fv(sc.m_loc, 1, GL_FALSE, value_ptr(model_base));
	glUniformMatrix4fv(sc.v_loc, 1, GL_FALSE, value_ptr(sc.view));
	//glUniform4f(sc.color_loc, 0.1, 0.1, 0.1, 1.0);
	sc.sphere->render();
	
	model_base = translate(mat4(1.0), vec3(-0.5 + 0.1, -0.1, 0.0));
	model_base = scale(model_base, vec3(sc.eyes, sc.eyes, sc.eyes));
	glUniformMatrix4fv(sc.m_loc, 1, GL_FALSE, value_ptr(model_base));
	glUniformMatrix4fv(sc.v_loc, 1, GL_FALSE, value_ptr(sc.view));
	//glUniform4f(sc.color_loc, 0.1, 0.1, 0.1, 1.0);
	sc.sphere->render();


	//build arms
	shade(1);
	model_base = translate(mat4(1.0), vec3(-0.5 + 0.45 * cos(radians(210.0)), 0.4 + 0.45 * sin(radians(210.0)), -0.05));
	model_base = rotate(model_base, (float)radians(30.0), vec3(0, 0, 1));
	model_base = scale(model_base, vec3(sc.arms, sc.arms*0.00005, 0.2));
	glUniformMatrix4fv(sc.m_loc, 1, GL_FALSE, value_ptr(model_base));
	glUniformMatrix4fv(sc.v_loc, 1, GL_FALSE, value_ptr(sc.view));
	//glUniform4f(sc.color_loc, 0.5, 0.25, 0.0, 1.0);
	sc.cube->render();

	model_base = translate(mat4(1.0), vec3(-0.5 + 0.45 * cos(radians(-30.0)), 0.4 + 0.45 * sin(radians(-30.0)), -0.1));
	model_base = rotate(model_base, (float)radians(150.0), vec3(0, 0, 1));
	model_base = scale(model_base, vec3(sc.arms2, sc.arms2*0.00005, 0.2));
	glUniformMatrix4fv(sc.m_loc, 1, GL_FALSE, value_ptr(model_base));
	glUniformMatrix4fv(sc.v_loc, 1, GL_FALSE, value_ptr(sc.view));
	//glUniform4f(sc.color_loc, 0.5, 0.25, 0.0, 1.0);
	sc.cube->render();

	
	//build nose
	shade(9);
	model_base = translate(mat4(1.0), vec3(-0.5-0.03, -0.1+0.1, 0));
	model_base = rotate(model_base, (float)radians(30.0), vec3(0, 0, 1));
	model_base = scale(model_base, vec3(sc.nose*0.6, sc.nose, sc.nose));
	glUniformMatrix4fv(sc.m_loc, 1, GL_FALSE, value_ptr(model_base));
	glUniformMatrix4fv(sc.v_loc, 1, GL_FALSE, value_ptr(sc.view));
	//glUniform4f(sc.color_loc, 0.9, 0.5, 0, 1.0);
	sc.trianglebar->render();


	//others

	//botton
	shade(14);
	model_base = translate(mat4(1.0), vec3(-0.5, 0.5, 0.0));
	model_base = scale(model_base, vec3(0.3*sc.others, 0.3*sc.others, 0.3*sc.others));
	glUniformMatrix4fv(sc.m_loc, 1, GL_FALSE, value_ptr(model_base));
	glUniformMatrix4fv(sc.v_loc, 1, GL_FALSE, value_ptr(sc.view));
	//glUniform4f(sc.color_loc, 0.1, 0.1, 0.1, 1.0);
	sc.sphere->render();
	model_base = translate(mat4(1.0), vec3(-0.5, 0.3, 0.0));
	model_base = scale(model_base, vec3(0.3 * sc.others, 0.3 * sc.others, 0.3 * sc.others));
	glUniformMatrix4fv(sc.m_loc, 1, GL_FALSE, value_ptr(model_base));
	glUniformMatrix4fv(sc.v_loc, 1, GL_FALSE, value_ptr(sc.view));
	//glUniform4f(sc.color_loc, 0.1, 0.1, 0.1, 1.0);
	sc.sphere->render();

	//hat
	shade(16);
	model_base = translate(mat4(1.0), vec3(-0.5, -0.25, 0.0));
	model_base = scale(model_base, vec3(4.5*sc.others, 0.5*sc.others, 0.0));
	glUniformMatrix4fv(sc.m_loc, 1, GL_FALSE, value_ptr(model_base));
	glUniformMatrix4fv(sc.v_loc, 1, GL_FALSE, value_ptr(sc.view));
	//glUniform4f(sc.color_loc, 0.1, 0.1, 0.1, 1.0);
	sc.cube->render();
	shade(16);
	model_base = translate(mat4(1.0), vec3(-0.5, -0.3, 0.0));
	model_base = scale(model_base, vec3(2*sc.others, -1.5*sc.others, 0.0));
	glUniformMatrix4fv(sc.m_loc, 1, GL_FALSE, value_ptr(model_base));
	glUniformMatrix4fv(sc.v_loc, 1, GL_FALSE, value_ptr(sc.view));
	//glUniform4f(sc.color_loc, 0.1, 0.1, 0.1, 1.0);
	sc.trianglebar->render();
	if (sc.others > 0) {
		//mouth
		shade(4);
		model_base = translate(mat4(1.0), vec3(-0.5-0.04, 0.07-0.02, 0.0));
		model_base = rotate(model_base, (float)radians(30.0), vec3(0, 0, 1));
		model_base = scale(model_base, vec3(0.1,0.005,0.01));
		glUniformMatrix4fv(sc.m_loc, 1, GL_FALSE, value_ptr(model_base));
		glUniformMatrix4fv(sc.v_loc, 1, GL_FALSE, value_ptr(sc.view));
		glUniform4f(sc.color_loc, 0.1, 0.1, 0.1, 1.0);
		sc.cube->render();
		model_base = translate(mat4(1.0), vec3(-0.5+0.04, 0.07-0.02, 0.0));
		model_base = rotate(model_base, (float)radians(150.0), vec3(0, 0, 1));
		model_base = scale(model_base, vec3(0.1,0.005,0.01));
		glUniformMatrix4fv(sc.m_loc, 1, GL_FALSE, value_ptr(model_base));
		glUniformMatrix4fv(sc.v_loc, 1, GL_FALSE, value_ptr(sc.view));
		glUniform4f(sc.color_loc, 0.1, 0.1, 0.1, 1.0);
		sc.cube->render();
	}



	glutSwapBuffers();
}




void shutdown()
{
	glDeleteVertexArrays(1, &sc.vertex_array_object);
	glDeleteProgram(sc.rendering_program);
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

	glutInitWindowSize(sc.width, sc.height);
	glutCreateWindow("20160392 박병제");
	glewInit();
	version();

	startup();
	glutDisplayFunc(render);
	glutIdleFunc(idle);

	glutMainLoop();
	shutdown();
	return 0;
}
