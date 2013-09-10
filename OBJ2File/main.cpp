/*****************************************************************
*	OBJ2File   By Jin Kim
*
*   Converts any OBJ file into a suitable XML resource for android
*
*	Takes three (3) arguments: output XML files for vertex, normal, 
*	and texture arrays.
*
******************************************************************/


#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <cstdio>
#include <string>
#include <fstream>
#include <vector>

int main(int argc, char *argv[]){

	std::string filename;
	std::ifstream objFile;
	std::ofstream vertFile, texFile, normFile;
	std::vector<std::vector<float> > verts;
	std::vector<std::vector<float> > tex;
	std::vector<std::vector<float> > normals;
	std::vector<int> finalVerts;
	std::vector<int> finalTex;
	std::vector<int> finalNormals;

	char buffer[256];
	char *charBuffer;

	int vertCount = 0, normCount = 0, texCount = 0, faceCount = 0;

	if(argc < 4){
		std::cout << "not enough arguments!\r\n\r\nRequires at least 3 arguments:" << std::endl;

		std::cout << "- vertex array filename...";
		if(argc < 2)
			std::cout<< "MISSING" << std::endl;
		else
			std::cout << "OK" << std::endl;

		std::cout << "- normal array filename...";
		if(argc < 3)
			std::cout << "MISSING" << std::endl;
		else
			std::cout << "OK" << std::endl;
		
		std::cout << "- texture array filename...MISSING\r\n" << std::endl;

		system("pause");
	}
	else{
		
		std::string vertArrayName = argv[1];
		std::string normArrayName = argv[2];
		std::string texArrayName = argv[3];

		objFile.open("monkey_normals.obj");

		if(objFile.bad() || objFile.fail()){
			std::cout << "file opening error! Better check that the filename is correct" << std::endl;
			system("pause");
			//exit(-1);
		}

		vertFile.open("vertex_array.xml", std::ofstream::out);
		texFile.open("texture_array.xml", std::ofstream::out);
		normFile.open("normal_array.xml", std::ofstream::out);

		if(vertFile.fail() || texFile.fail() || normFile.fail()){
			std::cout << "Couldn't open output file! Exiting." << std::endl;
			system("pause");
			/*exit(-1);*/
		}

		//parse obj file of vertices and faces
		while(objFile.good()){
			objFile.getline(buffer, 255);

			charBuffer = strtok(buffer," \0");

			if(buffer[0] == '#')
				continue;

			//verts, normals, and texture coordinates
			else if (buffer[0] == 'v'){

				float temp[3];

				for(int j = 0; j < 3; j++){
					charBuffer = strtok(NULL, " \0");
					temp[j] = atof(charBuffer);
				}
				if(buffer[1] == 'n'){
					normals.push_back(std::vector<float>(temp, temp + sizeof(temp)/sizeof(temp[0])));
					normCount++;
				} else if(buffer[1] == 't'){
					tex.push_back(std::vector<float>(temp, temp + sizeof(temp)/sizeof(temp[0])));
					texCount++;
				} else {
					verts.push_back(std::vector<float>(temp, temp + sizeof(temp)/sizeof(temp[0])));
					vertCount++;
				}
			}

			//Faces
			else if(buffer[0] == 'f'){
				int firstSlash, secondSlash, thirdSlash;

				for(int i = 0; i < 3; i++) {
					std::string stemp = (charBuffer = strtok(NULL, " "));
					//stemp.append("\0");

					//if '/' is found - i.e. mesh contains normals and/or textures
					if((firstSlash = stemp.find_first_of("/")) != std::string::npos){

						//take firstSlash # of chars from stemp, and convert that into a float
						float vertIndex = atof(stemp.substr(0, firstSlash).c_str()) - 1;

						for(int q = 0; q < 3; q++) {
							finalVerts.push_back(verts[vertIndex][q] * 1000000);
						}


						//check if there is a second slash
						if((secondSlash = stemp.find_first_of("/", firstSlash+1)) != std::string::npos){

							//if there are two slashes in a row, '//', no texture coordinates
							if((secondSlash - firstSlash) <= 1){
								float normIndex = atof(stemp.substr(secondSlash+1).c_str()) - 1;
								for(int t = 0; t < 3; t++)
									finalNormals.push_back(normals[normIndex][t] * 1000000);
							} 

							//if no two '/'s are in a row, then we have vert/tex/normal
							else {
								float texIndex = atof(stemp.substr(firstSlash, secondSlash-firstSlash).c_str()) - 1;
								float normIndex = atof(stemp.substr(secondSlash).c_str()) - 1;

								for(int w = 0; w < 3; w++)
									finalTex.push_back(tex[texIndex][w] * 1000000);

								for(int t = 0; t < 3; t++)
									finalNormals.push_back(normals[normIndex][t] * 1000000);
							}
						}

						//if there is no second slash, i.e. vert/tex
						else {
							float texIndex = atof(stemp.substr(firstSlash, secondSlash-firstSlash).c_str()) - 1;

							for(int w = 0; w < 3; w++)
								finalTex.push_back(tex[texIndex][w]);
						}
					} 

					else {	//no normals or tex coords 
						int index = atof(charBuffer) - 1;

						for(int i = 0; i < 3; i++){		
							finalVerts.push_back(verts[index][i] * 1000000);
						}
					}//end of else
				}//end of forloop
				faceCount++;
			}
			else {
				continue;
			}
		}

		//output vertex array
		std::cout << "Output to " << argv[1] << "...";
		vertFile << "<?xml version=\"1.0\" encoding=\"utf-8\"?>" << std::endl;
		vertFile << "<resources>" << std::endl;
		vertFile << "<integer-array name=\"" << vertArrayName << "\">" << std::endl;
		for(int i = 0; i < finalVerts.size(); i++){
			vertFile << "<item> " << finalVerts[i] << "</item>\t";
			if((i+1)%3 == 0) vertFile << std::endl;
		}
		vertFile << "</integer-array>" << std::endl;
		vertFile << "</resources>" << std::endl;
		std::cout << "Done." << std::endl;
		
		//output normal array
		std::cout << "\r\nOutput to " << argv[2] << "...";
		normFile << "<?xml version=\"1.0\" encoding=\"utf-8\"?>" << std::endl;
		normFile << "<resources>" << std::endl;
		if(finalNormals.size() > 0){
			normFile << "<integer-array name=\"" << normArrayName << "\">" << std::endl;
			for(int i = 0; i < finalNormals.size(); i++){
				normFile << "<item> " << finalNormals[i] << "</item>\t";
				if((i+1)%3 == 0) normFile << std::endl;
			}
			normFile << "</integer-array>" << std::endl;
		}
		normFile << "</resources>" << std::endl;
		std::cout << "Done." << std::endl;

		//output texture coordinate array
		std::cout << "\r\nOutput to " << argv[3] << "...";
		texFile << "<?xml version=\"1.0\" encoding=\"utf-8\"?>" << std::endl;
		texFile << "<resources>" << std::endl;
		if(finalTex.size() > 0){
			texFile << "<integer-array name=\"" << texArrayName << "\">" << std::endl;
			for(int i = 0; i < finalTex.size(); i++){
				texFile << "<item> " << finalTex[i] << "</item>\t";
				if((i+1)%2 == 0) texFile << std::endl;
			}
			texFile << "</integer-array>" << std::endl;
		} 
		texFile << "</resources>" << std::endl;
		std::cout << "Done.\r\n" << std::endl;
		
		
		objFile.close();
		vertFile.close();
		texFile.close();
		normFile.close();
		system("pause");
	}

}