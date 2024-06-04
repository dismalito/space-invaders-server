#include "_validations.h"

/*Funcion encagarda de validar que una cadena de caracteres contiene solo valores numericos.*/
int es_numero(const char* cadena)
{

    /*verifico que la cadena pueda convertirse a entero*/
    if(atoi(cadena) == 0)
        return 0; //error!!!

    /* Verificamos que todos los caracteres sean un numero */
    while(cadena && *cadena) //mientras la cadena tenga valores me voy moviendo en la cadena referencia por memoria (puntero)
    {
    	if(*cadena >= '0' && *cadena <= '9')
			cadena++;
		else
			return 0;//error!!!
    }

    return 1; //sin error!!!!!!
}

/*Funcion encagarda de validar que una cadena de caracteres contiene solo valores numericos OPCION 2.*/
int validaNumero(char*string)
{
        char *aux=string;aux+=strlen(string)-1;*aux='\0';//ELIMINO EL SALTO DE LINEA '\n'
        regex_t retmp;
        regmatch_t mtmp;
        regcomp(&retmp,"^[0-9]$",REG_EXTENDED);
        if(!regexec(&retmp,string, (size_t) 1, &mtmp, 0))
                return 0;
        return -1;
}

/* Funcion encagarda de validar que un path ingresado sea valido, y si existe y tenga permisos (DIRECTORIO) de LECTURA */
int validaArchPermExit (const char* path)
{
	struct stat datosFichero;
	int salida = 0;
	//lstat permite obtener información de un fichero: propietario, tamaño, permisos, tipo de fichero, etc
	if (lstat(path, &datosFichero) != -1)
	{
		//si el usuario que ejecuta es OWN igual verifica
		if((long) datosFichero.st_uid == (long) getuid())
		{
			if(datosFichero.st_mode & S_IRUSR)//tiene que tener permisos de lectura
				salida = 1;
		}
		else //sino es el own se tendra que fijar que permisos tiene el "OTHER"
		{
			if(datosFichero.st_mode & S_IROTH)//tiene que tener permisos de lectura
				salida = 1;
		}
	}
	return salida;
}

/*Valida si un archivo tiene "algo" sino es vacio*/
int archVacio(const char* path)
{
	struct stat dat;
	int salida = 0;

	if(stat(path,&dat) == -1)
		return(-1);

	if(dat.st_size > 0)
		salida = 1;

	return salida;
}

/*Se obtiene la cantidad de líneas del archivo*/
int getCantLineas (char * NOM_ARCHIVO)
{
	int nLineas = 0;
    FILE *archivo;
    char string[200];
    archivo = fopen(NOM_ARCHIVO, "r");
    while(fgets(string,200,archivo))
    	nLineas++;
    fclose(archivo);
    return nLineas;
}

/*valida si una cadena cumple con un patron determinado que se pasa por parametro*/
int validarLinea(char *string, const char* expresion)
{
	regex_t retmp;
    regmatch_t mtmp;
    int res;
    res = regcomp(&retmp, expresion ,REG_EXTENDED);
    if(res!=0)
       printf("Error al compilar la RegExp (Resultado: %d)\n",res);

    res = regexec(&retmp, string, MAX_MATCHES, &mtmp, 0);//si es 0 el patron esta en la cadena

    regfree(&retmp);

    return res;
}

/*Funcion encagarda de obtener los parametros mediante substring*/
char* getParamString (char* linea)
{
	 int ch = '=';
	 size_t len;
	 char *pdest;
	 char *inpfile = NULL;
	 //buscara la ultima ocurrencia del caracter /
	 pdest = strrchr(linea, ch);
	 //si el caracter separador no esta en la cadena
	 if(pdest == NULL)
		 inpfile = "";//NO TIENE EXTESION
	 else
	 {
		 pdest++; //Apunto al proximo caracter luego de la ultima /.
		 len = strlen(pdest);
		 inpfile = (char*)malloc(len+1);// Crea espacio dinamicamente.
		 strncpy(inpfile, pdest, len+1);
	 }
	 return inpfile;
}

/*Funcion que valida el formato del archivo*/
int validaConexiones(char *linea)
{
	int conex = 0, salida = 0;
		const char* parametroName = "^conexiones=-?[0-9]*$";//que contenga vidas= y que lo que sigue sea un numero pos nat

		if(validarLinea(linea,parametroName)==0)
			{
			conex = atoi(getParamString(linea));
				if (conex <= 0)
					printf("ERROR: El parámetro ingresado para las conexiones debe ser mayor a 0.\n");
				else
					salida = conex;
			}
			else
				printf("ERROR: Formato erroneo en linea de configuracion de conexiones iniciales.\n");

		return salida;
}

/*Funcion que valida el formato del archivo*/
int validaVidas(char *linea)
{
	int vidas = 0, salida = 0;
	const char* parametroName = "^vidas=-?[0-9]*$";//que contenga vidas= y que lo que sigue sea un numero pos nat

	if(validarLinea(linea,parametroName)==0)
		{
		vidas = atoi(getParamString(linea));
			if (vidas <= 0)
				printf("ERROR: El parámetro ingresado para las vidas debe ser mayor a 0.\n");
			else
				salida = vidas;
		}
		else
			printf("ERROR: Formato erroneo en linea de configuracion de vidas.\n");

	return salida;
}

