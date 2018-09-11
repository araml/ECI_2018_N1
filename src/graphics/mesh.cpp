#include <mesh.h>
#include <fstream>

bool empty_line(const std::string &line) {
    for (size_t i = 0; line[i] != '\n'; i++)
        if (line[i] != ' ')
            return false;
    return true;
}

std::vector<std::string> split(std::string line) {
    std::string s{""};
    std::vector<std::string> result;
    for (size_t i = 0; i < line.size() + 1; i++) {
        if (empty_line(line))
            continue;
        if (line[i] == ' ' || line[i] == '\n') {
            result.push_back(s);
            s = std::string{""};
        } 

        s.push_back(line[i]);

        if (i == line.size() - 1) {
            result.push_back(s);
            s = std::string{ "" };
        }
    } 

   for (auto w : result)
        printf("%s ", w.c_str());
    printf("\n");
    return result;
}

mesh::mesh(std::string obj_path, ID3D11Device *dev, ID3D11DeviceContext *dev_con) {
    //return; 
    vertex a_vertices[] = {

      { -0.5f, -0.5f, -0.5f, D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f) },
       { -0.5f,  0.5, -0.5f, D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f) },
       {  0.5f,  0.5f, -0.5f, D3DXCOLOR(0.0f, 1.f, 1.0f, 1.0f) },
       {  0.5f,  -0.5f, -0.5f, D3DXCOLOR(1.0f, 0.f, 0.f, 1.0f) },

       { -0.5f, -0.5f, .5f, D3DXCOLOR(1.f, 0.0f, 1.f, 1.0f) },
       { -0.5f,  0.5, .5f, D3DXCOLOR(1.0f, 1.f, 0.0f, 1.0f) },
       { 0.5f,  0.5f, .5f, D3DXCOLOR(1.0f, 1.0f, 1.f, 1.0f) },
       { 0.5f,  -0.5f, .5f, D3DXCOLOR(0.f, 0.f, 0.f, 1.0f) },

   };
    
   WORD indices[] = {
        0, 1, 2,
        0, 2, 3,

        5, 0, 4,
        5, 1, 0,
        // Right face
        3, 2, 6,
        3, 6, 7,

        6, 4, 7,
        6, 5, 4,

        1, 5, 6,
        1, 6, 2,

        0, 4, 7,
        0, 7, 3,
  };
    
    D3D11_BUFFER_DESC buffer_descriptor;
    ZeroMemory(&buffer_descriptor, sizeof(buffer_descriptor));
    
    buffer_descriptor.Usage = D3D11_USAGE_DYNAMIC;
    buffer_descriptor.ByteWidth = sizeof(a_vertices);
    buffer_descriptor.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    buffer_descriptor.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    dev->CreateBuffer(&buffer_descriptor, nullptr, &vertex_buffer);

    D3D11_MAPPED_SUBRESOURCE ms;
    dev_con->Map(vertex_buffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
    memcpy(ms.pData, a_vertices, sizeof(a_vertices));
    dev_con->Unmap(vertex_buffer, NULL);

    num_verts = sizeof(indices) / sizeof(WORD);

    D3D11_BUFFER_DESC index_buffer_descriptor;
    ZeroMemory(&index_buffer_descriptor, sizeof(index_buffer_descriptor));
    index_buffer_descriptor.Usage = D3D11_USAGE_DEFAULT;
    index_buffer_descriptor.ByteWidth = sizeof(indices);
    index_buffer_descriptor.BindFlags = D3D11_BIND_INDEX_BUFFER;
    index_buffer_descriptor.CPUAccessFlags = 0;
    index_buffer_descriptor.MiscFlags = 0;

    ID3D11Buffer *index_buffer;
    D3D11_SUBRESOURCE_DATA index_data;
    index_data.pSysMem = indices;
    dev->CreateBuffer(&index_buffer_descriptor, &index_data, &index_buffer);
    dev_con->IASetIndexBuffer(index_buffer, DXGI_FORMAT_R16_UINT, 0);

    /*
    std::ifstream fp(obj_path);
    if (!fp.is_open()) {
        printf("Error when opening file %s", obj_path.c_str());
        exit(-1);
    }

    std::string line;
    while (std::getline(fp, line)) {
        if (line.empty())
            continue;
        std::vector<std::string> words = split(line);
        if (words[0] == "v") { // We add vertices
            vertices.push_back({ std::stof(words[1]), 
                                 std::stof(words[2]), 
                                 std::stof(words[3]),
                                 {1.0, 1.0, 1.0, 1.0} });
        } else if (words[0] == "f") { // the vertices index.
            indices.push_back(std::stoi(words[1]));            
            indices.push_back(std::stoi(words[2]));
            indices.push_back(std::stoi(words[3]));
        }
    }
    /*
    D3D11_BUFFER_DESC v_buffer_desc; // vertex buffer
    ZeroMemory(&v_buffer_desc, sizeof(v_buffer_desc));
    v_buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
    v_buffer_desc.ByteWidth = sizeof(vertex) * vertices.size();
    v_buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    v_buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    dev->CreateBuffer(&v_buffer_desc, nullptr, &vertex_buffer);

    D3D11_MAPPED_SUBRESOURCE ms;
    dev_con->Map(vertex_buffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
    memcpy(ms.pData, (void *)vertices.data(), sizeof(vertex) * vertices.size());
    dev_con->Unmap(vertex_buffer, NULL);
    
    D3D11_BUFFER_DESC i_buffer_desc;
    ZeroMemory(&i_buffer_desc, sizeof(i_buffer_desc));
    i_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    i_buffer_desc.ByteWidth = sizeof(WORD) * indices.size();
    i_buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    i_buffer_desc.CPUAccessFlags = 0;
    i_buffer_desc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA index_data;
    index_data.pSysMem = indices.data();
    dev->CreateBuffer(&i_buffer_desc, &index_data, &index_buffer);
    dev_con->IASetIndexBuffer(index_buffer, DXGI_FORMAT_R16_UINT, 0);*/
}

mesh::~mesh() {}

