//--------------------------------------
// Diffuse reflecton function
//--------------------------------------
// N : Normals (normalized)
// L : Incident vector (normalized)
// C : Incident light (color・intensity)
// K : Reflectance (0 ～ 1.0f)
//---------------------------------------
float3 Diffuse(float3 N,float3 L,float3 C,float3 K)
{
    float D = dot(N, -L);
    D = max(0, D);
    return K * C * D;
}

//--------------------------------------------
//	鏡面反射関数(ブリン・フォン)
//--------------------------------------------
// N:法線(正規化済み)
// L:入射ベクトル(正規化済み)
// C:入射光(色・強さ)
// E:視点方向ベクトル(正規化済み)
// K:反射率(0～1.0)
// Power:ハイライトの強さ(輝度)

float3 BlinnPhongSpecular(float3 N, float3 L, float3 C, float3 E,
	float3 K, float Power)
{
	//ハーフベクトル
    float3 H = normalize(-L + E);

    float3 S = dot(H, N);
    S = max(0, S);
    S = pow(S, Power);
    S = S * K * C;
    return S;
}

//-------------------------------------
//  Phong's specular reflection function
//-------------------------------------
// N     : Normals (normalized)
// L     : Incident vector (normalized)
// E     : Eye vector
// S     : Strength of reflected light in the mirror
// K     : reflection coefficient
// power : power
//--------------------------------------
float3 PhongSpecular(float3 N, float3 L, float3 E, float S, float K, float power)
{
    float3 R = -E + 2 * dot(N, E) * N;

    float3 D = max(0, dot(-L, R));

    return S * K * pow(D, power);
}



//***********************************************
//
//  Distribution function
//
//***********************************************

//---------------------------------------------
// Beckmann distribution function
//---------------------------------------------
// m  : Roughness
// NH : dot (Normal * Harf vector)
//---------------------------------------------
float Beckmann (float m,float NH)
{
    float NH2 = NH * NH;
    
    return exp(-(1 - NH2) / (NH2 * m * m)) / (4 * m * m * NH2 * NH2);
}

