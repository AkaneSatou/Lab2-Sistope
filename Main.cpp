Lab2-Sistope
============

Laboratorio 2 sistope, procesos - División de archivos binarios.

#include <iostream>
#include <fstream> //stream de ficheros
#include <string.h>
#include <vector>
#include <algorithm>
#include <sstream>
#include <unistd.h>
#include <exception>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>   
#include <stdlib.h>
#define TAMANO 1024

using namespace std;

//Claudia Alejandra Bustamante Arce
int largoArchivo(string gVal){
   ifstream archivo(gVal.c_str(), ifstream::binary);
   if(archivo){
      archivo.seekg (0, archivo.end); //se va al final
      int len = archivo.tellg(); //obtiene el largo del archivo
      archivo.seekg (0, archivo.beg); //se devuelve al principio
      archivo.close(); //cierra el archivo
      //tengo len 
      //tomar el buffer y dividirlo de a 1024 para escribirlo en un nuevo archivo
      return len;
   }
   return 0;
}

vector<string> cortaBytes(string& ubicacionArchivo,int len){
   vector <string> resultado;
   string tmp;
   int cont=0;
   ifstream binario(ubicacionArchivo.c_str(), ifstream::binary);
   if(binario){
      while(cont<len){
         char c=binario.get();
         cont++;
         tmp.push_back(c);
         if(tmp.size()==1024){
            resultado.push_back(tmp);
            tmp.clear();
         }
      }
      if(tmp.size()>0){
         resultado.push_back(tmp);
      }
   } 
   return resultado;
}

int comprobarConfig(string& CVal){
   int numeroProcesos;
   string Numprocesos;
   ifstream ficheroConfig("./Config");
   if(ficheroConfig.fail()){
      ofstream Config("./Config");
      Config << CVal;
      Config.close();
      ficheroConfig.open("./Config");
   }
   getline(ficheroConfig,Numprocesos);
   numeroProcesos = atoi(Numprocesos.c_str());
   return numeroProcesos;
}

void crearCarpetas(int numeroProcesos){
   string algoPulento = "mkdir ./Discos\\ Raid";
   system(algoPulento.c_str());

   for(int i=0;i<numeroProcesos;i++){
      stringstream dir;
      dir << "mkdir ./Discos\\ Raid/Disco_" << i+1;
      system(dir.str().c_str());
      cout << "\nCreando carpeta N°" << i+1 << endl; 
   }
}

