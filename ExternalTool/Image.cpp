#include "Image.h"

bool ImageProcessingTools::Zoom_DefaultSampling4x4(TextureData& input, TextureData& result, const float32_t& magnification, const float32_t& CenterWeight,
	void (*WeightEffact)(const float32_t& dx, const float32_t& dy, floatVec4& weightResult))
{
	if (input.getRGBA_uint8().size() == 0)//Handle it well, otherwise there will be problems in parallel
		return false;

	result.width = input.width * magnification;
	result.height = input.height * magnification;

	float32_t scaleIndex = 1.0f / magnification;

	auto& resultRGBA = result.getRGBA_uint8();
	resultRGBA.resize(static_cast<size_t>(result.width) * result.height);

	float32_t center = CenterWeight * 0.250f;
	float32_t outerNear = (1.0f - CenterWeight) * 0.08772433f;//near * 8 + far * 4 = 1
	float32_t outerFar = (1.0f - CenterWeight) * 0.074551344f;

	const float32_t kernel[4][4]
	{
		{outerFar ,outerNear,outerNear,outerFar },
		{outerNear,center	,center	  ,outerNear},
		{outerNear,center	,center	  ,outerNear},
		{outerFar ,outerNear,outerNear,outerFar }
	};

	parallel::parallel_for(0u, result.height, [&result, &input, &WeightEffact, &kernel, &scaleIndex](uint32_t Y) {

		auto GetFloorIndex = [&scaleIndex](const uint32_t& index)
		{
			return static_cast<int64_t>(floorf(index * scaleIndex));
		};

		auto Row = GetFloorIndex(Y);

		for (auto X = 0u; X < result.width; ++X)
		{

			auto Column = GetFloorIndex(X);

			float32_t dx = X * scaleIndex - Column;
			float32_t dy = Y * scaleIndex - Row;

			//Calculate weight parameters
			floatVec4 weight;
			WeightEffact(dx, dy, weight);

			RGBAColor_32f rgba_f1(0.0f, 0.0f, 0.0f, 0.0f);
			RGBAColor_32f rgba_f2(0.0f, 0.0f, 0.0f, 0.0f);
			RGBAColor_32f rgba_f3(0.0f, 0.0f, 0.0f, 0.0f);
			RGBAColor_32f rgba_f4(0.0f, 0.0f, 0.0f, 0.0f);

			//Unrolling loops to enhance performance

			rgba_f1 += RGBAColor_32f(input(Column + (-1), Row + (-1)), kernel[0][0]);
			rgba_f1 += RGBAColor_32f(input(Column + (0), Row + (-1)), kernel[0][1]);
			rgba_f1 += RGBAColor_32f(input(Column + (-1), Row + (0)), kernel[1][0]);
			rgba_f1 += RGBAColor_32f(input(Column + (0), Row + (0)), kernel[1][1]);

			rgba_f2 += RGBAColor_32f(input(Column + (1), Row + (-1)), kernel[0][2]);
			rgba_f2 += RGBAColor_32f(input(Column + (2), Row + (-1)), kernel[0][3]);
			rgba_f2 += RGBAColor_32f(input(Column + (1), Row + (0)), kernel[1][2]);
			rgba_f2 += RGBAColor_32f(input(Column + (2), Row + (0)), kernel[1][3]);

			rgba_f3 += RGBAColor_32f(input(Column + (-1), Row + (1)), kernel[2][0]);
			rgba_f3 += RGBAColor_32f(input(Column + (0), Row + (1)), kernel[2][1]);
			rgba_f3 += RGBAColor_32f(input(Column + (-1), Row + (2)), kernel[3][0]);
			rgba_f3 += RGBAColor_32f(input(Column + (0), Row + (2)), kernel[3][1]);

			rgba_f4 += RGBAColor_32f(input(Column + (1), Row + (1)), kernel[2][2]);
			rgba_f4 += RGBAColor_32f(input(Column + (2), Row + (1)), kernel[2][3]);
			rgba_f4 += RGBAColor_32f(input(Column + (1), Row + (2)), kernel[3][2]);
			rgba_f4 += RGBAColor_32f(input(Column + (2), Row + (2)), kernel[3][3]);

			rgba_f1 *= weight.X;
			rgba_f2 *= weight.Y;
			rgba_f3 *= weight.Z;
			rgba_f4 *= weight.W;

			rgba_f1 += rgba_f2;
			rgba_f1 += rgba_f3;
			rgba_f1 += rgba_f4;

			result(X, Y) = rgba_f1.toRGBAColor_8i();
		}

		});
	return true;
}

