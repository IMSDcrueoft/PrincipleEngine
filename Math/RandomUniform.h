#pragma once
#ifndef RANDOMUNIFORM
#define RANDOMUNIFORM

#include<cassert>
#include<memory>
#include"RandomBase.h"

class RandomUniform :virtual public RandomBase
{
public:
	RandomUniform() = default;

	void SetUniform_int(const int64_t min);
	void SetUniform_int(const int64_t min, int64_t max);
	int64_t GetUniformInteger();

	void SetUniform_float(const float min, const float max);
	float GetUniformFloat();

	void SetUniform_double(const double min, const double max);
	double GetUniformDouble();

protected:

	//连续均匀分布
	std::unique_ptr<std::uniform_int_distribution<int64_t>> uniform_int;
	std::unique_ptr<std::uniform_real_distribution<float>> uniform_float;
	std::unique_ptr<std::uniform_real_distribution<double>> uniform_double;
};

//--------------------------------------------------------------------------------------
inline void RandomUniform::SetUniform_int(const int64_t min)
{
	if (uniform_int == nullptr)
		uniform_int = std::make_unique<std::uniform_int_distribution<int64_t>>(min);
	else
		uniform_int->param(std::uniform_int_distribution<int64_t>::param_type(min));
}

inline void RandomUniform::SetUniform_int(const int64_t min, const int64_t max)
{
	if (uniform_int == nullptr)
		uniform_int = std::make_unique<std::uniform_int_distribution<int64_t>>(min, max);
	else
		uniform_int->param(std::uniform_int_distribution<int64_t>::param_type(min, max));
}

inline int64_t RandomUniform::GetUniformInteger()
{
#if defined(DEBUG) || defined(_DEBUG)
	assert(uniform_int != nullptr);
#endif

	return uniform_int->operator()(random);
}

inline void RandomUniform::SetUniform_float(const float min, const float max)
{
	if (uniform_float == nullptr)
		uniform_float = std::make_unique<std::uniform_real_distribution<float>>();

	uniform_float->param(std::uniform_real_distribution<float>::param_type(min, max));
}

inline float RandomUniform::GetUniformFloat()
{
#if defined(DEBUG) || defined(_DEBUG)
	assert(uniform_float != nullptr);
#endif

	return uniform_float->operator()(random);
}

inline void RandomUniform::SetUniform_double(const double min, const double max)
{
	if (uniform_double == nullptr)
		uniform_double = std::make_unique<std::uniform_real_distribution<double>>();

	uniform_double->param(std::uniform_real_distribution<double>::param_type(min, max));
}

inline double RandomUniform::GetUniformDouble()
{
#if defined(DEBUG) || defined(_DEBUG)
	assert(uniform_double != nullptr);
#endif

	return uniform_double->operator()(random);
}
#endif // !RANDOMUNIFORM