int main (int argc, char **argv){

	string CVal = ""; 
	int Cflag=0;
	//string EVal = ""; // 
	int Eflag=0;
	string gVal = ""; // 
	int gflag=0;
	string eVal = ""; // 
	int eflag=0;
	string rVal = ""; // 
	int rflag=0;
	string RVal = ""; // 
	int Rflag=0;
	//string mVal = ""; // 
	int mflag=0;
	
	int c;
	opterr =0;
	while((c = getopt(argc,argv,"C:E:g:e:r:R:m:")) != -1)
		switch(c){
			case 'C':
					Cflag = 1;
					CVal= optarg;
					break;
			case 'E':
					Eflag = 1;
					//EVal = optarg;
					break;
			case 'g':
					gflag = 1;
					gVal = optarg;
					break;
			case 'e':
					eflag = 1;
					eVal = optarg;
					break;
			case 'r':
					rflag = 1;
					rVal=optarg;
					break;
			case 'R':
					Rflag = 1;
					RVal=optarg;
					break;
			case 'm':
					mflag = 1;
					//mVal=optarg;
					break;
			case '?':
				if (optopt=='C'||optopt == 'E'||optopt == 'g'||optopt == 'e'||optopt == 'r'||optopt == 'R'||optopt == 'm')
					fprintf (stderr, "Opcion -%c requiere un argumento.\n", optopt);
				else if (isprint (optopt))
					fprintf (stderr, "Opcion incorrecta `-%c'.\n", optopt);
				else
					fprintf (stderr,"Opcion desconocida `\\x%x'.\n", optopt);
				return 1;
			default:
				abort();
		}  
      
   int procesoPadre; // id del padre
   int nproc = comprobarConfig(CVal); // numero de procesos en int
   int pid; // identificador de procesos 
   int id; // identificador de procesos hijos
   crearCarpetas(nproc);
   int fd[nproc*2][2]; //array de pipes  
   for (int i =0;i<nproc*2;i++){
      pipe(fd[i]);
   }
   for (int i =0;i<nproc;i++){
      pid = fork(); //creacion de procesos
      if (pid == 0){
         close(fd[2*i][1]);   // procesos hijo escriben por fd[2*i+1][1]  y leen por fd[2*i][0]
         close(fd[2*i+1][0]);
         id=i;
         break;
      }
      else{
         procesoPadre=1;
         close(fd[2*i][0]);  // procesos padre lee por fd[2*i][0]  y esc por fd[2*i+1][1]
         close(fd[2*i+1][1]);
      }
   }
            

   if(Eflag==1){
      /*Aca se elimina el contenido de la carpeta "Discos RAID" 
      si no hay contenido AVISAR!
      si tenia contenido AVISAR! 
      NO HAY EVal*/
   }
   if(gflag==1){
      if(procesoPadre==1){
         int len;
         string nomArch = "./archivos originales/"  + gVal;
         FILE * archbin;
         archbin = fopen(nomArch.c_str(),"rb");
         if(archbin!=NULL){ //si el archivo existe
            cout << "\nEl archivo existe!\n"; //se divide en subpartes 
            len=largoArchivo(nomArch);
            //copiar en la mitad de los discos el bloque y en la otra mitad el espejo
            vector<string> bloques=cortaBytes(nomArch,len);
            for(int i=0; i<bloques.size();i++){
               int j = i % (nproc/2);
               write(fd[2*j+1][1],&i,TAMANO);
               write(fd[2*j+1][1],bloques[i],TAMANO);
               write(fd[2*j+1+(nproc/2)][1],&i,TAMANO);
               write(fd[2*j+1+(nproc/2)][1],bloques[i],TAMANO);
            }
         }
         else { 
            cout << "\nEl archivo no existe\n"; // no hace nada 
         }
      }
      else{ //soy hijo 
         while(true){
            string mensaje= "";
            string identif= "";
            read(fd[2*id][0],identif,TAMANO);
            read(fd[2*id][0],mensaje,TAMANO);
            //ofstream 
         }//al momento de crear el archivo le debo concatenar el identif para diferenciar los archivos creados 

      }
      /*si el archivo no esta en "originales", entonces AVISAR!
      si el archivo ya existe, AVISAR! y no hacer nada
      si la operacion se hace correctamente, se hace y AVISAR!*/
   }
   if(eflag==1){
    /*eVal="Archivo que se le borrará un bloque"
    Si el archivo no existe en "originales", entonces AVISAR!
    Si se hace correctamente, AVISAR! */
   }
   if(rflag==1){
    /*rVal="Archivo que se restaurará"
    si el archivo no existe en "originales" AVISAR!
    si no se puede restaurar, se debe AVISAR! y no hacer nada
    si la operacion es satisfactoria, se hace y AVISAR!
    */
   }
   if(Rflag==1){
   /*RVal="archivo recuperado" lo guarda en "originales"
   si el archivo no existe en originales, entonces AVISAR!
    si falta alguna parte en los discos originales y de respaldo AVISAR y no hacer nada
    si la operacion es satisfactoria, se hace y AVISAR!*/
   }
   if(mflag==1){
    /*NO HAY mVal
    aca se muestran los archivos
    si no hay sistema de almacenamiento AVISAR!
    si hay sistema de almacenamiento se listan los archivos
    sin errores
    corruptos 
    con bloques inexistentes en original y respaldo se deben mostrar con **
    */
   }

        /*aca se crean los discos, recibiendo como parametro el numero de discos a utilizar.
        Si ya existe el directorio con los discos, no se debe hacer nada AVISAR!
        Si no existe, se crea y se avisa qe se creo AVISAR! */
      //}
	return 0;
}