bool ImageProcessingTools::Zoom_CubicConvolutionSampling4x4(TextureData& input, TextureData& result, const float32_t& magnification, const float32_t& a)
{
	if (input.getRGBA_uint8().size() == 0)//Handle it well, otherwise there will be problems in parallel
		return false;

	result.width = input.width * magnification;
	result.height = input.height * magnification;

	float32_t scaleIndex = 1.0f / magnification;

	auto& resultRGBA = result.getRGBA_uint8();
	resultRGBA.resize(static_cast<size_t>(result.width) * result.height);

	parallel::parallel_for(0u, result.height, [&result, &input, &a, &scaleIndex](uint32_t Y) {

		auto GetFloorIndex = [&scaleIndex](const uint32_t& index)
		{
			return static_cast<int64_t>(floorf(index * scaleIndex));
		};

		auto Formula = ImageProcessingTools::cubicConvolutionZoomFormula;

		auto Row = GetFloorIndex(Y);

		for (auto X = 0u; X < result.width; ++X)
		{

			auto Column = GetFloorIndex(X);

			float32_t dx = X * scaleIndex - Column;
			float32_t dy = Y * scaleIndex - Row;

			//Construct the weight matrix
			floatVec4 kernelX[4];
			floatVec4 kernelY[4];

			//Pay attention to the high and low
			kernelX[0] = floatVec4(Formula(a, 2.0f - dx), Formula(a, 1.0f - dx), Formula(a, 0.0f - dx), Formula(a, -1.0f - dx));
			kernelX[1] = kernelX[0];
			kernelX[2] = kernelX[0];
			kernelX[3] = kernelX[0];

			kernelY[0] = floatVec4(Formula(a, -1.0f - dy));
			kernelY[1] = floatVec4(Formula(a, 0.0f - dy));
			kernelY[2] = floatVec4(Formula(a, 1.0f - dy));
			kernelY[3] = floatVec4(Formula(a, 2.0f - dy));

			kernelX[0] *= kernelY[0];
			kernelX[1] *= kernelY[1];
			kernelX[2] *= kernelY[2];
			kernelX[3] *= kernelY[3];

			const auto& kernel = kernelX;

			// Unrolling loops to enhance performance
			RGBAColor_32f rgba_f(0.0f, 0.0f, 0.0f, 0.0f);

			rgba_f += RGBAColor_32f(input(Column + (-1), Row + (-1)), kernel[0][0]);
			rgba_f += RGBAColor_32f(input(Column + (0), Row + (-1)), kernel[0][1]);
			rgba_f += RGBAColor_32f(input(Column + (1), Row + (-1)), kernel[0][2]);
			rgba_f += RGBAColor_32f(input(Column + (2), Row + (-1)), kernel[0][3]);

			rgba_f += RGBAColor_32f(input(Column + (-1), Row + (0)), kernel[1][0]);
			rgba_f += RGBAColor_32f(input(Column + (0), Row + (0)), kernel[1][1]);
			rgba_f += RGBAColor_32f(input(Column + (1), Row + (0)), kernel[1][2]);
			rgba_f += RGBAColor_32f(input(Column + (2), Row + (0)), kernel[1][3]);

			rgba_f += RGBAColor_32f(input(Column + (-1), Row + (1)), kernel[2][0]);
			rgba_f += RGBAColor_32f(input(Column + (0), Row + (1)), kernel[2][1]);
			rgba_f += RGBAColor_32f(input(Column + (1), Row + (1)), kernel[2][2]);
			rgba_f += RGBAColor_32f(input(Column + (2), Row + (1)), kernel[2][3]);

			rgba_f += RGBAColor_32f(input(Column + (-1), Row + (2)), kernel[3][0]);
			rgba_f += RGBAColor_32f(input(Column + (0), Row + (2)), kernel[3][1]);
			rgba_f += RGBAColor_32f(input(Column + (1), Row + (2)), kernel[3][2]);
			rgba_f += RGBAColor_32f(input(Column + (2), Row + (2)), kernel[3][3]);

			result(X, Y) = rgba_f.toRGBAColor_8i();
		}

		});
	return true;
}

