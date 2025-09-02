#include "PWConverter.hpp"


PWConverter::PWConverter() : color(false), loaded_percent(0)
{
	pI.x = 0.0f;
	pI.y = 0.0f;
	pI.z = 0.0f;
	pI.intensity = 1.0f;

	pC.x = 0.0f;
	pC.y = 0.0f;
	pC.z = 0.0f;
	pC.red = 0u;
	pC.green = 0u;
	pC.blue = 0u;
}


PWConverter::~PWConverter()
{
}


void PWConverter::convert_txt2pw(std::ifstream & in, std::wstring &path2, const std::vector<PWConverter::Element_Id> &element_ids){
	
	std::cout << "will convert txt" << std::endl;
	std::ofstream out(path2, std::ios::binary | std::ios::trunc);
	if (!out.good()){
		std::cout << "can't  open out file " << std::endl;
		return;
	}
	{
	/* calc the file size and counts of points in it. 
		the size of one point that will be calculate isn't the exact one. 
		f.e. an float value will be interpret with 4 bytes -> 4 char in the txt file. 
		That mean the point size in the reality will be bigger. And that mean, the loading screen wouldn't go to 100% maybe.
		It will break earlier, cause the convertion is finished with less points, as that approximately calculated. 
	*/
		long long size_file = 0;
		get_byte_size(in, size_file);
		calc_percent_count(size_file, element_ids);
		// we only need size_file as a temporary memory in this scope, now it will be deleted.
	}

	std::cout << "will write .pw file" << std::endl;
	// read first line
	char sep1 = check_seperator(in, ',') ? ',' : ' ';
	char sep2 = check_seperator(in, ';') ? ';' : ' ';
	if (sep1 != ' ' && sep2 != ' '){
		std::cout << " inconsistent data line. There are , and ; !" << std::endl;
		in.close();
		out.close();
		return;
	}
	if (sep1 == ' ' && sep2 == ' ')
		convert_txt(in, out, element_ids);
	else{
		if (sep1 != ' ')
			convert_txt(in, out, element_ids, sep1);
		else
			convert_txt(in, out, element_ids, sep2);
	}
}


bool PWConverter::check_seperator(std::ifstream& in, const char seperator){

	// check for seperator
	char line_c[256];
	in.getline(line_c, 256);
	std::string line_s(line_c);
	if (line_s.find_first_of(seperator) != line_s.npos){
		std::cout << "there are " << seperator << " in the line!!" << std::endl;
		in.seekg(0, in.beg);
		return true;
	}
	in.seekg(0, in.beg);
	return false;
}


void PWConverter::convert_txt(std::ifstream& in, std::ofstream& out, const std::vector<PWConverter::Element_Id> &element_ids, const char seperator){

	unsigned int r = 0;
	char line[256];
	std::string line_s;

	if (color)out.put(1);
	else out.put(0);

	std::cout << "start reading.\n";
	
	while (!in.eof() && !in.bad()){
		in.getline(line, 256);
		line_s = std::string(line);
		if (line_s.empty()) break;
		for (unsigned int e = 0u; e < element_ids.size(); ++e){
			std::string::size_type element_pos = line_s.find_first_of(seperator);
			if (element_pos == line_s.npos){
				parse_point(line_s ,element_ids[e]);
				continue;
			}
			std::string element_s = line_s.substr(0, element_pos);
			parse_point(element_s,element_ids[e]);
			line_s = line_s.substr(element_pos + 1, line_s.size());
		}
		write_point(out);
		++r;
		// "loading"
		if (five_percent_point_count > 0){
			if (r % five_percent_point_count == 0){
				loaded_percent += 5;
				std::cout << loaded_percent << " %\r" << std::flush;
			}
		}
	}
	in.close();
	out.close();
	std::cout << "\n convert " << r << " points" << std::endl;

}

