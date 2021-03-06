Lab2-Sistope
============

Laboratorio 2 sistope, procesos - División de archivos binarios.

/*Sistemas Operativos 2-2014
Claudia Bustamante Arce*/
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


const int TAMANO = 1024;


using namespace std;

/*Función que mide el largo del archivo binario de acuerdo a cantidad de bytes*/
int largoArchivo(string nombreFichero){
   ifstream archivo(nombreFichero.c_str(), ifstream::binary);
   if(archivo){ //si se abre correctamente
      archivo.seekg (0, archivo.end); //se va al final
      int len = archivo.tellg(); //obtiene el largo del archivo
      archivo.seekg (0, archivo.beg); //se devuelve al principio
      archivo.close(); //cierra el archivo
      //tengo len 
      return len;
   }
   return 0;
}

/*Función que corta los strings de tamaño 1024 y los almacena en un vector de strings, para su posterior escritura en discos*/
vector<string> cortaBytes(string& ubicacionArchivo,int len){
   vector <string> resultado; //vector resultante con strings de a 1024
   string tmp;
   int cont=0;
   ifstream binario(ubicacionArchivo.c_str(), ifstream::binary); //apertura del archivo binario
   if(binario){ //si se logra abrir
      while(cont<len){ 
         char c=binario.get(); //se obtiene cada caracter
         cont++;
         tmp.push_back(c); // se almacena en el string temporal tmp
         if(tmp.size()==1024){ //si cuenta 1024, entonces copia el string en el vector
            resultado.push_back(tmp);
            tmp.clear(); //limpia el temporal
         }
      }
      if(tmp.size()>0){ //si la cola del archivo es menos a 1024 y mayor a 0, entonces tambien se almacena
         resultado.push_back(tmp);
      }
   } 
   return resultado; //se retorna el vector de strings
}

/*Función que valida la existencia del fichero Config*/
int comprobarConfig(string& CVal){
   int numeroProcesos;
   string Numprocesos; //string para extraer el numero indicado en el config que sera el argumento de la bandera -C
   ifstream ficheroConfig("./Config");
   if(ficheroConfig.fail()){ //si la lectura falla
      ofstream Config("./Config"); //crea el archivo 
      Config << CVal; // y el argumento que recibirá el fichero es el argumento de la bandera -C
      Config.close(); //se cierra el fichero 
      ficheroConfig.open("./Config");
   }
   getline(ficheroConfig,Numprocesos); //toma el número guardado en config y se almacena como número de procesos
   numeroProcesos = atoi(Numprocesos.c_str()); //se transforma a entero
   return numeroProcesos; 
}

/*Función que crea las respectivas carpetas de acuerdo al número de procesos*/
void crearCarpetas(int numeroProcesos){ 
   string direc = "mkdir ./Discos\\ Raid"; //ingreso del comando
   system(direc.c_str()); 

   for(int i=0;i<numeroProcesos;i++){ //creación de discos de acuerdo al número de procesos
      stringstream dir;
      dir << "mkdir ./Discos\\ Raid/Disco_" << i+1;
      system(dir.str().c_str());
      cout << "\nCreando carpeta N°" << i+1 << endl; 
   }
}

