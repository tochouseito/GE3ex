#pragma once
#include"Mesh.h"
#include"Material.h"
#include"WorldTransform.h"
#include"ViewProjection.h"
class Sprite
{
public:
	Sprite() = default;
	~Sprite() = default;
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(const Vector3& position, ViewProjection* viewProjection,uint32_t textureHandle);

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// 頂点リソースを作る
	/// </summary>
	void CreateVertexResource();

	/// <summary>
	/// Sprite用のTransformationMatrix用のリソースを作る。
	/// </summary>
	void TransformResource();

	void UpdataTransform();

	void CreateMaterial();

	void CreateIndexResource();

	void uvTransformUpdata();
private:
	// Sprite用の頂点リソース
	Microsoft::WRL::ComPtr< ID3D12Resource> vertexResourceSprite_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferViewSprite_{};
	Mesh::VertexData* vertexDataSprite_ = nullptr;
	Microsoft::WRL::ComPtr < ID3D12Resource> transformationMatrixResourceSprite_;
	ViewProjection::wvpData* transformationMatrixDataSprite_ = nullptr;
	ViewProjection* viewProjection_;
	WorldTransform worldTransform_;
	WorldTransform uvTransformSprite_;
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU_;
	Microsoft::WRL::ComPtr < ID3D12Resource> materialResourceSprite_;
	Material::MaterialData* materialDataSprite_ = { nullptr };
	Microsoft::WRL::ComPtr < ID3D12Resource>indexResourceSprite_;
	D3D12_INDEX_BUFFER_VIEW indexBufferViewSprite_{};
	uint32_t* indexDataSprite_ = nullptr;
	uint32_t textureHandle_;
};

