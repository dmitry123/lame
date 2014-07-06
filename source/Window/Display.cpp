#include "Display.h"
#include "Window.h"

#include <GL\glut.h>

LAME_BEGIN

Display::_DisplayPtr Display::info = new Display::_Display();

Display::_Display::_Display() {

	this->current = NULL;
	this->width = glutGet(GLUT_SCREEN_WIDTH);
	this->height = glutGet(GLUT_SCREEN_HEIGHT);
}

WindowPtr Display::_Display::Find(StringC name) {

	Map<StringC, WindowPtr>::iterator i;
	
	if ((i = info->hash.find(name)) != info->hash.end()) {
		return i->second;
	}

	return LAME_NULL;
}

Bool Display::_Display::Attach(WindowPtr window) {

	if (this->hash.count(window->GetName())) {
		return FALSE;
	}

	this->hash.insert(std::make_pair(window->GetName(), window));

	return TRUE;
}

Bool Display::_Display::Detach(WindowPtr window) {

	if (this->hash.count(window->GetName())) {
		return FALSE;
	}

	this->hash.erase(this->hash.find(window->GetName()));

	return TRUE;
}

Bool Display::_Display::Detach(StringC name) {

	Map<StringC, WindowPtr>::iterator i;

	if ((i = this->hash.find(name)) != this->hash.end()) {
		return this->Detach(i->second);
	}

	return FALSE;
}

Bool Display::_Display::Loop(Void) {

	if (!this->hash.size()) {
		return FALSE;
	}

	glutMainLoop();

	return TRUE;
}

Void Display::_Display::_RegisterWindow(WindowPtr window, StringC name) {

	window->name = name;

	if (!this->current) {
		this->current = window;
	}

	if (this->hash.find(name) != this->hash.end()) {
		info->hash.insert(std::make_pair(window->GetName(), window));
	}
}

Void Display::_Display::_UnRegisterWindow(WindowPtr window) {

	Map<StringC, WindowPtr>::iterator i;

	if ((i = info->hash.find(window->GetName())) != info->hash.end()) {
		info->hash.erase(i);
	}
}

LAME_END