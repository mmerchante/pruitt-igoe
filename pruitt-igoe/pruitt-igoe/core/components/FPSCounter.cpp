#include "FPSCounter.h"

#define FRAME_WINDOW 30

void FPSCounter::Awake()
{
	this->text = this->gameObject->AddComponent<UIText>();
}

void FPSCounter::Update()
{
	glm::vec2 screenSize = Engine::GetScreenSize();
	this->text->GetTransform()->UISetLocalPosition(glm::vec2(0, screenSize.y));
	//this->text->SetTextSize(15);

	float deltaTime = Engine::DeltaTime();

	if (currentFrame == FRAME_WINDOW)
	{
		int fps = 1.f / currentFrameTimeAverage;
		float avgMs = 1000.f * currentFrameTimeAverage;
		float minMs = 1000.f * minTime;
		float maxMs = 1000.f * maxTime;
		std::string txt = glm::detail::format("hz: %i (%.1fms), min: (%.1fms), max: (%.1fms)", fps, avgMs, minMs, maxMs);
		this->text->SetText(txt);

		currentFrameTimeAverage = 0;
		minTime = deltaTime;
		maxTime = deltaTime;
	}

	currentFrame = (currentFrame == FRAME_WINDOW) ? 0 : currentFrame + 1;

	minTime = glm::min(minTime, deltaTime);
	maxTime = glm::max(maxTime, deltaTime);
	currentFrameTimeAverage += deltaTime / (float)FRAME_WINDOW;
}
