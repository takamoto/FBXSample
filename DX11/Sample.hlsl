// �T���v���`��

//! �R���X�^���g�o�b�t�@
/*!
�R���X�^���g�o�b�t�@���g�p���邱�Ƃł������̃R���X�^���g���W�X�^��1�܂Ƃ߂ɂ��邱�Ƃ��ł���
���W�X�^�̕ω��̃^�C�~���O�ɂ���ĕ����Ă��������悢
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

//! ���_����
/*!
��{��D3D9�ƕς��Ȃ�
�������A�������̃Z�}���e�B�N�X���ς���Ă���
�V�X�e�����ɓn���Z�}���e�B�N�X��SV_�̐ړ��������Ă���
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

//! ���_�V�F�[�_
OutputVS RenderVS( InputVS inVert )
{
	matrix	mtxWV = mul(mtxWorld, mtxView);
	matrix	mtxWVP = mul(mtxWV, mtxProj);

	OutputVS	outVert;

	outVert.pos_wvp = mul(float4(inVert.pos, 1), mtxWVP);
	outVert.uv = inVert.uv;
	outVert.normal_w = normalize(mul(inVert.normal, (float3x3)mtxWorld)); // ���ӁF�K��float3�ɂ��Ă���normalize
	outVert.lightDir_w = normalize(float3(6.0f, 0, -5.0f));

	return outVert;
}


//! �s�N�Z���V�F�[�_
float4 RenderPS(OutputVS inPixel) : SV_TARGET
{
	//return float4(inPixel.normal_w, 1); // normal�m�F3
	float3 light_w = -1 * inPixel.lightDir_w;
	float3 white = { 1.0f, 1.0f, 1.0f };
	return float4(white, 1);//calcColor(diffuse, light_w, inPixel.normal_w, specular);
}