bool ImageProcessingTools::SharpenLaplace3x3(TextureData& input, TextureData& result, const float32_t& strength)
{
	if (input.getRGBA_uint8().size() == 0)//Handle it well, otherwise there will be problems in parallel
		return false;

	result.width = input.width;
	result.height = input.height;

	auto& resultRGBA = result.getRGBA_uint8();
	resultRGBA.resize(input.getRGBA_uint8().size());

	const float32_t factor = -0.01f * strength;
	constexpr float32_t oneHalfRoot = 0.70710678f;
	constexpr float32_t oneHalfRootPlusOne = oneHalfRoot + 1.0f;

	/*
		kernel =
		{
		  -sqrt(0.5) -1 -sqrt(0.5)
		  -1       -outers	-1
		  -sqrt(0.5) -1 -sqrt(0.5)
		}

	*/

	const float32_t& outerNear = factor;
	const float32_t outerFar = factor * oneHalfRoot;
	const float32_t center = 1.0f - (4.0f * oneHalfRootPlusOne) * factor;

	parallel::parallel_for(0u, result.height, [&result, &input, &outerNear, &outerFar, &center](uint32_t Y) {

		for (auto X = 0u; X < result.width; ++X)
		{

			RGBAColor_32f rgba_f1(0.0f, 0.0f, 0.0f, 0.0f);
			RGBAColor_32f rgba_f2(0.0f, 0.0f, 0.0f, 0.0f);

			rgba_f1 += RGBAColor_32f(input(X - 1, Y - 1));
			rgba_f1 += RGBAColor_32f(input(X + 1, Y - 1));
			rgba_f1 += RGBAColor_32f(input(X - 1, Y + 1));
			rgba_f1 += RGBAColor_32f(input(X + 1, Y + 1));

			rgba_f2 += RGBAColor_32f(input(X + 0, Y - 1));
			rgba_f2 += RGBAColor_32f(input(X - 1, Y + 0));
			rgba_f2 += RGBAColor_32f(input(X + 1, Y + 0));
			rgba_f2 += RGBAColor_32f(input(X + 0, Y + 1));

			rgba_f1 *= outerFar;
			rgba_f2 *= outerNear;

			rgba_f1 += RGBAColor_32f(input(X + 0, Y + 0), center);
			rgba_f1 += rgba_f2;

			result(X, Y) = rgba_f1.toRGBAColor_8i();
		}

		});
	return true;
}

