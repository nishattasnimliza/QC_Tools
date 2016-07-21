#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cctype>
#include <locale>
#include <functional>
#include <algorithm>
#include <cstring>
#include <sstream>
#include <cstdlib>
#include <vector>
#include <sys/stat.h>
#include <math.h>

#include "io.hpp"
#include "../MATRIX/matrix.hpp"

void show_usage(std::string name) {
	
	std::cerr << "Usage: " << name << " <options(s)> SOURCES"
						<< '\n'
						<< "Options:\n"
						<< "\t-h,      --help             \tShow this help message\n"
						<< "\t-l_1,    --log1  PATH/LOGFILE\tSpecify the Gaussian .log file\n"
						<< "\t                            \tfor monomer 1 this is important\n"
						<< "\t                            \tfor determining the HOMO and LUMO\n"
						<< "\t-l_2,    --log2  PATH/LOGFILE\tSpecify the Gaussian .log file\n"
						<< "\t                            \tfor monomer 2 this is important\n"
						<< "\t                            \tfor determining the HOMO and LUMO\n"
						<< "\t-l_P,    --logP  PATH/LOGFILE\tSpecify the Gaussian .log file\n"
						<< "\t                            \tfor the dimer pair, must contain\n"
						<< "\t                            \tthe overlap matrix for the dimer\n"
						<< "\t-p_1,    --pun1  PATH/PUNFILE\tSpecify the Guassian .pun file\n"
						<< "\t                            \tfor the first monomer\n"
						<< "\t-p_2,    --pun2  PATH/PUNFILE\tSpecify the Guassian .pun file\n"
						<< "\t                            \tfor the second monomer\n"
						<< "\t-p_P,    --punP  PATH/PUNFILE\tSpecify the Gaussian .pun file\n"
						<< "\t                            \tfor the dimer pair\n"
						<< "\t-ho_1,   --homo1 #           \tSpecify the homo MO for monomer 1\n"
						<< "\t                            \tshould be an interger value\n"
						<< "\t-ho_2,   --homo2 #           \tSpecify the homo MO for monomer 2\n"
						<< "\t                            \tshould be an interger value\n"
						<< std::endl;
}

std::string lastStringInPath(std::string input){
	std::size_t found;
	std::string line;
	line = input;
	while ((int)(found=line.find("/"))!=-1){
		line = line.substr(found+1,line.size());
	}
	return line;
}

std::string lastN(std::string input, int n) {
	return input.substr(input.size()-n);
}

std::string cut_end(std::string input, int n) {
	return input.substr(0,input.size()-n);
}

std::string firstN(std::string input, int n) {
	return input.substr(0,n);
}

std::string cut_beg(std::string input, int n) {
	return input.substr(n,input.size());
}

// trim from start (in place)
void ltrim(std::string &s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
}

// trim from end (in place)
void rtrim(std::string &s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
}

// trim from both ends (in place)
void trim(std::string &s) {
	ltrim(s);
	rtrim(s);
}

// trim from start (copying)
std::string ltrimmed(std::string s) {
	ltrim(s);
	return s;
}

// trim from end (copying)
std::string rtrimmed(std::string s) {
	rtrim(s);
	return s;
}


// trim from both ends (copying)
std::string trimmed(std::string s) {
	trim(s);
	return s;
}

int file_exist(char *name) {
	struct stat buffer;
	return (stat (name, &buffer) == 0);
}

void removeSpace(char* s) {
	char* s2 = s;
	do {
		if (*s2 != ' ')
			*s++ = *s2;
	} while (*s2++);
}

