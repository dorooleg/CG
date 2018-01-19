#include <stdlib.h>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include "util.h"
#include "pipeline.h"
#include "camera.h"
#include "backend.h"
#include "mesh.h"
#include "particle_system.h"

static const auto WINDOW_WIDTH = 700;
static const auto WINDOW_HEIGHT = 600;

class Application : public ICallbacks
{
public:
	Application()
		: m_pGameCamera(nullptr)
	{
		m_persProjInfo.FOV = 60.0f;
		m_persProjInfo.Height = WINDOW_HEIGHT;
		m_persProjInfo.Width = WINDOW_WIDTH;
		m_persProjInfo.zNear = 1.0f;
		m_persProjInfo.zFar = 100.0f;

		m_currentTimeMillis = GetCurrentTimeMillis();
	}

	virtual ~Application()
	{
		delete m_pGameCamera; 
		m_pGameCamera = nullptr;
	}

	bool Init()
	{
		const Vector3f Pos(0.0f, 0.4f, -0.5f);
		const Vector3f Target(0.0f, 0.2f, 1.0f);
		const Vector3f Up(0.0, 1.0f, 0.0f);

		m_pGameCamera = new Camera(WINDOW_WIDTH, WINDOW_HEIGHT, Pos, Target, Up);

		const auto ParticleSystemPos = Vector3f(0.0f, 0.0f, 1.0f);

		return m_particleSystem.InitParticleSystem(ParticleSystemPos);
	}

	void Run()
	{
		GLUTBackendRun(this);
	}

	void RenderSceneCB() override
	{
		const auto TimeNowMillis = GetCurrentTimeMillis();
		const auto DeltaTimeMillis = static_cast<unsigned int>(TimeNowMillis - m_currentTimeMillis);
		m_currentTimeMillis = TimeNowMillis;
		m_pGameCamera->OnRender();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Pipeline p;
		p.Scale(20.0f, 20.0f, 1.0f);
		p.Rotate(90.0f, 0.0, 0.0f);
		p.SetCamera(m_pGameCamera->GetPos(), m_pGameCamera->GetTarget(), m_pGameCamera->GetUp());
		p.SetPerspectiveProj(m_persProjInfo);

		m_particleSystem.Render(DeltaTimeMillis, p.GetVPTrans(), m_pGameCamera->GetPos());

		glutSwapBuffers();
	}

	void KeyboardCB(const PCDEV_KEY PcdevKey, PCDEV_KEY_STATE State) override
	{
		switch (PcdevKey)
		{
		case PCDEV_KEY_ESCAPE:
		case PCDEV_KEY_q:
			glutLeaveMainLoop();
			break;
		default:
			m_pGameCamera->OnKeyboard(PcdevKey);
		}
	}

	void PassiveMouseCB(const int x, const int y) override
	{
		m_pGameCamera->OnMouse(x, y);
	}

private:
	long long m_currentTimeMillis;
	Camera* m_pGameCamera;
	PersProjInfo m_persProjInfo;
	ParticleSystem m_particleSystem;
};

int main(int argc, char** argv)
{
	srand(time(nullptr));

	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("Particle System");

	const auto res = glewInit();
	if (res != GLEW_OK)
	{
		fprintf(stderr, "Error: '%s'\n", reinterpret_cast<const char*>(glewGetErrorString(res)));
		return false;
	}

	glutSetCursor(GLUT_CURSOR_NONE);

	Application application;

	if (!application.Init())
	{
		return 1;
	}

	application.Run();

	return 0;
}