#pragma once
#include <DirectXMath.h>
#include "Principle.h"

/* 提供给脚本开发者 */
namespace Principle {

	/* Object状态指示 */
	enum class State
	{
		keep,
		active,
		inActive,
		destroy
	};

	class ObjectStateExtension
	{
	public:
		const State& GetObjectState() const;
		bool SetObjectState(const State& newState);

	protected:
		State objectState = State::keep;
	};


	/* 供开发人员使用的脚本接口 */
	class RawScript
	{
	public:
		/* 析构函数 */
		virtual ~RawScript() = 0;	
		/* 销毁脚本类 */
		virtual void Destroy() = 0;
		/* 脚本类初始化时调用 */
		virtual void Initialize() = 0;
		/* 引擎帧刷新时调用 */
		virtual void Update(const systemTimer& timer) = 0;
		/* 引擎固定刷新时调用 */
		virtual void fixedUpdate(const systemTimer& timer) = 0;
	};
}