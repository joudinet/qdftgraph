// Copyright (C) 2011, 2012 Johan Oudinet <oudinet@lri.fr>
//  
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//  
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//  
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//  
#ifndef QDFT_TEST_LIB_CC
# define QDFT_TEST_LIB_CC
# include <vector>
# include <string>
# define SIMPLIFY
# include <qdft/qdft.hh>
#include <iostream>
#include <ctime>

#define NS (7)
#define NC (10)

#include <iostream>
#include <fstream>
#include <cstring>
#include <list>
#include <algorithm>
#include <string>
using std::cout;
using std::endl;
using std::ifstream;

#define MAX_CHARS 30000			/* chars per line 	*/
#define MAX_ENTRY 500000		/* rows in file 	*/
#define MAX_TOKENS_LINE 20		/* tokens per line 	*/
#define MAIN_DELIMITER "|"		/* tokens delimiter */
#define PARAM_DELIMITER "*"		/* params name-value separator	*/
#define DEBUG(x) (std::cout)<<"deb:"<< x <<"\n";fflush(stdout); /* was worth it ;-) */

#define PROCESS "ProcessName" //PID or ProcessName
#define TRUNCATE 1 //0-no truncate action 1-truncate

typedef qdft::data_managers<>	data_managers_t;

int overwrite=0;

typedef struct transfer_t {
	char *event;
	char *src;
	char *dst;
	long int size;
} Transfert;

typedef struct parameter_t {
	char *name;
	char *value;
} Parameter;



Parameter parse_parameter(char *str) {
	Parameter param;
	//	param.name = (char *)malloc(sizeof(char)*MAX_CHARS);
	//param.value = (char *)malloc(sizeof(char)*MAX_CHARS);
	
	if (str) {
		param.name = strtok(str,PARAM_DELIMITER);
		param.value = strtok(0, PARAM_DELIMITER);
	}
	return param;
}


void update_transfert(Transfert *transfer, Parameter param) {
	
	if (!strcmp(transfer->event,"ReadFile")){
		if (!strcmp(param.name, "InFileName")) {
			transfer->src=(param.value);
		} else if (!strcmp(param.name, PROCESS)) {
			transfer->dst=(param.value);
		} else if (!(strcmp(param.name, "ToReadBytes"))) {
			transfer->size = std::atoi(param.value);
		}
	} else if(!strcmp(transfer->event,"WriteFile")){
		if (!strcmp(param.name, "InFileName")) {
			transfer->dst=(param.value);
		} else if (!strcmp(param.name, PROCESS)) {
			transfer->src=(param.value);
		} else if (!(strcmp(param.name, "ToWriteBytes"))) {
			transfer->size = std::atoi(param.value);
		}
	} else if(!strcmp(transfer->event,"GetClipboardData")){
		if (!strcmp(param.name, PROCESS)) {
			transfer->src=strdup("S-CLIPBOARD");
			transfer->dst=(param.value);
		} else if (!(strcmp(param.name, "ClipboardSize"))) {
			transfer->size = std::atoi(param.value);
		}
	} else if(!strcmp(transfer->event,"SetClipboardData")){
		if (!strcmp(param.name, PROCESS)) {
			transfer->src=(param.value);
			transfer->dst=strdup("S-CLIPBOARD");
		} else if (!(strcmp(param.name, "ClipboardSize"))) {
			transfer->size = std::atoi(param.value);
		}
	} else if(!strcmp(transfer->event,"Send")){
		if (!strcmp(param.name, PROCESS)) {
			transfer->src=(param.value);
		} else if (!(strcmp(param.name, "RemoteAddress"))) {
			transfer->dst=(param.value);
		} else if (!(strcmp(param.name, "ToSendBytes"))) {
			transfer->size = std::atoi(param.value);
		}
	} else if(!strcmp(transfer->event,"Recv")){
		if (!strcmp(param.name, PROCESS)) {
			transfer->dst=(param.value);
		} else if (!(strcmp(param.name, "RemoteAddress"))) {
			transfer->src=(param.value);
		} else if (!(strcmp(param.name, "ToReadBytes"))) {
			transfer->size = std::atoi(param.value);
		}
	} else {
		transfer->size=0;
		transfer->src=strdup("NOSOURCE");
		transfer->dst=strdup("NODEST");
	}
	
	//	printf ("\n\n    current transfer (%s,%s,%l)\n",transfer->src,transfer->dst,transfer->size);
}


