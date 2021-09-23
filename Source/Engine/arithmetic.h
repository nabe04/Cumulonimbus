#pragma once
#include <string>

#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>

namespace arithmetic
{
	/**
	 * @brief : 直線の定義
	 */
	struct Line
	{
		DirectX::SimpleMath::Vector3 point;	// 直線上の任意の点
		DirectX::SimpleMath::Vector3 vec;	// 直線の方向ベクトル
	};

	/**
	 * @brief : ウィンドウ比率を元にウィンドウサイズを計算
	 * @remark : widthのパラメータを優先にサイズを計算する
	 * @remark : widthから算出された計算後のウィンドウ縦幅が
	 *			 PCの縦画面のサイズよりも大きかった場合height
	 *			 のパラメータからウィンドウサイズを算出する
	 * @param aspect_ratio : ウィンドウ比率
	 * @param width : 計算前のウィンドウ横幅
	 * @param height : 計算前のウィンドウ縦幅
	 * @return : aspect_ratioに基づいて再計算されたウィンドウサイズ
	 */
	[[nodiscard]]
	DirectX::SimpleMath::Vector2 CalcWindowSize(
		const DirectX::XMUINT2& aspect_ratio,
		int width, int height);

	/**
	 * @brief  : 丸め誤差を考慮した同値関数
	 * @return : true -> 同値
	 */
	[[nodiscard]]
	bool IsEqual(float l_val, float r_val);

	/**
	 * @brief : [min,max]の範囲までnをクランプ
	 */
	[[nodiscard]]
	float Clamp(float n, float min, float max);

	/**
	 * @brief : スクリーン座標からNDC座標への変換
	 * @param screen_pos : スクリーン位置(Z値は無視される)
	 * @param window_width : ウィンドウ幅
	 * @param window_height : ウィンドウ高さ
	 * @return : NDC空間上の座標
	 */
	[[nodiscard]]
	DirectX::SimpleMath::Vector2 ConvertScreenToNDC(const DirectX::SimpleMath::Vector3& screen_pos,
													float window_width, float window_height);
	/**
	 * @brief : スクリーン座標からNDC座標への変換
	 * @param screen_pos : スクリーン位置(Z値は無視される)
	 * @param window_width : ウィンドウ幅
	 * @param window_height : ウィンドウ高さ
	 * @return : NDC空間上の座標
	 */
	[[nodiscard]]
	DirectX::SimpleMath::Vector2 ConvertScreenToNDC(const DirectX::SimpleMath::Vector2& screen_pos,
													float window_width, float window_height);

	/**
	 * @brief : スクリーン座標からワールド座標への変換
	 * @param screen_pos : スクリーン位置(z値を含む)
	 * @param view_mat : ビュー行列
	 * @param projection_mat : プロジェクション行列
	 * @return :
	 */
	[[nodiscard]]
	DirectX::SimpleMath::Vector3 ConvertScreenToWorld(DirectX::SimpleMath::Vector3 screen_pos,
													  const DirectX::SimpleMath::Matrix& view_mat,
													  const DirectX::SimpleMath::Matrix& projection_mat);

	[[nodiscard]]
	DirectX::XMFLOAT2 CalcVecFromTwoPositions(DirectX::XMFLOAT2 vec1, DirectX::XMFLOAT2 vec2);
	[[nodiscard]]
	DirectX::XMFLOAT3 CalcVecFromTwoPositions(DirectX::XMFLOAT3 vec1, DirectX::XMFLOAT3 vec2);

	/**
	* @brief		: upベクトルの算出
	* @param front  : front vector(z軸)
	* @param right  : right vector(x軸)
	*/
	[[nodiscard]]
	DirectX::XMFLOAT3 CalcUpVec(
		const DirectX::XMFLOAT3& front,
		const DirectX::XMFLOAT3& right);

	/**
	* @brief		: rightベクトルの算出
	* @param up     : up vector(y軸)
	* @param front  : front vector(z軸)
	*/
	[[nodiscard]]
	DirectX::XMFLOAT3 CalcRightVec(
		const DirectX::XMFLOAT3& up,
		const DirectX::XMFLOAT3& front);