/*Funcion que valida el formato del archivo*/
int validaPuerto(char *linea)
{
	int puerto = 0, salida = 0;
	const char* parametroName = "^puerto=[0-9][0-9][0-9][0-9][0-9]$";//que contenga puerto= y que lo que sigue sean 5 numeros

	if(validarLinea(linea,parametroName)==0)
	{
		puerto = atoi(getParamString(linea));
		if (puerto <= 49152 || puerto >= 65535)
			printf("ERROR: El parámetro ingresado debe ser un puerto del rango 49152-65535.\n");
		else
			salida = puerto;
	}
	else
		printf("ERROR: Formato erroneo en linea de configuracion de puerto.\n");

	return salida;
}

/*valida la solo existencia de un archivo*/
int existArch(const char* path)
{
struct stat datosFichero;
	int salida = 0;

	//lstat permite obtener información de un fichero: propietario, tamaño, permisos, tipo de fichero, etc
	if (lstat(path, &datosFichero) != -1)
		salida = 1;//existe!

	return salida;
}

/*quita los espacios de una cadena*/
char *trim(char *str)
{
      char *ibuf = str, *obuf = str;
      int i = 0, cnt = 0;
      if (str)
      {
            for (ibuf = str; *ibuf && isspace(*ibuf); ++ibuf);
            if (str != ibuf)
                  memmove(str, ibuf, ibuf - str);

            while (*ibuf)
            {
                  if (isspace(*ibuf) && cnt)
                        ibuf++;
                  else
                  {
                        if (!isspace(*ibuf))
                              cnt = 0;
                        else
                        {
                              *ibuf = ' ';
                              cnt = 1;
                        }
                        obuf[i++] = *ibuf++;
                  }
            }
            obuf[i] = '\0';
            while (--i >= 0)
            {
                  if (!isspace(obuf[i]))
                        break;
            }
            obuf[++i] = '\0';
      }
      return str;
}

/*obtener un string dinamico a partir de un buffer estatico*/
char *getDinamicString (char staticstring [])
{
	size_t len = strlen(staticstring);//LO MALO DE TENER EL ARRAY DE LA ESTRUCTURA ESTATICO
	char * aux = (char*)malloc(len+1);
	strncpy(aux, staticstring, len+1);
	return aux;
}

/*valida el formato del archivo en general y carga los parametros en las variables*/
int validaFormatoYcarga (char * path)
{
	int TAMBUF = 512;
	int salida = 1,cantLineas = 1,numero = 0;
	FILE *archivo;
	char linea[TAMBUF];
	char* string;
	archivo = fopen(path,"r");

	while(fgets(linea,TAMBUF,archivo) && salida == 1)//si ya encontro algun formateo erroneo no entra
	{
		string = trim(getDinamicString(linea));//me aseguro que el dato se integro sin blancos, espacios, etc
		switch(cantLineas)//mientras las n primeras cumplan con el formato y tengan los datos bien si tiene mas lineas no importaria
		{
			case 1:
				numero = validaPuerto(string);
				if (numero != 0)
					Param.puerto = numero;
				else
					salida = 0;
				break;

			case 2:
				numero = validaVidas(string);
				if (numero != 0)
					Param.vidas = numero;
				else
					salida = 0;
				break;

			case 3:
				numero = validaConexiones(string);
				if (numero != 0)
					Param.conexiones = numero;
				else
					salida = 0;
				break;
			default:
				break;

		}
		cantLineas++;
	}
	fclose(archivo);
	return salida;
}

/*Imprime el mensaje generico para la salida erronea*/
void mensajeSalida()
{
	printf("Formato del archivo y lo que debe contener: \n");
	printf("puerto=numero Natural mayor a cero que indica el Puerto Escucha de CLientes que va del rango 49152-65535\n");
	printf("vidas=numero Natural mayor a cero que indica la cantidad de Vidas que tendran los jugadores al inciar una partida\n");
	printf("conexiones=numero Natural mayor a cero que indica la cantidad de Conexiones maximas permitidas INICIALES\n");
	printf("Para más ayuda revise el manual del juego\n");
}

/*FUNCION LLAMADORA DESDE EL MAIN*/
int validacionGeneral(int argc, char *argv[])
{
	int salida = 0;
	char* pathArchConfig = "./config/config_server.ini";
	/*******VALIDACIONES******/
	//controlo si la cantidad de parametros ingresados en la correcta
	if (argc > 1)//solo se debe ejecutar con el nombre del ejecutable
	{
		printf("ERROR: La cantidad de Parametros ingresados no es correcta.\n");
		printf("Uso: %s n", argv[0]);
		salida = EXIT_FAILURE;
	}

	//QUE EL ARCHIVO EXISTA Y TENGA PERMISOS DE LECTURA
	else if(validaArchPermExit(pathArchConfig) == 0)
	{
		printf("El archivo de configuracion no es un archivo valido o no existe o no cuenta con permisos de lectura. \nEl archivo debe encontrarse en el siguiente path: %s\n\n", pathArchConfig);
		mensajeSalida();
		salida = EXIT_FAILURE;
	}

	//Valido que el archivo tenga algo
	else if (archVacio(pathArchConfig) == 0)
	{
		printf("El archivo ingresado esta Vacio \n\n");
		mensajeSalida();
		salida = EXIT_FAILURE;
	}

	//valido formato la cantidad de lineas deben ser 3 y ademas estar en correcto formato
	else if (getCantLineas (pathArchConfig) != 3 || validaFormatoYcarga(pathArchConfig) == 0)
	{
		printf("El archivo de configuracion no cumple con las especificaciones del formato.\n\n");
		mensajeSalida();
		salida = EXIT_FAILURE;
	}
	/********FIN VALIDACIONES********/
	return salida;
}
