
struct VS_INPUT
{
	float4 Position : POSITION;
	float2 UV0 : TEXCOORD0;
	float4 Color : COLOR0;
};

struct VS_OUTPUT
{
	float4 Position : POSITION;
	float2 UV0 : TEXCOORD0;
	float4 Color : COLOR0;
};

VS_OUTPUT main( VS_INPUT In )
{
	VS_OUTPUT Out;
	Out.Position = In.Position;
	Out.Color = In.Color;
	Out.UV0 = In.UV0;
	
	return Out;
}