void PWConverter::convert_txt(std::ifstream& in, std::ofstream& out, const std::vector<PWConverter::Element_Id> &element_ids){
	
	unsigned int r = 0;
	unsigned int c = 0;
	if (color)out.put(1);
	else out.put(0);

	std::cout << "start reading.\n";
	// in file
	while (!in.eof()){
		parse_point(in, element_ids[c]);
		// write to out if column is complete
		if (c == element_ids.size() - 1u){
			write_point(out);
			c = 0u;
			++r;
			// "loading"
			if (five_percent_point_count > 0){
				if (r % five_percent_point_count == 0){
					loaded_percent += 5;
					std::cout << loaded_percent << " %\r" << std::flush;
				}
			}
		}
		else ++c;
	}
	in.close();
	out.close();
	std::cout << "\n convert " << r << " points" << std::endl;

}


void PWConverter::get_byte_size(std::ifstream &in, long long &size){

	std::streampos begin = in.tellg();
	in.seekg(0, in.end);
	std::streampos end = in.tellg();
	in.seekg(0, in.beg);
	size = end - begin;
}

void PWConverter::calc_percent_count(const long long &byte_size, const std::vector<Element_Id> &element_ids){

	int point_byte = 0;

	for (auto e : element_ids){
		switch (e){
		case(Element_Id::x) :
			point_byte += sizeof(double);
			break;
		case(Element_Id::y) :
			point_byte += sizeof(double);
			break;
		case(Element_Id::z) :
			point_byte += sizeof(double);
			break;
		case(Element_Id::ignore_int) :
			point_byte += sizeof(int);
			break;
		case(Element_Id::ignore_float) :
			point_byte += sizeof(float);
			break;
		case(Element_Id::intensity) :
			point_byte += sizeof(float);
			break;
		case(Element_Id::ignore_Char) :
			point_byte += sizeof(char);
			break;
		case(Element_Id::red) :
			point_byte += sizeof(char);
			break;
		case(Element_Id::green) :
			point_byte += sizeof(char);
			break;
		case(Element_Id::blue) :
			point_byte += sizeof(char);
			break;
		}
	}

	if (point_byte == 0 || byte_size == 0){
		five_percent_point_count = 0;
		return;
	}

	// an dieser Stelle wird approximiert, da flot (*0.05f) nur eine 6 Stellen Genauigkeit hat. d.h. ab 10 mio pkte werden die hinteren Stellen zu 0 gesetzt.
	// statt 61 598 454 ergibt sich 61 598 450  -> Dieser Genauigkeitsverlust wird ignoriert. (Es muss nicht haargenau gerechnet werden) 

	five_percent_point_count = static_cast<long long>(std::ceil((byte_size / point_byte)*0.05));

}

void PWConverter::convert_pw(std::ifstream &in, std::wstring &path2){
	std::cout << "will convert pw" << std::endl;
	color = in.get() != 0;
	std::vector<Element_Id> ids;
	ids.push_back(Element_Id::x);
	ids.push_back(Element_Id::y);
	ids.push_back(Element_Id::z);
	if (color){
		ids.push_back(Element_Id::red);
		ids.push_back(Element_Id::green);
		ids.push_back(Element_Id::blue);
	}
	else ids.push_back(Element_Id::intensity);
	{
		long long size_file = 0;
		get_byte_size(in, size_file);
		calc_percent_count(size_file, ids);
	}
	// throw the first char away
	in.get();
	convert_bin2txt(in, path2, ids,false);
}