int check_arguments(char * argv[], int argc, std::string *log, std::string *pun1, std::string *pun2, std::string *punP, int * H1, int * H2){
		
	if(argc <= 1) {
		show_usage(argv[0]);
		return -1;
	}

	int logP_flag;
	int log1_flag;
	int log2_flag;
	int HOMO1_flag;
	int HOMO2_flag;
	int err_exit_flag;

	int HOMO1;
	int HOMO2;

	logP_flag = 0;
	log1_flag = 0;
	log2_flag = 0;
	HOMO1_flag = 0;
	HOMO2_flag = 0;
	err_exit_flag = 0;

	std::string temp;
	std::string HOMO1log;
	std::string HOMO2log;
	std::string P_log;
	std::string arg;
	std::string ext;

	for ( int i=1;i<argc;++i){
		arg=argv[i];
		if((arg=="-h")||(arg=="--help")){
			show_usage(argv[0]);
			return -1;
		}else if ((arg=="-l_1")||(arg=="--log1")){
			if(i+1<argc){
				if(check_string_input(argv[i+1])==-1){
					temp = argv[i+1];
					ext = lastN(temp,4);
					if(ext==".log"){
						if(file_exist(const_cast<char*>((temp).c_str()))==0){
							std::cerr << "ERROR The file "+(temp)+" does not exist!" << std::endl;
							err_exit_flag = -1;
						}else{
							log1_flag = 1;
							HOMO1log = temp;
						}
						i++;
					}else{
						std::cerr << "WARNING --log1 option has no file specified!" << std::endl;
					}
				}else{
					std::cerr << "ERROR --log1 option requires *.log file!" << std::endl;
					err_exit_flag = -1;
				}
			}else{
				std::cerr << "ERROR --log1 option requires one argument!" << std::endl;
				err_exit_flag = -1;
			}
		}else if ((arg=="-l_2")||(arg=="--log2")){
			if(i+1<argc){
				if(check_string_input(argv[i+1])==-1){
					temp = argv[i+1];
					ext = lastN(temp,4);
					if(ext==".log"){
						if(file_exist(const_cast<char*>((temp).c_str()))==0){
							std::cerr << "ERROR The file "+(temp)+" does not existi!" << std::endl;
							err_exit_flag = -1;
						}else{
							log2_flag = 1;
							HOMO2log = temp;
						}
						i++;
					}else{
						std::cerr << "WARNING --log2 option has no file specified!" << std::endl;
					}
				}else{
					std::cerr << "ERROR --log2 option requires *.log file!" << std::endl;
					err_exit_flag = -1;
				}
			}else{
				std::cerr << "ERROR --log2 option requires one argument!" << std::endl;
				err_exit_flag = -1;
			}
		}else if ((arg=="-l_P")||(arg=="--logP")){
			if(i+1<argc){
				if(check_string_input(argv[i+1])==-1){
					*log = argv[i+1];
					ext = lastN(*log,4);
					if(ext==".log"){
						if(file_exist(const_cast<char*>((*log).c_str()))==0){
							std::cerr << "ERROR The file "+(*log)+" does not exist!" << std::endl;
							err_exit_flag = -1;
						}else{
							logP_flag = 1;
						}
						i++;
					}else{
						std::cerr << "WARNING --logP option has no file specified!" << std::endl;
					}
				}else{
					std::cerr << "ERROR --logP option requires *.log file!" << std::endl;
					err_exit_flag = -1;
				}
			}else{
				std::cerr << "ERROR --logP option requires one argument!" << std::endl;
				err_exit_flag = -1;
			}
		}else if ((arg=="-p_1")||(arg=="--pun1")){
			if(i+1<argc){
				if(check_string_input(argv[i+1])==-1){
					*pun1 = argv[i+1];
					ext = lastN(*pun1,4);
					if(ext==".pun"){
						if(file_exist(const_cast<char*>((*pun1).c_str()))==0){
							std::cerr << "ERROR The file "+(*pun1)+" does not exist!" << std::endl;
							err_exit_flag = -1;
						}
						//Check if log file exist with same name as pun1 file
						temp = cut_end(*pun1,4);
						temp = temp + ".log";
						if(file_exist(const_cast<char*>((temp).c_str()))!=0){
							if(log1_flag==0){
								log1_flag = 1;
								HOMO1log = temp;
							}
						}
						i++;
					}else{
						std::cerr << "WARNING --pun1 option has no file specified!" << std::endl;
					}
				}else{
					std::cerr << "ERROR --pun1 option requires *.pun file!" << std::endl;
					err_exit_flag = -1;
				}
			}else{
				std::cerr << "ERROR --pun1 option requires one argument!" << std::endl;
				err_exit_flag = -1;
			}
		}else if ((arg=="-p_2")||(arg=="--pun2")){
			if(i+1<argc){
				if(check_string_input(argv[i+1])==-1){
					*pun2 = argv[i+1];
					ext = lastN(*pun2,4);
					if(ext==".pun"){
						if(file_exist(const_cast<char*>((*pun2).c_str()))==0){
							std::cerr << "ERROR The file "+(*pun2)+" does not exist!" << std::endl;
							err_exit_flag = -1;
						}
						//Check if log file exist with same name as pun2 file
						temp = cut_end(*pun2,4);
						temp = temp + ".log";
						if(file_exist(const_cast<char*>((temp).c_str()))!=0){
							if(log2_flag==0){
								log2_flag = 1;
								HOMO2log = temp;
							}
						}
						i++;
					}else{
						std::cerr << "WARNING --pun2 option has no file specified!" << std::endl;
					}
				}else{
					std::cerr << "ERROR --pun2 option requires *.pun file!" << std::endl;
					err_exit_flag = -1;
				}
			}else{
				std::cerr << "ERROR --pun2 option requires one argument!" << std::endl;
				err_exit_flag = -1;
			}
	
		}else if ((arg=="-p_P")||(arg=="--punP")){
			if(i+1<argc){
				if(check_string_input(argv[i+1])==-1){
					*punP = argv[i+1];
					ext = lastN(*punP,4);
					if(ext==".pun"){
						if(file_exist(const_cast<char*>((*punP).c_str()))==0){
							std::cerr << "ERROR: The file "+(*punP)+" does not exist!" << std::endl;
							err_exit_flag = -1;
						}
						//Check if log file exist with same name as pun1 file
						temp = cut_end(*punP,4);
						temp = trimmed(temp + ".log");

						if(file_exist(const_cast<char*>(temp.c_str()))!=0){
							logP_flag = 2;
							P_log = temp;
						}
						i++;
					}else{
						std::cerr << "WARNING --punP option has no file specified!" << std::endl;
					}
				}else{
					std::cerr << "ERROR --punP option requires *.pun file!" << std::endl;
					err_exit_flag = -1;
				}
			}else{
				std::cerr << "ERROR --punP option requires one argument!" << std::endl;
				err_exit_flag = -1;
			}
		}else if ((arg=="-ho_1")||(arg=="--homo1")){
			if(i+1<argc){
				if(check_string_input(argv[i+1])==-1){
					HOMO1 = atoi(argv[i+1]);
					if(HOMO1 < 1){
						std::cerr << "HOMO for monomer 1 must be greater than 0." << std::endl;
					}else{
						*H1 = HOMO1;
						HOMO1_flag = 1;
						log1_flag = 2;
					}
					i++;
				}else{
						std::cerr << "WARNING --homo1 option has no interger value specified!" << std::endl;
					}
			}else{
				std::cerr << "ERROR --homo1 option requires one argument!" << std::endl;
				err_exit_flag = -1;
		  }
		}else if ((arg=="-ho_2")||(arg=="--homo2")){
			if(i+1<argc){
				if(check_string_input(argv[i+1])==-1){
					HOMO2 = atoi(argv[i+1]);
					if(HOMO2 < 1){
						std::cerr << "HOMO for monomer 2 must be greater than 0." << std::endl;
					}else{
						*H2 = HOMO2;
						HOMO2_flag = 1;
						log2_flag = 2;
					}
					i++;
				}else{
						std::cerr << "WARNING --homo2 option has no interger value specified!" << std::endl;
					}
			}else{
				std::cerr << "ERROR --homo2 option requires one argument!" << std::endl;
				err_exit_flag = -1;
		  }

		}else {

			std::cerr << "WARNING the following is not an option " << arg << "\n";
		}
	}

	if(HOMO1_flag==0 && log1_flag==1 && err_exit_flag!=-1){
		std::cerr << "Will determine HOMO1 value from the "<< HOMO1log <<" file." << std::endl;
		*H1 = log_getHOMO(&HOMO1log);
	}

	if(HOMO2_flag==0 && log2_flag==1 && err_exit_flag!=-1){
		std::cerr << "Will determine HOMO2 value from the "<< HOMO2log << " file." << std::endl;
		*H2 = log_getHOMO(&HOMO2log);
	}
		
	if(logP_flag ==2 && err_exit_flag!=-1){
		*log = P_log;	
	}

	if(argc<7){
		std::cerr << "ERROR Must have a *.pun file for each monomer and the dimer,\n";
		std::cerr << "and a *.log file for the dimer containing the overlap matrix,\n";
		std::cerr << "to execute. By default the program will check to see if the\n";
		std::cerr << "log file has the same name as the pun files" << std::endl;
		show_usage(argv[0]);
		err_exit_flag = -1;
	}
	
	if(err_exit_flag==-1){
		return -1;
	}

	return 0;
}

