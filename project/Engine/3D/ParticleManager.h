#pragma once
#include"WorldTransform.h"
#include"mathShapes.h"
#include"ObjectColor.h"
#include"Material.h"
#include"Mesh.h"
#include"DirectionalLight.h"
#include"ViewProjection.h"
#include<unordered_map>
#include<random>
#include<list>
class ParticleManager
{
public:/*パーティクルの要素構造体*/
	struct Particle {
		WorldTransform worldTransform;
		Vector3 Velocity;
		ObjectColor objectColor;
		float lifeTime;
		float currentTime;
	};
	struct Emitter {
		WorldTransform worldTransform;
		uint32_t count;		 //!<発生数
		float frequency;	 //!<発生頻度
		float frequencyTime; //!<頻度用タイマー
	};
	struct AccelerationField {
		Vector3 acceleration;//!<加速度
		AABB area;			 //!<範囲
	};
	struct ParticleGroup {
		std::list<Particle> particles;
		//Emitter emitter;
		AccelerationField accelerationField;
		WorldTransform worldTransform;
		ObjectColor objectColor;
		std::string textureHandle;
		uint32_t current_blend = 2;
		uint32_t numInstance = 0;
		bool useBillboard = false;
		bool useField = false;
		/*グラフィックパイプラインに合わせるため*/
		std::unique_ptr<Material> material;
		std::unique_ptr<Mesh> mesh;
		std::unique_ptr<DirectionalLight> directionalLight;
	};
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	ParticleManager();
	/// <summary>
	/// デストラクタ
	/// </summary>
	~ParticleManager();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(ViewProjection* viewProjection);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// パーティクル追加
	/// </summary>
	/// <param name="name"></param>
	void AddParticle(std::string name,std::string textureHandle);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="randomEngine"></param>
	/// <param name="translate"></param>
	/// <returns></returns>
	Particle MakeParticles(std::mt19937& randomEngine, const Vector3& translate);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="emitter"></param>
	/// <param name="randomEngine"></param>
	/// <returns></returns>
	std::list<Particle> Emit(const Emitter& emitter, std::mt19937& randomEngine);

private:
	/*ビュープロジェクション*/
	ViewProjection* viewProjection_;
	/*デルタタイム*/
	const float kDeltaTime = 1.0f / 60.0f;
	/*最大インスタンス数*/
	uint32_t kNumMaxInstance_ = 10;
	/*パーティクルコンテナ*/
	std::unordered_map<std::string, ParticleGroup>particleGroups;
};