void PWConverter::convert_bin2txt(std::ifstream & in, std::wstring &path2, const std::vector<PWConverter::Element_Id> &element_ids, bool calc_file_size){
	std::cout << "will convert bin" << std::endl;
	if (calc_file_size)
	{
		long long size_file = 0;
		get_byte_size(in, size_file);
		calc_percent_count(size_file, element_ids);
	}
	

	double x, y, z;
	float intensity;
	float float_val;
	int int_val;
	unsigned int r = 0;
	unsigned int c = 0;
	x = y = z = float_val = 0.0f;  
	intensity = 1.0f;
	int_val = 0;

	std::ofstream out(path2, std::ios::trunc);
	if (!out.good()){
		std::cout << "can't  open out file " << std::endl;
		return;
	}

	std::cout << "will write .txt file" << std::endl;
	std::cout << "start reading.\n";

	double recv = 0.0;
	while (!in.eof()){

		switch (element_ids[c]){
		case(Element_Id::ignore_int) :
			if (in.read(reinterpret_cast<char*>(&int_val), sizeof(int_val)))
				out << int_val << "\t";
			break;
		case(Element_Id::x) :
			if (in.read(reinterpret_cast<char*>(&x), sizeof(x))) {
				double recv = *reinterpret_cast<double*>(&x);
				out << std::fixed << x << "\t";
				//std::cout << "write x: " << recv << std::endl;
			}
			break;
		case(Element_Id::y) :
			if (in.read(reinterpret_cast<char*>(&y), sizeof(y))) {
				out << std::fixed << y << "\t";
				//std::cout << "write y: " << y << std::endl;
			}
			break;
		case(Element_Id::z) :
			if (in.read(reinterpret_cast<char*>(&z), sizeof(z))) {
				out << z << "\t";
				//std::cout << "write x: " << z << std::endl;
			}
			break;
		case(Element_Id::ignore_float) :
			if (in.read(reinterpret_cast<char*>(&float_val), sizeof(float_val)))
				out << float_val << "\t";
			break;
		case(Element_Id::intensity) :
			if (in.read(reinterpret_cast<char*>(&intensity), sizeof(intensity)))
				out << intensity << "\t";
			break;
		case(Element_Id::red) :
			int_val = in.get();
			if (!in.eof())
				out << int_val << "\t";
			break;
		case(Element_Id::green) :
			int_val = in.get();
			if (!in.eof())
				out << int_val << "\t";
			break;
		case(Element_Id::blue) :
			int_val = in.get();
			if (!in.eof())
				out << int_val << "\t";
			break;
		case(Element_Id::ignore_Char) :
			int val = in.get();
			if (!in.eof())
				out << int_val << "\t";
			break;
		}

		if (c == element_ids.size() - 1u){
			// column complete, write new line
			out << "\n";
			c = 0u;
			++r;
			// "loading"
			if (five_percent_point_count > 0){
				if (r % five_percent_point_count == 0){
					loaded_percent += 5;
					std::cout << loaded_percent << " %\r" << std::flush;
				}
			}
		}
		else ++c;
	}
	in.close();
	out.close();
	std::cout << "\n convert " << r << " points" << std::endl;
}

void PWConverter::convert_ply2pw(std::ifstream & in, std::wstring &path2){

	std::cout << "will convert ply" << std::endl;
	std::ofstream out(path2,std::ios::binary | std::ios::trunc);
	if (!out.good()){
		std::cout << "can't  open out file " << std::endl;
		return;
	}

	std::string line_s;
	read_ply_line(in, line_s);

	if (line_s != "ply"){
		std::cout << "broken ply-file. Missing ply at the top of header!" << std::endl;
		in.close();
		out.close();
		return;
	}

	ply_point ply_p;
	ply_p.error = false;
	ply_p.parse_elements = false;
	ply_p.count = 0;

	do{
		read_ply_line(in, line_s);
		parse_ply_line(line_s, ply_p);
		if (ply_p.error || in.eof())break;
	} while (line_s != "end_header");
	if (ply_p.error){
		std::cout << "can't read file, cause of error" << std::endl;
		in.close();
		out.close();
		return;
	}
	convert_ply(in,out, ply_p);
}


void PWConverter::read_ply_line(std::ifstream &in, std::string &line_s){

	char line[256];
	in.getline(line, 256);
	line_s = std::string(line);
	std::string::size_type pos = line_s.find_first_of("\r");
	if (pos == line_s.npos) pos = line_s.find_first_of("\n");
	if(pos != line_s.npos) line_s = line_s.substr(0, pos);
}


