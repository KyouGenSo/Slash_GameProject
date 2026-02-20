#pragma once
#include <string>

class Boss;

/// <summary>
/// ボスステート基底クラス
/// 各状態の Enter/Update/Exit インターフェースを定義
/// PlayerState パターンに準拠
/// </summary>
class BossState {
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="name">状態名</param>
	BossState(const std::string& name) : stateName_(name) {}

	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~BossState() = default;

	/// <summary>
	/// 状態開始時の処理
	/// </summary>
	/// <param name="boss">ボスインスタンス</param>
	virtual void Enter(Boss* boss) = 0;

	/// <summary>
	/// 状態の更新処理
	/// </summary>
	/// <param name="boss">ボスインスタンス</param>
	/// <param name="deltaTime">前フレームからの経過時間</param>
	virtual void Update(Boss* boss, float deltaTime) = 0;

	/// <summary>
	/// 状態終了時の処理
	/// </summary>
	/// <param name="boss">ボスインスタンス</param>
	virtual void Exit(Boss* boss) = 0;

	/// <summary>
	/// 状態名を取得
	/// </summary>
	/// <returns>状態名の参照</returns>
	const std::string& GetName() const { return stateName_; }

protected:
	std::string stateName_;
};
