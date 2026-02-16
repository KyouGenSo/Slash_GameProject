#pragma once
#include <memory>
#include <unordered_map>
#include <string>

class Boss;
class BossState;

/// <summary>
/// ボス状態管理マシン
/// 外部イベント駆動の状態（スタン・離脱等）を管理
/// AI意思決定はBehaviorTreeに委譲
/// </summary>
class BossStateMachine {
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="boss">管理対象のボス</param>
	BossStateMachine(Boss* boss);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~BossStateMachine();

	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="deltaTime">前フレームからの経過時間</param>
	void Update(float deltaTime);

	/// <summary>
	/// 状態を名前で変更
	/// </summary>
	/// <param name="stateName">遷移先の状態名</param>
	void ChangeState(const std::string& stateName);

	/// <summary>
	/// 状態を登録
	/// </summary>
	/// <param name="name">状態名</param>
	/// <param name="state">状態インスタンス</param>
	void RegisterState(const std::string& name, std::unique_ptr<BossState> state);

	/// <summary>
	/// 現在の状態を取得
	/// </summary>
	/// <returns>現在の状態ポインタ</returns>
	BossState* GetCurrentState() const { return currentState_; }

	/// <summary>
	/// 現在の状態名を取得
	/// </summary>
	/// <returns>現在の状態名</returns>
	const std::string& GetCurrentStateName() const;

	/// <summary>
	/// 名前で状態を取得
	/// </summary>
	/// <param name="name">取得する状態名</param>
	/// <returns>状態ポインタ（見つからない場合はnullptr）</returns>
	BossState* GetState(const std::string& name) const;

private:
	Boss* boss_;
	BossState* currentState_ = nullptr;
	std::unordered_map<std::string, std::unique_ptr<BossState>> states_;

	static const std::string kEmptyString;
};