bool ImageProcessingTools::SharpenGaussLaplace5x5(TextureData& input, TextureData& result, const float32_t& strength)
{
	if (input.getRGBA_uint8().size() == 0)//Handle it well, otherwise there will be problems in parallel
		return false;

	result.width = input.width;
	result.height = input.height;

	auto& resultRGBA = result.getRGBA_uint8();
	resultRGBA.resize(input.getRGBA_uint8().size());

	float32_t factor = -0.002f * strength;
	/*
		kernel =
		{
		-2 -4 -4 -4 -2
		-4  0  8  0 -4
		-4  8  24 8 -4
		-4  0  8  0 -4
		-2 -4 -4 -4 -2
		}
	*/

	const float32_t& outerl2Far = factor;
	const float32_t outerl2Near = 2.0f * factor;
	const float32_t outerl1Near = -4.0f * factor;
	//const float32_t outerl1Far = 0.0f;
	const float32_t center = 1.0f - 12.0f * factor;

	parallel::parallel_for(0u, result.height, [&result, &input, &outerl2Far, &outerl2Near, &outerl1Near, &center](uint32_t Y) {

		for (auto X = 0u; X < result.width; ++X)
		{
			RGBAColor_32f rgba_f1(0.0f, 0.0f, 0.0f, 0.0f);
			RGBAColor_32f rgba_f2(0.0f, 0.0f, 0.0f, 0.0f);
			RGBAColor_32f rgba_f3(0.0f, 0.0f, 0.0f, 0.0f);

			rgba_f1 += RGBAColor_32f(input(X - 2, Y - 2));
			rgba_f1 += RGBAColor_32f(input(X + 2, Y - 2));
			rgba_f1 += RGBAColor_32f(input(X - 2, Y + 2));
			rgba_f1 += RGBAColor_32f(input(X + 2, Y + 2));

			rgba_f2 += RGBAColor_32f(input(X - 1, Y - 2));
			rgba_f2 += RGBAColor_32f(input(X + 0, Y - 2));
			rgba_f2 += RGBAColor_32f(input(X + 1, Y - 2));
			rgba_f2 += RGBAColor_32f(input(X - 2, Y - 1));
			rgba_f2 += RGBAColor_32f(input(X - 2, Y + 0));
			rgba_f2 += RGBAColor_32f(input(X - 2, Y + 1));
			rgba_f2 += RGBAColor_32f(input(X + 2, Y - 1));
			rgba_f2 += RGBAColor_32f(input(X + 2, Y + 0));
			rgba_f2 += RGBAColor_32f(input(X + 2, Y + 1));
			rgba_f2 += RGBAColor_32f(input(X - 1, Y + 2));
			rgba_f2 += RGBAColor_32f(input(X + 0, Y + 2));
			rgba_f2 += RGBAColor_32f(input(X + 1, Y + 2));

			rgba_f3 += RGBAColor_32f(input(X + 0, Y - 1));
			rgba_f3 += RGBAColor_32f(input(X - 1, Y + 0));
			rgba_f3 += RGBAColor_32f(input(X + 1, Y + 0));
			rgba_f3 += RGBAColor_32f(input(X + 0, Y + 1));

			rgba_f1 *= outerl2Far;
			rgba_f2 *= outerl2Near;
			rgba_f3 *= outerl1Near;

			rgba_f1 += RGBAColor_32f(input(X + 0, Y + 0), center);
			rgba_f1 += rgba_f2;
			rgba_f1 += rgba_f3;

			result(X, Y) = rgba_f1.toRGBAColor_8i();
		}

		});
	return true;
}

bool ImageProcessingTools::AecsHdrToneMapping(TextureData& inputOutput, const float32_t& lumRatio)
{
	if (inputOutput.getRGBA_uint8().size() == 0)//Handle it well, otherwise there will be problems in parallel
		return false;

	//not need this time
	inputOutput.clearImage();

	parallel::parallel_for(0u, inputOutput.height, [&inputOutput, &lumRatio](uint32_t Y) {

		for (auto X = 0u; X < inputOutput.width; ++X)
		{
			RGBAColor_32f color(inputOutput(X, Y));

			ImageProcessingTools::ACESToneMappingColor(color, lumRatio);

			inputOutput(X, Y) = color.toRGBAColor_8i();
		}

		});
	return true;
}

bool ImageProcessingTools::ReverseColorImage(TextureData& inputOutput)
{
	if (inputOutput.getRGBA_uint8().size() == 0)//Handle it well, otherwise there will be problems in parallel
		return false;

	//not need this time
	inputOutput.clearImage();

	parallel::parallel_for(0u, inputOutput.height, [&inputOutput](uint32_t Y) {

		for (auto X = 0u; X < inputOutput.width; ++X)
		{
			ReverseColor(inputOutput(X, Y));
		}

		});
	return true;
}

