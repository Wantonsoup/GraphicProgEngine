////////////////////////////////////////////////////////////////////////////////
// Filename: fpsclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "fpsclass.h"

FpsClass::FpsClass()
{
}

FpsClass::FpsClass(const FpsClass& other)
{
}

FpsClass::~FpsClass()
{
}

void FpsClass::Initialize()
{
	m_fps = 0;
	m_count = 0;

	m_startTime = std::chrono::steady_clock::now();

	return;
}

void FpsClass::Frame()
{
	m_count++;

	auto currentTime = std::chrono::steady_clock::now();
	auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - m_startTime).count();

	if (elapsedTime >= 1000)
	{
		m_fps = m_count;
		m_count = 0;
		m_startTime = currentTime;
	}

	return;
}

int FpsClass::GetFps()
{
	return m_fps;
}