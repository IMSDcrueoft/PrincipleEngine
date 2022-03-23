#pragma once
#include"RawScript.h"
#include<vector>
#include <memory>

using namespace Principle;

//场景类,负责管理Object
class Scene
{
protected:
	std::vector<std::unique_ptr<RawScript>> activeObjects;//活跃Object
	std::vector<std::unique_ptr<RawScript>> inActiveObjects;//不活跃Object
public:
	Scene();
	~Scene();

	void Update(systemTimer& timer);
};