bool ImageProcessingTools::Grayscale(TextureData& input, TextureData& result)
{
	if (input.getRGBA_uint8().size() == 0)//Handle it well, otherwise there will be problems in parallel
		return false;

	//not need this time
	input.clearImage();

	result.width = input.width;
	result.height = input.height;
	result.image.resize(input.getRGBA_uint8().size());

	parallel::parallel_for(0u, input.height, [&input, &result](uint32_t Y) {

		size_t offset = static_cast<size_t>(input.width) * Y;

		for (auto X = 0u; X < input.width; ++X)
		{
			ImageProcessingTools::GrayColor(input(X, Y), result.image[offset + X]);
		}

		});
	return true;
}

bool ImageProcessingTools::ChannelGrayScale(TextureData& input, TextureData& resultR, TextureData& resultG, TextureData& resultB)
{
	if (input.image.size() == 0)
		return false;

	resultR.width = input.width;
	resultR.height = input.height;
	resultG.width = input.width;
	resultG.height = input.height;
	resultB.width = input.width;
	resultB.height = input.height;

	resultR.image.resize(input.image.size() >> 2);
	resultG.image.resize(input.image.size() >> 2);
	resultB.image.resize(input.image.size() >> 2);

	for (size_t i = 0; i < input.image.size(); i += 4)
	{
#if LITTLE_ENDIAN
		resultR.image[i >> 2] = input.image[i];
		resultG.image[i >> 2] = input.image[i + 1];
		resultB.image[i >> 2] = input.image[i + 2];

#else
		resultB.image[i >> 2] = input.image[i + 1];
		resultG.image[i >> 2] = input.image[i + 2];
		resultR.image[i >> 2] = input.image[i + 3];
#endif
	}
	return true;
}

bool ImageProcessingTools::VividnessAdjustment(TextureData& inputOutput, const float32_t& vividRatio)
{
	if (inputOutput.getRGBA_uint8().size() == 0)//Handle it well, otherwise there will be problems in parallel
		return false;

	//not need this time
	inputOutput.clearImage();

	parallel::parallel_for(0u, inputOutput.height, [&inputOutput, &vividRatio](uint32_t Y) {

		for (auto X = 0u; X < inputOutput.width; ++X)
		{
			RGBAColor_32f color(inputOutput(X, Y));

			ImageProcessingTools::VividnessAdjustmentColor(color, vividRatio);

			inputOutput(X, Y) = color.toRGBAColor_8i();
		}

		});
	return true;
}

bool ImageProcessingTools::NatualVividnessAdjustment(TextureData& inputOutput, const float32_t& vividRatio)
{
	if (inputOutput.getRGBA_uint8().size() == 0)//Handle it well, otherwise there will be problems in parallel
		return false;

	//not need this time
	inputOutput.clearImage();

	parallel::parallel_for(0u, inputOutput.height, [&inputOutput, &vividRatio](uint32_t Y) {

		for (auto X = 0u; X < inputOutput.width; ++X)
		{
			RGBAColor_32f color(inputOutput(X, Y));

			ImageProcessingTools::NatualVividnessAdjustmentColor(color, vividRatio);

			inputOutput(X, Y) = color.toRGBAColor_8i();
		}

		});
	return true;
}

bool ImageProcessingTools::Binarization(TextureData& input, TextureData& result, const float32_t& threshold)
{
	if (input.getRGBA_uint8().size() == 0)//Handle it well, otherwise there will be problems in parallel
		return false;

	//not need this time
	input.clearImage();

	result.width = input.width;
	result.height = input.height;
	result.image.resize(input.getRGBA_uint8().size());

	parallel::parallel_for(0u, input.height, [&input, &result, &threshold](uint32_t Y) {

		size_t offset = static_cast<size_t>(input.width) * Y;

		for (auto X = 0u; X < input.width; ++X)
		{
			ImageProcessingTools::BinarizationColor(input(X, Y), threshold, result.image[offset + X]);
		}

		});
	return true;
}

