#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <algorithm>

int dodane=0;

struct Kamien {
    std::string typ;
    int waga;
    int ilosc;
};

struct Taczka {
    int czasPrzybycia;
    int numer;
    Kamien kamien;
    int czasRozladunku;
};

struct Robot {
    int id;
    std::queue<Taczka> kolejkaTaczek;
    int pozostalyCzas;
};

bool sprawdzRoboty(const std::vector<Robot>& roboty) {
    for (const Robot& robot : roboty) {
        if (!robot.kolejkaTaczek.empty()) {
            return true;
        }
    }
    return false;
}

const Robot* znajdzRobota(const std::vector<Robot>& roboty) {
    const Robot* robotMIN = &roboty.front();

    for (const Robot& robot : roboty) {
        std::queue<Taczka> temp = robot.kolejkaTaczek;
        int sumaElementow = 0;

        while (!temp.empty()) {
            sumaElementow += temp.front().czasRozladunku;
            temp.pop();
        }
        std::queue<Taczka> tempQueueMin = robotMIN->kolejkaTaczek;
        int sumaElementowMin = 0;
        while (!tempQueueMin.empty()) {
            sumaElementowMin += tempQueueMin.front().czasRozladunku;
            tempQueueMin.pop();
        }
        
        if (sumaElementow < sumaElementowMin) {
            robotMIN = &robot;
        }
    }

    return robotMIN;
}



void FCFS(std::vector<Taczka> &taczki, std::vector<Robot>& roboty){

    using namespace std;

    int lacznaLiczbaTaczek = taczki.size();
    int aktualnyCzas = 0;
    int usuniete = 0;

    while (!taczki.empty() || sprawdzRoboty(roboty)) {
        cout << "Moment: " << aktualnyCzas << endl;

        while (!taczki.empty() && taczki.front().czasPrzybycia == aktualnyCzas) {
            cout << " Taczka przyjechała <" << taczki.front().numer << " " << taczki.front().kamien.typ << " "
                 << taczki.front().kamien.waga << " " << taczki.front().kamien.ilosc << " ["
                 << taczki.front().czasRozladunku << "]>" << endl;

            Robot* robotZMinimalnaKolejka = const_cast<Robot*>(znajdzRobota(roboty));
            robotZMinimalnaKolejka->kolejkaTaczek.push(taczki.front());
            taczki.erase(taczki.begin());
        }

        for (Robot& robot : roboty) {
            if (!robot.kolejkaTaczek.empty()) {
                cout << "                [" << robot.kolejkaTaczek.front().kamien.typ << " "
                     << robot.kolejkaTaczek.front().czasRozladunku << "]";

                robot.kolejkaTaczek.front().czasRozladunku--;

                if (robot.kolejkaTaczek.front().czasRozladunku == 0) {
                    robot.kolejkaTaczek.pop();
                    usuniete++;
                }
            }
            else {
                cout << "                []";
            }
        }
        cout << "" << endl;

        aktualnyCzas++;
    }
}

