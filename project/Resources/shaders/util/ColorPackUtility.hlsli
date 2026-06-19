uint PackColor(float4 _color)
{
    uint r = (uint) (saturate(_color.r) * 255.0) << 24;
    uint g = (uint) (saturate(_color.g) * 255.0) << 16;
    uint b = (uint) (saturate(_color.b) * 255.0) << 8;
    uint a = (uint) (saturate(_color.a) * 255.0);
    return r | g | b | a;
}

float4 PackColor(uint _color)
{
    float4 color;
    color.r = ((_color >> 24) & 0xFF) / 255.0;
    color.g = ((_color >> 16) & 0xFF) / 255.0;
    color.b = ((_color >> 8) & 0xFF) / 255.0;
    color.a = (_color & 0xFF) / 255.0;
    return color;
}