bool ImageProcessingTools::Quaternization(TextureData& input, TextureData& result, const float32_t& threshold)
{
	if (input.getRGBA_uint8().size() == 0)//Handle it well, otherwise there will be problems in parallel
		return false;

	//not need this time
	input.clearImage();

	result.width = input.width;
	result.height = input.height;
	result.image.resize(input.getRGBA_uint8().size());


	parallel::parallel_for(0u, input.height, [&input, &result, &threshold](uint32_t Y) {

		size_t offset = static_cast<size_t>(input.width) * Y;

		for (auto X = 0u; X < input.width; ++X)
		{
			ImageProcessingTools::QuaternizationColor(input(X, Y), threshold, result.image[offset + X]);
		}

		});
	return true;
}

bool ImageProcessingTools::Hexadecimalization(TextureData& input, TextureData& result)
{
	if (input.getRGBA_uint8().size() == 0)//Handle it well, otherwise there will be problems in parallel
		return false;

	//not need this time
	input.clearImage();

	result.width = input.width;
	result.height = input.height;
	result.image.resize(input.getRGBA_uint8().size());

	parallel::parallel_for(0u, input.height, [&input, &result](uint32_t Y) {

		size_t offset = static_cast<size_t>(input.width) * Y;

		for (auto X = 0u; X < input.width; ++X)
		{
			ImageProcessingTools::HexadecimalizationColor(input(X, Y), result.image[offset + X]);
		}

		});
	return true;
}

bool ImageProcessingTools::SurfaceBlur(TextureData& input, TextureData& result, const int32_t& radius, const float32_t& threshold)
{
	if (input.getRGBA_uint8().size() == 0)//Handle it well, otherwise there will be problems in parallel
		return false;

	result.width = input.width;
	result.height = input.height;

	auto& resultRGBA = result.getRGBA_uint8();
	resultRGBA.resize(input.getRGBA_uint8().size());

	const float32_t denominator = 0.40f / threshold;

	parallel::parallel_for(0u, result.height, [&result, &input, &radius, &denominator](uint32_t Y) {

		auto toGray = [](const RGBAColor_32f& rgba_f)
		{
			return fabsf((rgba_f.R + rgba_f.G + rgba_f.B) * 0.33333f);
		};

		for (auto X = 0u; X < result.width; ++X)
		{
			RGBAColor_32f center(input(X, Y));
			RGBAColor_32f pixelSum(0.0f, 0.0f, 0.0f, 0.0f);

			float32_t sum = 0.0f;
			float32_t weight;

			for (int64_t h = -radius; h <= radius; ++h)
			{
				for (int64_t w = -radius; w <= radius; ++w)
				{
					RGBAColor_32f pixel(input(X + w, Y + h));
					pixel -= center;

					weight = 1.0f - (toGray(pixel) * denominator);

					sum += weight;

					pixelSum += RGBAColor_32f(input(X + w, Y + h), weight);
				}
			}

			pixelSum /= sum;
			pixelSum.A = center.A;

			result(X, Y) = pixelSum.toRGBAColor_8i();
		}

		});
	return true;
}

