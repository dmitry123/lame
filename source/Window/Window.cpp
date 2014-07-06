#include "Window.h"
#include "Display.h"

#include <GL\glut.h>

LAME_BEGIN

namespace internal {
	Bool isGlutInitialized = FALSE;
}

Window::Window(StringC name) {

	this->dipslay->_RegisterWindow(this, name);

	this->position.x = 0;
	this->position.y = 0;
	this->width = 100;
	this->height = 100;
	this->title = "Title";
}

Window::~Window() {
	this->dipslay->_UnRegisterWindow(this);
}

WindowPtr Window::SetPosition(Sint32 x, Sint32 y) {

	this->position.x = x;
	this->position.y = y;

	return this;
}

WindowPtr Window::SetSize(Uint32 width, Uint32 height) {

	this->width = width;
	this->height = height;

	return this;
}

WindowPtr Window::SetTitle(StringC title) {

	this->title = title;
	return this;
}

WindowPtr Window::Create(Sint32 argc, StringC argv[]) {

	if (!internal::isGlutInitialized) {
		glutInit(&argc, (char**) argv);
	}

	internal::isGlutInitialized = TRUE;

	glutInitWindowPosition(this->position.x, this->position.y);
	glutInitWindowSize(this->width, this->height);

	glutDisplayFunc([]() {

		WindowPtr window = Display()->GetCurrentWindow();

		if (window != NULL) {

		}
	});

	glutReshapeFunc([](int width, int height) {

	});

	glutMouseFunc([](int button, int state, int x, int y) {

	});

	glutMotionFunc([](int x, int y) {

	});

	glutPassiveMotionFunc([](int x, int y) {

	});

	glutKeyboardFunc([](unsigned char key, int x, int y) {

	});

	glutSpecialFunc([](int key, int x, int y) {

	});

	this->id = glutCreateWindow(this->title);

	return this;
}

LAME_END