int check_string_input(std::string str){

	if(str=="--log1" || str=="-l_1"){
		return 0;
	}else if(str=="--log2" || str=="-l_2"){
		return 0;
	}else if(str=="--logP" || str=="-l_P"){
		return 0;
	}else if(str=="--homo1" || str=="-ho_1"){
		return 0;
	}else if(str=="--homo2" || str=="-ho_2"){
		return 0;
	}else if(str=="--help" || str=="-h"){
		return 0;
	}else if(str=="--pun1" || str=="-p_1"){
		return 0;
	}else if(str=="--pun2" || str=="-p_2"){
		return 0;
	}else if(str=="--punP" || str=="-p_P"){
		return 0;
	}else{
		return -1;
	}
}

int pun_countMO(std::string *pun){

	std::string ext;
	ext = lastN(*pun,4);
	if(ext==".pun"){
		if(file_exist(const_cast<char*>((*pun).c_str()))==0){
			std::cerr << "The file "+(*pun)+" does not exist." << std::endl;
			return -1;
		}
	}else{
		std::cerr << "The file "+(*pun)+" does not have the correct extension." << std::endl;
		std::cerr << "The file must be a .pun file, it should have the same" << std::endl;
		std::cerr << "format as the Gaussian fort.7 file" << std::endl;
		return -1;
	}

	int temp;
	int MO;
	std::size_t found;
	std::ifstream PunFile;
	std::string line;
	std::string str;

	PunFile.open(const_cast<char*>((*pun).c_str()),std::ifstream::in);
	if(PunFile.is_open()){
		temp = 0;
		MO = 0;
		while(std::getline(PunFile,line)){
			if(((int)(found=line.find("Alpha")))!=-1){
				str = trimmed(firstN(line,(int)found));
				temp = atoi(str.c_str());
				if(temp>MO){
					MO=temp;
				}
			}
		}
	}
	PunFile.close();
	return MO;
}