/*Bloque principal de ingreso de parámetros*/
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
      
   //int procesoPadre; // id del padre
   int nproc = comprobarConfig(CVal); // numero de procesos en int
   int pid; // identificador de procesos 
   int id; // identificador de procesos hijos
   crearCarpetas(nproc);
   int fd[nproc*2][2]; //array de pipes  
   for (int i =0;i<nproc*2;i++){
      pipe(fd[i]); //asignación de pipes
   }
   for (int i =0;i<nproc;i++){
      pid = fork(); //creacion de procesos
      if (pid == 0){
         close(fd[2*i][1]);   // procesos hijo escriben por fd[2*i+1][1]
         close(fd[2*i+1][0]); // leen por fd[2*i][0]
         id=i;
         cout << "\nnaci y soy el hijo: " << id << endl;
         break;
      }
      else{
         pid=1;
         close(fd[2*i][0]);  // procesos padre lee por fd[2*i][0] 
         close(fd[2*i+1][1]); // escribe por fd[2*i+1][1]
         sleep(1);

      }
   }
   if(pid!=0){
    cout << "\nsoy padre" << endl;
   }

    
   if(Eflag==1){
      /*Aca se elimina el contenido de la carpeta "Discos RAID" 
      si no hay contenido AVISAR!
      si tenia contenido AVISAR! 
      NO HAY EVal*/
   }
   if(gflag==1){
      //cout << "bbbbbbbbbbb" << id << endl;
      if(pid==1){ // si la ejecucion esta en el proceso padre  

         //cout << "cccccccc" << id << endl;
         int len;
         string nomArch = "./archivos originales/"  + gVal;
         char mensajep_h[TAMANO];
         FILE * archbin;
         archbin = fopen(nomArch.c_str(),"rb");
         if(archbin!=NULL){ //si el archivo existe
            len=largoArchivo(nomArch);
            vector<string> bloques=cortaBytes(nomArch,len); 
            //copiar en la mitad de los discos el bloque y en la otra mitad el espejo
            cout << "Soy el padre y hay " << bloques.size() << " bloques\n";
            for(int i=0; i<bloques.size();i++){ // mientras no llegue al final del bloque
               int j = i % (nproc/2); // el contador j almacenará el modulo la mitad del número de procesos para seleccionar los originales y las copias por separado
               //cout << "contenido blooooooooooooooooques: " << bloques.at(i) << endl;
               stringstream index;
               string ident;
               index << "" << i;
               ident=index.str();
               
               // envia el índice del bloque
               strcpy(mensajep_h,ident.c_str());
               write(fd[2*j][1],mensajep_h,TAMANO); //escribe en el pipe el mensaje (string de 1024)
               write(fd[2*j+nproc][1],mensajep_h,TAMANO);  
               
               // envía el bloque.
               strcpy(mensajep_h,bloques[i].c_str());
               write(fd[2*j][1],mensajep_h,TAMANO);
               write(fd[2*j+nproc][1],mensajep_h,TAMANO); 
            }

            // enviar mensaje a los hijos para que se cierren.
            for(int k=0;k<nproc;k++){
              char ultimatum[TAMANO];
              strcpy(ultimatum,"-1");
              write(fd[2*k][1],ultimatum,TAMANO);
            }    

            // después de mandar a los hijos a acostar, esperar un ratito para que el padre los vea que se metan a la camita y todo eso
            sleep(1);        

            //cout << "\nEl archivo existe!" << endl; //se divide en subpartes 
         }
         else { 
            //cout << "\nEl archivo no existe\n"; // no hace nada 
            exit(0);
          }

          cout << "soy padre y ya no me queda ni una hueá más por hacer\n";
      }
      else{ //soy hijo 
        
          while(true){
              char mensajeh_p[TAMANO];
              int identif; 
              
              read(fd[2*id][0],mensajeh_p,TAMANO);
              identif = atoi(mensajeh_p); 

            // entonces si identif es -1, quiere decir que no hay más mensajes y el hijo se tiene que ir a dormir :D jajaj siii
              if(identif==-1){
                //cout << "Soy hijo " << id << " y me fui a acostar\n"; 
                break; // te voy a hacer caso.
              }
              
              //cout << "\nsoy hijo " << id << "lei id:" <<mensajeh_p <<endl;
              read(fd[2*id][0],mensajeh_p,TAMANO); 
              //cout << "\nsoy hijo " << id << "lei :" <<mensajeh_p <<endl;
                  
              stringstream block;
              char direccion [25];
              if(identif < 10) 
                block << "file_0" << identif;
              else 
                block << "file_" << identif;

              // :yap, entonces... el padre envía bloque1, por lo tanto el hijo 1 recibe "1" y por ende crea "file01" 
              // y así, sustantivamente yeeeeeep. Probemos?
              // esos archivos eran de antes, se nos olvidó borrarlos aaaaaaaaaaaaah 
              //FUNCIOOOOOOOOOOOOOOOOOOOONAAAAAAAAAAAAAAA!!!!!!!!!!!!!! :D
                          
              sprintf(direccion, "Discos Raid/Disco_%d/%s", id+1, block.str().c_str()); 
              //cout << "aqui estoy: " << direccion << endl;
              //chdir(direccion);
              FILE * bla;
              bla = fopen(direccion, "wb");
              if(bla==NULL){
                //cout << "\nNo se creó el fichero";
                exit(0);
              }
              //cout << "\nSe creo el fichero exitosamente" << endl; 
              fwrite(mensajeh_p,1,TAMANO, bla);
              fclose(bla);
          }//al momento de crear el archivo le debo concatenar el identif para diferenciar los archivos creados 
      }
      /*si el archivo no esta en "originales", entonces AVISAR!
      si el archivo ya existe, AVISAR! y no hacer nada
      si la operacion se hace correctamente, se hace y AVISAR!*/ // pulenta
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
