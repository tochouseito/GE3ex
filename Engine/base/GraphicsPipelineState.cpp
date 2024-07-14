#include "GraphicsPipelineState.h"
#include"ConvertString.h"
/// <summary>
/// シングルトンインスタンスの取得
/// </summary>
GraphicsPipelineState* GraphicsPipelineState::GetInstance() {
	static GraphicsPipelineState instance;
	return &instance;
}
/// <summary>
/// dxcCompilerを初期化
/// </summary>
void GraphicsPipelineState::InitializeDxcCompiler() {
	HRESULT hr;
	// dxcCompilerを初期化
	hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils_));
	assert(SUCCEEDED(hr));
	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler_));
	assert(SUCCEEDED(hr));

	// 現時点でincludeはしないが、includeに対応するための設定を作っておく
	hr = dxcUtils_->CreateDefaultIncludeHandler(&includeHandler_);
	assert(SUCCEEDED(hr));
}
/// <summary>
/// グラフィックスパイプラインの作成
/// </summary>
void GraphicsPipelineState::CreateGraphicsPipeline(ID3D12Device* device) {
	InitializeDxcCompiler();
	HRESULT hr;
	//// RootSignature作成
	//// obj3d用
	//D3D12_ROOT_SIGNATURE_DESC descriptionRooTSignature{};
	//descriptionRooTSignature.Flags =
	//	D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	//// RootParameter作成。PixelShaderのMaterialとVertexShaderのTransform
	//D3D12_ROOT_PARAMETER rootParameter[4] = {};
	//rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;// CBVを使う
	//rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderを使う
	//rootParameter[0].Descriptor.ShaderRegister = 0;// レジスタ番号０とバインド
	//D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	//descriptorRange[0].BaseShaderRegister = 0;// ０から始まる
	//descriptorRange[0].NumDescriptors = 1;// 数は1つ
	//descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;// SRVを使う
	//descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;// Offsetを自動計算
	//rootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;// CBVを使う
	//rootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;// VertexShaderを使う
	//rootParameter[1].Descriptor.ShaderRegister = 0;// レジスタ番号０とバインド
	//
	//
	//rootParameter[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;// DescriptorTableを使う
	//rootParameter[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderを使う
	//rootParameter[2].DescriptorTable.pDescriptorRanges = descriptorRange;// Tableの中身の配列
	//rootParameter[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);// Tableで利用する数
	//rootParameter[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;// CBVを使う
	//rootParameter[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderを使う
	//rootParameter[3].Descriptor.ShaderRegister = 1;// レジスタ番号1を使う
	//descriptionRooTSignature.pParameters = rootParameter;// ルートパラメータ配列へのポインタ
	//descriptionRooTSignature.NumParameters = _countof(rootParameter);// 配列の長さ

	// RootSignature作成
	// obj3d用
	D3D12_ROOT_SIGNATURE_DESC descriptionRooTSignature{};
	descriptionRooTSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// RootParameter作成。PixelShaderのMaterialとVertexShaderのTransform
	D3D12_ROOT_PARAMETER rootParameter[7] = {};
	rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;// CBVを使う
	rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderを使う
	rootParameter[0].Descriptor.ShaderRegister = 0;// レジスタ番号０とバインド

	// Vertex Shader用のSRV
	D3D12_DESCRIPTOR_RANGE srvDescriptorRangeVS1 = {};
	srvDescriptorRangeVS1.BaseShaderRegister = 0;
	srvDescriptorRangeVS1.NumDescriptors = 1;
	srvDescriptorRangeVS1.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	srvDescriptorRangeVS1.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// Pixel Shader用のSRV
	D3D12_DESCRIPTOR_RANGE srvDescriptorRangePS = {};
	srvDescriptorRangePS.BaseShaderRegister = 0;
	srvDescriptorRangePS.NumDescriptors = 1;
	srvDescriptorRangePS.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	srvDescriptorRangePS.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// World
	rootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;// VertexShaderを使う
	rootParameter[1].DescriptorTable.pDescriptorRanges = &srvDescriptorRangeVS1;// Tableの中身の配列を指定
	rootParameter[1].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数

	// テクスチャ
	rootParameter[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;// DescriptorTableを使う
	rootParameter[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderを使う
	rootParameter[2].DescriptorTable.pDescriptorRanges = &srvDescriptorRangePS;// Tableの中身の配列
	rootParameter[2].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数

	// ライト
	rootParameter[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;// CBVを使う
	rootParameter[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderを使う
	rootParameter[3].Descriptor.ShaderRegister = 1;// レジスタ番号1を使う

	// WVPデータ
	rootParameter[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameter[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;// VertexShaderを使う
	rootParameter[4].Descriptor.ShaderRegister = 0;// レジスタ番号2とバインド

	// ポイントライト
	rootParameter[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;// CBVを使う
	rootParameter[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderを使う
	rootParameter[5].Descriptor.ShaderRegister = 2;// レジスタ番号2とバインド

	// スポットライト
	rootParameter[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;// CBVを使う
	rootParameter[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderを使う
	rootParameter[6].Descriptor.ShaderRegister = 3;// レジスタ番号3とバインド

	descriptionRooTSignature.pParameters = rootParameter;// ルートパラメータ配列へのポインタ
	descriptionRooTSignature.NumParameters = _countof(rootParameter);// 配列の長さ

	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;// バイリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;// 0~1の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;// 比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;// ありったけのMipmapを使う
	staticSamplers[0].ShaderRegister = 0;// レジスタ番号を使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderで使う
	descriptionRooTSignature.pStaticSamplers = staticSamplers;
	descriptionRooTSignature.NumStaticSamplers = _countof(staticSamplers);

	// シリアライズしてバイナリにする
	Microsoft::WRL::ComPtr < ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr < ID3DBlob> errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionRooTSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(SUCCEEDED(hr));
	}

	// バイナリをもとに生成
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));
	assert(SUCCEEDED(hr));

	// InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
	inputElementDescs[0].SemanticName = "POSITIONT";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[2].SemanticName = "NORMAL";
	inputElementDescs[2].SemanticIndex = 0;
	inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);



	// BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};
	// すべての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;

	// RasiterzerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	// 裏面（時計回り）を表示しな
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	// 三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	// Shaderをコンパイルする
	Microsoft::WRL::ComPtr < IDxcBlob> vertexShaderBlob = CompilerShader(L"Resources/Shader/obj.VS.hlsl",
		L"vs_6_0", dxcUtils_.Get(), dxcCompiler_.Get(), includeHandler_);
	assert(vertexShaderBlob != nullptr);

	/*Microsoft::WRL::ComPtr < IDxcBlob> GeometryShaderBlob = CompilerShader(L"Resources/Shader/BasicGeometryShader.hlsl",
		L"gs_6_0", dxcUtils_.Get(), dxcCompiler_.Get(), includeHandler_);
	assert(GeometryShaderBlob != nullptr);*/

	Microsoft::WRL::ComPtr < IDxcBlob> pixelShaderBlob = CompilerShader(L"Resources/Shader/obj.PS.hlsl",
		L"ps_6_0", dxcUtils_.Get(), dxcCompiler_.Get(), includeHandler_);
	assert(pixelShaderBlob != nullptr);

	// DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	// Depthの機能を有効化する
	depthStencilDesc.DepthEnable = true;
	// 書き込みをします
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	// 比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rootSignature_.Get();  // RootSignature
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;   // InputLayout
	graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),
	vertexShaderBlob->GetBufferSize() };                       // VertexShader
	/*graphicsPipelineStateDesc.GS = { GeometryShaderBlob->GetBufferPointer(),
	GeometryShaderBlob->GetBufferSize() };*/
	graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),
	pixelShaderBlob->GetBufferSize() };                        // PixelShader
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;//RasterizerState
	// 書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	// 利用するトポロジ（形状）のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		//D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
	// どのように画面に色を打ち込むかの設定（気にしなくてもいい）
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	// DepthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	// 実際に生成
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineState_[kBlendModeNone]));
	assert(SUCCEEDED(hr));

	// ここからブレンドPSOの各設定
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;

	//!< 通常のaブレンド。デフォルト。 Src * SrcA + Dest * (1 - SrcA)
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineState_[kBlendModeNormal]));
	assert(SUCCEEDED(hr));

	//!< 加算。Src * SrcA + Dest * 1
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineState_[kBlendModeAdd]));
	assert(SUCCEEDED(hr));

	//!< 減算。Dest * 1 - Src * SrcA
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineState_[kBlendModeSubtract]));
	assert(SUCCEEDED(hr));

	//!< 乗算。Src * 0 + Dest * Src
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineState_[kBlendModeMultily]));
	assert(SUCCEEDED(hr));

	//!< スクリーン。Src * (1 - Dest) *Dest * 1
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineState_[kBlendModeScreen]));
	assert(SUCCEEDED(hr));


}

