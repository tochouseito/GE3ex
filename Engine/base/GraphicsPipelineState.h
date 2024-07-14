#pragma 
#include"DirectXCommon.h"
class GraphicsPipelineState
{
public:
	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns>シングルトンインスタンス</returns>
	static GraphicsPipelineState* GetInstance();

	enum BlendMode {
		//!< ブレンドなし
		kBlendModeNone,
		//!< 通常のaブレンド。デフォルト。 Src * SrcA + Dest * (1 - SrcA)
		kBlendModeNormal,
		//!< 加算。Src * SrcA + Dest * 1
		kBlendModeAdd,
		//!< 減算。Dest * 1 - Src * SrcA
		kBlendModeSubtract,
		//!< 乗算。Src * 0 + Dest * Src
		kBlendModeMultily,
		//!< スクリーン。Src * (1 - Dest) *Dest * 1
		kBlendModeScreen,
		//!< 利用してはいけない
		kCountOfBlendMode,
	};
private:// メンバ関数
	GraphicsPipelineState() = default;
	~GraphicsPipelineState() = default;
	GraphicsPipelineState(const GraphicsPipelineState&) = delete;
	const GraphicsPipelineState& operator=(const GraphicsPipelineState&) = delete;
public:

	static ID3D12PipelineState* GetPipelineState(uint32_t BlendMode) { return GetInstance()->graphicsPipelineState_[BlendMode].Get(); }
	static ID3D12PipelineState* GetParticlePipelineState(uint32_t BlendMode) { return GetInstance()->graphicsPipelineStateParticle_[BlendMode].Get(); }
	static ID3D12PipelineState* GetPipelineStateGSO(uint32_t BlendMode) { return GetInstance()->graphicsPipelineStateGSO_[BlendMode].Get(); }

	static ID3D12RootSignature* GetRootSignature() { return GetInstance()->rootSignature_.Get(); }
	static ID3D12RootSignature* GetRootSignatureParticle() { return GetInstance()->rootSignatureParticle_.Get(); }
	static ID3D12RootSignature* GetRootSignatureGSO() { return GetInstance()->rootSignatureGSO_.Get(); }

	/// <summary>
	/// グラフィックスパイプラインの作成
	/// </summary>
	void CreateGraphicsPipeline(ID3D12Device* device);

	/// <summary>
	/// パーティクル用のグラフィックスパイプラインの作成
	/// </summary>
	void CreateParticleGraphicsPipeline(ID3D12Device* device);

	/// <summary>
	/// グラフィックスパイプラインの作成
	/// </summary>
	void CreateGraphicsPipelineGSO(ID3D12Device* device);

	/// <summary>
	/// dxcCompilerを初期化
	/// </summary>
	void InitializeDxcCompiler();

	/// <summary>
	/// shaderのコンパイル
	/// </summary>
	/// <param name="filePath"></param>
	/// <param name="profile"></param>
	/// <param name="dxcUtils"></param>
	/// <param name="dxcCompiler"></param>
	/// <param name="includeHandler"></param>
	/// <returns></returns>
	IDxcBlob* CompilerShader(
		// CompilerするShaderファイルへのパス
		const std::wstring& filePath,
		// Compilerに使用するProfile
		const wchar_t* profile,
		// 初期化で生成したものを3つ
		IDxcUtils* dxcUtils,
		IDxcCompiler3* dxcCompiler,
		IDxcIncludeHandler* includeHandler);
private:
	Microsoft::WRL::ComPtr < ID3D12PipelineState> graphicsPipelineState_[kCountOfBlendMode];
	Microsoft::WRL::ComPtr < ID3D12PipelineState> graphicsPipelineStateParticle_[kCountOfBlendMode];
	Microsoft::WRL::ComPtr < ID3D12PipelineState> graphicsPipelineStateGSO_[kCountOfBlendMode];
	//ID3D12PipelineState* graphicsPipelineState_;
	Microsoft::WRL::ComPtr < IDxcUtils> dxcUtils_;
	Microsoft::WRL::ComPtr < IDxcCompiler3> dxcCompiler_;
	IDxcIncludeHandler* includeHandler_;
	Microsoft::WRL::ComPtr < ID3D12RootSignature> rootSignature_;
	Microsoft::WRL::ComPtr < ID3D12RootSignature>rootSignatureParticle_;
	Microsoft::WRL::ComPtr < ID3D12RootSignature> rootSignatureGSO_;
};

