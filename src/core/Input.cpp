#include "core/Input.hpp"

#include "GLFW/glfw3.h"

Input* Input::s_instance(nullptr);

Input::Input(GLFWwindow* window) : m_window(window), m_frame(0)
{
	s_instance = this;

	glfwSetKeyCallback(m_window, Input::keyCallback);
	glfwSetMouseButtonCallback(m_window, Input::mouseButtonCallback);
}

bool Input::getKey(int key)
{
	return m_keys[key].down;
}

bool Input::getKeyPressed(int key)
{
	button& k = m_keys[key];
	return k.down && (k.lastAction == m_frame);
}

bool Input::getKeyReleased(int key)
{
	button& k = m_keys[key];
	return (!k.down) && (k.lastAction == m_frame);
}

bool Input::getMouseButton(int mbutton)
{
	return m_mbuttons[mbutton].down;
}

bool Input::getMouseButtonPressed(int mbutton)
{
	button& mb = m_mbuttons[mbutton];
	return mb.down && (mb.lastAction == m_frame);
}

bool Input::getMouseButtonReleased(int mbutton)
{
	button& mb = m_mbuttons[mbutton];
	return (!mb.down) && (mb.lastAction == m_frame);
}

bool Input::isCursorLocked() const
{
	return glfwGetInputMode(m_window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED;
}

void Input::setCursorLocked(bool val)
{
	glfwSetInputMode(m_window, GLFW_CURSOR, val ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}

void Input::getCursorPos(float& x, float& y) const
{
	x = m_cursorX;
	y = m_cursorY;
}

glm::vec2 Input::getCursorPos() const
{
	return glm::vec2(m_cursorX, m_cursorY);
}

void Input::getCursorDelta(float& x, float& y) const
{
	x = m_cursorDeltaX;
	y = m_cursorDeltaY;
}

glm::vec2 Input::getCursorDelta() const
{
	return glm::vec2(m_cursorDeltaX, m_cursorDeltaY);
}

void Input::update()
{
	++m_frame;

	glfwPollEvents();

	double cx, cy;
	glfwGetCursorPos(m_window, &cx, &cy);
	float newX = float(cx), newY = float(cy);

	m_cursorDeltaX = newX - m_cursorX;
	m_cursorDeltaY = newY - m_cursorY;

	m_cursorX = newX;
	m_cursorY = newY;
}

void Input::onKey(int key, int action)
{
	button& k = m_keys[key];
	if (action != GLFW_REPEAT) {
		k.down = action == GLFW_PRESS;
		k.lastAction = m_frame;
	}
}

void Input::onMouseButton(int mbutton, int action)
{
	button& mb = m_mbuttons[mbutton];
	mb.down = action == GLFW_PRESS;
	mb.lastAction = m_frame;
}