/// <summary>
/// パーティクル用のグラフィックスパイプラインの作成
/// </summary>
void GraphicsPipelineState::CreateParticleGraphicsPipeline(ID3D12Device* device) {
	InitializeDxcCompiler();
	HRESULT hr;
	// RootSignature作成
	// obj3d用
	D3D12_ROOT_SIGNATURE_DESC descriptionRooTSignature{};
	descriptionRooTSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// RootParameter作成。PixelShaderのMaterialとVertexShaderのTransform
	D3D12_ROOT_PARAMETER rootParameter[6] = {};
	rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;// CBVを使う
	rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderを使う
	rootParameter[0].Descriptor.ShaderRegister = 0;// レジスタ番号０とバインド

	// Vertex Shader用のSRV
	D3D12_DESCRIPTOR_RANGE srvDescriptorRangeVS1 = {};
	srvDescriptorRangeVS1.BaseShaderRegister = 0;
	srvDescriptorRangeVS1.NumDescriptors = 1;
	srvDescriptorRangeVS1.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	srvDescriptorRangeVS1.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// Pixel Shader用のSRV
	D3D12_DESCRIPTOR_RANGE srvDescriptorRangePS = {};
	srvDescriptorRangePS.BaseShaderRegister = 0;
	srvDescriptorRangePS.NumDescriptors = 1;
	srvDescriptorRangePS.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	srvDescriptorRangePS.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// World
	rootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;// VertexShaderを使う
	rootParameter[1].DescriptorTable.pDescriptorRanges = &srvDescriptorRangeVS1;// Tableの中身の配列を指定
	rootParameter[1].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数

	// テクスチャ
	rootParameter[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;// DescriptorTableを使う
	rootParameter[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderを使う
	rootParameter[2].DescriptorTable.pDescriptorRanges = &srvDescriptorRangePS;// Tableの中身の配列
	rootParameter[2].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数

	// ライト
	rootParameter[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;// CBVを使う
	rootParameter[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderを使う
	rootParameter[3].Descriptor.ShaderRegister = 1;// レジスタ番号1を使う

	// WVPデータ
	rootParameter[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameter[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;// VertexShaderを使う
	rootParameter[4].Descriptor.ShaderRegister = 0;// レジスタ番号０とバインド

	// Vertex Shader用の追加SRVテーブル
	D3D12_DESCRIPTOR_RANGE srvDescriptorRangeVS2 = {};
	srvDescriptorRangeVS2.BaseShaderRegister = 1; // シェーダーレジスタを変更
	srvDescriptorRangeVS2.NumDescriptors = 1;
	srvDescriptorRangeVS2.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	srvDescriptorRangeVS2.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// カラーデータ
	rootParameter[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;// VertexShaderを使う
	rootParameter[5].DescriptorTable.pDescriptorRanges =&srvDescriptorRangeVS2;// Tableの中身の配列を指定
	rootParameter[5].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数

	

	descriptionRooTSignature.pParameters = rootParameter;// ルートパラメータ配列へのポインタ
	descriptionRooTSignature.NumParameters = _countof(rootParameter);// 配列の長さ

	

	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;// バイリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;// 0~1の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;// 比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;// ありったけのMipmapを使う
	staticSamplers[0].ShaderRegister = 0;// レジスタ番号を使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderで使う
	descriptionRooTSignature.pStaticSamplers = staticSamplers;
	descriptionRooTSignature.NumStaticSamplers = _countof(staticSamplers);



	// シリアライズしてバイナリにする
	Microsoft::WRL::ComPtr < ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr < ID3DBlob> errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionRooTSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(SUCCEEDED(hr));
	}

	// バイナリをもとに生成
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignatureParticle_));
	assert(SUCCEEDED(hr));

	// InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[4] = {};
	inputElementDescs[0].SemanticName = "POSITIONT";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[2].SemanticName = "NORMAL";
	inputElementDescs[2].SemanticIndex = 0;
	inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[3].SemanticName = "COLOR";
	inputElementDescs[3].SemanticIndex = 0;
	inputElementDescs[3].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[3].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);



	// BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};
	// すべての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;

	// RasiterzerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	// 裏面（時計回り）を表示しな
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	// 三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	// Shaderをコンパイルする
	Microsoft::WRL::ComPtr < IDxcBlob> vertexShaderBlob = CompilerShader(L"Resources/Shader/Particle.VS.hlsl",
		L"vs_6_0", dxcUtils_.Get(), dxcCompiler_.Get(), includeHandler_);
	assert(vertexShaderBlob != nullptr);

	Microsoft::WRL::ComPtr < IDxcBlob> pixelShaderBlob = CompilerShader(L"Resources/Shader/Particle.PS.hlsl",
		L"ps_6_0", dxcUtils_.Get(), dxcCompiler_.Get(), includeHandler_);
	assert(pixelShaderBlob != nullptr);

	// DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	// Depthの機能を有効化する
	depthStencilDesc.DepthEnable = true;
	// 書き込みをします
	//depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	// 比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rootSignatureParticle_.Get();  // RootSignature
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;   // InputLayout
	graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),
	vertexShaderBlob->GetBufferSize() };                       // VertexShader                     
	graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),
	pixelShaderBlob->GetBufferSize() };                        // PixelShader
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;//RasterizerState
	// 書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	// 利用するトポロジ（形状）のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	// どのように画面に色を打ち込むかの設定（気にしなくてもいい）
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	// DepthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	// 実際に生成
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineStateParticle_[kBlendModeNone]));
	assert(SUCCEEDED(hr));

	// ここからブレンドPSOの各設定
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;

	//!< 通常のaブレンド。デフォルト。 Src * SrcA + Dest * (1 - SrcA)
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineStateParticle_[kBlendModeNormal]));
	assert(SUCCEEDED(hr));

	//!< 加算。Src * SrcA + Dest * 1
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineStateParticle_[kBlendModeAdd]));
	assert(SUCCEEDED(hr));

	//!< 減算。Dest * 1 - Src * SrcA
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineStateParticle_[kBlendModeSubtract]));
	assert(SUCCEEDED(hr));

	//!< 乗算。Src * 0 + Dest * Src
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineStateParticle_[kBlendModeMultily]));
	assert(SUCCEEDED(hr));

	//!< スクリーン。Src * (1 - Dest) *Dest * 1
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineStateParticle_[kBlendModeScreen]));
	assert(SUCCEEDED(hr));


}

