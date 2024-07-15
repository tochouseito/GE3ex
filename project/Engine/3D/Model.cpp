#include "Model.h"
#include"GraphicsPipelineState.h"
#include"DirectXCommon.h"
#include"TextureManager.h"
#include"imgui.h"
#include <iostream>
#include"fstream"
#include"sstream"
#include<random>
#include<numbers>
#include"Mymath.h"
Model::Model() {
	material_ = new Material();
	directionalLight_ = new DirectionalLight();
	pointLight_ = new PointLight();
	spotLight_ = new SpotLight();
	mesh_ = new Mesh();
}
Model::~Model() {
	delete mesh_;
	delete material_;
	delete directionalLight_;
	delete pointLight_;
	delete spotLight_;
}

/*平面オブジェクト*/
Model* Model::CreatePlane() {
	Model* model = new Model();
	GraphicsPipelineState::GetInstance()->CreateGraphicsPipeline(DirectXCommon::GetInstance()->GetDevice());
	model->mesh_->CreateObjectVertex(Mesh::Object::kPlane);
	model->material_->CreateMaterialResource();
	model->directionalLight_->CreateDirectionalLightResource();
	model->pointLight_->CreatePointLightResource();
	model->spotLight_->CreateSpotLightResource();
	return model;
}
/*球オブジェクト*/
Model* Model::CreateSphere(uint32_t Subdivision) {
	Model* model = new Model();
	GraphicsPipelineState::GetInstance()->CreateGraphicsPipeline(DirectXCommon::GetInstance()->GetDevice());
	model->mesh_->CreateObjectVertex(Mesh::Object::kSphere);
	model->material_->CreateMaterialResource();
	model->directionalLight_->CreateDirectionalLightResource();
	model->pointLight_->CreatePointLightResource();
	model->spotLight_->CreateSpotLightResource();
	return model;
}

