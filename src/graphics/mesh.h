#pragma once

#include <d3d11.h>
#include <string>
#include <vector>
#include <vertex.h>

class mesh {
public:
    mesh(std::string obj_path, ID3D11Device *dev, ID3D11DeviceContext *dev_con);
    ~mesh();
    ID3D11Buffer *vertex_buffer;
    ID3D11Buffer *index_buffer;

    std::vector<vertex> vertices;
    std::vector<WORD> indices;
    int num_verts;
};

