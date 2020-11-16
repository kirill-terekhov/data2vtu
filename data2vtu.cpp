#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <deque>
#include <vector>



void copy_until_string(std::ifstream & fin, std::ofstream & fout, std::string match)
{
	char c;
	std::deque<char> buf(match.size(),'\0');
	while( std::string(buf.begin(),buf.end()) != match && fin.get(c)) 
	{
		fout.put(c);
		buf.push_back(c);
		buf.pop_front();
	}
}

std::vector<std::string> explode(std::string const & s, char delim)
{
	std::string token;
    std::vector<std::string> result;
    std::istringstream iss(s);
    while(std::getline(iss, token, delim))
        result.push_back(token);
    return result;
}

int main(int argc, char **argv)
{
	if( argc < 3 )
	{
		std::cout << "Usage:" << argv[0] << " input.vtu datalist.txt [output.pvtu]" << std::endl;
		std::cout << "datalist.txt contains one or multiple lines with the following format:" << std::endl;
		std::cout << "dataname datafile.txt [type=0 (0:cell,1:point)] [comps=1]" << std::endl;
		return -1; 
	}
	else
	{
		size_t line;
		std::string dataline;
		std::string input = std::string(argv[1]);
		std::string datalist = std::string(argv[2]);
		std::string output = "output.vtu";
		if( argc > 3 ) output = std::string(argv[3]);
		if( input.find(".vtu") == std::string::npos )
		{
			std::cout << "input file format should be .vtu" << std::endl;
			return -1;
		}
		if( output.find(".vtu") == std::string::npos )
		{
			std::cout << "output file format should be .vtu" << std::endl;
			return -1;
		}
		if( input == output )
		{
			std::cout << "input file should not match output file!" << std::endl;
			return -1;
		}
		std::ifstream fin(input.c_str());
		std::ofstream fout(output.c_str());
		
		if( fin.fail() )
		{
			std::cout << "failed to open input file " << input << std::endl;
			return -1;
		}
		if( fout.fail() )
		{
			std::cout << "failed to open output file " << output << std::endl;
			return -1;
		}
		
		std::ifstream fdl(datalist);
		
		if( fdl.fail() )
		{
			std::cout << "failed to open datalist file " << datalist << std::endl;
			return -1;
		}
		
		copy_until_string(fin, fout, "PointData"); //copy until data description begins
		copy_until_string(fin, fout,">"); //copy to closing scope
		fout << std::endl;
		
		line = 0;
		while( std::getline(fdl,dataline) )
		{
			++line;
			if( !dataline.empty() && dataline[0] == '#' ) continue;
			if( dataline.empty() ) break;
			std::vector<std::string> params = explode(dataline,' ');
			if( params.size() < 2 )
			{
				std::cout << "Error in dataline " << dataline << ", line number " << line << " no enough parameters." << std::endl;
				continue;
			}
			std::string dataname = params[0];
			std::string datafile = params[1];
			int dtype = params.size() > 2 ? atoi(params[2].c_str()) : 0;
			int comps = params.size() > 3 ? atoi(params[3].c_str()) : 1;
			if( dtype != 0 )
			{
				std::ifstream fdata(datafile.c_str());
				std::cout << "Opening " << datafile.c_str() << " to write point data " << dataname << "." << std::endl;
				if( fdata.fail() )
				{
					std::cout << "Error, failed to open " << datafile << " to record " << dataname << " in line number " << line << "." << std::endl;
					continue;
				}
				fout << "<DataArray";
				fout << " type=\"Float64\"";
				fout << " format=\"ascii\"";
				fout << " NumberOfComponents=\"" << comps << "\"";
				fout << " Name=\"" << dataname.c_str() << "\"";
				fout << ">" << std::endl;
				size_t nout = 0, size;
				double value;
				fdata >> size; //skip size
				while( fdata >> value ) 
				{
					fout << value << " ";
					if( ++nout % 10 == 0 )
						fout << std::endl;
				}
				if( nout % 10 != 0 ) 
					fout << std::endl;
				fout << "</DataArray>" << std::endl;
				fdata.close();
			}
		}
		
		copy_until_string(fin, fout, "CellData"); //copy until data description begins
		copy_until_string(fin, fout,">"); //copy to closing scope
		fout << std::endl;
		
		
		fdl.clear();
		fdl.seekg(0);
		line = 0;
		while( std::getline(fdl,dataline) )
		{
			++line;
			if( !dataline.empty() && dataline[0] == '#' ) continue;
			if( dataline.empty() ) break;
			std::vector<std::string> params = explode(dataline,' ');
			if( params.size() < 2 )
			{
				std::cout << "Error in dataline " << dataline << ", line number " << line << " no enough parameters." << std::endl;
				continue;
			}
			std::string dataname = params[0];
			std::string datafile = params[1];
			int dtype = params.size() > 2 ? atoi(params[2].c_str()) : 0;
			int comps = params.size() > 3 ? atoi(params[3].c_str()) : 1;		
			if( dtype == 0 )
			{
				std::ifstream fdata(datafile.c_str());
				std::cout << "Opening " << datafile.c_str() << " to write point data " << dataname << "." << std::endl;
				if( fdata.fail() )
				{
					std::cout << "Error, failed to open " << datafile << " to record " << dataname << " in line number " << line << "." << std::endl;
					continue;
				}
				fout << "<DataArray";
				fout << " type=\"Float64\"";
				fout << " format=\"ascii\"";
				fout << " NumberOfComponents=\"" << comps << "\"";
				fout << " Name=\"" << dataname.c_str() << "\"";
				fout << ">" << std::endl;
				size_t nout = 0, size;
				double value;
				fdata >> size; //skip size
				while( fdata >> value ) 
				{
					fout << value << " ";
					if( ++nout % 10 == 0 )
						fout << std::endl;
				}
				if( nout % 10 != 0 ) 
					fout << std::endl;
				fout << "</DataArray>" << std::endl;
				fdata.close();
			}
		}
		
		fout << fin.rdbuf(); //copy till the end
		
		fout.close();
		fin.close();
	}
	return 0;
}
