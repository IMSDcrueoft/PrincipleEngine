#include"CORE/D3D12App.h"
#include"Principle/DebugHelper.h"
#include"ExternalTool/Image.h"
const int gNumFrameResources = 3;

class blankWindow :public D3D12App
{
public:
	blankWindow(HINSTANCE hInstance) :D3D12App(hInstance) {  }

	virtual bool Initialize()override 
	{
		if (!D3D12App::Initialize())
			return false;

		mShowWindow();
		return true;
	}
	virtual void Update(const PerformanceTimer& pt) override 
	{ 
		static DebugHelper dbgTimer(L"Render");
		dbgTimer.Start(); 
		//Sleep(13); 
		for (UINT32 i = 0; i < 1000; i++)
			for (UINT32 j = 0; j < 10000; j++);
		dbgTimer.Stop();
	};
	virtual void Draw(const PerformanceTimer& pt)override
	{
	};
};

int WINAPI WinMain(
	_In_		HINSTANCE hInstance,
	_In_opt_	HINSTANCE prevInstance,
	_In_		PSTR cmdLine,
	_In_		int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	try
	{
		blankWindow Window(hInstance);
		if (!Window.Initialize())
			return 0;

		return Window.Run();
	}
	catch (DxException& e)
	{
		MessageBox(nullptr, e.ToString().c_str(), L"HR Failed", MB_OK);
		return 0;
	}
}
