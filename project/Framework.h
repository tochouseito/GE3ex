#pragma once
/// <summary>
/// ゲーム全体
/// </summary>
class Framework
{
public:/*メンバ関数*/
	virtual ~Framework() = default;
	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Initialize();
	/// <summary>
	/// 終了
	/// </summary>
	virtual void Finalize();
	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update();
	/// <summary>
	/// 描画
	/// </summary>
	virtual void Draw();
	virtual bool IsEndRequest() { return endRequest_; }
private:
	bool endRequest_ = false;
};