Transfert parse_row(char *row) {

	char *token[MAX_TOKENS_LINE];
	Parameter tmp;
	Transfert result;
	long int i;
	long int count;
	char* event=NULL;

	if (strcmp(row,"")==0){
		result.event=strdup("");
		result.src=strdup("");
		result.dst=strdup("");
		result.size=0;
		//		std::cout<< "empty row, leaving";
		return result;
	}
	
	token[0] = strtok(row,MAIN_DELIMITER);
	//std::cout << "token0:" << token[0];
		
	if (token[0]) {
		for (i=1; i < MAX_TOKENS_LINE; i++) {
			token[i] = strtok(0, MAIN_DELIMITER);
			//std::cout << "token["<<i<<"]:" << token[i];
		}
	}
	count = i;
	
	//printf("count=%d\n",count);
	
	if (i>0){
		tmp = parse_parameter(token[0]); //EVENT MUST BE FIRST PARAMETER TO MATCH
		//printf ("pname=%s,pvalue=%s\n",tmp.name,tmp.value);
		if (!(strcmp(tmp.name, "event"))) { 
			result.event=strdup(tmp.value);
			//			free (tmp.value);
			//free (tmp.name);
		}	else {
			//free (tmp.value);
			//free (tmp.name);
			return result;
		}
	}

	for (long int i=1; i<count; i++) {
		//printf ("update_transfer %d\n",i);
		tmp = parse_parameter(token[i]); 
		if (tmp.value) {
			update_transfert(&result,tmp);
		}
		//free (tmp.value);
		//free (tmp.name);
	}

	/* some happy debugging */
	// DEBUG(result.event);
	// DEBUG(result.src);
	// DEBUG(result.dst);
	// DEBUG(result.size);

	return result;

}

int main(int argc, char* argv[]) {

	ifstream f;
	//	Transfert *tarray = (Transfert *)malloc(MAX_ENTRY*sizeof(Transfert));
	Transfert tran;
	std::list<std::string> existingContainers;
	std::list<std::string>::iterator findIter;
	long int i=0;
	char *secondargument="";
	unsigned sensitive_size=50000;
	unsigned public_size=100;
	data_managers_t dmanagers;
	qdft::dname_type d = dmanagers.new_data (NS * sensitive_size, qdft::unknown, "D-phones");
	typedef std::vector<std::string>	strings_t;
	strings_t sources (NS);
	strings_t containers (NC);
	char buff[MAX_CHARS];
		
#ifndef NO_SIMPLIFY
	std::cerr << "Simplification rules: activated\n";
#else
	std::cerr << "Simplification rules: deactivated\n";
#endif
			
	if (argc < 2) {
		std::cout << "Too few arguments.\n\n Usage: testlib <log_file> [initial sensitive container]";
		exit(-1);
	}
	
	if (argc > 2) secondargument=strdup((char*)argv[2]);

	f.open((const char *)argv[1]);
	
	if (!f.good()) {
		std::cout << "Non existing file";
		exit(-1);
	}
	
	while (!f.eof() && i<MAX_ENTRY) {
		f.getline(buff, MAX_CHARS);
		//printf("row[%d]=%s\n",i,buff);
		if (f) {
			//			tarray[i] = parse_row(buff);
			tran = parse_row(buff);
			printf ("\ntran (%lu)=(%s,%s,%lu)\n",i,tran.src,tran.dst,tran.size);

			if (strcmp(tran.event,""))	i++;

			if (!(strcmp(tran.event,"ReadFile")&&
						strcmp(tran.event,"WriteFile")&&
						strcmp(tran.event,"SetClipboardData")&&
						strcmp(tran.event,"GetClipboardData")&&
						strcmp(tran.event,"Send")&&
						strcmp(tran.event,"Recv"))){
				if ((argc<3)||(strstr(tran.src, secondargument))) {
					findIter=std::find(existingContainers.begin(), existingContainers.end(), tran.src);
					if (findIter==existingContainers.end()){
						std::cerr<<"--> Adding "<< tran.src <<" to the list of containers (with initialization)"<< std::endl;
						existingContainers.push_back(tran.src);
						dmanagers.init (d, sensitive_size, tran.src);
					} 				
				}
				findIter=std::find(existingContainers.begin(), existingContainers.end(), tran.dst);
				if (findIter==existingContainers.end()){
					std::cerr<<"--> Adding "<< tran.dst <<" to the list of containers (no initialization)"<< std::endl;
					existingContainers.push_back(tran.dst);
				}
	
				if (!(strcmp(tran.event,"SetClipboardData"))&& TRUNCATE){
					dmanagers.truncate (d, tran.dst, 0);
				}

				std::cerr << i << " - Event "<<tran.event << "\n"; //<< "): transferring " << tran.size << " from " << tran.src << " to " << tran.dst << std::endl;
				dmanagers.transfer (d, tran.size, tran.src, tran.dst);
			}
		}
	}
	dmanagers.show_graphs ();
	std::cerr << "\n--------\n";
	while (!existingContainers.empty()) {
		std::cerr<<existingContainers.front()<<std::endl;
		existingContainers.pop_front();
	}
	f.close();
}
#endif // ! QDFT_TEST_LIB_CC
