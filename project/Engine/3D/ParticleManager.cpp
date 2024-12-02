#include "ParticleManager.h"
#include"GraphicsPipelineState.h"
#include"DirectXCommon.h"
#include"TextureManager.h"
#include"Mymath.h"
#include"imgui.h"

std::random_device seedGenerator;
std::mt19937 randomEngine(seedGenerator());
ParticleManager::ParticleManager()
{

}

ParticleManager::~ParticleManager()
{

}

void ParticleManager::Initialize(ViewProjection* viewProjection)
{
	// PSOの生成
	GraphicsPipelineState::GetInstance()->CreateParticleGraphicsPipeline(DirectXCommon::GetInstance()->GetDevice());
	viewProjection_ = viewProjection;
}

void ParticleManager::Update()
{
#ifdef _DEBUG
	// ImGuiフレーム開始
	ImGui::Begin("Particles");
	for (auto& particle : particleGroups) {
		if (ImGui::CollapsingHeader(particle.first.c_str())) {
			if (ImGui::Button(("Generate" + particle.first).c_str())) {
				particle.second.particles.splice(particle.second.particles.end(), Emit(particle.second.emitter, randomEngine));
			}
		}
	}
	ImGui::End();
#endif
	for (auto& particle : particleGroups) {
		particle.second.emitter.frequencyTime += kDeltaTime;// 時刻を進める
		uint32_t numInstance = 0;
		for (std::list<Particle>::iterator particleIterator = particle.second.particles.begin();
			particleIterator != particle.second.particles.end();) {
			if ((*particleIterator).lifeTime <= (*particleIterator).currentTime) {
				particleIterator = particle.second.particles.erase(particleIterator);
				continue;
			}
			if (particle.second.useField) {
				// Fieldの範囲内のParticleには加速度を適用する
				if (IsCollision((*particleIterator).worldTransform.translation_, particle.second.accelerationField.area)) {
					(*particleIterator).Velocity += particle.second.accelerationField.acceleration * kDeltaTime;
				}
			}
			Matrix4x4 backToFrontMatrix = MakeRotateYMatrix(std::numbers::pi_v<float>);
			Matrix4x4 billboardMatrix = Multiply(backToFrontMatrix, viewProjection_->cameraMatrix_);
			billboardMatrix.m[3][0] = 0.0f;
			billboardMatrix.m[3][1] = 0.0f;
			billboardMatrix.m[3][2] = 0.0f;

			std::random_device seedGenerator;
			std::mt19937 randomEngine(seedGenerator());
			(*particleIterator).worldTransform.translation_ += (*particleIterator).Velocity * kDeltaTime;
			(*particleIterator).currentTime += kDeltaTime;

			if (particle.second.useBillboard) {
				Matrix4x4 scaleMatrix = MakeScaleMatrix((*particleIterator).worldTransform.scale_);
				Matrix4x4 translateMatrix = MakeTranslateMatrix((*particleIterator).worldTransform.translation_);
				(*particleIterator).worldTransform.matWorld_ = Multiply(billboardMatrix, Multiply(scaleMatrix, translateMatrix));
				// そのまま転送
				particle.second.worldTransform.matWorld_ = (*particleIterator).worldTransform.matWorld_;
				particle.second.worldTransform.numInstance_ = numInstance;
				particle.second.objectColor.numInstance_ = numInstance;
				particle.second.worldTransform.TransferMatrix();
			} else {
				particle.second.worldTransform.scale_ = (*particleIterator).worldTransform.scale_;
				particle.second.worldTransform.rotation_ = (*particleIterator).worldTransform.rotation_;
				particle.second.worldTransform.translation_ = (*particleIterator).worldTransform.translation_;
				particle.second.worldTransform.numInstance_ = numInstance;
				particle.second.objectColor.numInstance_ = numInstance;
				particle.second.worldTransform.UpdataMatrix();
			}
			particle.second.objectColor.color_ = (*particleIterator).objectColor.color_;
			particle.second.objectColor.color_.a = 1.0f - ((*particleIterator).currentTime / (*particleIterator).lifeTime);
			particle.second.objectColor.TransferMatrix();
			++numInstance;
			++particleIterator;
		}
		particle.second.numInstance = numInstance;
	}
}