	/**
	* @brief	   : calculate front vector
	* @param right : right vector(x軸)
	* @param up    : up vector(y軸)
	*/
	[[nodiscard]]
	DirectX::XMFLOAT3 CalcFrontVec(
		const DirectX::XMFLOAT3& right,
		const DirectX::XMFLOAT3& up);

	[[nodiscard]]
	DirectX::XMFLOAT3 SphereLinear(
		const DirectX::XMVECTOR& v1,
		const DirectX::XMVECTOR& v2, float s);

	/**
	 * @brief	 : 角度を(-180° ～ 180°)に正規化
	 * @example	 : 190° -> -170°、-200° -> 160°
	 */
	[[nodiscard]]
	float NormalizeAngle(const float angle);

	/**
	 * @brief     : 二つのベクトルから角度(ラジアン)を算出
	 * @attention : 引数に指定するベクトルは正規化済みのベクトルを使用する事
	 * @return    : 弧度法での角度
	 */
	[[nodiscard]]
	float CalcAngleFromTwoVec(
		const DirectX::SimpleMath::Vector3& v0,
		const DirectX::SimpleMath::Vector3& v1);

	/**
	 * @brief : 左手座標系を基底にした時のそれぞれの角度
	 */
	[[nodiscard]] float CalcAngle_X(const DirectX::SimpleMath::Vector3& v);
	[[nodiscard]] float CalcAngle_Y(const DirectX::SimpleMath::Vector3& v);
	[[nodiscard]] float CalcAngle_Z(const DirectX::SimpleMath::Vector3& v);

	/**
	 * @brief : クォータニオンからオイラー角の算出
	 */
	DirectX::SimpleMath::Vector3 QuaternionToEulerAngle(const DirectX::SimpleMath::Quaternion& q);

	/**
	 * @brief				: 正射影ベクトルの算出
	 *						  project_vec から onto_vecへの正射影ベクトルを算出する
	 * @param project_vec	: 射影元ベクトル
	 * @param onto_vec		: 射影先ベクトル
	 */
	[[nodiscard]]
	DirectX::XMFLOAT3 CalcProjectVector(
		const DirectX::XMFLOAT3& project_vec,
		const DirectX::XMFLOAT3& onto_vec);

	/**
	 * @brief  : 乱数生成(int)
	 * @return : minからmaxの範囲での乱数(int)
	 */
	[[nodiscard]]
	int RandomIntInRange(int min, int max);
	/**
	 * @brief  : 乱数生成(float)
	 * @return : minからmaxの範囲での乱数(float)
	 */
	[[nodiscard]]
	float RandomFloatInRange(float min, float max);

	/**
	 * @brief  : 2つのベクトルが平行どうか判定
	 * @return : true -> 平行、 false -> 平行じゃない
	 */
	[[nodiscard]]
	bool IsParallel(
		const DirectX::SimpleMath::Vector3& v1,
		const DirectX::SimpleMath::Vector3& v2);

	/**
	 * @brief			: 直線と点との最近接点を算出
	 * @param l_start	: 直線の端点(このままだと線分になるので関数内でLineに変換)
	 * @param l_end		: 直線の端点(このままだと線分になるので関数内でLineに変換)
	 * @param p			: 任意の点
	 * @return			: 直線上の最近接点の位置
	 */
	[[nodiscard]]
	DirectX::SimpleMath::Vector3 ClosestPtPointLine(
		const DirectX::SimpleMath::Vector3& l_start,
		const DirectX::SimpleMath::Vector3& l_end,
		const DirectX::SimpleMath::Vector3& p
	);

	/**
	 * @brief	: 直線と点との最近接点を算出
	 * @param l	: 直線l
	 * @param p	: 任意の点p
	 * @return  : 直線上の最近接点の位置
	 */
	[[nodiscard]]
	DirectX::SimpleMath::Vector3 ClosestPtPointLine(
		const Line& l,
		const DirectX::SimpleMath::Vector3& p
	);

