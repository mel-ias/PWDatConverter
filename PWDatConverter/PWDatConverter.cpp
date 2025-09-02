// PWDatConverter.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include"PWConverter.hpp"



int _tmain(int argc, _TCHAR* argv[])
{
	
	// input paras

	if (argc < 2){
		std::cout << " keine Paras eingegeben " << std::endl;
		return 0;
	}

	std::wstring path = L"test.txt";
	std::vector<PWConverter::Element_Id> element_ids;
	//bool color = false;
	
	bool bin = false;
	bool col = false;
	//bool ply_file = false;

	for (int i = 1; i < argc; ++i){

		if (argv[i][0] == L'b'){
			bin = true;
			continue;
		}

		if (argv[i][0] == L'I'){
			// input path
			path = std::wstring(argv[i + 1]);
			++i;
			continue;
		}

		std::wstring arg(argv[i]);

		for (unsigned int i = 0u; i < arg.size(); ++i){
			switch (arg[i]){
			case(L'i'):
				// int value
				element_ids.push_back(PWConverter::Element_Id::ignore_int);
				break;
			case(L'x') :
				// x coloumn
				element_ids.push_back(PWConverter::Element_Id::x);
				break;
			case(L'y') :
				// y coloumn
				element_ids.push_back(PWConverter::Element_Id::y);
				break;
			case(L'z') :
				// z coloumn
				element_ids.push_back(PWConverter::Element_Id::z);
				break;
			case(L'f') :
				// float value
				element_ids.push_back(PWConverter::Element_Id::ignore_float);
				break;
			case(L'n') :
				// intensity
				element_ids.push_back(PWConverter::Element_Id::intensity);
				break;
				
			case(L'R') :
				// red channel
				element_ids.push_back(PWConverter::Element_Id::red);
				col = true;
				break;
			case(L'G') :
				// green channel
				element_ids.push_back(PWConverter::Element_Id::green);
				col = true;
				break;
			case(L'B') :
				// blue channel
				element_ids.push_back(PWConverter::Element_Id::blue);
				col = true;
				break;
				
			case(L'C') :
				// alhpa channel (ignored)
				element_ids.push_back(PWConverter::Element_Id::ignore_Char);
				break;
				
			}
		}
	}

	// cut " from path if exists
	if (path[0] == L'\"'){
		std::wstring::size_type l = path.find_last_of(L'\"');
		path = path.substr(1, l);
	}

	std::ifstream in(path,std::ios::binary);
	if (!in.good()){
		std::cout << "can't  open in file " << std::endl;
		return 0;
	}

	PWConverter converter;
	
	std::wstring::size_type last = path.find_last_of(L".");
	std::wstring path2 = path.substr(0, last);
	std::wstring file_type = path.substr(last + 1, path.size());

	// convert ply file
	if (file_type == L"ply"){
		path2.append(L".pw");
		converter.convert_ply2pw(in, path2);
		return 0;
	}
	
	// convert pw to txt
	if (file_type == L"pw"){
		path2.append(L".txt");
		converter.convert_pw(in, path2);
		return 0;
	}

	//ply_file = path.substr(last + 1, path.size()) == L"ply";

	if (element_ids.empty()){
		std::cout << "please enter element ids " << std::endl;
		return 0;
	}

	if(!bin) path2.append(L".pw");
	else path2.append(L".txt");

	converter.set_color(col);

	if (!bin){
		// convert from txt to pw
		converter.convert_txt2pw(in, path2, element_ids);
		return 0;
	}

	// else convert .pw to .txt
	converter.convert_bin2txt(in, path2, element_ids);

	return 0;
}

