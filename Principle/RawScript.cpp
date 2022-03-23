#include"RawScript.h"
namespace Principle
{
	const State& ObjectStateExtension::GetObjectState() const
	{
		return objectState;
	}

	bool ObjectStateExtension::SetObjectState(const State& newState)
	{
		if (objectState != State::destroy)
		{
			objectState = newState;
			return true;
		}
		else
			return false;
	}
}