bool ImageProcessingTools::SobelEdgeEnhancement(TextureData& input, TextureData& result, const float32_t& thresholdMin, const float32_t& thresholdMax, const float32_t& strength)
{
	if (input.getRGBA_uint8().size() == 0)//Handle it well, otherwise there will be problems in parallel
		return false;

	result.width = input.width;
	result.height = input.height;

	auto& resultRGBA = result.getRGBA_uint8();
	resultRGBA.resize(input.getRGBA_uint8().size());

	parallel::parallel_for(0u, result.height, [&result, &input, &thresholdMin, &thresholdMax, &strength](uint32_t Y) {

		for (auto X = 0u; X < result.width; ++X)
		{
			RGBAColor_32f Gx(0.0f, 0.0f, 0.0f, 0.0f);
			RGBAColor_32f Gy(0.0f, 0.0f, 0.0f, 0.0f);

			Gx += RGBAColor_32f(input(X - 1, Y - 1), -1.0f);
			Gx += RGBAColor_32f(input(X + 1, Y - 1), 1.0f);
			Gx += RGBAColor_32f(input(X - 1, Y + 0), -2.0f);
			Gx += RGBAColor_32f(input(X + 1, Y + 0), 2.0f);
			Gx += RGBAColor_32f(input(X - 1, Y + 1), -1.0f);
			Gx += RGBAColor_32f(input(X + 1, Y + 1), 1.0f);

			Gy += RGBAColor_32f(input(X - 1, Y - 1), 1.0f);
			Gy += RGBAColor_32f(input(X + 0, Y - 1), 2.0f);
			Gy += RGBAColor_32f(input(X + 1, Y - 1), 1.0f);
			Gy += RGBAColor_32f(input(X - 1, Y + 1), -1.0f);
			Gy += RGBAColor_32f(input(X + 0, Y + 1), -2.0f);
			Gy += RGBAColor_32f(input(X + 1, Y + 1), -1.0f);

			float32_t gx = Gx.R + Gx.G + Gx.B;
			float32_t gy = Gy.R + Gy.G + Gy.B;

			float32_t G = sqrtf((gx * gx) + (gy * gy)) * 0.33333f;

			if (G < thresholdMin || G > thresholdMax)
			{
				G = 1.0f;
			}
			else
			{
				G *= strength;
				G = 1.0f - G;
				Clamp(G, 0.0f, 1.0f);
			}

			float32_t alpha = input(X, Y).A;
			RGBAColor_32f  center(input(X, Y), G);

			result(X, Y) = center.toRGBAColor_8i();
			result(X, Y).A = alpha;
		}

		});
	return true;
}

bool ImageProcessingTools::MosaicPixelation(TextureData& inputOutput, const uint32_t& sideLength)
{
	if (inputOutput.getRGBA_uint8().size() == 0)//Handle it well, otherwise there will be problems in parallel
		return false;

	//not need this time
	inputOutput.clearImage();

	parallel::parallel_for(0u, inputOutput.height, sideLength, [&inputOutput, &sideLength](uint32_t Y) {

		//add column
		for (auto h = 0u; (h < sideLength) && ((Y + h) < inputOutput.height); ++h)
		{
			for (auto X = 0u; X < inputOutput.width; X += sideLength)
			{
				RGBAColor_32f columnSum(inputOutput(X, (Y + h)));

				for (auto w = 1; w < sideLength; ++w)
				{
					columnSum += RGBAColor_32f(inputOutput(X + w, (Y + h)));
				}

				columnSum /= sideLength;

				inputOutput(X, (Y + h)) = columnSum.toRGBAColor_8i();
			}
		}

		//add row
		for (auto X = 0u; X < inputOutput.width; X += sideLength)
		{
			RGBAColor_32f rowSum(inputOutput(X, Y));

			for (auto h = 1u; h < sideLength; ++h)
			{
				rowSum += RGBAColor_32f(inputOutput(X, Y + h));
			}

			rowSum /= sideLength;

			inputOutput(X, Y) = rowSum.toRGBAColor_8i();

			//set value to column
			for (auto w = 1u; (X + w) < inputOutput.width; ++w)
			{
				inputOutput((X + w), Y) = inputOutput(X, Y);
			}
		}

		//copy values
		for (auto h = 1u; (h < sideLength) && ((Y + h) < inputOutput.height); ++h)
		{
			std::copy(&inputOutput(0u, Y), &inputOutput(0u, Y) + inputOutput.width,
				&inputOutput(0u, Y) + h * inputOutput.width);
		}

		});
	return true;
}

bool ImageProcessingTools::MixedPictures(TextureData& inputOutside, TextureData& inputInside,
	TextureData& result,
	void (*filteringMethod)(const RGBAColor_8i& colorOut, byte& resultOut, const RGBAColor_8i& colorIn, byte& resultIn))
{
	if (inputOutside.getRGBA_uint8().size() == 0u || inputInside.getRGBA_uint8().size() == 0u)
		return false;

	result.width = Min(inputOutside.width, inputInside.width);
	result.height = Min(inputOutside.height, inputInside.height);

	result.image.resize((static_cast<size_t>(result.width) * result.height) << 1u);//gray and alpha

	parallel::parallel_for(0u, result.height, [&inputOutside, &inputInside, &result, &filteringMethod](uint32_t Y) {

		size_t offset = (static_cast<size_t>(Y) * result.width) << 1u;

		for (auto X = 0u; X < result.width; ++X)
		{
			byte gray1;
			byte gray2;

			filteringMethod(inputOutside(X, Y), gray1, inputInside(X, Y), gray2);
			ImageProcessingTools::MixedPicturesColor(gray1, gray2, result.image[offset], result.image[offset + 1u]);

			offset += 2;
		}

		});
	return true;
}