	/**
	 * @brief   : 点cと線分abの最短距離の平方の算出
	 * @param a : 線分abの端点a
	 * @param b : 線分abの端点b
	 * @param c : 点c
	 * @return  : 最短距離の平方の値
	 */
	[[nodiscard]]
	float SqDistPointSegment(
		const DirectX::SimpleMath::Vector3& a,
		const DirectX::SimpleMath::Vector3& b,
		const DirectX::SimpleMath::Vector3& c
	);

	/**
	 * @brief	  : 与えられた線分abおよび点cに対して、ab上の最近接点dを計算
	 *			  : d(t) = a + t * (b - a)により表されるdの位置に対するtも返す
	 * @param a	  : 線分abの端点
	 * @param b   : 線分abの端点
	 * @param c	  : 任意の点
	 * @param d	  : 最近接点
	 * @param t	  : dの位置に対するt
	 * @attention : d,tは参照渡しをしているので算出された結果が入ることに注意
	 */
	void ClosestPtPointSegment(
		const DirectX::SimpleMath::Vector3& a,
		const DirectX::SimpleMath::Vector3& b,
		const DirectX::SimpleMath::Vector3& c,
		DirectX::SimpleMath::Vector3& d,
		float& t);

	/**
	 * @brief			: 2直線の最短距離の算出
	 * @param l1_start	: 直線の端点(このままだと線分になるので関数内でLineに変換)
	 * @param l1_end    : 直線の端点(このままだと線分になるので関数内でLineに変換)
	 * @param l2_start  : 直線の端点(このままだと線分になるので関数内でLineに変換)
	 * @param l2_end    : 直線の端点(このままだと線分になるので関数内でLineに変換)
	 * @param p1		: 直線l1の垂線の位置
	 * @param p2		: 直線l2の垂線の位置
	 * @return			: 2直線の最短距離
	 */
	[[nodiscard]]
	float ClosestPtLineLine(
		const DirectX::SimpleMath::Vector3& l1_start,
		const DirectX::SimpleMath::Vector3& l1_end,
		const DirectX::SimpleMath::Vector3& l2_start,
		const DirectX::SimpleMath::Vector3& l2_end,
		DirectX::SimpleMath::Vector3& p1,
		DirectX::SimpleMath::Vector3& p2
	);

	/**
	 * @brief			: 2直線の最短距離の算出
	 * @param l1		: 直線の設定
	 * @param l2		: 直線の設定
	 * @param p1		: 直線l1の垂線の位置
	 * @param p2		: 直線l2の垂線の位置
	 * @return			: 2直線の最短距離
	 */
	[[nodiscard]]
	float ClosestPtLineLine(
		const Line& l1,
		const Line& l2,
		DirectX::SimpleMath::Vector3& p1,
		DirectX::SimpleMath::Vector3& p2
	);

	/**
	 * @brief			: 線分と線分の最近接点と最近接点間の距離の算出
	 * @param p_start	: 線分pの始点
	 * @param p_end		: 線分pの終点
	 * @param q_start	: 線分qの始点
	 * @param q_end		: 線分qの終点
	 * @param s			: S1(s) = p_start + s * (p_end - p_start)
	 * @param t			: S2(t) = q_start + t * (q_end - q_start)
	 * @param c1		: 点pの最近接点
	 * @param c2		: 点qの最近接点
	 * @return			: 最近接点間の距離(最短距離)
	 */
	[[nodiscard]]
	float ClosestPtSegmentSegment(
		const DirectX::SimpleMath::Vector3& p_start,
		const DirectX::SimpleMath::Vector3& p_end,
		const DirectX::SimpleMath::Vector3& q_start,
		const DirectX::SimpleMath::Vector3& q_end,
		float& s, float& t,
		DirectX::SimpleMath::Vector3& c1,
		DirectX::SimpleMath::Vector3& c2);
} // arithmetic