#pragma once
#include<iostream>
#include<fstream>
#include<string>
#include<vector>

class PWConverter
{
public:

	struct point{
		double x, y, z;
	};

	struct pointI: public point{
		float intensity;
	};

	struct pointC: public point{
		unsigned char red, green, blue;
	};

	enum class Element_Id :int{
		ignore_int=1,
		x,
		y,
		z,
		ignore_float,
		intensity,
		red,
		green,
		blue,
		ignore_Char,
		x_float,
		y_float,
		z_float
	};
	
	/*This struct construct the input information we need for pw-files from a ply header*/
	struct ply_point{
		// element ids for each point (same as input parameters)
		std::vector<Element_Id> element_ids;
		// ply format ascii or binary
		bool ascii;
		// indicate, that an error has occured while parsing the ply header
		bool error;
		// indicate, that the properity lines will generate element_ids
		bool parse_elements;
		// count of points
		long long count;
	};

	PWConverter();
	~PWConverter();

	/*convert from txt-files to pw files. 
	the coloumns and parameters of each point must be completly described in element_ids.
	in must be an open filestream. 
	The elements of one point can be seperate by a free space (tabulator or gab) or with an , or ; */
	void convert_txt2pw(std::ifstream &in, std::wstring &path2, const std::vector<Element_Id> &element_ids);

	/*convert from binary-files to txt-files.
	the coloumns and parameters of each point must be completly described in element_ids.
	in must be an open filestream. 
	This methods simply reads the bytes in in and write the value as string in the output stream*/
	void convert_bin2txt(std::ifstream & in, std::wstring &path2, const std::vector<Element_Id> &element_ids, bool calc_file_size =true);

	/*convert from ply-files to pw files.
	Only vertex elements of the ply file will be written as points to the pw file. 
	Because ply files can name theire vertex propertys in very different ways, the Converter must try its best, to indicate which property is need. 
	To get the right result you should keep this in mind:
		- x,y,z and the upper case ones indicate the coordinates of one Point. And the Coordinates will be casted to float values
		- The fourth dimension of an pw point can be an float intensity value or a color, represented by 3 unsigned char channels.
		-If there is a color channel found, the intensity wouldn't be written, because the color gets a bigger weight. 
			A Color channel is represented by a char or uchar property which should be named red, green, blue or something similar.
			To indicate that channel the converter look for the first letter in the property name so r or R will indicate the red channel.
			All color channels will be cast to unsigned char.
		-If there isn't one color channel the last float property, that isn't called x,y or z will be written as the intensity.
	*/
	void convert_ply2pw(std::ifstream & in, std::wstring &path2);

	/*This write a pw-file as txt file.
	So t's the same as convert_bin2txt with the element_ids written to the pw file.
	This is x y z intensity (if there is no color)
	or x y z R G B */
	void convert_pw(std::ifstream &in, std::wstring &path2);

	/*This indicates, if the points should have a color or will be write with intensity.*/
	void set_color(bool c){ color = c; }

private:

	/*This search for seperators in the first line of the in file.  */
	bool check_seperator(std::ifstream& in, const char seperator);

	/*Converts the txt in-file and writes the out file. 
	The elements of each point(one line) must be seperate with the given seperator*/
	void convert_txt(std::ifstream& in, std::ofstream& out, const std::vector<Element_Id> &element_ids, const char seperator);

	/*Converts the txt in-file and writes the out file.
	The elements of each point(one line) must be seperate with a gab or tabulator*/
	void convert_txt(std::ifstream& in, std::ofstream& out, const std::vector<Element_Id> &element_ids);

	/*Calc the bytesize and save it in size*/
	void get_byte_size(std::ifstream &in, long long &size);

	/*This calcs the five percent of the point count. 
	  For that we ned the size of the instream in byte and calc the size of one point in byte with the given element_ids
	*/
	void calc_percent_count( const long long &byte_size, const std::vector<Element_Id> &element_ids);

	/*Write the next value in in to the correct value of an intesity point or color point.
	The value is represented with the element_id.*/
	void parse_point(std::ifstream &in, Element_Id element_id);
	void parse_pI(std::ifstream& in, Element_Id element_id);
	void parse_pC(std::ifstream& in, Element_Id element_id);

	/*Write the value of element to the correct value of an intesity point or color point.
	The value is represented with the element_id.*/
	void parse_point(std::string &element, Element_Id element_id);
	void parse_pI(std::string &element, Element_Id element_id);
	void parse_pC(std::string &element, Element_Id element_id);

	/*Reads the next bytes in in and write the value to the correct point value.*/
	void parse_point_bin(std::ifstream& in, Element_Id element_id);

	/*Write current point to out. 
	The point's value should be set with parse_point() or parse_point_bin before!*/
	void write_point(std::ofstream &out);

	/*Gets the next line in a ply file header and write it to line_s*/
	void read_ply_line(std::ifstream &in, std::string &line_s);

	/*Looks for information for the point properties in a ply file header line and writes the information we need to ply_point.*/
	void parse_ply_line(std::string &line_s, ply_point &ply_point);

	/*Read the ply file lines (in) and writes the point informations to a pw file (out). 
	The point Informations must be stored in ply_point. 
	You should read and parse (read_ply_line() and parse_ply_line()) the header first to fill ply_point correctly.*/
	void convert_ply(std::ifstream &in, std::ofstream& out, ply_point &ply_point);

	// an Intensity point, that can be written to a pw file
	pointI pI;
	// an color point, that can be written to a pw file
	pointC pC;

	//indicates if the have a color channel or not.
	bool color;

	// 5% of the point count to convert ( need for show loading)
	long long five_percent_point_count;
	// summ of all percents 
	int loaded_percent;

};

