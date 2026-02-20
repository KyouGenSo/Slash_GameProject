#pragma once
#include <memory>
#include <unordered_map>
#include <string>

class Player;
class PlayerState;

/// <summary>
/// プレイヤー状態管理マシン
/// プレイヤーの各状態を管理し、状態遷移を制御
/// </summary>
class PlayerStateMachine
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="player">管理対象のプレイヤー</param>
	PlayerStateMachine(Player* player);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~PlayerStateMachine();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="deltaTime">前フレームからの経過時間</param>
	void Update(float deltaTime);

	/// <summary>
	/// 入力処理
	/// </summary>
	void HandleInput();

	/// <summary>
	/// 状態を名前で変更
	/// </summary>
	/// <param name="stateName">遷移先の状態名</param>
	void ChangeState(const std::string& stateName);

	/// <summary>
	/// 状態をポインタで変更
	/// </summary>
	/// <param name="newState">遷移先の状態ポインタ</param>
	void ChangeState(PlayerState* newState);

	/// <summary>
	/// 状態を登録
	/// </summary>
	/// <param name="name">状態名</param>
	/// <param name="state">状態インスタンス</param>
	void RegisterState(const std::string& name, std::unique_ptr<PlayerState> state);

	/// <summary>
	/// 現在の状態を取得
	/// </summary>
	/// <returns>現在の状態ポインタ</returns>
	PlayerState* GetCurrentState() const { return currentState_; }

	/// <summary>
	/// 名前で状態を取得
	/// </summary>
	/// <param name="name">取得する状態名</param>
	/// <returns>状態ポインタ（見つからない場合は nullptr）</returns>
	PlayerState* GetState(const std::string& name) const;

	/// <summary>
	/// 全ての登録済み状態名を取得
	/// </summary>
	/// <returns>状態名のベクター（ソート済み）</returns>
	std::vector<std::string> GetAllStateNames() const;

private:
	Player* player_;
	PlayerState* currentState_;
	PlayerState* previousState_;
	std::unordered_map<std::string, std::unique_ptr<PlayerState>> states_;
};