void PWConverter::parse_ply_line(std::string &line_s, ply_point &ply_point){

	// cut line_s at " " 
	std::string::size_type pos = line_s.find_first_of(" ");
	if (pos != line_s.npos){
		std::string first_part = line_s.substr(0, pos);
		std::string last_part = line_s.substr(pos + 1, line_s.size());

		if (first_part == "comment") return;
		if (first_part == "format"){
			std::string format_s = last_part.substr(0, last_part.find_first_of(" "));
			if (format_s == "ascii"){
				ply_point.ascii = true;
				return;
			}
			if (format_s == "binary_little_endian" || format_s == "binary_big_endian"){
				ply_point.ascii = false;
				return;
			}
			std::cout << "error: can't read format" << std::endl;
			ply_point.error = true;
			return;
		}
		if (first_part == "element"){
			std::string::size_type pos_count = last_part.find_first_of(" ");
			std::string element_s = last_part.substr(0, pos_count);
			if (element_s != "vertex"){
				ply_point.parse_elements = false;
				return;
			}
			ply_point.parse_elements = true;
			ply_point.count = std::stoll(last_part.substr(pos_count + 1, last_part.size()));
			return;
		}
		if (first_part == "property" && ply_point.parse_elements){
			std::string::size_type pos_name = last_part.find_last_of(" ");
			std::string property_type = last_part.substr(0, pos_name);
			std::string property_name = last_part.substr(pos_name +1,last_part.size());
			if (property_name == "x" || property_name == "X"){
				if (property_type == "float" && !ply_point.ascii) ply_point.element_ids.push_back(Element_Id::x_float);
				else ply_point.element_ids.push_back(Element_Id::x);
				return;
			}
			if (property_name == "y" || property_name == "Y"){
				if (property_type == "float" && !ply_point.ascii) ply_point.element_ids.push_back(Element_Id::y_float);
				else ply_point.element_ids.push_back(Element_Id::y);
				return;
			}
			if (property_name == "z" || property_name == "Z"){
				if (property_type == "float" && !ply_point.ascii) ply_point.element_ids.push_back(Element_Id::z_float);
				else ply_point.element_ids.push_back(Element_Id::z);
				return;
			}

			if (property_type == "float"){
				// this will write the last float with another name as x y or z to intensity 
				// This is neccessary, cause other programms can name intensity in another way (f.e. cloudcomaper : "scalar_Scalar_field") 
				// just don't write other float propertys to the vertizes. Or vonvert from a txt file. 
				ply_point.element_ids.push_back(Element_Id::intensity);
				return;
			}
			if (property_type == "char" || property_type == "uchar"){
				// look for red, green , blue channel. (indicatet with its first letter
				if (property_name[0] == 'r' || property_name[0] == 'R'){
					ply_point.element_ids.push_back(Element_Id::red);
					color = true;
					return;
				}
				if (property_name[0] == 'b' || property_name[0] == 'B'){
					ply_point.element_ids.push_back(Element_Id::blue);
					color = true;
					return;
				}
				if (property_name[0] == 'g' || property_name[0] == 'G'){
					ply_point.element_ids.push_back(Element_Id::green);
					color = true;
					return;
				}
				ply_point.element_ids.push_back(Element_Id::ignore_Char);
				return;
			}
			if (property_type == "int" || property_type == "uint"){
				ply_point.element_ids.push_back(Element_Id::ignore_int);
				return;
			}
			// TODO short and double

		}

	}
}


void PWConverter::convert_ply(std::ifstream &in, std::ofstream& out, ply_point &ply_point){

	long long r = 0;
	unsigned int c = 0u;
	if (color)out.put(1);
	else out.put(0);
	five_percent_point_count = static_cast<long long>(std::ceil(ply_point.count *0.05));
	std::cout << "start reading.\n";
	// in file
	while (!in.eof() && r < ply_point.count){
		if (ply_point.ascii) 
			parse_point(in, ply_point.element_ids[c]);
		else parse_point_bin(in, ply_point.element_ids[c]);
		// write to out if column is complete
		if (c == ply_point.element_ids.size() - 1u){
			write_point(out);
			c = 0u;
			++r;
			// "loading"
			if (five_percent_point_count > 0){
				if (r % five_percent_point_count == 0){
					loaded_percent += 5;
					std::cout << loaded_percent << " %\r" << std::flush;
				}
			}
			
		}
		else ++c;
	}
	in.close();
	out.close();
	std::cout << "\n convert " << r << " points" << std::endl;
}

//////////////////////////////////7
// parse point with in stream

void PWConverter::parse_point(std::ifstream &in,PWConverter::Element_Id element_id/*, std::string element*/){
	if (color)
		parse_pC(in, element_id);
	else parse_pI(in, element_id);
	
}

