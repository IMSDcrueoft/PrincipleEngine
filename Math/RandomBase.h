#pragma once
#ifndef RANDOMBASE
#define RANDOMBASE

#include<random>
#include<cstdint>

class RandomBase
{
public:
	RandomBase() = default;

	static uint32_t Rand();
	static float RandF();
	static float RandF(float min, float max);

	//use uint8_t,uint16_t,uint32_t,uint64_t,size_t,int8_t,int16_t,int32_t,int64_t
	template<typename TyInt = int32_t>
	static TyInt Rand(const TyInt min, const TyInt max);

	static void SetSeed();
	static void SetSeed(const uint32_t RandSeed);

public:
	static constexpr float MaxUint32F = (float)UINT32_MAX;

protected:
	//use as seed
	static std::random_device seed;
	//mt19937
	static std::default_random_engine random;
};
//C++17,内联变量
inline std::random_device			RandomBase::seed = std::random_device();
inline std::default_random_engine	RandomBase::random = std::default_random_engine(RandomBase::seed());

//--------------------------------------------------------------------------------------
inline uint32_t RandomBase::Rand()
{
	return random();
}

// Returns random float in [0, 1).
inline float RandomBase::RandF()
{
	return static_cast<float>((Rand()) / MaxUint32F);
}

// Returns random float in [min, max).
inline float RandomBase::RandF(float min, float max)
{
	return RandF() * (max - min) + min;
}

inline void RandomBase::SetSeed()
{
	random.seed(seed());
}

inline void RandomBase::SetSeed(const uint32_t RandSeed)
{
	random.seed(RandSeed);
}

template<typename TyInt>
inline TyInt RandomBase::Rand(const TyInt min, const TyInt max)
{
	static_assert(std::is_integral_v<TyInt>, "Rand Type must be Integer.");
	return static_cast<TyInt>(random() % (max - min + 1)) + min;
}
#endif // !RANDOMBASE