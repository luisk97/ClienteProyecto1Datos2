//
// Created by luisk on 02/10/18.
//

//#include "socketCliente.h"
#include <jsoncpp/json/json.h>
#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>
#include <string>
#include <pthread.h>
#include <unistd.h>
#include <arpa/inet.h>

using namespace std;

class socketCliente {
public:
    socketCliente(){};
    bool conectar(){
        descriptor = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
        if(descriptor < 0)
            return false;
        info.sin_family = AF_INET;
        info.sin_addr.s_addr = inet_addr("127.0.0.1");
        info.sin_port = htons(4050);
        memset(&info.sin_zero,0, sizeof(info.sin_zero));

        if((connect(descriptor,(sockaddr *)&info,(socklen_t) sizeof(info))) < 0){
            return false;
        }


        /*
        pthread_t hilo;
        pthread_create(&hilo,0,socketCliente::controlador,(void *)this);
        pthread_detach(hilo);
        */
        return true;
    };
    void setMensaje(const char *msn){
        //cout<<"bytes enviados "<< send(descriptor,msn,strlen(msn),0)<<endl;
        send(descriptor,msn,strlen(msn),0);
    };
    string recibirMensaje(){
        while(true) {
            string mensaje;
            while (1) {
                char buffer[10];
                int bytes = recv(this->descriptor, buffer, 10, 0);
                mensaje.append(buffer, bytes);

                if(bytes <= 0){
                    close(this->descriptor);
                }
                if (bytes < 10)
                    break;
            }
            return mensaje;

        }
        close(this->descriptor);
    };

private:
    int descriptor;
    sockaddr_in info;
    static void *controlador(void * obj){
        socketCliente *padre = (socketCliente *)obj;
        while(true) {
            string mensaje;
            while (1) {
                char buffer[10];
                int bytes = recv(padre->descriptor, buffer, 10, 0);
                mensaje.append(buffer, bytes);

                if (bytes < 10)
                    break;
            }
            cout<<"mensaje: ";
            cout<<mensaje<<endl;
            break;

        }
    };

};



/*
 * Esta es la clase MPointer la cual contiene un puntero generico y permitira manipular este puntero
 * de manera mas simple
 */
template<class T>
class MPointer{
private:
    int id = 0;
    socketCliente *conexion;
    T dato = NULL;
    //MPointerGC* gc1 = NULL;
    void Destructor(int idEliminar,int idAumentar){
        conexion = new socketCliente;
        if(!(conexion->conectar())) {
            cout << "ERROR! No se pudo conectar con el servidor" << endl;
        }else {
            //cout << "Conexion al servidor exitosa" << endl;

            string strIdElim = std::to_string(idEliminar);
            string strIdAum = std::to_string(idAumentar);

            Json::Value root;
            string text = "{ \"Solicitud\": \"liberarEspacio\", \"idElim\": " + strIdElim + ", \"idAument\": " + strIdAum +"}";

            Json::Reader reader;
            reader.parse(text, root);

            //Json::Value root = "{ \"Solicitud\": \"hello\", \"ID\": 147852}";
            Json::FastWriter fastWriter;
            std::string output = fastWriter.write(root);
            //cout << output << endl;
            //cout << "Solicitud: " << root["Solicitud"].asString() << endl;

            string strAux;
            //cin >> strAux;

            conexion->setMensaje(output.c_str());

            strAux = conexion->recibirMensaje();

            if(strAux == "Error"){
                cout<<"[ERROR] No se pudo no se pudo liberar el espacio"<<endl;
            }
        }
    };

public:
    void New(){
        conexion = new socketCliente;
        if(!(conexion->conectar())) {
            cout << "ERROR! No se pudo conectar con el servidor" << endl;
        }else {
            cout << "Conexion al servidor exitosa" << endl;

            int tam = sizeof(T);
            cout<<"tam: "<<tam<<endl;
            string strtam = std::to_string(14);

            Json::Value root;
            string text = "{ \"Solicitud\": \"recerbarEspacio\", \"tam\": " + strtam + "}";

            Json::Reader reader;
            reader.parse(text, root);

            //Json::Value root = "{ \"Solicitud\": \"hello\", \"ID\": 147852}";
            Json::FastWriter fastWriter;
            std::string output = fastWriter.write(root);
            //cout << output << endl;
            //cout << "Solicitud: " << root["Solicitud"].asString() << endl;

            string respuesta;
            //cin >> respuesta;

            conexion->setMensaje(output.c_str());

            respuesta = conexion->recibirMensaje();

            Json::Value msnRecibido;
            reader.parse(respuesta, msnRecibido);

            text = msnRecibido["Id"].asString();
            int id = atoi(text.c_str());

            if(id == 0){
                cout<<"[ERROR] no se pudo reservar el espacio en el servidor para el MPointer variable"<<endl;
            }else{
                this->id = id;
                cout<<"Exito al reservar el espacio"<<endl;
            }

            cout<<endl;
            cout<<endl;
        }

    };

