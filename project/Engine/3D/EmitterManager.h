#pragma once
#include"WorldTransform.h"
#include<unordered_map>
class ParticleManager;
class EmitterManager
{
public:
	EmitterManager();
	~EmitterManager();
	struct Emitter {
		WorldTransform worldTransform;
		uint32_t count;		 //!<発生数
		float frequency;	 //!<発生頻度
		float frequencyTime; //!<頻度用タイマー
	};
	/// <summary>
	/// 発生
	/// </summary>
	void Emit();
private:
	std::unordered_map<std::string,Emitter> emitter;
};

