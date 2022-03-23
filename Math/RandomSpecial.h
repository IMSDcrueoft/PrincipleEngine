#pragma once
#ifndef RANDOMSPECIAL
#define RANDOMSPECIAL

#include<cassert>
#include<memory>
#include"RandomBase.h"

class RandomSpecial :virtual public RandomBase
{
public:
	RandomSpecial() = default;

	void SetBernoulli(const double probability = 0.5);
	bool GetBernoulli();

	void SetPoisson(const uint32_t probability = 1);
	size_t GetPoisson();

	void SetNormalF(float expected, float sigma);
	float GetNormalF();

	void SetNormalD(double expected, double sigma);
	double GetNormalD();

protected:
	//广义伯努利分布
	std::unique_ptr<std::bernoulli_distribution> bernouli;
	//泊松分布
	std::unique_ptr<std::poisson_distribution<size_t>> poisson;
	//正态分布
	std::unique_ptr<std::normal_distribution<float>> normalF;
	std::unique_ptr<std::normal_distribution<double>> normalD;
};

//--------------------------------------------------------------------------------------
inline void RandomSpecial::SetBernoulli(const double probability)
{
	if (bernouli != nullptr)
		bernouli.reset();

	bernouli = std::make_unique<std::bernoulli_distribution>(probability);
}

inline bool RandomSpecial::GetBernoulli()
{
#if defined(DEBUG) || defined(_DEBUG)
	assert(bernouli != nullptr);
#endif

	return bernouli->operator()(random);
}

inline void RandomSpecial::SetPoisson(const uint32_t probability)
{
	if (poisson == nullptr)
		poisson = std::make_unique<std::poisson_distribution<size_t>>(probability);
	else
		poisson->param(std::poisson_distribution<size_t>::param_type(probability));
}

inline size_t RandomSpecial::GetPoisson()
{
	return poisson->operator()(random);
}

inline void RandomSpecial::SetNormalF(float expected, float sigma)
{
	if (normalF == nullptr)
		normalF = std::make_unique<std::normal_distribution<float>>(expected, sigma);
	else
		normalF->param(std::normal_distribution<float>::param_type(expected, sigma));
}

inline float RandomSpecial::GetNormalF()
{
#if defined(DEBUG) || defined(_DEBUG)
	assert(normalF != nullptr);
#endif

	return normalF->operator()(random);
}

inline void RandomSpecial::SetNormalD(double expected, double sigma)
{
	if (normalD == nullptr)
		normalD = std::make_unique<std::normal_distribution<double>>(expected, sigma);
	else
		normalD->param(std::normal_distribution<double>::param_type(expected, sigma));
}

inline double RandomSpecial::GetNormalD()
{
#if defined(DEBUG) || defined(_DEBUG)
	assert(normalD != nullptr);
#endif

	return normalD->operator()(random);
}
#endif // !RANDOMSPECIAL