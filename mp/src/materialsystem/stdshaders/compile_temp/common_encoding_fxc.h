// Helper Function
float2 fixedSign(float2 v)
{
    return float2((v.x >= 0.0) ? +1.0 : -1.0, (v.y >= 0.0) ? +1.0 : -1.0);
}

// Octagonal Encode / Decode
float2 octEncodeFast(float3 v)
{
    float2 p = v.xy * (1.0 / (abs(v.x) + abs(v.y) + abs(v.z)));

    return (v.z <= 0.0) ? ((1.0 - abs(p.yx)) * fixedSign(p)) : p;
}

float3 octDecodeFast(float2 e) {
    float3 v = float3(e, 1.0 - abs(e.x) - abs(e.y));

    if (v.z < 0)
        v.xy = (1.0 - abs(v.yx)) * fixedSign(v.xy);

    return normalize(v);
}

// SNORM 12
float3 to24Bit(float2 f)
{
    float2 u = round(clamp(f, -1.0, 1.0) * 2047 + 2047);
    float t = floor(u.y / 256.0);

    return floor(float3(
        u.x / 16.0,
        frac(u.x / 16.0) * 256.0 + t,
        u.y - t * 256.0
    )) / 255.0;
}

float2 from24Bit(float3 u)
{
    u *= 255.0;
    u.y *= (1.0 / 16.0);

    float2 s = float2(
        u.x * 16.0 + floor(u.y),
        frac(u.y) * (16.0 * 256.0) + u.z
    );

    return clamp(s * (1.0 / 2047.0) - 1.0, -1.0, 1.0);
}