    /*
     * Este es el destructor de la clase si funciona pero la cosa es que se llama solo y destruye la instancia
     * antes de lo esperado entoncs aqui queda para ver como se acopla o que con lo del MPointerGC
     */
    ~MPointer(){};

    //Sobrecarga del operador &
    T operator&() {
        conexion = new socketCliente;
        if(!(conexion->conectar())) {
            cout << "ERROR! No se pudo conectar con el servidor" << endl;
        }else {
            //cout << "Conexion al servidor exitosa" << endl;

            string strID = std::to_string(id);

            Json::Value root;
            string text = "{ \"Solicitud\": \"pedirDato\", \"id\": " + strID + "}";

            Json::Reader reader;
            reader.parse(text, root);

            //Json::Value root = "{ \"Solicitud\": \"hello\", \"ID\": 147852}";
            Json::FastWriter fastWriter;
            std::string output = fastWriter.write(root);
            //cout << output << endl;
            //cout << "Solicitud: " << root["Solicitud"].asString() << endl;

            string strAux;
            //cin >> strAux;

            conexion->setMensaje(output.c_str());

            strAux = conexion->recibirMensaje();


            Json::Value msnRecibido;
            reader.parse(strAux, msnRecibido);

            text = msnRecibido["Dato"].asString();
            if (instanceof<int>(dato)) {
                int num = atoi(text.c_str());
                return num;
            }else if (instanceof<float>(dato)) {
                float num = atof(text.c_str());
                return num;
            } else if (instanceof<double>(dato)) {
                double num = stof(text.c_str());
                return num;
            }else if(instanceof<char>(dato)) {
                int buf = atoi(text.c_str());
                return buf;
            }

        }
        return 0;
    }

    //Sobrecarga del operador = para asignar un dato al espacio de memoria a el que apunta dato
    void operator=(T dat) {
        conexion = new socketCliente;
        if(!(conexion->conectar())) {
            cout << "ERROR! No se pudo conectar con el servidor" << endl;
        }else {
            //cout << "Conexion al servidor exitosa" << endl;
            int numAux = (int) dat;
            string strID = std::to_string(id);
            string strDato = std::to_string(numAux);

            Json::Value root;
            string text = "{ \"Solicitud\": \"actualizarDato\", \"id\": " + strID + ",\"dato\":"+strDato+"}";

            Json::Reader reader;
            reader.parse(text, root);

            //Json::Value root = "{ \"Solicitud\": \"hello\", \"ID\": 147852}";
            Json::FastWriter fastWriter;
            std::string output = fastWriter.write(root);
            //cout << output << endl;
            //cout << "Solicitud: " << root["Solicitud"].asString() << endl;

            string strAux;

            conexion->setMensaje(output.c_str());

            strAux = conexion->recibirMensaje();

            if(strAux == "Error"){
                cout<<"[ERROR] No se pudo guardar el dato ingresado en el servidor"<<endl;
            }
        }

        //dato = dat;
    }