/// <summary>
/// グラフィックスパイプラインの作成
/// </summary>
void GraphicsPipelineState::CreateGraphicsPipelineGSO(ID3D12Device* device) {
	InitializeDxcCompiler();
	HRESULT hr;
	

	// RootSignature作成
	// obj3d用
	D3D12_ROOT_SIGNATURE_DESC descriptionRooTSignature{};
	descriptionRooTSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// RootParameter作成。PixelShaderのMaterialとVertexShaderのTransform
	D3D12_ROOT_PARAMETER rootParameter[4] = {};
	rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;// CBVを使う
	rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderを使う
	rootParameter[0].Descriptor.ShaderRegister = 0;// レジスタ番号０とバインド

	// Vertex Shader用のSRV
	D3D12_DESCRIPTOR_RANGE srvDescriptorRangeVS1 = {};
	srvDescriptorRangeVS1.BaseShaderRegister = 0;
	srvDescriptorRangeVS1.NumDescriptors = 1;
	srvDescriptorRangeVS1.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	srvDescriptorRangeVS1.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// Pixel Shader用のSRV
	D3D12_DESCRIPTOR_RANGE srvDescriptorRangePS = {};
	srvDescriptorRangePS.BaseShaderRegister = 0;
	srvDescriptorRangePS.NumDescriptors = 1;
	srvDescriptorRangePS.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	srvDescriptorRangePS.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// World
	rootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;// VertexShaderを使う
	rootParameter[1].DescriptorTable.pDescriptorRanges = &srvDescriptorRangeVS1;// Tableの中身の配列を指定
	rootParameter[1].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数

	// テクスチャ
	rootParameter[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;// DescriptorTableを使う
	rootParameter[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderを使う
	rootParameter[2].DescriptorTable.pDescriptorRanges = &srvDescriptorRangePS;// Tableの中身の配列
	rootParameter[2].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数

	

	// WVPデータ
	rootParameter[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameter[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;// VertexShaderを使う
	rootParameter[3].Descriptor.ShaderRegister = 0;// レジスタ番号2とバインド

	

	descriptionRooTSignature.pParameters = rootParameter;// ルートパラメータ配列へのポインタ
	descriptionRooTSignature.NumParameters = _countof(rootParameter);// 配列の長さ

	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;// バイリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;// 0~1の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;// 比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;// ありったけのMipmapを使う
	staticSamplers[0].ShaderRegister = 0;// レジスタ番号を使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderで使う
	descriptionRooTSignature.pStaticSamplers = staticSamplers;
	descriptionRooTSignature.NumStaticSamplers = _countof(staticSamplers);

	// シリアライズしてバイナリにする
	Microsoft::WRL::ComPtr < ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr < ID3DBlob> errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionRooTSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(SUCCEEDED(hr));
	}

	// バイナリをもとに生成
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignatureGSO_));
	assert(SUCCEEDED(hr));

	// InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[2].SemanticName = "NORMAL";
	inputElementDescs[2].SemanticIndex = 0;
	inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);



	// BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};
	// すべての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;

	// RasiterzerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	// 裏面（時計回り）を表示しな
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	// 三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	// Shaderをコンパイルする
	Microsoft::WRL::ComPtr < IDxcBlob> vertexShaderBlob = CompilerShader(L"Resources/Shader/Primitive.VS.hlsl",
		L"vs_6_0", dxcUtils_.Get(), dxcCompiler_.Get(), includeHandler_);
	assert(vertexShaderBlob != nullptr);

	Microsoft::WRL::ComPtr < IDxcBlob> GeometryShaderBlob = CompilerShader(L"Resources/Shader/BasicGeometryShader.hlsl",
		L"gs_6_0", dxcUtils_.Get(), dxcCompiler_.Get(), includeHandler_);
	assert(GeometryShaderBlob != nullptr);

	Microsoft::WRL::ComPtr < IDxcBlob> pixelShaderBlob = CompilerShader(L"Resources/Shader/Primitive.PS.hlsl",
		L"ps_6_0", dxcUtils_.Get(), dxcCompiler_.Get(), includeHandler_);
	assert(pixelShaderBlob != nullptr);

	// DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	// Depthの機能を有効化する
	depthStencilDesc.DepthEnable = true;
	// 書き込みをします
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	// 比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rootSignature_.Get();  // RootSignature
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;   // InputLayout
	graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),
	vertexShaderBlob->GetBufferSize() };                       // VertexShader
	graphicsPipelineStateDesc.GS = { GeometryShaderBlob->GetBufferPointer(),
	GeometryShaderBlob->GetBufferSize() };
	graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),
	pixelShaderBlob->GetBufferSize() };                        // PixelShader
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;//RasterizerState
	// 書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	// 利用するトポロジ（形状）のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		//D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
