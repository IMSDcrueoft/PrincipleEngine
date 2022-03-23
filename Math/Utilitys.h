#pragma once
#ifndef UTILITYS_H
#define UTILITYS_H

namespace Utilitys
{
	//触发器工具
	class Trigger
	{
	private:
		static constexpr float defaultLatency = 0.01;

		struct basicTimer
		{
		public:
			float triggerLatency;
			float timer = 0.0f;
		};

		//简单累加计时触发器
		class simpleTrigger :protected basicTimer
		{
		public:
			simpleTrigger() = delete;
			explicit simpleTrigger(const float latency);

			bool UpdateTrigger(const float totalTime);
			const float getUpdateStep(const float totalTime) const;
		};

		//可复用清零计时触发器
		class reusableTrigger :protected basicTimer
		{
		private:
			bool  isTriggered = false;

		public:
			reusableTrigger() = default;
			explicit reusableTrigger(const float latency);

			void resetTrigger();//zero
			void nextTrigger();//dec
			void UpdateTrigger(const float deltaTime);
			void setNewLatency(const float latency = defaultLatency);
			const bool& ifTriggered() const;
			const float& getTimer() const;
			const float getUpdateStep() const;
		};
	public:
		Trigger() = delete;

		static inline basicTimer GetBasicTimer(const float latency = defaultLatency)
		{
			return basicTimer{ latency };
		}

		static inline simpleTrigger GetSimpleTrigger(const float latency = defaultLatency)
		{
			return simpleTrigger(latency);
		}

		static inline reusableTrigger GetReusableTrigger(const float latency = defaultLatency)
		{
			return reusableTrigger(latency);
		}
	};
}
#endif