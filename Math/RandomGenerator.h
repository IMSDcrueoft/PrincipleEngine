#pragma once
#ifndef RANDOMGENERATOR
#define RANDOMGENERATOR

#include"RandomUniform.h"
#include"RandomSpecial.h"

class RandomGenerator:public RandomUniform,public RandomSpecial
{
public:
	RandomGenerator() = default;
};
#endif // !RANDOMGENERATOR