// どのように画面に色を打ち込むかの設定（気にしなくてもいい）
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	// DepthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	// 実際に生成
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineStateGSO_[kBlendModeNone]));
	assert(SUCCEEDED(hr));

	// ここからブレンドPSOの各設定
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;

	//!< 通常のaブレンド。デフォルト。 Src * SrcA + Dest * (1 - SrcA)
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineStateGSO_[kBlendModeNormal]));
	assert(SUCCEEDED(hr));

	//!< 加算。Src * SrcA + Dest * 1
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineStateGSO_[kBlendModeAdd]));
	assert(SUCCEEDED(hr));

	//!< 減算。Dest * 1 - Src * SrcA
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineStateGSO_[kBlendModeSubtract]));
	assert(SUCCEEDED(hr));

	//!< 乗算。Src * 0 + Dest * Src
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineStateGSO_[kBlendModeMultily]));
	assert(SUCCEEDED(hr));

	//!< スクリーン。Src * (1 - Dest) *Dest * 1
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineStateGSO_[kBlendModeScreen]));
	assert(SUCCEEDED(hr));


}

IDxcBlob* GraphicsPipelineState::CompilerShader(
	// CompilerするShaderファイルへのパス
	const std::wstring& filePath,
	// Compilerに使用するProfile
	const wchar_t* profile,
	// 初期化で生成したものを3つ
	IDxcUtils* dxcUtils,
	IDxcCompiler3* dxcCompiler,
	IDxcIncludeHandler* includeHandler)
{
	// hlslファイルを読む
	// これからシェーダーをコンパイルする旨をログに出す
	Log(ConvertString(std::format(L"Begin CompilerShader,path:{},profile:{}\n", filePath, profile)));
	// hlslファイルを読む
	IDxcBlobEncoding* shaderSource = nullptr;
	HRESULT hr = dxcUtils->LoadFile(filePath.c_str(), nullptr, &shaderSource);
	// 読めなかったら止める
	assert(SUCCEEDED(hr));
	// 読み込んだファイルの内容を設定する
	DxcBuffer shaderSourceBuffer;
	shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size = shaderSource->GetBufferSize();
	shaderSourceBuffer.Encoding = DXC_CP_UTF8;// UTF8の文字コードであることを通知
	LPCWSTR arguments[] = {
		filePath.c_str(),       //コンパイル対象のhlslファイル名
		L"-E",L"main",          // エントリーポイントの指定。基本的にmain以外にはしない
		L"-T",profile,          // ShaderPrifileの設定
		L"-Zi",L"-Qembed_debug",// デバッグ用の情報を埋め込む
		L"-Od",                 // 最適化を外しておく
		L"-Zpr",                // メモリレイアウトは行優先
	};
	// 実際にShaderをコンパイルする
	IDxcResult* shaderResult = nullptr;
	hr = dxcCompiler->Compile(
		&shaderSourceBuffer,       // 読み込んだファイル
		arguments,                 // コンパイルオプション
		_countof(arguments),       // コンパイル結果
		includeHandler,            // includeが含まれた諸々
		IID_PPV_ARGS(&shaderResult)// コンパイル結果
	);
	// コンパイルエラーではなくdxcが起動できないなど致命的な状況
	assert(SUCCEEDED(hr));

	// コンパイル結果から実行用のバイナリ部分を取得
	IDxcBlob* shaderBlob = nullptr;
	hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
	assert(SUCCEEDED(hr));
	// 成功したログを出す
	Log(ConvertString(std::format(L"Compile Succeeded,path:{},profile:{}\n", filePath, profile)));
	// もう使わないリソースを解放
	shaderSource->Release();
	shaderResult->Release();
	// 実行用のバイナリを返却
	return shaderBlob;
}