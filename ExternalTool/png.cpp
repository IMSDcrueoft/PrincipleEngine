/*
PNG Processor Version 20220322

Copyright (c) 2005-2022 IM&SD

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. Do not distort the source of this software; you cannot Claiming that you wrote the original software.
if you use this software In a product, the confirmation in the product documentation will be Appreciated but not required.

2. Changed source versions must be clearly identified and must not be Mistaken for original software.

3. This notice may not be deleted or changed from any source distribute.

4. The LodePng header files and source files are modified from the project of GItHub Lode Vandevenne,
so you should also comply with the requirements of its header declaration
*/

#include"png.h"

void PngProcessingTools::importFile(TextureData& data, std::filesystem::path& pngfile)
{
	auto path = AdaptString::toString(pngfile.wstring());

	uint32_t error = lodepng::decode(data.image, data.width, data.height, path);

	//if there's an error, display it
	if (error)
	{
		std::cout << "Decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
		exit(0);
	}
}

void PngProcessingTools::exportFile(TextureData& result, std::wstring& resultname,const LodePNGColorType& colorType,const uint32_t& bitdepth)
{
	auto path = AdaptString::toString(resultname);

	if ((static_cast<size_t>(result.width) * result.height * 4) >= 0xFF'FF'FF'FF)
	{
		assert(false && "Byte size is bigger than UINT32,need cut.");
		std::cout << "Byte size is bigger than UINT32,need cut." << std::endl;
	}
	else
	{
		uint32_t error = lodepng::encode(path, result.image, result.width, result.height,colorType,bitdepth);

		if (error)
		{
			std::cout << "Encoder error " << error << ": " << lodepng_error_text(error) << std::endl;
			exit(0);
		}

		std::cout << "Result filename:" << path << '\n' << std::endl;
	}
}

void PngProcessingTools::exportFile(const byte* result, const uint32_t& width, const uint32_t& height, std::wstring& resultname, const LodePNGColorType& colorType, const uint32_t& bitdepth)
{
	auto path = AdaptString::toString(resultname);

	if ((static_cast<size_t>(width) * height * 4) >= 0xFF'FF'FF'FF)
	{
		assert(false && "Byte size is bigger than UINT32,need cut.");
		std::cout << "Byte size is bigger than UINT32,need cut." << std::endl;
	}
	else
	{
		uint32_t error = lodepng::encode(path, result, width, height, colorType, bitdepth);

		if (error)
		{
			std::cout << "Encoder error " << error << ": " << lodepng_error_text(error) << std::endl;
			exit(0);
		}

		std::cout << "Result filename:" << path << '\n' << std::endl;
	}
}

void PngProcessingTools::help()
{
	std::cout << "Check Help Info.\n\n"
		<< "Help:[---] is a prompt, not an input.[DF] means it has a default value.\n"
		<< "Startup parameters--->\n"
		<< "[    Default Zoom    ]: z     \n"
		<< "[    Bicubic Zoom    ]: Z     \n"
		<< "[   Laplace Sharpen  ]: s     \n"
		<< "[GaussLaplace Sharpen]: S     \n"
		<< "[    Tone Mapping    ]: t or T\n"
		<< "[     Gray Scale     ]: g     \n"
		<< "[ Channle Gray Scale ]: G     \n"
		<< "[  Sobel Edge Filter ]: f     \n"
		<< "[ Surface Blur Filter]: F     \n"
		<< "[    Reverse Color   ]: r or R\n"
		<< "[    Binarization    ]: b or B\n"
		<< "[   Quaternization   ]: q or Q\n"
		<< "[ Hexadecimalization ]: h or H\n"
		<< "[Vividness Adjustment]: v     \n"
		<< "[ Natual Vivid Adjust]: V     \n"
		<< "[      Block Cut     ]: c     \n"
		<< "[     Cut horizon    ]: C     \n"
		<< "[       Mosaic       ]: m     \n"
		<< "[   Mixed Pictures   ]: M     \n"
		<< '\n'
		<< "Input Sample-->\n"
		<< "./pngProcessor.exe filename.png z[default zoom] 1.0[zoom ratio:DF] 0.5[center weight:DF] 2[Exponent:DF]\n"
		<< "[default zoom]\n"
		<< "[zoom ratio(from 0.001 to 32.0)]\n"
		<< "[center weight(from 0.25 to 13.0, 0.25:similar to MSAAx16, 1.0 : similar to bilinear, >1:sharp)]\n"
		<< "[Exponent(from 1 to 4:0.5, 1.0, 2.0, 4.0)]\n"
		<< '\n'
		<< "./pngProcessor.exe filename.png Z[bicubic zoom] -1.0[formula factor:DF]\n"
		<< "[bicubic zoom]\n"
		<< "[zoom ratio(from 0.001 to 32.0)]\n"
		<< "[formula factor(from -3.0 to -0.1)]\n"
		<< '\n'
		<< "./pngProcessor.exe filename.png t[tone mapping] 2.0[lumming ratio:DF]\n"
		<< "[tone mapping]\n"
		<< "[lumming ratio(from 0.1 to 16.0)]\n"
		<< '\n'
		<< "./pngProcessor.exe filename.png g[gray scale]\n"
		<< "[gray scale]\n"
		<< '\n'
		<< "./pngProcessor.exe filename.png G[channle gray scale]\n"
		<< "[channle gray scale]\n"
		<< '\n'
		<< "./pngProcessor.exe filename.png b[binarization] 0.5[binarization threshold:DF]\n"
		<< "[binarization]\n"
		<< "[binarization threshold(from 0 to 1-1/255)]\n"
		<< '\n'
		<< "./pngProcessor.exe filename.png q[quaternization] 0.5[quaternization threshold:DF]\n"
		<< "[quaternization]\n"
		<< "[quaternization threshold(from 0 to 1]\n"
		<< '\n'
		<< "./pngProcessor.exe filename.png h[hexadecimalization]\n"
		<< "[hexadecimalization]\n"
		<< '\n'
		<< "./pngProcessor.exe filename.png v[vividness Adjustment] 0.2[vivid ratio:DF]\n"
		<< "[vividness Adjustment]\n"
		<< "[vivid ratio(from -1.0 to 254.0)]\n"
		<< '\n'
		<< "./pngProcessor.exe filename.png V[natual vividness Adjustment] 0.2[vivid ratio:DF]\n"
		<< "[natual vividness Adjustment]\n"
		<< "[vivid ratio(from -1.0 to 254.0)]\n"
		<< '\n'
		<< "./pngProcessor.exe filename.png r[reverse color]\n"
		<< "[reverse color]\n"
		<< '\n'
		<< "./pngProcessor.exe filename.png s[laplace sharpen] 4.0[sharpen ratio:DF]\n"
		<< "[laplace sharpen]\n"
		<< "[sharpen ratio(from 1 to 1000)]\n"
		<< '\n'
		<< "./pngProcessor.exe filename.png S[gauss-laplace sharpen] 4.0[sharpen ratio:DF]\n"
		<< "[gauss-laplace sharpen]\n"
		<< "[sharpen ratio(from 1 to 1000)]\n"
		<< '\n'
		<< "./pngProcessor.exe filename.png f[sobel edge enhancement Filter] 0.5[thresholdMix:DF] 1.0f[thresholdMax:DF] 1.0f[edge strength:DF]\n"
		<< "[surface blur filter]\n"
		<< "[thresholdMix(from 0 to 1)]\n"
		<< "[thresholdMax(from 0 to 1)]\n"
		<< "[Edge strength(from 0.05 to 10)]"
		<< '\n'
		<< "./pngProcessor.exe filename.png F[surface blur filter] 0.5[threshold:DF] 2[radius:DF]\n"
		<< "[surface blur filter]\n"
		<< "[threshold(from 1/255 to 1)]\n"
		<< "[radius(from 1 to 24)]\n"
		<< '\n'
		<< "./pngProcessor.exe filenameOut.png m[Mosaic] 4[mosaic side length:DF]\n"
		<< "[Mosaic]\n"
		<< "[mosaic side length(from 2 to 512)]\n"
		<< '\n'
		<< "./pngProcessor.exe filenameOut.png M[mixed pictures] 1[WorkMode] filenameIn.png\n"
		<< "[mixed pictures]\n"
		<< "[workMode(from 1 to 4,1->1:1,2->1:2,3->2:1,4->1:3)]\n"
		<< '\n'
		<< "./pngProcessor.exe filename.png c[cut] 1024[Horizontal Interval:DF] 1024[Vertical Interval:DF]\n"
		<< "[cut]\n"
		<< "[Horizontal Interval(>0)]\n"
		<< "[Vertical Interval(>0)]\n"
		<< '\n'
		<< "./pngProcessor.exe filename.png C[cut horizon] 1024[Vertical Interval:DF]\n"
		<< "[cut horizon]\n"
		<< "[Vertical Interval(>0)]"
		<< std::endl;
}

void PngProcessingTools::commandStartUps(int32_t argCount, STR argValues[])
{
	std::filesystem::path pngfile;
	std::filesystem::path pngfile2;
	char mode = (char)Mode::unknown;
	clockTimer timer;
	std::istringstream iss;

	timer.TimerStart();

	if (argCount == 1)
	{
		std::cout << "No image or parameters entered!\n";
		help();
		exit(0);
	}
	else
		if (argCount > 1)
		{
			pngfile = argValues[1];
			std::cout << "Input filename:" << pngfile << '\n' << std::endl;
		}

	if (argCount == 2)
	{
		std::cout << "Too few parameters.\n";
		help();
		exit(0);
	}

	iss.str(argValues[2]);
	iss >> mode;

	float32_t param1 = 1.0f;
	float32_t param2 = 0.64f;
	float32_t param3 = 1.0f;
	uint32_t exponent = (uint32_t)ImageProcessingTools::Exponent::square;

	uint32_t interval_horizontal = 1024u;
	uint32_t interval_vertical = 1024u;

	int32_t radius = 1;

	auto GetParam = [&iss, &argValues](const uint32_t& id, auto& target)
	{
		iss.clear();
		iss.str(argValues[id]);
		iss >> target;
	};

	switch (mode)
	{
	case (int)Mode::zoom:
		param1 = 0.5f;
		param2 = 0.64f;

		if (argCount > 3)
		{
			GetParam(3, param1);

			if (argCount > 4)
			{
				GetParam(4, param2);

				if (argCount > 5)
				{
					GetParam(5, exponent);
				}
			}
		}

		std::cout << "Input exponent factor:" << exponent << '\n';
		Clamp(exponent, 1, 4);
		std::cout << "Adoption exponent factor:";
		
		if (exponent == 1)
		{
			std::cout << "Half\n";
		}
		else
			if (exponent == 2)
			{
				std::cout << "One\n";
			}
			else
				if (exponent == 3)
				{
					std::cout << "Square\n";
				}
				else
					if (exponent == 4)
					{
						std::cout << "Quartet\n";
					}

		PngProcessingTools::zoomProgramDefault(param1, pngfile, param2, (ImageProcessingTools::Exponent)exponent);
		break;

	case (int)Mode::Zoom:
		param1 = 2.0f;
		param2 = -0.5f;

		if (argCount > 3)
		{
			GetParam(3, param1);

			if (argCount > 4)
			{
				GetParam(4, param2);
			}
		}

		PngProcessingTools::zoomProgramCubicConvolution(param1, pngfile, param2);
		break;

	case (int)Mode::sharpen:
		param1 = 15.0f;

		if (argCount > 3)
		{
			GetParam(3, param1);
		}

		PngProcessingTools::laplaceSharpenProgram(param1, pngfile);
		break;

	case (int)Mode::Sharpen:
		param1 = 15.0f;

		if (argCount > 3)
		{
			iss.clear();
			iss.str(argValues[3]);
			iss >> param1;
		}
		PngProcessingTools::gaussLaplaceSharpenProgram(param1, pngfile);
		break;

	case (int)Mode::toneMapping:
	case (int)Mode::ToneMapping:
		param1 = 1.0f;

		if (argCount > 3)
		{
			GetParam(3, param1);
		}

		PngProcessingTools::hdrToneMappingColorProgram(param1, pngfile);
		break;

	case (int)Mode::grayScale:
		PngProcessingTools::grayColorProgram(pngfile);
		break;

	case (int)Mode::GrayScale:
		PngProcessingTools::channelGrayColorProgram(pngfile);
		break;

	case (int)Mode::reverseColor:
	case (int)Mode::ReverseColor:
		PngProcessingTools::reverseColorProgram(pngfile);
		break;

	case (int)Mode::vividness:
		param1 = 0.2f;

		if (argCount > 3)
		{
			GetParam(3, param1);
		}

		PngProcessingTools::vividnessAdjustmentColorProgram(param1, pngfile);
		break;

	case (int)Mode::Vividness:
		param1 = 0.2f;

		if (argCount > 3)
		{
			GetParam(3, param1);
		}

		PngProcessingTools::natualvividnessAdjustmentColorProgram(param1, pngfile);
		break;

	case (int)Mode::binarization:
	case (int)Mode::Binarization:
		param1 = 0.5f;

		if (argCount > 3)
		{
			GetParam(3, param1);
		}

		PngProcessingTools::binarizationColorProgram(param1, pngfile);
		break;

	case (int)Mode::quaternization:
	case (int)Mode::Quaternization:
		param1 = 0.5f;

		if (argCount > 3)
		{
			GetParam(3, param1);
		}

		PngProcessingTools::quaternizationColorProgram(param1,pngfile);
		break;

	case (int)Mode::hexadecimalization:
	case (int)Mode::Hexadecimalization:
		PngProcessingTools::hexadecimalizationColorProgram(pngfile);
		break;

	case (int)Mode::cut:
		if (argCount > 3)
		{
			GetParam(3,interval_horizontal);

			if (argCount > 4)
			{
				GetParam(4,interval_vertical);
			}
		}

		PngProcessingTools::blockSplitProgram(interval_horizontal, interval_vertical, pngfile);
		break;

	case (int)Mode::Cut:
		if (argCount > 3)
		{
			GetParam(3, interval_vertical);
		}

		PngProcessingTools::fastSplitHorizonProgram(interval_vertical, pngfile);
		break;

	case (int)Mode::mosaicPixelation:
		exponent = 4u;

		if (argCount > 3)
		{
			GetParam(3, exponent);
		}

		PngProcessingTools::mosaicPixelationProgram(exponent, pngfile);
		break;

	case (int)Mode::MixedGraph:
		exponent = 1;

		if (argCount > 3)
		{
			GetParam(3,exponent);

			if (argCount > 4)
			{
				GetParam(4, pngfile2);
			}
		}

		if(argCount <= 4)
		{
			std::cout << "No Inside picture,Wrong!" << std::endl;
			exit(0);
		}
		PngProcessingTools::mixedPicturesProgram(exponent, pngfile, pngfile2);
		break;

	case (int)Mode::filter:
		param1 = 0.0f;
		param2 = 1.0f;
		param3 = 1.0f;

		if (argCount > 3)
		{
			GetParam(3, param1);

			if (argCount > 4)
			{
				GetParam(4,param2);
				
				if (argCount > 5)
				{
					GetParam(5, param3);
				}
			}
		}

		PngProcessingTools::sobelEdgeEnhancementProgram(param3, pngfile, param1, param2);
		break;

	case (int)Mode::Filter:
		param1 = 0.5f;
		radius = 1;

		if (argCount > 3)
		{
			GetParam(3, param1);

			if (argCount > 4)
			{
				GetParam(4,radius);
			}
		}

		PngProcessingTools::surfaceBlurfilterProgram(param1, pngfile, radius);
		break;

	default:
		std::cout << "Error:unknown working mode.\n";
		help();
		exit(0);
		break;
	}

	timer.TimerStop();

	std::cout << "End processing . . .\n"
		<< "Time used:" << timer.getTime() << "(second).\n" << std::endl;
}

void PngProcessingTools::zoomProgramDefault(float32_t& zoomRatio, std::filesystem::path& pngfile,float32_t& CenterWeight,const Exponent& exponent)
{
	std::cout << "Zoom Default:\n"
		<<"Input zoom factor:" << zoomRatio << '\n';

	Clamp(zoomRatio, 0.001f, 32.0f);
	std::cout << "Adoption zoom factor:" << zoomRatio << '\n';

	std::cout<< "Input center weight:" << CenterWeight << '\n';

	Clamp(CenterWeight, 0.25f, 13.0f);

	std::cout << "Adoption center weight:" << CenterWeight << '\n'
		<< "Start processing . . ." << std::endl;


	TextureData image, result;
	importFile(image, pngfile);


	void (*WeightEffact)(const float32_t&, const float32_t&, floatVec4&) = nullptr;

	if (exponent == Exponent::half)
	{
		WeightEffact = ImageProcessingTools::weightEffectHalf;
	}
	else
		if (exponent == Exponent::one)
		{
			WeightEffact = ImageProcessingTools::weightEffectOne;
		}
		else
			if (exponent == Exponent::quartet)
			{
				WeightEffact = ImageProcessingTools::weightEffectQuartet;
			}
			else
			{
				WeightEffact = ImageProcessingTools::weightEffectSquare;
			}

	if (ImageProcessingTools::Zoom_DefaultSampling4x4(image, result, zoomRatio, CenterWeight, WeightEffact))
	{
		image.clear();

		std::wstring resultname;
		resultname.append(pngfile.parent_path()).append(L"/").append(pngfile.stem())
			.append(L"_zoom_x").append(std::to_wstring(zoomRatio))
			.append(L"_centerWeight_").append(std::to_wstring(CenterWeight))
			.append(L"_Exponent_Mode_").append(std::to_wstring((uint32_t)exponent))
			.append(pngfile.extension());

#if LITTLE_ENDIAN
		exportFile(reinterpret_cast<byte*>(result.getRGBA_uint8().data()), result.width, result.height, resultname);
#else
		//load result into stream to save to file
		result.loadRGBAtoByteStream();
		result.clearRGBA_uint8();

		exportFile(result, resultname);
#endif
	}
	else
	{
		std::cout << "Something wrong in convert." << std::endl;
		exit(0);
	}
}

void PngProcessingTools::zoomProgramCubicConvolution(float32_t& zoomRatio, std::filesystem::path& pngfile, float32_t& a)
{
	std::cout << "Zoom Cubic:\n"
		<< "Input zoom factor:" << zoomRatio << '\n';

	Clamp(zoomRatio, 0.001f, 32.0f);
	std::cout << "Adoption zoom factor:" << zoomRatio << '\n';

	std::cout << "Input formula factor:" << a << '\n';

	Clamp(a, -3.0f, -0.1f);

	std::cout << "Adoption formula factor:" << a << '\n'
		<< "Start processing . . ." << std::endl;


	TextureData image, result;
	importFile(image, pngfile);

	if (ImageProcessingTools::Zoom_CubicConvolutionSampling4x4(image, result, zoomRatio, a))
	{
		image.clear();

		std::wstring resultname;
		resultname.append(pngfile.parent_path()).append(L"/").append(pngfile.stem())
			.append(L"_Zoom_x").append(std::to_wstring(zoomRatio))
			.append(L"_cubicFactor_").append(std::to_wstring(a))
			.append(pngfile.extension());

#if LITTLE_ENDIAN
		exportFile(reinterpret_cast<byte*>(result.getRGBA_uint8().data()), result.width, result.height, resultname);
#else
		//load result into stream to save to file
		result.loadRGBAtoByteStream();
		result.clearRGBA_uint8();

		exportFile(result, resultname);
#endif
	}
	else
	{
		std::cout << "Something wrong in convert." << std::endl;
		exit(0);
	}
}

void PngProcessingTools::laplaceSharpenProgram(float32_t& sharpenRatio, std::filesystem::path& pngfile)
{
	std::cout << "Laplace Sharpen:\n" 
		<< "Input sharpen factor:" << sharpenRatio << '\n' << std::endl;

	Clamp(sharpenRatio, 1.0f, 1000.0f);

	std::cout << "Adoption sharpen factor:" << sharpenRatio << "%\n"
		<< "Start processing . . ." << std::endl;

	TextureData image, result;
	importFile(image, pngfile);

	if (ImageProcessingTools::SharpenLaplace3x3(image, result, sharpenRatio))
	{
		image.clear();

		std::wstring resultname;
		resultname.append(pngfile.parent_path()).append(L"/").append(pngfile.stem())
			.append(L"_L_sharpen_x").append(std::to_wstring(sharpenRatio))
			.append(pngfile.extension());

#if LITTLE_ENDIAN
		exportFile(reinterpret_cast<byte*>(result.getRGBA_uint8().data()), result.width, result.height, resultname);
#else
		//load result into stream to save to file
		result.loadRGBAtoByteStream();
		result.clearRGBA_uint8();

		exportFile(result, resultname);
#endif
	}
	else
	{
		std::cout << "Something wrong in convert." << std::endl;
		exit(0);
	}
}

void PngProcessingTools::gaussLaplaceSharpenProgram(float32_t& sharpenRatio, std::filesystem::path& pngfile)
{
	std::cout << "Gauss-Laplace Sharpen:\n"
		<< "Input sharpen factor:" << sharpenRatio << '\n' << std::endl;

	Clamp(sharpenRatio, 1.0f, 1000.0f);

	std::cout << "Adoption sharpen factor:" << sharpenRatio << "%\n"
		<< "Start processing . . ." << std::endl;

	TextureData image, result;
	importFile(image, pngfile);

	if (ImageProcessingTools::SharpenGaussLaplace5x5(image, result, sharpenRatio))
	{
		image.clear();

		std::wstring resultname;
		resultname.append(pngfile.parent_path()).append(L"/").append(pngfile.stem())
			.append(L"_GL_sharpen_x").append(std::to_wstring(sharpenRatio))
			.append(pngfile.extension());

#if LITTLE_ENDIAN
		exportFile(reinterpret_cast<byte*>(result.getRGBA_uint8().data()), result.width, result.height, resultname);
#else
		//load result into stream to save to file
		result.loadRGBAtoByteStream();
		result.clearRGBA_uint8();

		exportFile(result, resultname);
#endif
	}
	else
	{
		std::cout << "Something wrong in convert." << std::endl;
		exit(0);
	}
}

void PngProcessingTools::hdrToneMappingColorProgram(float32_t& lumRatio, std::filesystem::path& pngfile)
{
	std::cout << "ToneMapping:\n"
		<< "Input lumming factor:" << lumRatio << '\n' << std::endl;

	Clamp(lumRatio, 0.1f, 16.0f);

	std::cout << "Adoption lumming factor:" << lumRatio << '\n'
		<< "Start processing . . ." << std::endl;

	TextureData image;
	importFile(image, pngfile);

	if (ImageProcessingTools::AecsHdrToneMapping(image, lumRatio))
	{
		std::wstring resultname;
		resultname.append(pngfile.parent_path()).append(L"/").append(pngfile.stem())
			.append(L"_toneMapping_x").append(std::to_wstring(lumRatio))
			.append(pngfile.extension());

#if LITTLE_ENDIAN
		exportFile(reinterpret_cast<byte*>(image.getRGBA_uint8().data()), image.width, image.height, resultname);
#else
		//load result into stream to save to file
		image.loadRGBAtoByteStream();
		image.clearRGBA_uint8();

		exportFile(image, resultname);
#endif
	}
	else
	{
		std::cout << "Something wrong in convert." << std::endl;
		exit(0);
	}
}

void PngProcessingTools::reverseColorProgram(std::filesystem::path& pngfile)
{
	std::cout << "ReverseColor:\n"
		<< "Start processing . . ." << std::endl;

	TextureData image;
	importFile(image, pngfile);

	if (ImageProcessingTools::ReverseColorImage(image))
	{
		std::wstring resultname;
		resultname.append(pngfile.parent_path()).append(L"/").append(pngfile.stem())
			.append(L"_reverse")
			.append(pngfile.extension());

#if LITTLE_ENDIAN
		exportFile(reinterpret_cast<byte*>(image.getRGBA_uint8().data()), image.width, image.height, resultname);
#else
		//load result into stream to save to file
		image.loadRGBAtoByteStream();
		image.clearRGBA_uint8();

		exportFile(image, resultname);
#endif
	}
	else
	{
		std::cout << "Something wrong in convert." << std::endl;
		exit(0);
	}
}

void PngProcessingTools::grayColorProgram(std::filesystem::path& pngfile)
{
	std::cout << "Grayscale:\n"
		<< "Start processing . . ." << std::endl;

	TextureData image,result;
	importFile(image, pngfile);

	if (ImageProcessingTools::Grayscale(image,result))
	{
		image.clear();

		std::wstring resultname;
		resultname.append(pngfile.parent_path()).append(L"/").append(pngfile.stem())
			.append(L"_gray")
			.append(pngfile.extension());

		exportFile(result.image.data(), result.width, result.height, resultname, LodePNGColorType::LCT_GREY);
	}
	else
	{
		std::cout << "Something wrong in convert." << std::endl;
		exit(0);
	}
}

void PngProcessingTools::channelGrayColorProgram(std::filesystem::path& pngfile)
{
	std::cout << "ChannelGrayscale:\n"
		<< "Start processing . . ." << std::endl;

	TextureData image;
	importFile(image, pngfile);

	TextureData imageR;
	TextureData imageG;
	TextureData imageB;

	if (ImageProcessingTools::ChannelGrayScale(image, imageR, imageG, imageB))
	{
		image.clearImage();

		std::wstring resultnameR;
		std::wstring resultnameG;
		std::wstring resultnameB;

		resultnameR.append(pngfile.parent_path()).append(L"/").append(pngfile.stem())
			.append(L"_Red_gray")
			.append(pngfile.extension());

		resultnameG.append(pngfile.parent_path()).append(L"/").append(pngfile.stem())
			.append(L"_Green_gray")
			.append(pngfile.extension());

		resultnameB.append(pngfile.parent_path()).append(L"/").append(pngfile.stem())
			.append(L"_Blue_gray")
			.append(pngfile.extension());

		auto exportChannel = [](TextureData *result, std::wstring *resultname) {
			PngProcessingTools::exportFile(result->image.data(), result->width, result->height, *resultname, LodePNGColorType::LCT_GREY);
		};

		std::thread RedChannel(exportChannel, &imageR, &resultnameR);
		std::thread GreenChannel(exportChannel, &imageG, &resultnameG);
		std::thread BlueChannel(exportChannel, &imageB, &resultnameB);

		if (RedChannel.joinable())
			RedChannel.join();

		if (GreenChannel.joinable())
			GreenChannel.join();

		if (BlueChannel.joinable())
			BlueChannel.join();
	}
	else
	{
		std::cout << "Something wrong in convert." << std::endl;
		exit(0);
	}
}

void PngProcessingTools::vividnessAdjustmentColorProgram(float32_t& VividRatio, std::filesystem::path& pngfile)
{
	std::cout << "Vividness Adjustment:\n"
		<< "Input Vivid factor:" << (1.0f + VividRatio) * 100.0f << "%\n" << std::endl;

	Clamp(VividRatio, -1.0f, 254.0f);


	std::cout << "Adoption Vivid factor:" << (1.0f + VividRatio) * 100.0f << "%\n"
		<< "Start processing . . ." << std::endl;

	TextureData image;
	importFile(image, pngfile);

	if (ImageProcessingTools::VividnessAdjustment(image,VividRatio))
	{
		std::wstring resultname;
		resultname.append(pngfile.parent_path()).append(L"/").append(pngfile.stem())
			.append(L"_vivid_x").append(std::to_wstring(VividRatio))
			.append(pngfile.extension());

#if LITTLE_ENDIAN
		exportFile(reinterpret_cast<byte*>(image.getRGBA_uint8().data()), image.width, image.height, resultname);
#else
		//load result into stream to save to file
		image.loadRGBAtoByteStream();
		image.clearRGBA_uint8();

		exportFile(image, resultname);
#endif
	}
	else
	{
		std::cout << "Something wrong in convert." << std::endl;
		exit(0);
	}
}

void PngProcessingTools::natualvividnessAdjustmentColorProgram(float32_t& VividRatio, std::filesystem::path& pngfile)
{
	std::cout << "Natual Vividness Adjustment:\n"
		<< "Input Vivid factor:" << (1.0f + VividRatio) * 100.0f << "%\n" << std::endl;

	Clamp(VividRatio, -1.0f, 1.0f);

	std::cout << "Adoption Vivid factor:" << (1.0f + VividRatio) * 100.0f << "%\n"
		<< "Start processing . . ." << std::endl;

	TextureData image;
	importFile(image, pngfile);

	if (ImageProcessingTools::NatualVividnessAdjustment(image, VividRatio))
	{
		std::wstring resultname;
		resultname.append(pngfile.parent_path()).append(L"/").append(pngfile.stem())
			.append(L"_natualVivid_x").append(std::to_wstring(VividRatio))
			.append(pngfile.extension());

#if LITTLE_ENDIAN
		exportFile(reinterpret_cast<byte*>(image.getRGBA_uint8().data()), image.width, image.height, resultname);
#else
		//load result into stream to save to file
		image.loadRGBAtoByteStream();
		image.clearRGBA_uint8();

		exportFile(image, resultname);
#endif
	}
	else
	{
		std::cout << "Something wrong in convert." << std::endl;
		exit(0);
	}
}

void PngProcessingTools::binarizationColorProgram(float32_t& threshold, std::filesystem::path& pngfile)
{
	std::cout << "Binarization:\n"
		<<"Input threshold factor:" << threshold << '\n' << std::endl;

	Clamp(threshold, 0.0f, 1.0f - ColorPixTofloat);

	std::cout << "Adoption threshold factor:" << threshold << '\n'
		<< "Start processing . . ." << std::endl;

	TextureData image, result;
	importFile(image, pngfile);

	if (ImageProcessingTools::Binarization(image, result, threshold))
	{
		image.clear();

		std::wstring resultname;
		resultname.append(pngfile.parent_path()).append(L"/").append(pngfile.stem())
			.append(L"_binarization_").append(std::to_wstring(threshold))
			.append(pngfile.extension());

		exportFile(result.image.data(), result.width, result.height, resultname, LodePNGColorType::LCT_GREY);
	}
	else
	{
		std::cout << "Something wrong in convert." << std::endl;
		exit(0);
	}
}

void PngProcessingTools::quaternizationColorProgram(float32_t& threshold,std::filesystem::path& pngfile)
{
	std::cout << "Quaternization:\n"
		<<"Input threshold factor:" << threshold << '\n' << std::endl;

	Clamp(threshold, 0.0f, 1.0f);

	std::cout << "Adoption threshold factor:" << threshold << '\n'
		<< "Start processing . . ." << std::endl;

	TextureData image, result;
	importFile(image, pngfile);

	if (ImageProcessingTools::Quaternization(image, result,threshold))
	{
		image.clear();

		std::wstring resultname;
		resultname.append(pngfile.parent_path()).append(L"/").append(pngfile.stem())
			.append(L"_quaternization_").append(std::to_wstring(threshold))
			.append(pngfile.extension());

		exportFile(result.image.data(), result.width, result.height, resultname, LodePNGColorType::LCT_GREY);
	}
	else
	{
		std::cout << "Something wrong in convert." << std::endl;
		exit(0);
	}
}

void PngProcessingTools::hexadecimalizationColorProgram(std::filesystem::path& pngfile)
{
	std::cout << "Hexadecimalization:\n"
		<< "Start processing . . ." << std::endl;

	TextureData image, result;
	importFile(image, pngfile);

	if (ImageProcessingTools::Hexadecimalization(image, result))
	{
		image.clear();

		std::wstring resultname;
		resultname.append(pngfile.parent_path()).append(L"/").append(pngfile.stem())
			.append(L"_hexadecimalization")
			.append(pngfile.extension());

		exportFile(result.image.data(), result.width, result.height, resultname, LodePNGColorType::LCT_GREY);
	}
	else
	{
		std::cout << "Something wrong in convert." << std::endl;
		exit(0);
	}
}

void PngProcessingTools::fastSplitHorizonProgram(uint32_t& splitInterval, std::filesystem::path& pngfile)
{
	std::cout << "FastSplitHorizon:\n"
		<< "Input split interval factor:" << splitInterval << '\n' << std::endl;

	if (splitInterval == 0u)
		splitInterval = 128u;

	std::cout << "Adoption split interval factor:" << splitInterval << '\n'
		<< "Start processing . . ." << std::endl;

	TextureData image;
	importFile(image, pngfile);

	if (image.image.size() != 0u)
	{
		uint32_t splitNum = std::ceil(float64_t(image.height) / float64_t(splitInterval));

		if (splitNum == 1u)
		{
			std::cout << "Wrong size, cannot be split." << std::endl;
			exit(0);
		}

		size_t byteSplitInterval = (static_cast<size_t>(image.width) * splitInterval) << 2;

		std::vector<std::unique_ptr<std::thread>> allthreads;
		
		allthreads.reserve(splitNum);

		auto exportSplitSlice = [&image](const byte* result, uint32_t height, std::wstring resultName)
		{
			PngProcessingTools::exportFile(result, image.width, height, resultName);
		};

		std::wstring resultnamepart;

		resultnamepart.append(pngfile.parent_path()).append(L"/").append(pngfile.stem())
			.append(L"_splitHorizon_").append(std::to_wstring(splitInterval))
			.append(L"slice_");

		for (size_t Current = 0u, currentSlice = 1u; Current < image.image.size(); Current += byteSplitInterval, ++currentSlice)
		{
			std::wstring thisName;
			thisName.append(resultnamepart).append(std::to_wstring(currentSlice)).append(pngfile.extension());

			if ((Current + byteSplitInterval) < image.image.size())
			{
				allthreads.push_back(std::make_unique<std::thread>(exportSplitSlice, image.image.data() + Current, splitInterval, thisName));
			}
			else
				if ((image.image.size() - Current) > 0u)
				{
					allthreads.push_back(std::make_unique<std::thread>(exportSplitSlice, image.image.data() + Current, image.height % splitInterval, thisName));
				}
		}

		for (auto& thread : allthreads)
		{
			if (thread->joinable())
				thread->join();
		}
	}
	else
	{
		std::cout << "Something wrong in convert." << std::endl;
		exit(0);
	}
}

void PngProcessingTools::blockSplitProgram(uint32_t& horizontalInterval, uint32_t& verticalInterval, std::filesystem::path& pngfile)
{
	std::cout << "BlockSplit:\n"
		<< "Input horizontal interval factor:" << horizontalInterval << '\n'
		<< "Input  vertical  interval factor:" << verticalInterval << '\n' << std::endl;

	if (horizontalInterval == 0u)
		horizontalInterval = 128u;
	if (verticalInterval == 0u)
		verticalInterval = 128u;

	std::cout << "Adoption horizontal interval factor:" << horizontalInterval << '\n'
		<< "Adoption vertical interval factor:" << verticalInterval << '\n'
		<< "Start processing . . ." << std::endl;

	TextureData image;
	importFile(image, pngfile);

	if (image.image.size() != 0u)
	{
		uint32_t horizontalSplitNum = std::ceil(float64_t(image.width) / float64_t(horizontalInterval));
		uint32_t verticalSplitNum = std::ceil(float64_t(image.height) / float64_t(verticalInterval));

		if (horizontalSplitNum == 1u && verticalSplitNum == 1u)
		{
			std::cout << "Wrong size, cannot be split." << std::endl;
			exit(0);
		}

		std::vector<uint32_t>widths(horizontalSplitNum);
		std::vector<uint32_t>heights(verticalSplitNum);

		for (auto& w : widths)
			w = horizontalInterval;

		for (auto& h : heights)
			h = verticalInterval;

		if ((image.width % horizontalInterval) > 0u)
			widths.back() = image.width % horizontalInterval;
		if ((image.height % verticalInterval) > 0u)
			heights.back() = image.height % verticalInterval;

		std::vector<std::vector<byte>>LineBlocks(horizontalSplitNum);
		//to ensure space is enough
		for (auto& blocks : LineBlocks)
			blocks.reserve((static_cast<size_t>(horizontalInterval) * verticalInterval) << 2u);

		std::vector<std::unique_ptr<std::thread>> allthreads;

		allthreads.resize(horizontalSplitNum);

		size_t byteOffset = 0u;

		auto exportSplitSlice = [](const byte* result, uint32_t width, uint32_t height, std::wstring resultName)
		{
			PngProcessingTools::exportFile(result, width, height, resultName);
		};

		std::wstring resultnamepart;

		resultnamepart.append(pngfile.parent_path()).append(L"/").append(pngfile.stem())
			.append(L"_h_").append(std::to_wstring(horizontalInterval))
			.append(L"_v_").append(std::to_wstring(verticalInterval))
			.append(L"_slice_");

		const auto& data = image.image.data();

		for (uint32_t y = 0u; y < verticalSplitNum; ++y)
		{
			for (uint32_t i = 0u; i < heights[y]; ++i)
			{
				for (uint32_t x = 0u; x < horizontalSplitNum; ++x)
				{
					size_t size = LineBlocks[x].size();
					LineBlocks[x].resize(size + (static_cast<size_t>(widths[x]) << 2u));

					std::copy((data + byteOffset), (data + byteOffset) + (static_cast<size_t>(widths[x]) << 2u), LineBlocks[x].begin() + size);
					byteOffset += (static_cast<size_t>(widths[x]) << 2u);
				}
			}

			for (uint32_t x = 0u; x < horizontalSplitNum; ++x)
			{
				std::wstring thisName;
				thisName.append(resultnamepart)
					.append(L"V").append(std::to_wstring(y + 1)).append(L"_")
					.append(L"H").append(std::to_wstring(x + 1)).append(pngfile.extension());

				allthreads[x] = std::make_unique<std::thread>(exportSplitSlice, LineBlocks[x].data(), widths[x], heights[y], thisName);
			}

			for (auto& thread : allthreads)
			{
				if (thread->joinable())
					thread->join();
			}

			for (auto& blocks : LineBlocks)
				blocks.clear();
		}
	}
	else
	{
		std::cout << "Something wrong in convert." << std::endl;
		exit(0);
	}
}

void PngProcessingTools::surfaceBlurfilterProgram(float32_t& threshold, std::filesystem::path& pngfile, int32_t& radius)
{
	std::cout << "Surface Blur Filter:\n"
		<< "Input blur factor:" << threshold << '\n' << std::endl;

	Clamp(threshold, ColorPixTofloat, 1.0f);

	std::cout << "Adoption blur factor:" << threshold << '\n';

	std::cout<< "Input radius factor:" << radius << '\n' << std::endl;

	Clamp(radius, 1u, 24u);

	std::cout << "Adoption radius factor:" << radius << '\n'
		<< "Start processing . . ." << std::endl;

	TextureData image, result;
	importFile(image, pngfile);

	if (ImageProcessingTools::SurfaceBlur(image, result,radius,threshold))
	{
		image.clear();

		std::wstring resultname;
		resultname.append(pngfile.parent_path()).append(L"/").append(pngfile.stem())
			.append(L"_surfaceBlur_").append(std::to_wstring(threshold))
			.append(L"_radius_").append(std::to_wstring(radius))
			.append(pngfile.extension());

#if LITTLE_ENDIAN
		exportFile(reinterpret_cast<byte*>(result.getRGBA_uint8().data()), result.width, result.height, resultname);
#else
		//load result into stream to save to file
		result.loadRGBAtoByteStream();
		result.clearRGBA_uint8();

		exportFile(result, resultname);
#endif
	}
	else
	{
		std::cout << "Something wrong in convert." << std::endl;
		exit(0);
	}
}

void PngProcessingTools::sobelEdgeEnhancementProgram(float32_t& strength, std::filesystem::path& pngfile, float32_t& thresholdMin, float32_t& thresholdMax)
{
	std::cout << "Sobel Edge Enhancement Filter:\n"
		<< "Input thresholdMin factor:" << thresholdMin << '\n'
		<< "Input thresholdMax factor:" << thresholdMax << '\n' << std::endl;

	Clamp(thresholdMin, 0.0f, 1.0f);
	Clamp(thresholdMax, 0.0f, 1.0f);

	if (thresholdMin >= thresholdMax)
	{
		thresholdMin = 0.0f;
		thresholdMax = 1.0f;
	}

	std::cout << "Adoption thresholdMin factor:" << thresholdMin << '\n'
		<< "Adoption thresholdMax factor:" << thresholdMax << '\n';

	std::cout << "Input strength factor:" << (strength) * 100.0f << "%\n";

	Clamp(strength, 0.05f, 10.0f);

	std::cout << "Adoption strength factor:" << (strength) * 100.0f << "%\n"
		<< "Start processing . . ." << std::endl;

	TextureData image, result;
	importFile(image, pngfile);

	if (ImageProcessingTools::SobelEdgeEnhancement(image, result, thresholdMin, thresholdMax, strength))
	{
		image.clear();

		std::wstring resultname;
		resultname.append(pngfile.parent_path()).append(L"/").append(pngfile.stem())
			.append(L"_sobelEdge_min_").append(std::to_wstring(thresholdMin))
			.append(L"_max_").append(std::to_wstring(thresholdMax))
			.append(L"_strength_").append(std::to_wstring(strength))
			.append(pngfile.extension());

#if LITTLE_ENDIAN
		exportFile(reinterpret_cast<byte*>(result.getRGBA_uint8().data()), result.width, result.height, resultname);
#else
		//load result into stream to save to file
		result.loadRGBAtoByteStream();
		result.clearRGBA_uint8();

		exportFile(result, resultname);
#endif
	}
	else
	{
		std::cout << "Something wrong in convert." << std::endl;
		exit(0);
	}
}

void PngProcessingTools::mosaicPixelationProgram(uint32_t& sideLength, std::filesystem::path& pngfile)
{
	std::cout << "Mosaic Pixelation:\n"
		<< "Input mosaic length:" << sideLength << '\n' << std::endl;

	Clamp(sideLength, 2u, 512u);

	std::cout << "Adoption  mosaic length:" << sideLength << '\n'
		<< "Start processing . . ." << std::endl;

	TextureData image;
	importFile(image, pngfile);

	if (ImageProcessingTools::MosaicPixelation(image,sideLength))
	{
		std::wstring resultname;
		resultname.append(pngfile.parent_path()).append(L"/").append(pngfile.stem())
			.append(L"_mosaic_").append(std::to_wstring(sideLength))
			.append(pngfile.extension());

#if LITTLE_ENDIAN
		exportFile(reinterpret_cast<byte*>(image.getRGBA_uint8().data()), image.width, image.height, resultname);
#else
		//load result into stream to save to file
		image.loadRGBAtoByteStream();
		image.clearRGBA_uint8();

		exportFile(image, resultname);
#endif
	}
	else
	{
		std::cout << "Something wrong in convert." << std::endl;
		exit(0);
	}
}

void PngProcessingTools::mixedPicturesProgram(uint32_t& workMode, std::filesystem::path& pngfileOut, std::filesystem::path& pngfileIn)
{
	std::cout << "Mixed Pictures:\n"
		<< "Input Out Picture:" << pngfileOut << '\n'
		<< "Input In Picture:" << pngfileIn << '\n' << std::endl;

	void (*filteringMethod)(const RGBAColor_8i&, byte&, const RGBAColor_8i&, byte&) = nullptr;

	Clamp(workMode, 1, 4);


	if (workMode == 2)
	{
		std::cout << "workMode:1:2\n";
		filteringMethod = filteringMethod1_2;
	}
	else
		if (workMode == 3)
		{
			std::cout << "workMode:2:1\n";
			filteringMethod = filteringMethod2_1;
		}
		else
			if (workMode == 4)
			{
				std::cout << "workMode:1:3\n";
				filteringMethod = filteringMethod1_3;
			}
			else
			{
				std::cout << "default workMode:1:1\n";
				filteringMethod = filteringMethod1_1;
			}

	std::cout << "Start processing . . ." << std::endl;

	TextureData imageOut, imageIn, result;

	importFile(imageOut, pngfileOut);
	importFile(imageIn, pngfileIn);

	if (ImageProcessingTools::MixedPictures(imageOut, imageIn, result, filteringMethod))
	{
		imageOut.clear();
		imageIn.clear();

		std::wstring resultname;
		resultname.append(pngfileIn.parent_path()).append(L"/").append(pngfileIn.stem())
			.append(L"_MixPicture_workMode_").append(std::to_wstring(workMode))
			.append(pngfileIn.extension());

		exportFile(result.image.data(), result.width, result.height, resultname, LodePNGColorType::LCT_GREY_ALPHA);
	}
	else
	{
		std::cout << "Something wrong in convert." << std::endl;
		exit(0);
	}
}
