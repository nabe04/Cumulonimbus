#pragma once

#include <functional>
#include <map>

namespace cumulonimbus::system
{
	template<class Key,class ReturnVal = void,class... Args>
	class BehaviorTree final
	{
	public:
		using Sequence = std::vector<std::function<ReturnVal(Args...)>>;

		/**
		 * @brief : ビヘイビアツリーの新規作成
		 */
		void CreateBehavior(const Key behavior_type)
		{
			if (behaviors.contains(behavior_type))
				return;

			behaviors.emplace(behavior_type, Sequence{});
		}

		/**
		 * @brief : ビヘイビアツリー内関数の追加
		 */
		void AddSequence(const Key behavior_type, std::function<ReturnVal(Args...)> callback)
		{
			// ビヘイビアが存在しない場合新しく作る
			if (!behaviors.contains(behavior_type))
				CreateBehavior(behavior_type);

			behaviors.at(behavior_type).emplace_back(callback);
		}

		void Clear(const Key behavior_type)
		{
			if (!behaviors.contains(behavior_type))
				return;

			behaviors.erase(behavior_type);
		}

		void ClearAll()
		{
			behaviors.clear();
		}

		void Update(Args... args)
		{
			// ビヘイビアの処理が登録されていない場合処理を中断する
			if (!behaviors.contains(current_behavior))
				//return;
				assert(!"Not found behavior");

			// 現在のビヘイビアが全て終了しているので処理を飛ばす
			if (is_completed)
				return;

			// ビヘイビアが変更された処理の初めのフレームの場合
			if (is_switched)
			{
				// 初期フラグをtrueにする
				is_start = true;
				is_switched = false;
			}

			// ビヘイビアツリー更新処理
			behaviors.at(current_behavior).at(index)(args...);

			// 初期フラグがTrueの場合Falseに
			if (is_start)
				is_start = false;
		}

		void NextSequence()
		{
			++index;
			is_switched = true;
			is_start	= true;

			if (index >= behaviors.at(current_behavior).size())
			{
				index = 0;
				is_completed = true;
			}
		}

		void SetBehavior(const Key behavior)
		{
			if(!behaviors.contains(behavior))
				assert(!"Not found behavior");

			old_behavior		= current_behavior;
			current_behavior	= behavior;
			index				= 0;
			is_switched			= true;
			is_start			= true;
			is_completed		= false;

		}

		[[nodiscard]]
		bool GetIsStart() const
		{
			return is_start;
		}

		[[nodiscard]]
		bool GetIsCompleted() const
		{
			return is_completed;
		}
	private:
		// 現在のビヘイビア状態
		Key current_behavior{};
		// 過去のビヘイビア状態
		Key old_behavior{};
		// ビヘイビアツリーの要素番号
		u_int index{ 0 };
		// ビヘイビアが変更されたか
		bool is_switched{ false };
		// ビヘイビアが変更された最初のフレームか
		bool is_start{ false };
		// 現在のビヘイビアツリーに登録されている処理が全て完了したか
		bool is_completed{ false };
		// ビヘイビアツリー本体
		std::map<Key, Sequence> behaviors{};
	};
} // cumulonimbus::system
