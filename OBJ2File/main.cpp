/*****************************************************************
*   OBJ2File   By Jin Kim     updated September 11, 2013
*
*   Converts any OBJ file into a suitable XML resource for android
*
*   Argument: takes one input cfg file with the following 4 entries: 
*   - OBJ file to parse
*   output XML files for:
*   - vertex array
*   - normal array 
*   - texture array
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
	std::ifstream configFile, objFile;
	std::ofstream vertFile, texFile, normFile;
	std::vector<std::vector<float> > verts;
	std::vector<std::vector<float> > tex;
	std::vector<std::vector<float> > normals;
	std::vector<int> finalVerts;
	std::vector<int> finalTex;
	std::vector<int> finalNormals;
	std::vector<std::string> configv;

	char cBuffer[256], buffer[256];
	char *charBuffer;

	int vertCount = 0, normCount = 0, texCount = 0, faceCount = 0;
	int size = 0;

	//if no arguments on program execution
	if(argc < 2) {
		std::cout << "Requires input config file" << std::endl;
		system("pause");
	}
	else if(argc > 2) {
		std::cout << "Too many arguments" << std::endl;
		system("pause");
	}
	else {
		configFile.open(argv[1]);

		if(configFile.bad() || configFile.fail()){
			std::cout << "file opening error! Better check that the filename is correct" << std::endl;
			system("pause");
		}

		configv.resize(4);

		while(configFile.good()){
			int c;
			configFile.getline(cBuffer, 256);
			std::string strBuffer = std::string(cBuffer);

			if(strBuffer.substr(0, 8) == "OBJFile=") {
				configv[0] = strBuffer.substr(8);
				size = (configv[0].size() > 0) ? (size+1) : size;
				continue;
			}

			else if(strBuffer.substr(0, 10) == "vertArray=") {
				configv[1] = strBuffer.substr(10);
				size = (configv[1].size() > 0) ? (size+1) : size;
				continue;
			}

			else if(strBuffer.substr(0, 10) == "normArray=") {
				configv[2] = strBuffer.substr(10);
				size = (configv[2].size() > 0) ? (size+1) : size;
				continue;
			}

			else if(strBuffer.substr(0, 9) == "texArray=") {
				configv[3] = strBuffer.substr(9);
				size = (configv[3].size() > 0) ? (size+1) : size;
				continue;
			}
			
		}
	
	}

	if(size < 4){
		std::cout << "not enough arguments in config file!\r\n\r\nRequires 4 arguments:" << std::endl;

		std::cout << "- OBJ file...";
		if(configv[0].size() > 0)
			std::cout << "OK" << std::endl;
		else
			std::cout<< "MISSING" << std::endl;

		std::cout << "- vertex array filename...";
		if(configv[1].size() > 0)
			std::cout << "OK" << std::endl;
		else
			std::cout<< "MISSING" << std::endl;

		std::cout << "- normal array filename...";
		if(configv[2].size() > 0)
			std::cout << "OK" << std::endl;
		else
			std::cout << "MISSING" << std::endl;

		std::cout << "- texture array filename...";
		if(configv[3].size() > 0)
			std::cout << "OK" << std::endl;
		else
			std::cout << "MISSING\r\n" << std::endl;

		system("pause");
	}
	else{
		
		std::string vertArrayName = configv[1];
		std::string normArrayName = configv[2];
		std::string texArrayName = configv[3];

		objFile.open(configv[0]);

		if(objFile.bad() || objFile.fail()){
			std::cout << "file opening error! Better check that the filename is correct" << std::endl;
			system("pause");
			//exit(-1);
		}

		vertFile.open(vertArrayName, std::ofstream::out);
		texFile.open(texArrayName, std::ofstream::out);
		normFile.open(normArrayName, std::ofstream::out);

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
		std::cout << "Output to " << configv[1] << "...";
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
		std::cout << "\r\nOutput to " << configv[2] << "...";
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
		std::cout << "\r\nOutput to " << configv[3] << "...";
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