int pun_getMO(std::string *pun, Matrix *mat_Coef, Matrix *mat_OE){

	int i;
	int j;
	int temp;
	int flag;
	int indent;
	int MO;

	double temp_d;
	double temp_d2;

	std::size_t found;

	std::string str;
	std::string line;
	std::ifstream PunFile;

	MO = pun_countMO(pun);

	(*mat_Coef).resize(MO,MO);
	(*mat_OE).resize(MO,1);
	
	PunFile.open(const_cast<char*>((*pun).c_str()),std::ifstream::in);
	if(PunFile.is_open()){
		i = 0;
		j = 0;
		//Skip the first line
		std::getline(PunFile,line);

		while(std::getline(PunFile,line)){
			flag = (int)(found=line.find("OE"));
			if(flag!=-1){
				i++;
				str = line.substr(found+3,11);
				str = trimmed(str);
				temp_d = atof(str.c_str());
				str = line.substr(found+15,4);
				str = trimmed(str);
				temp_d2 = atof(str.c_str());
				(*mat_OE).set_elem(temp_d*pow(10,temp_d2),i);
			}else{
				temp = 1;
				indent = 0;
				while (temp<6 && j<MO){
					j++;
					str = line.substr(0+indent,11);
					str = trimmed(str);
					temp_d = atof(str.c_str());
					str = line.substr(12+indent,3);
					str = trimmed(str);
					temp_d2 = atof(str.c_str());
					(*mat_Coef).set_elem(temp_d*pow(10,temp_d2),i,j);
					indent += 15;
					temp++;
				}

				if(j>=MO){
					j=0;
				}
			}
		}
	}

	return 0;
}

