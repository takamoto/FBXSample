// サンプル描画

//! コンスタントバッファ
/*!
コンスタントバッファを使用することでいくつかのコンスタントレジスタを1まとめにすることができる
レジスタの変化のタイミングによって分けておく方がよい
*/
cbuffer cbSceneParam : register( b0 )
{
	matrix	mtxView			: packoffset( c0 );
	matrix	mtxProj			: packoffset( c4 );
};

cbuffer cbMeshParam : register( b1 )
{
	matrix	mtxWorld		: packoffset( c0 );
};

cbuffer cbMaterialParm : register(b2)
{
	float3 ambient : packoffset(c0);
	float3 diffuse : packoffset(c1);
	float3 emissive : packoffset(c2);
	float3 specular : packoffset(c3);
};

//! 頂点属性
/*!
基本はD3D9と変わらない
ただし、いくつかのセマンティクスが変わっている
システム側に渡すセマンティクスはSV_の接頭辞がついている
*/
struct InputVS
{
	float3	pos			: IN_POSITION;
	float3  normal		: IN_NORMAL;
	float2  uv          : IN_UV;
};
struct OutputVS
{
	float4	pos_wvp		: SV_POSITION;
	float3  normal_w	: IN_NORMAL;
	float2  uv			: IN_UV;
	float3  lightDir_w	: LIGHT;
};

Texture2D texDiffuse : register(t0);
Texture2D texNormal : register(t1);
Texture2D texSpecular : register(t2);
SamplerState samDiffuse : register(s0);

//! 頂点シェーダ
OutputVS RenderVS( InputVS inVert )
{
	matrix	mtxWV = mul(mtxWorld, mtxView);
	matrix	mtxWVP = mul(mtxWV, mtxProj);

	OutputVS	outVert;

	outVert.pos_wvp = mul(float4(inVert.pos, 1), mtxWVP);
	outVert.uv = inVert.uv;
	outVert.normal_w = normalize(mul(inVert.normal, (float3x3)mtxWorld)); // 注意：必ずfloat3にしてからnormalize
	outVert.lightDir_w = normalize(float3(6.0f, 0, -5.0f));

	return outVert;
}


//! ピクセルシェーダ
float4 RenderPS(OutputVS inPixel) : SV_TARGET
{
	//return float4(inPixel.normal_w, 1); // normal確認3
	float3 light_w = -1 * inPixel.lightDir_w;
	float3 white = { 1.0f, 1.0f, 1.0f };
	return float4(white, 1);//calcColor(diffuse, light_w, inPixel.normal_w, specular);
}