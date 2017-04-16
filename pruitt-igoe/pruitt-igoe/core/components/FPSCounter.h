#pragma once

#include "../ui/uicomponent.h"
#include "../ui/uitext.h"

class FPSCounter : public UIComponent
{
public:
	void Awake();

	void Update();

private:
	UIText * text;
	int currentFrame;
	float currentFrameTimeAverage;
	float minTime;
	float maxTime;

};