void PWConverter::parse_pI(std::ifstream &in, PWConverter::Element_Id element_id){

	float float_val = 0.0f;
	int int_val = 0;

	switch (element_id){
	case(Element_Id::ignore_int) :
		in >> int_val;
		break;
	case(Element_Id::x) :
		in >> pI.x;
		break;
	case(Element_Id::y) :
		in >> pI.y;
		break;
	case(Element_Id::z) :
		in >> pI.z;
		break;
	case(Element_Id::ignore_float) :
		in >> float_val;
		break;
	case(Element_Id::intensity) :
		in >> pI.intensity;
		break;
	case(Element_Id::ignore_Char) :
		in >> int_val;
		break;
	}
}

void PWConverter::parse_pC(std::ifstream &in, PWConverter::Element_Id element_id){

	float float_val = 0.0f;
	int int_val = 0;

		switch (element_id){
		case(Element_Id::ignore_int) :
			in >> int_val;
			break;
		case(Element_Id::x) :
			in >> pC.x;
			break;
		case(Element_Id::y) :
			in >> pC.y;
			break;
		case(Element_Id::z) :
			in >> pC.z;
			break;
		case(Element_Id::ignore_float) :
			in >> float_val;
			break;
		case(Element_Id::intensity) :
			in >> float_val;
			break;
		case(Element_Id::red) :
			in >> int_val;
			pC.red = static_cast<unsigned char>(int_val);
			break;
		case(Element_Id::green) :
			in >> int_val;
			pC.green = static_cast<unsigned char>(int_val);
			break;
		case(Element_Id::blue) :
			in >> int_val;
			pC.blue = static_cast<unsigned char>(int_val);
			break;
		case(Element_Id::ignore_Char) :
			in >> int_val;
			break;
		}

}


///////////////////////////////
// parse string elemet to point

void PWConverter::parse_point(std::string &element, PWConverter::Element_Id element_id){
	if (color)
		parse_pC(element, element_id);
	else parse_pI(element, element_id);
}

void PWConverter::parse_pI(std::string &element,PWConverter::Element_Id element_id){

	float float_val = 0.0f;
	int int_val = 0;

	switch (element_id){
	case(Element_Id::ignore_int) :
		int_val = std::stoi(element);
		break;
	case(Element_Id::x) :
		pI.x = std::stod(element);
		break;
	case(Element_Id::y) :
		pI.y = std::stod(element);
		break;
	case(Element_Id::z) :
		pI.z = std::stod(element);
		break;
	case(Element_Id::ignore_float) :
		float_val = std::stof(element);
		break;
	case(Element_Id::intensity) :
		pI.intensity = std::stof(element);
		break;
	case(Element_Id::ignore_Char) :
		int_val = std::stoi(element);
		break;
	}

}


void PWConverter::parse_pC(std::string &element, PWConverter::Element_Id element_id){

	float float_val = 0.0f;
	int int_val = 0;

	switch (element_id){
	case(Element_Id::ignore_int) :
		int_val = std::stoi(element);
		break;
	case(Element_Id::x) :
		pC.x = std::stod(element);
		break;
	case(Element_Id::y) :
		pC.y = std::stod(element);
		break;
	case(Element_Id::z) :
		pC.z = std::stod(element);
		break;
	case(Element_Id::ignore_float) :
		float_val = std::stof(element);
		break;
	case(Element_Id::intensity) :
		float_val = std::stof(element);
		break;
	case(Element_Id::red) :
		pC.red = static_cast<unsigned char>(std::stoi(element));
		break;
	case(Element_Id::green) :
		pC.green = static_cast<unsigned char>(std::stoi(element));
		break;
	case(Element_Id::blue) :
		pC.blue = static_cast<unsigned char>(std::stoi(element));
		break;
	case(Element_Id::ignore_Char) :
		int_val = std::stoi(element);
		break;
	}

}




