#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "util.h"
#include "backend.h"

static ICallbacks* s_pCallbacks = nullptr;

static bool sWithDepth = false;

PCDEV_KEY GLUTKeyToPCDEVKey(uint Key)
{
	switch (Key)
	{
	case GLUT_KEY_F1:
		return PCDEV_KEY_F1;
	case GLUT_KEY_F2:
		return PCDEV_KEY_F2;
	case GLUT_KEY_F3:
		return PCDEV_KEY_F3;
	case GLUT_KEY_F4:
		return PCDEV_KEY_F4;
	case GLUT_KEY_F5:
		return PCDEV_KEY_F5;
	case GLUT_KEY_F6:
		return PCDEV_KEY_F6;
	case GLUT_KEY_F7:
		return PCDEV_KEY_F7;
	case GLUT_KEY_F8:
		return PCDEV_KEY_F8;
	case GLUT_KEY_F9:
		return PCDEV_KEY_F9;
	case GLUT_KEY_F10:
		return PCDEV_KEY_F10;
	case GLUT_KEY_F11:
		return PCDEV_KEY_F11;
	case GLUT_KEY_F12:
		return PCDEV_KEY_F12;
	case GLUT_KEY_LEFT:
		return PCDEV_KEY_LEFT;
	case GLUT_KEY_UP:
		return PCDEV_KEY_UP;
	case GLUT_KEY_RIGHT:
		return PCDEV_KEY_RIGHT;
	case GLUT_KEY_DOWN:
		return PCDEV_KEY_DOWN;
	case GLUT_KEY_PAGE_UP:
		return PCDEV_KEY_PAGE_UP;
	case GLUT_KEY_PAGE_DOWN:
		return PCDEV_KEY_PAGE_DOWN;
	case GLUT_KEY_HOME:
		return PCDEV_KEY_HOME;
	case GLUT_KEY_END:
		return PCDEV_KEY_END;
	case GLUT_KEY_INSERT:
		return PCDEV_KEY_INSERT;
	case GLUT_KEY_DELETE:
		return PCDEV_KEY_DELETE;
	default:
		exit(1);
	}
}


static PCDEV_MOUSE GLUTMouseToPCDEVMouse(uint Button)
{
	switch (Button)
	{
	case GLUT_LEFT_BUTTON:
		return PCDEV_MOUSE_BUTTON_LEFT;
	case GLUT_RIGHT_BUTTON:
		return PCDEV_MOUSE_BUTTON_RIGHT;
	case GLUT_MIDDLE_BUTTON:
		return PCDEV_MOUSE_BUTTON_MIDDLE;
	}

	return PCDEV_MOUSE_UNDEFINED;
}


static void SpecialKeyboardCB(int Key, int x, int y)
{
	PCDEV_KEY PcdevKey = GLUTKeyToPCDEVKey(Key);
	s_pCallbacks->KeyboardCB(PcdevKey);
}


static void KeyboardCB(unsigned char Key, int x, int y)
{
	if (
		Key >= '+' && Key <= '9' ||
		Key >= 'A' && Key <= 'Z' ||
		Key >= 'a' && Key <= 'z'
	)
	{
		PCDEV_KEY PcdevKey = (PCDEV_KEY)Key;
		s_pCallbacks->KeyboardCB(PcdevKey);
	}
	else
	{
		printf("%d\n", Key);
	}
}


static void PassiveMouseCB(int x, int y)
{
	s_pCallbacks->PassiveMouseCB(x, y);
}


static void RenderSceneCB()
{
	s_pCallbacks->RenderSceneCB();
}


static void IdleCB()
{
	s_pCallbacks->RenderSceneCB();
}


static void MouseCB(int Button, int State, int x, int y)
{
	PCDEV_MOUSE PcdevMouse = GLUTMouseToPCDEVMouse(Button);
	PCDEV_KEY_STATE PcdevKeyState = State == GLUT_DOWN ? PCDEV_KEY_STATE_PRESS : PCDEV_KEY_STATE_RELEASE;

	s_pCallbacks->MouseCB(PcdevMouse, PcdevKeyState, x, y);
}


static void InitCallbacks()
{
	glutDisplayFunc(RenderSceneCB);
	glutIdleFunc(IdleCB);
	glutSpecialFunc(SpecialKeyboardCB);
	glutPassiveMotionFunc(PassiveMouseCB);
	glutKeyboardFunc(KeyboardCB);
	glutMouseFunc(MouseCB);
}

void GLUTBackendRun(ICallbacks* pCallbacks)
{
	if (!pCallbacks)
	{
		fprintf(stderr, "%s : callbacks not specified!\n", __FUNCTION__);
		return;
	}

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glFrontFace(GL_CW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

	if (sWithDepth)
	{
		glEnable(GL_DEPTH_TEST);
	}

	s_pCallbacks = pCallbacks;
	InitCallbacks();
	glutMainLoop();
}