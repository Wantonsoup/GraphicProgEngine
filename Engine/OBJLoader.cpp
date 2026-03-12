#include "OBJLoader.h"


bool OBJLoader::LoadOBJ(const string& filename, vector<VertexType>& vertices, vector<unsigned int>& indices)
{
	vector <DirectX::XMFLOAT3> temp_positions;
	vector <DirectX::XMFLOAT3> temp_normals;
	vector <DirectX::XMFLOAT2> temp_texcoords;

	ifstream file(filename);
	if (!file.is_open()){
	{
			cout << "Failed to open OBJ: " << filename << endl;
			return false;
	}

	string line;
	while (getline(file, line)) {
		istringstream iss(line);
		string prefix;
		iss >> prefix;
		if (prefix == "v") {
			float x, y, z;
			iss >> x >> y >> z;
			temp_positions.emplace_back(x, y, z);
		}
		else if (prefix == "vn") {
			float x, y, z;
			iss >> x >> y >> z;
			temp_normals.emplace_back(x, y, z);
		}
		else if (prefix == "vt") {
			float u, v;
			iss >> u >> v;
			temp_texcoords.emplace_back(u, v);
		}
		else if (prefix == "f") {
			unsigned int posIdx[3], texIdx[3], normIdx[3];
			char slash; // To consume the '/' characters
			for (int i = 0; i < 3; ++i) {
				iss >> posIdx[i] >> slash >> texIdx[i] >> slash >> normIdx[i];
				posIdx[i]--; // OBJ indices are 1-based
				texIdx[i]--;
				normIdx[i]--;
			}
			for (int i = 0; i < 3; ++i) {
				VertexType vertex;
				vertex.Position = temp_positions[posIdx[i]];
				vertex.TexCoord = temp_texcoords[texIdx[i]];
				vertex.Normal = temp_normals[normIdx[i]];
				vertices.push_back(vertex);
				indices.push_back(static_cast<unsigned int>(vertices.size() - 1));
			}
		}
		
	}

	file.close();
	return true;
}
