#include"Utilitys.h"
using namespace Utilitys;

Trigger::simpleTrigger::simpleTrigger(const float latency)
{
	triggerLatency = latency;
}

bool Trigger::simpleTrigger::UpdateTrigger(const float totalTime)
{
	if ((totalTime - timer) >= triggerLatency)
	{
		timer += triggerLatency;
		return true;
	}

	return false;
}

const float Utilitys::Trigger::simpleTrigger::getUpdateStep(const float totalTime) const
{
	return (totalTime - timer) / triggerLatency;
}

Trigger::reusableTrigger::reusableTrigger(const float latency)
{
	triggerLatency = latency;
}

void Trigger::reusableTrigger::resetTrigger()
{
	timer = 0.0f;
	isTriggered = false;
}

void Trigger::reusableTrigger::nextTrigger()
{
	if (timer >= triggerLatency)
	{
		while (timer >= triggerLatency)
			timer -= triggerLatency;
	}
	else
		timer = 0.0f;

	isTriggered = false;
}

void Trigger::reusableTrigger::UpdateTrigger(const float deltaTime)
{
	timer += deltaTime;

	if (timer > triggerLatency)
		isTriggered = true;
}

void Trigger::reusableTrigger::setNewLatency(const float latency)
{
	triggerLatency = latency;
	resetTrigger();
}

const bool& Trigger::reusableTrigger::ifTriggered() const
{
	return isTriggered;
}

const float& Trigger::reusableTrigger::getTimer() const
{
	return timer;
}

const float Trigger::reusableTrigger::getUpdateStep() const
{
	//update between frame
	return timer / triggerLatency;
}
