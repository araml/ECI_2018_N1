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
        if (line[i] == ' ' || line[i] == '\0' ||line[i] == '\n') {
            result.push_back(s);
            s = std::string{""};
        } 

        s.push_back(line[i]);
    } 

 //  for (auto w : result)
  //      printf("%s ", w.c_str());
  //  printf("\n");
    return result;
}

mesh::mesh(std::string obj_path, ID3D11Device *dev, ID3D11DeviceContext *dev_con) {
    std::ifstream fp(obj_path);
    if (!fp.is_open()) {
        printf("Error when opening file %s", obj_path.c_str());
        exit(-1);
    }

    std::string line;
    float i = 0;
    float k = 0.1;
    float z = 0;
    while (std::getline(fp, line)) {
        if (line.empty())
            continue;
        std::vector<std::string> words = split(line);
        if (words[0] == "v") { // We add vertices
            vertices.push_back({ (float)std::stod(words[1]), 
                                 (float)std::stod(words[2]),
                                 (float)std::stod(words[3]),
                                 {0.5, 0.5, 0.5, 1.0} });
        } else if (words[0] == "f") { // the vertices index.
            indices.push_back(std::stoi(words[1]) - 1);            
            indices.push_back(std::stoi(words[2]) - 1);
            indices.push_back(std::stoi(words[3]) - 1);
        }

        i += 0.1;
        z += 0.3;
        k *= 2;
        if (i >= 1.0) {
            i = 0;
            z = 0;
            k = 0.1;
        }
    }

    printf("Vertices %d\n", vertices.size());
    printf("faces %d\n", indices.size());
    
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
    //dev_con->IASetIndexBuffer(index_buffer, DXGI_FORMAT_R16_UINT, 0);
    
    num_verts = indices.size();
}

mesh::~mesh() {}