    //Sobrecarga del operador = para igualar dos instancias de MPointer     dynamic_cast<const T*>(dat) != nullptr
    void operator=(const MPointer mp) {
        if(mp.id == 0){
            cout<<"NO se puede igualar algo NULL"<<endl;
        }else {
            Destructor(this->id,mp.id);
            this->id = mp.id;
        }
    }

    void iniciarMemoriaServer(int tam){
        conexion = new socketCliente;
        if(!(conexion->conectar())) {
            cout << "ERROR! No se pudo conectar con el servidor" << endl;
        }else {
            cout << "Conexion al servidor exitosa" << endl;

            string strtam;
            if(tam < 12){
                strtam = to_string(12);
            }else {
                strtam = to_string(tam);
            }

            Json::Value root;
            string text = "{ \"Solicitud\": \"recerbarMemoria\", \"tam\": " + strtam + "}";

            Json::Reader reader;
            reader.parse(text, root);

            //Json::Value root = "{ \"Solicitud\": \"hello\", \"ID\": 147852}";
            Json::FastWriter fastWriter;
            std::string output = fastWriter.write(root);
            //cout << output << endl;
            //cout << "Solicitud: " << root["Solicitud"].asString() << endl;

            string strAux;
            //cin >> pausa;

            conexion->setMensaje(output.c_str());

            strAux = conexion->recibirMensaje();

            if(strAux == "Error"){
                cout<<"[ERROR] No se pudo reservar la memoria en el servidor"<<endl;
            }

            cout<<endl;
            cout<<endl;
        }
    }


    void liberarMemoriaServer(){
        conexion = new socketCliente;
        if(!(conexion->conectar())) {
            cout << "ERROR! No se pudo conectar con el servidor" << endl;
        }else {
            cout << "Conexion al servidor exitosa" << endl;

            Json::Value root;
            string text = "{ \"Solicitud\": \"liberarMemoria\"}";

            Json::Reader reader;
            reader.parse(text, root);

            //Json::Value root = "{ \"Solicitud\": \"hello\", \"ID\": 147852}";
            Json::FastWriter fastWriter;
            std::string output = fastWriter.write(root);
            //cout << output << endl;
            //cout << "Solicitud: " << root["Solicitud"].asString() << endl;

            string strAux;
            //cin >> pausa;

            conexion->setMensaje(output.c_str());

            strAux = conexion->recibirMensaje();

            if(strAux == "Error"){
                cout<<"[ERROR] No se pudo liberar la memoria en el servidor"<<endl;
            }else{
                cout<<"Se libero la memoria en el servidor exitosamente"<<endl;
            }
        }
    }


    template <typename Of, typename What>
    inline bool instanceof(const What &w)
    {
        return typeid(w) == typeid(Of);
    }
};


template<class T>
MPointer<T> operator *(const MPointer<T> mp){
    return mp;
}



//--------------------------------------------------------------------------------



//socketCliente *conexion;

int main() {

    MPointer<int> mp1;
    mp1.iniciarMemoriaServer(520);
    mp1.New();

    MPointer<char> mp2;
    mp2.New();

    MPointer<int> mp3;
    mp3.New();

    MPointer<int> mp4;
    mp4.New();

    mp1 = 32;

    mp2 = 'd';

    mp3 = 99;

    cout<<"EL valor de mp1 es "<<&mp3<<endl;

    int num = &mp1;

    char buf = &mp2;

    mp1 = mp3;

    cout<<"EL valor de num es "<<num<<endl;
    cout<<"EL valor de buf es "<<buf<<endl;
    cout<<"EL valor de mp1 es "<<&mp1<<endl;
    cout<<"EL valor de mp3 es "<<&mp3<<endl;

    mp2 = 'a';

    mp1.liberarMemoriaServer();

    return 0;
}