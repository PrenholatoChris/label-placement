#include <iostream>
#include <fstream>     
#include <sstream>  
#include <vector>
#include <string>
#include <cstdio>




// #include <ilcplex/cplex.h>
// #include <ilcplex/ilocplex.h>


using namespace std;

//#if !defined(_WIN32) && !defined(_WIN64) // Linux - Unix
//#  include <sys/time.h>
//typedef timeval sys_time_t;
//inline void system_time(sys_time_t* t) {
//    gettimeofday(t, NULL);
//}
//inline long long time_to_msec(const sys_time_t& t) {
//    return t.tv_sec * 1000LL + t.tv_usec / 1000;
//}
//#else // Windows and MinGW
//#  include <sys/timeb.h>
//typedef _timeb sys_time_t;
//inline void system_time(sys_time_t* t) { _ftime(t); }
//inline long long time_to_msec(const sys_time_t& t) {
//    return t.time * 1000LL + t.millitm;
//}
//#endif

//long long getSystemTimeMsec() {
//    sys_time_t t;
//    system_time(&t);
//    return time_to_msec(t);
//}

void writeLpFile(string file) {

    string conflFile = "pfclp/" + file + ".confl";
    ifstream f(conflFile);

    if (!f.is_open()) {
        cerr << "Error opening the file!";
        return;
    }

    string lpFile = ("lps/" + file + ".lp");

    string s;
    getline(f, s);
    int N = stoi(s);
    getline(f, s);
    int P = stoi(s);

    ofstream file_stream(lpFile, ios::out | ios::trunc); // Abre para come�ar do zero
    if (!file_stream.is_open()) {
        cerr << "Error opening the output file!";
        return;
    }

    cout << "Reading File: " << conflFile << "\nN: " << N << "\nP: " << P << endl;
    file_stream << "Maximize\n obj: ";
    file_stream.close();

    vector<vector<float>> w(N, vector<float>(P, 0));
    vector<vector<vector<vector<int>>>> conflicts(N, vector<vector<vector<int>>>(P, vector<vector<int>>(N, vector<int>(P, 0))));

    for (int i = 0; i < N; i++) {
        for (int k = 0; k < P; k++) {
            getline(f, s);
            int J = stoi(s);
            getline(f, s);
            stringstream ss(s);

            for (int j = 0; j < J; j++) {
                int id;
                float distance;
                ss >> id >> distance;
                w[id / P][id % P] += distance;
                conflicts[i][k][id / P][id % P] = 1;
                conflicts[id / P][id % P][i][k] = 1;
            }
            getline(f, s);
        }
    }

    int counter = 0; // Contador para monitorar o progresso

    // Maximization terms
    file_stream.open(lpFile, ios::out | ios::app); // Reabre o arquivo em modo append
    if (!file_stream.is_open()) {
        cerr << "Error opening the output file!";
        return;
    }

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < P; j++) {
            file_stream << w[i][j] << " x" << i << j << " + ";
            if (++counter % 100000 == 0) {
                file_stream.close();
                file_stream.open(lpFile, ios::out | ios::app);
            }
        }
    }

    file_stream.seekp(-3, ios_base::cur); // Remove o �ltimo " + "
    for (int i = 0; i < N; i++) {
        file_stream << " - z" << i;
    }
    file_stream << "\nSubject To\n";
    file_stream.close();

    // Constraints
    counter = 0;
    file_stream.open(lpFile, ios::out | ios::app);
    if (!file_stream.is_open()) {
        cerr << "Error opening the output file!";
        return;
    }

    for (int i = 0; i < N; i++) {
        file_stream << " c" << i << ": ";
        for (int j = 0; j < P; j++) {
            file_stream << "x" << i << j << " + ";
            if (++counter % 100000 == 0) {
                file_stream.close();
                file_stream.open(lpFile, ios::out | ios::app);
            }
        }
        file_stream.seekp(-3, ios_base::cur); // Remove o �ltimo " + "
        file_stream << " = 1\n";
    }

    int constraint_id = N;

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < P; j++) {
            for (int t = 0; t < N; t++) {
                for (int u = 0; u < P; u++) {
                    if (conflicts[i][j][t][u] == 1 && i != t) {
                        file_stream << " c" << constraint_id++ << ": "
                            << "x" << i << j << " + "
                            << "x" << t << u << " - "
                            << "z" << i << " <= 1\n";
                        if (++counter % 100000 == 0) {
                            file_stream.close();
                            file_stream.open(lpFile, ios::out | ios::app);
                        }
                    }
                }
            }
        }
    }
    file_stream << "Binary\n";

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < P; j++) {
            file_stream << " x" << i << j << "\n";
            if (++counter % 100000 == 0) {
                file_stream.close();
                file_stream.open(lpFile, ios::out | ios::app);
            }
        }
        file_stream << " z" << i << "\n";
    }

    file_stream << "End";
    file_stream.close();

    cout << "Arquivo salvo" << endl;
}



int solveLpFile(const char *lpFile){
    // // Create the CPLEX enviroment
    // IloEnv env;

    // // Create the model and solver
    // IloModel model(env);
    // IloCplex cplex(env);

    // IloObjective   obj;
    // IloNumVarArray var(env);
    // IloRangeArray  rng(env);

    // cout << "created" << endl;
    // // Import the model of LP file
    // cplex.importModel(model, lpFile, obj, var, rng);
    // cout << "created" << endl;

    ////// Use the model loaded in solver
    //cplex.extract(model);

    //if (!cplex.solve()) {
    //    env.error() << "Failed to optimize LP" << endl;
    //    throw(-1);
    //}

    //IloNumArray vals(env);
    //cplex.getValues(vals, var);
    //env.out() << "Solution status = " << cplex.getStatus() << endl;
    //env.out() << "Solution value  = " << cplex.getObjValue() << endl;
    //env.out() << "Solution vector = " << vals << endl;

    //// Finaliza o ambiente
    //env.end();

    return 0;
}













int main_antiga(int argc, char* argv[]) {
    //long long currentTimeMs = getSystemTimeMsec();



    // write the LP file with .confl file
     string fileName = "h2_w32_58_all";
    //string fileName = argv[1];
    writeLpFile(fileName);


    // Load the LP file
    //  const char* file = "lp.lp";
    //Solve LP file
    //solveLpFile(file);

    //long long afterTime = getSystemTimeMsec();


    //std::cout << "Total time: " << (afterTime - currentTimeMs) / 1000 << "seconds" << std::endl;

    getchar();  // wait for keyboard input
    return 0;
}