struct vertex_out {
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

vertex_out v_shader(float4 position : POSITION, float4 color : COLOR) {
    vertex_out output;

    output.position = position;
    output.color = color;

    return output;
}


float4 p_shader(float4 position : SV_POSITION, float4 color : COLOR) : SV_TARGET {
    return color;
}