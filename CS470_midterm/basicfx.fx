// World-View-Projection matrix buffer
cbuffer cbPerObject
{
	float4x4 gWVP;
};

// Input vertex datatype
struct VIn
{
	float3 inPos : POSITION;
	float4 inCol : COLOR;
};

// Output vertex datatype
struct VOut
{
	float4 pos : SV_POSITION;
	float4 col : COLOR;
};

// Vertex shader
VOut VS(VIn vin)
{
	VOut vout;

	// Apply world-view-projection transformation
	vout.pos = mul(float4(vin.inPos, 1.0f), gWVP);

	// Pass color through
	vout.col = vin.inCol;

	// Return transformed vertex
	return vout;
}

// Pixel shader
float4 PS(VOut pin) : SV_Target
{
	// Simply set color
	return pin.col;
}

// Technique
technique10 BasicTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}
}
