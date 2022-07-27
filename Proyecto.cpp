#include <iostream>
#include <string>

//Estructura para guardar las coordenadas de la DP
class Point2D{
    public:
        int x;
        int y;
};

int editDistance(std::string palabra1, std::string palabra2, int m, int n, int** edit);
Point2D* steps(std::string palabra1, std::string palabra2, int* len, int m, int n, int** edit);
void printSteps(std::string palabra1, std::string palabra2, int len, int** edit, Point2D* path);
void insertPoint(Point2D* path, int* len, int i, int j);
int min(int x, int y, int z);

int main(){
    std::string palabra1, palabra2;
    int distance;
    int len;
    Point2D* path;

    std::cout << "Ingrese la palabra 1: ";
    std::cin >> palabra1;
    std::cout << "Ingrese la palabra 2: ";
    std::cin >> palabra2;

    int m = palabra1.length();
    int n = palabra2.length();

    int** edit = new int*[m + 1];
    for(int i = 0; i < m + 1; i++)
        edit[i] = new int[n + 1];

    //Calculamos la distancia edit
    distance = editDistance(palabra1, palabra2, m, n, edit);
    //Encontramos uno de los caminos con la distancia edit
    path = steps(palabra1, palabra2, &len, m, n, edit);
    //Imprimimos las operaciones hechas
    printSteps(palabra1, palabra2, len, edit, path);

    std::cout << "La distancia edit es: " << distance << std::endl;
    
    //Borramos edit y path
    for(int i = 0; i < m; i++)
        delete[] edit[i];
    delete[] edit;
    delete[] path;

    return 0;
}

//Esta funcion calcula la distancia edit usando una DP
int editDistance(std::string palabra1, std::string palabra2, int m, int n, int** edit){
    int distance, insertion, deletion, replacement;

    for(int i = 0; i < n + 1; i++)
        edit[0][i] = i;

    for(int i = 1; i < m + 1; i++){
        edit[i][0] = i;
        
        for(int j = 1; j < n + 1; j++){
            //Insertar avanza en horizontal, borrar en vertical y sustituir en diagonal
            insertion = edit[i][j - 1] + 1;
            deletion = edit[i - 1][j] + 1;

            if(palabra1[i - 1] == palabra2[j - 1])
                replacement = edit[i - 1][j - 1];
            else
                replacement = edit[i - 1][j - 1] + 1;
            
            //Ponemos la operacion que haya hecho menos cambios
            edit[i][j] = min(insertion, deletion, replacement);
        }
    }
    
    //La distancia edit sera el resultado en la ultima celda
    distance = edit[m][n];
    return distance;
}

//Esta funcion regresa el caminito de las operaciones seguidas para llegar a la distancia edit
Point2D* steps(std::string palabra1, std::string palabra2, int* len, int m, int n, int** edit){
    int i = m;
    int j = n;
    int minim, start, end;
    *len = 1;

    //Creamos el arreglo donde vamos a guardar el camino
    Point2D* path = new Point2D[m + n + 1];
    path[0].x = m;
    path[0].y = n;

    while(i != 0 && j != 0){
        //Encontramos el minimo vecino a la izquierda, arriba o arriba a la izquierda mientras estos 3 existan
        minim = min(edit[i - 1][j - 1], edit[i][j - 1], edit[i - 1][j]);

        //Ponemos la coordenada del minimo en el camino y nos movemos
        if(minim == edit[i - 1][j - 1]){ //Si el minimo es arriba a la izquierda fue una sustitucion
            i--;
            j--;
            insertPoint(path, len, i, j);
        }
        else if(minim == edit[i][j - 1]){ //Si el minimo es arriba a la izquierda fue una insercion
            j--;
            insertPoint(path, len, i, j);
        }
        else{ //Si el minimo es arriba fue una eliminacion
            i--;
            insertPoint(path, len, i, j);
        }
    }
    
    //Si llegamos al borde izquierdo nada mas subimos hasta llegar a la esquina
    while(i != 0){
        i--;
        insertPoint(path, len, i, j);
    }
    
    //Si llegamos al borde superior nos movemos a la izquierda hasta llegar a la esquina
    while(j != 0){
        j--;
        insertPoint(path, len, i, j);
    }

    start = 0;
    end = *len - 1;

    //Revertimos el arreglo para iniciar en la esquina superior izquierda el camino
    while (start < end){
        Point2D temp = path[start];
        path[start] = path[end];
        path[end] = temp;
        start++;
        end--;
    }

    return path;
}

//Esta funcion auxiliar inserta una nueva coordenada en el camino. La creamos porque es una operacion muy usada
void insertPoint(Point2D* path, int* len, int i, int j){
    path[*len].x = i;
    path[*len].y = j;
    (*len)++;
}

//Esta funcion utiliza el caminito obtenida en la anterior para reconstruir las operaciones hechas
void printSteps(std::string palabra1, std::string palabra2, int len, int** edit, Point2D* path){
    std::string palabra = palabra1;
    std::string a;
    std::cout << "\nPalabra:\t\t" << palabra << std::endl;
    /*Estas cosas cuentan el numero de inserciones y eliminaciones que hemos hecho. Son importantes porque
    mueven lo que debemos de considerar como la posicion actual de la palabra para imprimir las letras correctas */
    int deletion = 0, insertion = 0; 
    int size;

    for(int i = 1; i < len; i++){
        //Si nos movimos en diagonal y la letra cambio, fue una sustitucion. Si no cambio, no hicimos nada
        if(path[i].x - path[i - 1].x == path[i].y - path[i - 1].y){
            if(edit[path[i].x][path[i].y] != edit[path[i - 1].x][path[i - 1].y]){
                std::cout << "Sustituir " << palabra[path[i].x - 1 - deletion + insertion] << " por " << palabra2[path[i].y - 1] << ":\t";
                palabra[path[i].x - 1 - deletion + insertion] = palabra2[path[i].y - 1];
            }
            else{
                continue;
            }
        }
        //Si nos movimos en vertical, fue una eliminacion, asi que borramos el caracter correspondiente
        else if(path[i].x - path[i - 1].x == 1){
            std::cout << "Eliminar " << palabra[i - 1 - deletion] << ":\t\t";
            palabra.erase(palabra.begin() + i - 1 - deletion);
            deletion++;
        }
        //En otro caso tuvimos que movernos en horizontal (insercion), asi que ponemos el caracter correspondiente
        else{
            a = palabra2[path[i].y - 1];
            std::cout << "Insertar " << a << ":\t\t";
            size = palabra.size();
            palabra.resize(size + 1);
            palabra.insert(i - 1 - deletion, a);
            insertion++;
        }

    //Imprimimos como va la palabra actualmente
    std::cout << palabra << std::endl;
    }

    return;
}

//Esta funcion encuentra el minimo de 3 numeros
int min(int x, int y, int z){
    if(x < y && x < z)
        return x;
    if(y < z)
        return y;
    return z;
}