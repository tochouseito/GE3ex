#include "Sprite.h"
#include"DirectXCommon.h"
#include"TextureManager.h"
#include"Mymath.h"
#include"imgui.h"
/// <summary>
/// 初期化
/// </summary>
void Sprite::Initialize(const Vector3& position, ViewProjection* viewProjection,uint32_t textureHandle) {
	// Transformのリソースを作る
	TransformResource();
	// 頂点リソースを作る
	CreateVertexResource();
	// マテリアルリソースを作る
	CreateMaterial();
	// インデックスリソースを作る
	CreateIndexResource();

	//worldTransform_.Initialize();
	//uvTransformSprite_.Initialize();

	worldTransform_.translation_ = position;
	textureHandle_ = textureHandle;
	viewProjection_ = viewProjection;
}

/// <summary>
/// 毎フレーム処理
/// </summary>
void Sprite::Update() {

#ifdef _DEBUG

	ImGui::Begin("Demo");

	ImGui::DragFloat3("Sprite:Translation", &worldTransform_.translation_.x, 1.0f);
	ImGui::DragFloat2("UVTranslate", &uvTransformSprite_.translation_.x, 0.01f, -10.0f, 10.0f);
	ImGui::DragFloat2("UVScale", &uvTransformSprite_.scale_.x, 0.01f, -10.0f, 10.0f);
	ImGui::SliderAngle("UVRotate", &uvTransformSprite_.rotation_.z);

	ImGui::End(); 

#endif
	uvTransformUpdata();
	UpdataTransform();
}

