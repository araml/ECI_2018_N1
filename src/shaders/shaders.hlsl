cbuffer constant_buffer : register (b0) {
    matrix wpv;
}

struct vertex_out {
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

vertex_out v_shader(float4 position : POSITION, float4 color : COLOR) {
    vertex_out output;

    output.position = mul(position, wpv);
    output.color = color;

    return output;
}


float4 p_shader(vertex_out input) : SV_TARGET {
    return input.color;
}