int log_countMO(std::string *log){

	std::string ext;
	ext = lastN(*log,4);
	if(ext==".log"){
		if(file_exist(const_cast<char*>((*log).c_str()))==0){
			std::cerr << "The file "+(*log)+" does not exist." << std::endl;
			std::cerr << "Cannot execute log_countMO function." << std::endl;
			return -1;
		}
	}else{
		std::cerr << "The file "+(*log)+" does not have the correct extension." << std::endl;
		std::cerr << "The file must be a .log file, it should have the same" << std::endl;
		std::cerr << "format as the Gaussian .log file, cannot execute " << std::endl;
		std::cerr << "log_countMO function." << std::endl;
		return -1;
	}

	int MO;
	int flag1;
	int flag2;
	
	std::size_t index; 
	std::size_t found1;
	std::size_t found2;

	std::string str;
	std::string line;
	std::ifstream LogFile;

	LogFile.open(const_cast<char*>((*log).c_str()),std::ifstream::in);
	if(LogFile.is_open()){

		MO = 0;

		while(std::getline(LogFile,line)){
			flag1 = (int)(found1=line.find("Alpha  occ. eigenvalues -- "));
			flag2 = (int)(found2=line.find("Alpha virt. eigenvalues -- "));
			
			index = 28;
			if(flag1!=-1){
				found1 = found1+27;
				str = line.substr(found1,10);
				str = trimmed(str);
				while(!str.empty()){
					MO++;
					index = index+10;
					if(index<=line.length()){
						found1 = found1+10;
						str = line.substr(found1,10);
						str = trimmed(str);
					}
				}
			}
			if(flag2!=-1){
				found2 = found2+27;
				str = line.substr(found2,10);
				str = trimmed(str);
				while(!str.empty()){
					MO++;
					index = index+10;
					if(index<=line.length()){
						found2 = found2+10;
						str = line.substr(found2,10);
						str = trimmed(str);
					}
				}
			}
		}
	}
	return MO;
}