/// <summary>
/// 描画
/// </summary>
void Sprite::Draw() {
	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = DirectXCommon::GetInstance()->GetCommandList();
	// Spriteの描画。変更が必要なものだけ変更する
	//commandList->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetTexture(textureHandle_).gpuDescHandleSRV);
	commandList->IASetVertexBuffers(0, 1, &vertexBufferViewSprite_);// VBVを設定
	commandList->IASetIndexBuffer(&indexBufferViewSprite_);// IBVを設定
	// マテリアルCBufferの場所を設定
	commandList->SetGraphicsRootConstantBufferView(0, materialResourceSprite_->GetGPUVirtualAddress());
	// TransformationMatrixCBufferの場所を設定
	commandList->SetGraphicsRootConstantBufferView(1, transformationMatrixResourceSprite_->GetGPUVirtualAddress());

	// 描画！(DrawCall/ドローコール)。３頂点で1つのインスタンス。インスタンスについては今後
	//commandList->DrawInstanced(6, 1, 0, 0);
	commandList->DrawIndexedInstanced(6, 1, 0, 0, 0);
}
/// <summary>
/// 頂点リソースを作る
/// </summary>
void Sprite::CreateVertexResource() {
	// Sprite用の頂点リソースを作る
	vertexResourceSprite_ = DirectXCommon::GetInstance()->CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(Mesh::VertexData) * 6);
	// リソースの先頭のアドレスから使う
	vertexBufferViewSprite_.BufferLocation = vertexResourceSprite_->GetGPUVirtualAddress();
	// 使用するリソースのサイズは頂点に6つ分のサイズ
	vertexBufferViewSprite_.SizeInBytes = sizeof(Mesh::VertexData) * 6;
	// 1頂点アタリのサイズ
	vertexBufferViewSprite_.StrideInBytes = sizeof(Mesh::VertexData);

	// 頂点リソースにデータを書き込む
	

	// 書き込むためのアドレスを取得
	vertexResourceSprite_->Map(0, nullptr, reinterpret_cast<void**>(&vertexDataSprite_));

	// １枚目の三角形
	vertexDataSprite_[0].position = { 0.0f,360.0f,0.0f,1.0f };// 左下
	vertexDataSprite_[0].texcoord = { 0.0f,1.0f };
	vertexDataSprite_[0].normal = { 0.0f,0.0f,-1.0f };
	vertexDataSprite_[1].position = { 0.0f,0.0f,0.0f,1.0f };// 左上
	vertexDataSprite_[1].texcoord = { 0.0f,0.0f };
	vertexDataSprite_[1].normal = { 0.0f,0.0f,-1.0f };
	vertexDataSprite_[2].position = { 640.0f,360.0f,0.0f,1.0f };// 右下
	vertexDataSprite_[2].texcoord = { 1.0f,1.0f };
	vertexDataSprite_[2].normal = { 0.0f,0.0f,-1.0f };
	vertexDataSprite_[3].position = { 640.0f,0.0f,0.0f,1.0f };// 左上
	vertexDataSprite_[3].texcoord = { 1.0f,0.0f };
	vertexDataSprite_[3].normal = { 0.0f,0.0f,-1.0f };
	// 2枚目の三角形
	//vertexDataSprite_[3].position = { 0.0f,0.0f,0.0f,1.0f };// 左下
	//vertexDataSprite_[3].texcoord = { 0.0f,0.0f };
	//vertexDataSprite_[3].normal = { 0.0f,0.0f,-1.0f };
	//vertexDataSprite_[4].position = { 640.0f,0.0f,0.0f,1.0f };// 左上
	//vertexDataSprite_[4].texcoord = { 1.0f,0.0f };
	//vertexDataSprite_[4].normal = { 0.0f,0.0f,-1.0f };
	//vertexDataSprite_[5].position = { 640.0f,360.0f,0.0f,1.0f };// 右下
	//vertexDataSprite_[5].texcoord = { 1.0f,1.0f };
	//vertexDataSprite_[5].normal = { 0.0f,0.0f,-1.0f };
}
/// <summary>
/// Sprite用のTransformationMatrix用のリソースを作る。
/// </summary>
void Sprite::TransformResource() {
	// Sprite用のTransformationMatrix用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
	transformationMatrixResourceSprite_ = DirectXCommon::GetInstance()->CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(ViewProjection::wvpData));
	// データを書き込む
	
	// 書き込むためのアドレスを取得
	transformationMatrixResourceSprite_->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixDataSprite_));
	// 単位行列を書き込んでいく
	transformationMatrixDataSprite_->WVP_ = MakeIdentity4x4();
	transformationMatrixDataSprite_->World_ = MakeIdentity4x4();
}
void Sprite::UpdataTransform() {
	// Sprite用のWorldViewProjectionMatrixを作る
	Matrix4x4 worldMatrixSprite = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	Matrix4x4 viewMatrixSprite = MakeIdentity4x4();
	Matrix4x4 projectionMatrixSprite = MakeOrthographicMatrix(0.0f, 0.0f,
		static_cast<float>(DirectXCommon::GetInstance()->GetBackBufferWidth()), static_cast<float>(DirectXCommon::GetInstance()->GetBackBufferHeight())
		, 0.0f, 100.0f);
	Matrix4x4 worldViewProjectionMatrixSprite = Multiply(worldMatrixSprite, Multiply(viewMatrixSprite, projectionMatrixSprite));
	transformationMatrixDataSprite_->WVP_ = worldViewProjectionMatrixSprite;
}
void Sprite::CreateMaterial() {
	// スプライト用のマテリアルリソースを作る。
	materialResourceSprite_ = DirectXCommon::GetInstance()->CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(Material::MaterialData));
	// マテリアルデータを書き込む
	// 書き込むためのアドレスを取得
	materialResourceSprite_->Map(0, nullptr, reinterpret_cast<void**>(&materialDataSprite_));
	// 今回は白を書き込んでいる
	materialDataSprite_->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	// SpriteではLightingしないのでfalseを設定する
	materialDataSprite_->enableLighting = false;
	materialDataSprite_->uvTransform = MakeIdentity4x4();
}
void Sprite::CreateIndexResource() {
	// インデックスリソースを作成する
	indexResourceSprite_ = DirectXCommon::GetInstance()->CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(uint32_t) * 6);
	indexBufferViewSprite_.BufferLocation = indexResourceSprite_->GetGPUVirtualAddress();
	indexBufferViewSprite_.SizeInBytes = sizeof(uint32_t) * 6;
	// インデックスはuint32_tとする
	indexBufferViewSprite_.Format = DXGI_FORMAT_R32_UINT;
	indexResourceSprite_->Map(0, nullptr, reinterpret_cast<void**>(&indexDataSprite_));
	indexDataSprite_[0] = 0; indexDataSprite_[1] = 1; indexDataSprite_[2] = 2;
	indexDataSprite_[3] = 1; indexDataSprite_[4] = 3; indexDataSprite_[5] = 2;
}
void Sprite::uvTransformUpdata() {
	Matrix4x4 uvTransformMatrix = MakeScaleMatrix(uvTransformSprite_.scale_);
	uvTransformMatrix = Multiply(uvTransformMatrix, MakeRotateZMatrix(uvTransformSprite_.rotation_.z));
	uvTransformMatrix = Multiply(uvTransformMatrix, MakeTranslateMatrix(uvTransformSprite_.translation_));
	materialDataSprite_->uvTransform = uvTransformMatrix;
}