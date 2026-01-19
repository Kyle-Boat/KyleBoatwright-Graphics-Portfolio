

struct ShaderData
{
    float4x4 projection;
    float4x4 view;
    float4x4 model[3];
    float4   lightPos;
    uint     selected;
};

struct VSIn
{
    float3 pos    : LOCATION0;
    float3 normal : LOCATION1;
    float2 uv     : LOCATION2;
};

struct VSOut
{
    float4 pos        : SV_Position;
    float3 normalWS   : LOCATION0;
    float3 worldPos   : LOCATION1;
    float  viewZ      : LOCATION2;
    uint   instId     : LOCATION3;
};

[[vk::binding(0, 0)]]
ConstantBuffer<ShaderData> gShader;

[shader("vertex")]
VSOut mainVS(VSIn input, uint instanceId : SV_InstanceID)
{
    ShaderData sd = gShader;

    float4x4 M = sd.model[instanceId];

    float4 worldPos4 = mul(M, float4(input.pos, 1.0));
    float3 worldPos  = worldPos4.xyz;

    float3 nWS = normalize(mul((float3x3)M, input.normal));

    float4 viewPos = mul(sd.view, worldPos4);
    float4 clipPos = mul(sd.projection, viewPos);

    VSOut o;
    o.pos      = clipPos;
    o.normalWS = nWS;
    o.worldPos = worldPos;
    o.viewZ    = -viewPos.z;     
    o.instId   = instanceId;
    return o;
}

[shader("fragment")]
float4 mainFS(VSOut input) : SV_Target0
{
    ShaderData sd = gShader;

    float3 N = normalize(input.normalWS);

    float3 L;
    if (sd.lightPos.w == 0.0)
    {
        L = normalize(-sd.lightPos.xyz);
    }
    else
    {
        L = normalize(sd.lightPos.xyz - input.worldPos);
    }

    float ndl = max(dot(N, L), 0.0);

    float3 albedo  = float3(0.85, 0.85, 0.9);
    float3 ambient = 0.12 * albedo;
    float3 diffuse = ndl * albedo;

    float3 color = ambient + diffuse;

    float fadeStart = 5.0;
    float fadeEnd   = 15.0;
    float t = saturate((fadeEnd - input.viewZ) / (fadeEnd - fadeStart));
    float alpha = t * t * (3.0 - 2.0 * t); // smoothstep

    return float4(color, alpha);
}