int log_getS(std::string *log, Matrix *mat_S, int MO){

	std::string ext;
	ext = lastN(*log,4);
	if(ext==".log"){
		if(file_exist(const_cast<char*>((*log).c_str()))==0){
			std::cerr << "The file "+(*log)+" does not exist." << std::endl;
			std::cerr << "Cannot execute log_getS function." << std::endl;
			return -1;
		}
	}else{
		std::cerr << "The file "+(*log)+" does not have the correct extension." << std::endl;
		std::cerr << "The file must be a .log file, it should have the same" << std::endl;
		std::cerr << "format as the Gaussian .log file, cannot execute " << std::endl;
		std::cerr << "log_getS function." << std::endl;
		return -1;
	}
	
	if(MO<=0){
		std::cerr << "The total number of Molecular orbitals" << std::endl;
		std::cerr << "submitted to the log_getS function must" << std::endl;
		std::cerr << "be greater than 0." << std::endl;
		return -1;
	}

	int flag1;
	int flag2;
	int count1;
	int count2;
	int count2Correction;
	int count3;
	int Total;
	int Column;
	int MatrixCol;
	int MatrixColInit;
	int index;
	double val;

	std::size_t found1;
	
	std::string str1;
	std::string str2;
	std::string line;
	std::ifstream LogFile;

	(*mat_S).resize(MO,MO);

	LogFile.open(const_cast<char*>((*log).c_str()),std::ifstream::in);
	if(LogFile.is_open()){

		flag2 = 0;
		MatrixColInit = 1;
		count2Correction = 0;

		while(std::getline(LogFile,line)){
			flag1 = (int)(found1=line.find(" *** Overlap ***"));
			
			if(flag1!=-1){
				
				flag2 = 1;
				//Skip first line
				std::getline(LogFile,line);
				str1 = line.substr(8,10);
				str1 = trimmed(str1);
				count1 = 1;
				Total = MO;
				while (count1<=MO){
					count2 = 1;
					Column = 1;
					while (count2<=Total){
						std::getline(LogFile,line);
						count3 = 1;
						index = 8;
						MatrixCol = MatrixColInit;

						while (count3<=Column){
							str1 = line.substr(index,9);
							str1 = trimmed(str1);
							str2 = line.substr(index+10,3);
							str2 = trimmed(str2);
							val = std::stod(str1)*pow(10,std::stod(str2));
							index = index+14;
							(*mat_S).set_elem(val,count2+count2Correction,MatrixCol);
							if((count2+count2Correction)!=MatrixCol){
								(*mat_S).set_elem(val,MatrixCol,count2+count2Correction);
							}
							count3++;
							MatrixCol++;
						}
						if (Column<5){
							Column++;
						}
						count2++;
					}	
					count2Correction = count2Correction+5;
					std::getline(LogFile,line);
					MatrixColInit = MatrixColInit+5;
					Total = Total-5;
					count1++;
				}
			}
		}
	}

	if(flag2==0){
		std::cerr << "The file "+(*log)+" does not appear to contain the" << std::endl;
		std::cerr << "overlap matrix, cannot execute log_getS function, be" << std::endl;
		std::cerr << "sure to include the following keywords in your gaussian" << std::endl;
		std::cerr << "run file." << std::endl;
		std::cerr << "iop(3/33=4) pop=full" << std::endl;
		return -1;
	}

	return 0;
}

int log_getHOMO(std::string *log){

	int j;
	int k;
	int flag;
	int HOMO;
	int indent;
	int MO;

	std::ifstream LogFile;

	std::size_t found;

	std::string str;
	std::string line;

	MO = log_countMO(log);

	LogFile.open(const_cast<char*>((*log).c_str()),std::ifstream::in);
	if(LogFile.is_open()){

		flag = 0;
		HOMO = 0;
		j = 0;

		while(std::getline(LogFile,line) && j<MO && flag < 3){

			if(((int)(found=line.find("*** Overlap ***")))!=-1){
				flag = 1;
				std::getline(LogFile,line);
			}
			if(((int)(found=line.find("occ")))!=-1){
				flag = 2;
			}
			if(((int)(found=line.find("virt")))!=-1){
				flag = 3;
			}
			if(flag==2){
				indent = 0;
				k = 1;
				while(k<=5 && ((int)line.length())>=(29+indent+9) ){
					indent += 10;
					k++;
					j++;
					HOMO++;
				}
			}
		}
	}

	return HOMO;
}


int log_getLUMO(std::string *log){
	
	int j;
	int k;
	int flag;
	int indent;
	int LUMO;
	int HOMO;
	int MO;

	std::ifstream LogFile;

	std::size_t found;

	std::string str;
	std::string line;

	MO = log_countMO(log);

	LogFile.open(const_cast<char*>((*log).c_str()),std::ifstream::in);
	if(LogFile.is_open()){

		HOMO = 0;
		LUMO = 1;
		flag = 0;
		j = 0;

		while(std::getline(LogFile,line) && j<MO && flag < 3){

			if(((int)(found=line.find("*** Overlap ***")))!=-1){
				flag = 1;
				std::getline(LogFile,line);
			}
			if(((int)(found=line.find("occ")))!=-1){
				flag = 2;
			}
			if(((int)(found=line.find("virt")))!=-1){
				flag = 3;
			}
			if(flag==2){
				indent = 0;
				k = 1;
				str = "Init";
				while(k<=5 && ((int)line.length())>=(29+indent+9) ){
					indent += 10;
					k++;
					j++;
					HOMO++;
				}

				LUMO=HOMO+1;
			}
		}
	}

	return LUMO;
}