void ParticleManager::Draw()
{
	for (auto& particle : particleGroups) {
		// コマンドリストの取得
		ID3D12GraphicsCommandList* commandList = DirectXCommon::GetInstance()->GetCommandList();
		// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばいい
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		// Roosignatureを設定。PSOに設定しているけど別途設定が必要
		commandList->SetGraphicsRootSignature(GraphicsPipelineState::GetRootSignatureParticle());
		commandList->SetPipelineState(GraphicsPipelineState::GetParticlePipelineState(particle.second.current_blend));// PSOを設定
		commandList->IASetVertexBuffers(0, 1,particle.second.mesh->GetVertexBufferView());// VBVを設定
		// マテリアルCBufferの場所を設定
		commandList->SetGraphicsRootConstantBufferView(0, particle.second.material->GetMaterialResource()->GetGPUVirtualAddress());
		// wvp用のCBufferの場所を設定
		commandList->SetGraphicsRootConstantBufferView(4, viewProjection_->GetWvpResource()->GetGPUVirtualAddress());
		// ライト用のCBufferの場所を設定
		commandList->SetGraphicsRootConstantBufferView(3, particle.second.directionalLight->GetLightResource()->GetGPUVirtualAddress());
		commandList->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetTextureHandle(particle.second.textureHandle));
		// instancing用のDataを読むためにStructuredBufferのSRVを設定する
		commandList->SetGraphicsRootDescriptorTable(1, particle.second.worldTransform.GetSrvHandleGPU());
		commandList->SetGraphicsRootDescriptorTable(5, particle.second.objectColor.GetSrvHandleGPU());
		// 描画！(DrawCall/ドローコール)。３頂点で1つのインスタンス。インスタンスについては今後
		commandList->DrawInstanced(static_cast<UINT> (6), particle.second.numInstance, 0, 0);
	}
}

void ParticleManager::AddParticle(std::string name, std::string textureHandle)
{
	/*同じ名前のパーティクルがあれば追加しない*/
	if (particleGroups.contains(name)) {
		return;
	}
	/*パーティクルを追加してデータを書き込む*/
	ParticleGroup& particleGroup= particleGroups[name];
	/*リソースの作成*/
	particleGroup.material = std::make_unique<Material>();
	particleGroup.mesh = std::make_unique<Mesh>();
	particleGroup.directionalLight = std::make_unique<DirectionalLight>();
	particleGroup.mesh->CreateParticleVertexResource(size_t(6));
	particleGroup.material->CreateMaterialResource();
	particleGroup.directionalLight->CreateDirectionalLightResource();
	particleGroup.emitter.count = 3;
	particleGroup.emitter.frequency = 0.5f;
	particleGroup.emitter.frequencyTime = 0.0f;
	particleGroup.emitter.worldTransform.translation_ = { 0.0f,0.0f,0.0f };
	particleGroup.emitter.worldTransform.rotation_ = { 0.0f,0.0f,0.0f };
	particleGroup.emitter.worldTransform.scale_ = { 1.0f,1.0f,1.0f };
	particleGroup.accelerationField.acceleration = { 15.0f,0.0f,0.0f };
	particleGroup.accelerationField.area.min = { -1.0f,-1.0f,-1.0f };
	particleGroup.accelerationField.area.max = { 1.0f,1.0f,1.0f };
	particleGroup.worldTransform.numInstance_ = 100;
	particleGroup.worldTransform.Initialize();
	particleGroup.objectColor.numInstance_ = 100;
	particleGroup.objectColor.Initialize();
	particleGroup.textureHandle = textureHandle;
	particleGroup.worldTransform.rootMatrix_ = MakeIdentity4x4();
}

ParticleManager::Particle ParticleManager::MakeParticles(std::mt19937& randomEngine, const Vector3& translate)
{
	Particle particles;
	std::uniform_real_distribution<float> distribution(-1.0f, 1.0f);
	std::uniform_real_distribution<float> distColor(0.0f, 1.0f);
	std::uniform_real_distribution<float> distTime(1.0f, 3.0f);

	particles.worldTransform.scale_ = { 1.0f,1.0f,1.0f };
	particles.worldTransform.rotation_ = { 0.0f,0.0f,0.0f };
	Vector3 randomTranslate = { distribution(randomEngine),distribution(randomEngine),distribution(randomEngine) };
	particles.worldTransform.translation_ = translate + randomTranslate;
	particles.Velocity = { distribution(randomEngine),distribution(randomEngine),distribution(randomEngine) };
	particles.objectColor.color_ = { distColor(randomEngine),distColor(randomEngine),distColor(randomEngine) ,1.0f };
	particles.lifeTime = distTime(randomEngine);
	particles.currentTime = 0.0f;
	return particles;
}

std::list<ParticleManager::Particle> ParticleManager::Emit(const Emitter& emitter, std::mt19937& randomEngine)
{
	std::list<Particle> particles;
	for (uint32_t count = 0; count < emitter.count; ++count) {
		particles.push_back(MakeParticles(randomEngine, emitter.worldTransform.translation_));
	}
	return particles;
}