/// <summary>
/// 描画
/// </summary>
void Model::Draw(WorldTransform& worldTransform,ViewProjection& viewProjection,
	std::string textureHandle) {
	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = DirectXCommon::GetInstance()->GetCommandList();
	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばいい
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// Roosignatureを設定。PSOに設定しているけど別途設定が必要
	commandList->SetGraphicsRootSignature(GraphicsPipelineState::GetRootSignature());
	commandList->SetPipelineState(GraphicsPipelineState::GetPipelineState(current_blend));// PSOを設定
	commandList->IASetVertexBuffers(0, 1, mesh_->GetVertexBufferView());// VBVを設定
	if(useMonsterBall){
	commandList->IASetIndexBuffer(mesh_->GetIndexBufferView());// IBVを設定
	 }
	// マテリアルCBufferの場所を設定
	commandList->SetGraphicsRootConstantBufferView(0, material_->GetMaterialResource()->GetGPUVirtualAddress());
	
	// wvp用のCBufferの場所を設定
	commandList->SetGraphicsRootConstantBufferView(4, viewProjection.GetWvpResource()->GetGPUVirtualAddress());
	// wvp用のCBufferの場所を設定
	commandList->SetGraphicsRootConstantBufferView(3, directionalLight_->GetLightResource()->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(5, pointLight_->GetLightResource()->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(6, spotLight_->GetLightResource()->GetGPUVirtualAddress());
	commandList->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetTextureHandle(textureHandle));
	commandList->SetGraphicsRootDescriptorTable(1, worldTransform.GetSrvHandleGPU());
	// 描画！(DrawCall/ドローコール)。３頂点で1つのインスタンス。インスタンスについては今後
	commandList->DrawInstanced(static_cast<UINT>(mesh_->GetVertices()), 1, 0, 0);
	
}
Model* Model::LordModel(const std::string& filename) {
	Model* model = new Model();
	model->modelData_= LoadModelFile("./Resources", filename);
	
	GraphicsPipelineState::GetInstance()->CreateGraphicsPipeline(DirectXCommon::GetInstance()->GetDevice());
	model->mesh_->SetVertices(static_cast<UINT>(model->modelData_.vertices.size()));
	model->mesh_->CreateOBJVertexResource(model->modelData_.vertices.size());
	// 頂点データをリソースにコピー
	std::memcpy(model->mesh_->GetVertexData(), model->modelData_.vertices.data(), sizeof(Mesh::VertexData) * model->modelData_.vertices.size());
	model->material_->CreateMaterialResource();
	model->directionalLight_->CreateDirectionalLightResource();
	model->pointLight_->CreatePointLightResource();
	model->spotLight_->CreateSpotLightResource();
	return model;
	//modelData_ = LoadObjFile("resources", filename);
	//modelData_ = LoadModelFile("./Resources", filename);
}
Model::OBJModelData Model::LoadObjFile(const std::string& directoryPath, const std::string& filename)
{
	// 1. 中で必要となる変数の宣言
	OBJModelData modelData; // 構築するModelData
	std::vector<Vector4>positions;// 位置
	std::vector<Vector3>normals;// 法線
	std::vector<Vector2>texcoords;// テクスチャ座標
	std::string line; // ファイルから読んだ1行を格納するもの
	// 2. ファイルを開く
	std::ifstream file(directoryPath + "/" + filename);// ファイルを開く
	assert(file.is_open());//とりあえず開けなかったら止める
	// 3. 実際にファイルを読み、ModelDataを構築していく
	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;// 先頭識別子を読む

		// identifierに応じた処理
		if (identifier == "v") {
			Vector4 position;
			s >> position.x >> position.y >> position.z;
			position.x *= -1.0f;
			//position.y *= -1.0f;
			position.w = 1.0f;
			positions.push_back(position);
		} else if (identifier == "vt") {
			Vector2 texcoord;
			s >> texcoord.x >> texcoord.y;
			texcoord.y = 1.0f - texcoord.y;
			//texcoord.y = texcoord.y-1.0f;
			texcoords.push_back(texcoord);
		} else if (identifier == "vn") {
			Vector3 normal;
			s >> normal.x >> normal.y >> normal.z;
			//normal.x *= -1.0f;
			//normal.y *= -1.0f;
			normals.push_back(normal);
		} else if (identifier == "f") {
			Mesh::VertexData triangle[3];
			// 面は三角形限定。その他は未対応
			for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
				std::string vertexDefinition;
				s >> vertexDefinition;
				// 頂点の要素へのIndexは「位置/UV/法線」で格納されているので、分解してIndexを取得する
				std::istringstream v(vertexDefinition);
				uint32_t elementIndices[3];
				for (int32_t element = 0; element < 3; ++element) {
					std::string index;
					std::getline(v, index, '/');//区切りでインデックスを読んでいく
					elementIndices[element] = std::stoi(index);
				}
				// 要素へのインデックスから、実際の要素の値を取得して、頂点を構築する
				Vector4 position = positions[elementIndices[0] - 1];
				Vector2 texcoord = elementIndices[1] > 0 ? texcoords[elementIndices[1] - 1] : Vector2{ 0, 0 };
				Vector3 normal = normals[elementIndices[2] - 1];
				//VertexData vertex = { position,texcoord,normal };
				//modelData.vertices.push_back(vertex);
				triangle[faceVertex] = { position,texcoord,normal };
			}
			// 頂点を逆順で登録することで、周り順を逆にする
			modelData.vertices.push_back(triangle[2]);
			modelData.vertices.push_back(triangle[1]);
			modelData.vertices.push_back(triangle[0]);
		} else if (identifier == "mtllib") {
			// materialTemplateLibraryファイルの名前を取得する
			std::string materialFilename;
			s >> materialFilename;
			// 基本的にobjファイルと同一階層にmtlは存在させるので、ディレクトリ名とファイル名を渡す
			modelData.material = LoadMaterialTemplateFile(directoryPath, materialFilename);
		}
	}
	// 4. ModelDataを返す
	return modelData;
}
Model::Node Model::ReadNode(aiNode* node) {
	Node result;
	aiMatrix4x4 aiLocalMatrix = node->mTransformation;// nodeのlocalMatrixを取得
	aiLocalMatrix.Transpose();// 列ベクトルを行ベクトルに転置
	for (uint32_t mindex = 0; mindex < 4; ++mindex) {
		for (uint32_t index = 0; index < 4; ++index) {
			result.localMatrix.m[mindex][index] = aiLocalMatrix[mindex][index];
		}
	}
	result.name = node->mName.C_Str();// Node名を格納
	result.children.resize(node->mNumChildren);// 子供の数だけ確保
	for (uint32_t childIndex = 0; childIndex < node->mNumChildren; ++childIndex) {
		// 再帰的に読んで階層構造を作っていく
		result.children[childIndex] = ReadNode(node->mChildren[childIndex]);
	}
	return result;
}
Model::OBJModelData Model::LoadModelFile(const std::string& directoryPath, const std::string& filename)
{

	// 1. 中で必要となる変数の宣言
	OBJModelData modelData; // 構築するModelData
	std::vector<Vector4>positions;// 位置
	std::vector<Vector3>normals;// 法線
	std::vector<Vector2>texcoords;// テクスチャ座標
	std::string line; // ファイルから読んだ1行を格納するもの
	// 2. ファイルを開く
	//std::ifstream file(directoryPath + "/" + filename);// ファイルを開く
	//assert(file.is_open());//とりあえず開けなかったら止める
	// assimp
	Assimp::Importer importer;
	std::string filePath = directoryPath + "/" + filename;
	//const aiScene* scene = importer.ReadFile(filePath.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals);
	const aiScene* scene = importer.ReadFile(filePath.c_str(), aiProcess_FlipWindingOrder|aiProcess_FlipUVs);
	assert(scene->HasMeshes());// メッシュがないのは対応しない
	// SceneのRootNodeを読んでシーン全体の階層構造を作り上げる
	modelData.rootNode = ReadNode(scene->mRootNode);
	// meshを解析する
	for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
		aiMesh* mesh = scene->mMeshes[meshIndex];
		assert(mesh->HasNormals());// 法線がないMeshは今回は非対応
		assert(mesh->HasTextureCoords(0));// TexcoordがないMeshは今回は非対応
		// ここからMeshの中身(Face)の解析を行っていく
		for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
			aiFace& face = mesh->mFaces[faceIndex];
			assert(face.mNumIndices == 3);// 三角形のみサポート
			// ここからFaceの中身(Vertex)の解析を行っていく
			for (uint32_t element = 0; element < face.mNumIndices; ++element) {
				uint32_t vertexIndex = face.mIndices[element];
				aiVector3D& position = mesh->mVertices[vertexIndex];
				aiVector3D& normal = mesh->mNormals[vertexIndex];
				aiVector3D& texcoord = mesh->mTextureCoords[0][vertexIndex];
				Mesh::VertexData vertex;
				vertex.position = { position.x,position.y,position.z,1.0f };
				vertex.normal = { normal.x,normal.y,normal.z };
				vertex.texcoord = { texcoord.x,texcoord.y };
				// aiProcess_MakeLeftHandedはz*=-1で、右手->左手に変換するので手動で対処
				vertex.position.x *= -1.0f;
				vertex.normal.x *= -1.0f;
				modelData.vertices.push_back(vertex);
			}
		}
	}
	// materialを解析する
	for (uint32_t materialIndex = 0; materialIndex < scene->mNumMaterials; ++materialIndex) {
		aiMaterial* material = scene->mMaterials[materialIndex];
		if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
			aiString textureFilePath;
			material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);
			modelData.material.textureFilePath = directoryPath + "/" + textureFilePath.C_Str();
		}
	}
	return modelData;
}
Material::OBJMaterialData Model::LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename)
{
	// 1. 中で必要となる変数の宣言
	Material::OBJMaterialData materialData;// 構築するMaterialData
	std::string line;// ファイルから読んだ1行を格納するもの

	// 2. ファイルを開く
	std::ifstream file(directoryPath + "/" + filename);;//ファイルを開く
	assert(file.is_open());
	// 3. 実際にファイルを読み、MaterialDataを構築していく
	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;// 先頭識別子を読む
		// identifierに応じた処理
		if (identifier == "map_Kd") {
			std::string textureFilename;
			s >> textureFilename;
			// 連結してファイルパスにする
			materialData.textureFilePath = directoryPath + "/" + textureFilename;
		}
	}
	// 4. MaterialDataを返す
	return materialData;
}