bool ImageProcessingTools::PixelToRGB3x3(TextureData& input, TextureData& result, const float32_t& brightness)
{
	if (input.getRGBA_uint8().size() == 0u)
		return false;

	input.clearImage();

	result.width = input.width * 3u;
	result.height = input.height * 3u;

	result.getRGBA_uint8().resize(static_cast<size_t>(result.width) * result.height);

	parallel::parallel_for(0u, input.height, [&input, &result, &brightness](uint32_t Y)
		{
			for (auto X = 0u; X < input.width; ++X)
			{
				result(X * 3, Y * 3) = input(X, Y);
				result(X * 3 + 1, Y * 3) = input(X, Y);
				result(X * 3 + 2, Y * 3) = input(X, Y);

				result(X * 3, Y * 3).G *= brightness;
				result(X * 3, Y * 3).B *= brightness;

				result(X * 3 + 1, Y * 3).R *= brightness;
				result(X * 3 + 1, Y * 3).B *= brightness;

				result(X * 3 + 2, Y * 3).R *= brightness;
				result(X * 3 + 2, Y * 3).G *= brightness;
			}

			//use copy to boost
			std::copy(reinterpret_cast<byte*>(&result(0, Y * 3)), reinterpret_cast<byte*>(&result(0, Y * 3 + 1)), reinterpret_cast<byte*>(&result(0, Y * 3 + 1)));
			std::copy(reinterpret_cast<byte*>(&result(0, Y * 3)), reinterpret_cast<byte*>(&result(0, Y * 3 + 1)), reinterpret_cast<byte*>(&result(0, Y * 3 + 2)));
		});

	return true;
}

bool ImageProcessingTools::Encryption_xor_reverse(TextureData& inputOutput, const uint32_t& key)
{
	if (inputOutput.getRGBA_uint8().size() == 0)//Handle it well, otherwise there will be problems in parallel
		return false;

	//not need this time
	inputOutput.clearImage();

	parallel::parallel_for(0u, inputOutput.height, [&inputOutput, key](uint32_t Y) {
		uint32_t tempKey = (key << (Y % 16)) | (key >> (32 - (Y % 16)));

		for (auto X = 0u; X < inputOutput.width; ++X)
		{
			auto& color = inputOutput(X, Y);
			color.R ^= tempKey;
			color.G ^= tempKey >> 8;
			color.B ^= tempKey >> 16;

			tempKey = ((tempKey << (X % 16)) | (tempKey >> (32 - (X % 16))));

			if ((X + Y) % 2 == 0) {
				ImageProcessingTools::ReverseColor(color);
			}
		}

		});
	return true;
}

bool ImageProcessingTools::HSLAdjustment(TextureData& inputOutput, const float32_t& hueRatio, const float32_t& saturationRatio, const float32_t& lightnessRatio)
{
	if (inputOutput.getRGBA_uint8().size() == 0)//Handle it well, otherwise there will be problems in parallel
		return false;

	//not need this time
	inputOutput.clearImage();

	parallel::parallel_for(0u, inputOutput.height, [&inputOutput, &hueRatio, &saturationRatio, &lightnessRatio](uint32_t Y) {

		for (auto X = 0u; X < inputOutput.width; ++X)
		{
			RGBAColor_32f color(inputOutput(X, Y));

			ImageProcessingTools::HSLAdjustmentColor(color, hueRatio, saturationRatio, lightnessRatio);

			inputOutput(X, Y) = color.toRGBAColor_8i();
		}

		});
	return true;
}