void PWConverter::parse_point_bin(std::ifstream& in, Element_Id element_id){
	float float_val = 0.0f;
	int int_val = 0;

	if (color){
		switch (element_id){
		case(Element_Id::ignore_int) :
			in.read(reinterpret_cast<char*>(&int_val), sizeof(int_val));
			break;
		case(Element_Id::x) :
			in.read(reinterpret_cast<char*>(&pC.x), sizeof(pC.x));
			break;
		case(Element_Id::y) :
			in.read(reinterpret_cast<char*>(&pC.y), sizeof(pC.y));
			break;
		case(Element_Id::z) :
			in.read(reinterpret_cast<char*>(&pC.z), sizeof(pC.z));
			break;
		case(Element_Id::ignore_float) :
			in.read(reinterpret_cast<char*>(&float_val), sizeof(float_val));
			break;
		case(Element_Id::intensity) :
			in.read(reinterpret_cast<char*>(&float_val), sizeof(float_val));
			break;
		case(Element_Id::red) :
			pC.red =static_cast<unsigned char>( in.get());
			break;
		case(Element_Id::green) :
			pC.green = static_cast<unsigned char>(in.get());
			break;
		case(Element_Id::blue) :
			pC.blue = static_cast<unsigned char>(in.get());
			break;
		case(Element_Id::ignore_Char) :
			int_val = static_cast<unsigned char>(in.get());
			break;
		case(Element_Id::x_float) :
			// coordinates are saved as float: read as float and cast to double
			in.read(reinterpret_cast<char*>(&float_val), sizeof(float_val));
			pC.x = static_cast<double>(float_val);
			break;
		case(Element_Id::y_float) :
			// coordinates are saved as float: read as float and cast to double
			in.read(reinterpret_cast<char*>(&float_val), sizeof(float_val));
			pC.y = static_cast<double>(float_val);
			break;
		case(Element_Id::z_float) :
			// coordinates are saved as float: read as float and cast to double
			in.read(reinterpret_cast<char*>(&float_val), sizeof(float_val));
			pC.z = static_cast<double>(float_val);
			break;
		}
	}
	else{
		switch (element_id){
		case(Element_Id::ignore_int) :
			in.read(reinterpret_cast<char*>(&int_val), sizeof(int_val));
			break;
		case(Element_Id::x) :
			in.read(reinterpret_cast<char*>(&pI.x), sizeof(pI.x));
			break;
		case(Element_Id::y) :
			in.read(reinterpret_cast<char*>(&pI.y), sizeof(pI.y));
			break;
		case(Element_Id::z) :
			in.read(reinterpret_cast<char*>(&pI.z), sizeof(pI.z));
			break;
		case(Element_Id::ignore_float) :
			in.read(reinterpret_cast<char*>(&float_val), sizeof(float_val));
			break;
		case(Element_Id::intensity) :
			in.read(reinterpret_cast<char*>(&pI.intensity), sizeof(pI.intensity));
			break;
		case(Element_Id::ignore_Char) :
			int_val = static_cast<unsigned char>(in.get());
			break;
		case(Element_Id::x_float) :
			// coordinates are saved as float: read as float and cast to double
			in.read(reinterpret_cast<char*>(&float_val), sizeof(float_val));
			pI.x = static_cast<double>(float_val);
			break;
		case(Element_Id::y_float) :
			// coordinates are saved as float: read as float and cast to double
			in.read(reinterpret_cast<char*>(&float_val), sizeof(float_val));
			pI.y = static_cast<double>(float_val);
			break;
		case(Element_Id::z_float) :
			// coordinates are saved as float: read as float and cast to double
			in.read(reinterpret_cast<char*>(&float_val), sizeof(float_val));
			pI.z = static_cast<double>(float_val);
			break;
		}
	}
}


void PWConverter::write_point(std::ofstream &out){
	if (!color){
		out.write(reinterpret_cast<char*>(&pI.x), sizeof(pI.x));
		out.write(reinterpret_cast<char*>(&pI.y), sizeof(pI.y));
		out.write(reinterpret_cast<char*>(&pI.z), sizeof(pI.z));
		out.write(reinterpret_cast<char*>(&pI.intensity), sizeof(pI.intensity));
	}
	else{
		out.write(reinterpret_cast<char*>(&pC.x), sizeof(pC.x));
		out.write(reinterpret_cast<char*>(&pC.y), sizeof(pC.y));
		out.write(reinterpret_cast<char*>(&pC.z), sizeof(pC.z));
		out.put(pC.red);
		out.put(pC.green);
		out.put(pC.blue);
	}
}