void RR(std::vector<Taczka>& taczki, std::vector<Robot>& roboty, int timeQuantum) {
 int aktualnyCzas = 0;
 int usuniete =0;
 std::queue<Taczka> kolejka;
 

while (!taczki.empty() || sprawdzRoboty(roboty)) {

for (Robot& robot : roboty) {
    //awaryjne zakonczenie
    //std::cout<<kolejka.front().kamien.typ;
    if (taczki.empty() && kolejka.empty() && robot.kolejkaTaczek.front().czasRozladunku==0) {
        return;
    
    }
}

if(dodane==usuniete){
    return;
}


    for (Robot& robot : roboty) {
        
    
            if(robot.pozostalyCzas==0){
            
            if(!robot.kolejkaTaczek.empty()){

            if(robot.kolejkaTaczek.front().czasRozladunku==0){
            }
            else{
                
                kolejka.push(robot.kolejkaTaczek.front());
                robot.kolejkaTaczek.pop();
            }


            }
            }
        }

    std::cout << "Moment: " << aktualnyCzas << std::endl;


    while (!taczki.empty() && taczki.front().czasPrzybycia == aktualnyCzas) {
        
        std::cout << " Taczka dotarła <" << taczki.front().numer << " " << taczki.front().kamien.typ << " "
                 << taczki.front().kamien.waga << " " << taczki.front().kamien.ilosc << " ["
                 << taczki.front().czasRozladunku << "]>" << std::endl;

        kolejka.push(taczki.front());
        taczki.erase(taczki.begin());
    
    }


    //zgodnie z Round-Robin
        for (Robot& robot : roboty) {
        
    
            if(robot.pozostalyCzas==0){
            
            if(!robot.kolejkaTaczek.empty()){

            if(robot.kolejkaTaczek.front().czasRozladunku==0){
                
                
                robot.kolejkaTaczek.pop();
                if(!kolejka.empty()){
                robot.kolejkaTaczek.push(kolejka.front());
                kolejka.pop();
                robot.pozostalyCzas=timeQuantum;
                }
            }
            else{

                if(!kolejka.empty()){
                robot.kolejkaTaczek.push(kolejka.front());
                kolejka.pop();
                robot.pozostalyCzas=timeQuantum;
                }

            }


            }
            
            else{
            if(!kolejka.empty()){
                robot.kolejkaTaczek.push(kolejka.front());
                kolejka.pop();
                robot.pozostalyCzas=timeQuantum;
                }

            }
            }
        }
    

    for (Robot& robot : roboty) {
        if (!robot.kolejkaTaczek.empty()) {
            std::cout << "                [" << robot.kolejkaTaczek.front().kamien.typ << " "
                     << robot.kolejkaTaczek.front().czasRozladunku << "]";


            if(robot.pozostalyCzas>0&&robot.kolejkaTaczek.front().czasRozladunku>0){
            robot.kolejkaTaczek.front().czasRozladunku--;
            robot.pozostalyCzas--;
            }

        }
        else {
            std::cout << "                []";
        }
    }
    std::cout << "" << std::endl;

   
    aktualnyCzas++;

}
}

int main(int argc, char* argv[]) {
    using namespace std;



    if (argc != 5) {
        cerr << "Użycie programu: " << argv[0] << " <liczba robotow> <czas kwantu> <plik_wej> <RR=1 FCFS=2>" << endl;
        return 1;
    }

    int timeQuantum = stoi(argv[2]);
    int algorytm = stoi(argv[4]);
   
        ifstream plikDanych;
  
        plikDanych.open(argv[3]);
        if (!plikDanych.is_open()) {
        cerr << "Nie można otworzyć pliku z danymi!" << endl;
        return 1;
    }
    


    vector<Taczka> taczki;  
    vector<Robot> roboty(stoi(argv[1]));
    
    for (Robot& robot : roboty) {
    robot.pozostalyCzas=0;
    }
    

    int minuta, numerTaczki, waga, ilosc;
    string nazwaKamienia;

    while (plikDanych >> minuta) {

        Taczka taczka;
        taczka.czasPrzybycia = minuta;

        char znak1, znak2;
        plikDanych.get(znak1);
        if (znak1 == '\n') {
            continue;  
        }
        else if (znak1 == ' ') {
            plikDanych.get(znak2);
            if (znak2 == '\n') {
                continue;  
            }
            plikDanych.putback(znak2);
            plikDanych.putback(znak1);
        }

        while (plikDanych >> numerTaczki >> nazwaKamienia >> waga >> ilosc) {
            taczka.numer = numerTaczki;
            taczka.kamien.typ = nazwaKamienia;
            taczka.kamien.waga = waga;
            taczka.kamien.ilosc = ilosc;
            taczka.czasRozladunku = waga * ilosc;  
            taczki.push_back(taczka);
            
            char nastepnyZnak;
            plikDanych.get(nastepnyZnak);
            if (nastepnyZnak == '\n') {
                break;  
            }
            plikDanych.putback(nastepnyZnak);
        }

    }

    plikDanych.close();

    for (const Taczka& taczka : taczki) {
        cout << "Numer taczki: " << taczka.numer << " Kamień: " << taczka.kamien.typ << " Waga: " << taczka.kamien.waga
             << " Ilość: " << taczka.kamien.ilosc << " Czas przybycia: " << taczka.czasPrzybycia
             << " Czas rozładunku: " << taczka.czasRozladunku << endl;
             dodane++;
    }
    
   switch (algorytm) {
        case 1:
            cout<<"Round Robin algorytm z kwantem czasu: "<<timeQuantum<<endl;
            RR(taczki, roboty, timeQuantum);
            break;
        case 2:
            cout<<"FCFS algorytm"<<endl;
            FCFS(taczki, roboty);
            break;
        default:
            cerr<<"Zly numer algorytmu";
            return 1;
    }

    return 0;
}