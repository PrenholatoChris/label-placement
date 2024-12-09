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

    // string conflFile = "pfclp/h2_w24_58_all.confl";
    string conflFile = "pfclp/" + file + ".confl";
    ifstream f(conflFile);

    ofstream file_stream;
    string lpFile = ("lps/" + file + ".lp");



    // Check if the file is successfully opened
    if (!f.is_open()) {
        cerr << "Error opening the file!";
    }
    string s;
    getline(f, s);
    int N = stoi(s);
    getline(f, s);
    int P = stoi(s);

    s = "N: " + to_string(N) + "\nP: " + to_string(P);
    cout << s << endl;


    string x = "x";
    string z = "z";

    string out = "Maximize\n";
    out = out + " obj: ";
    string temp = "";
    vector<vector<float>> w(N, vector<float>(P, 0)); // Preenchida com 0
    vector<vector<vector<vector<int>>>> conflicts(N, vector<vector<vector<int>>>(P, vector<vector<int>>(N, vector<int>(P, 0)))); // Preenchida com 0

    // For each point (N)
    for (int i = 0; i < N; i++) {
        // For each label (P)
        for (int k = 0; k < P; k++) {
            getline(f, s);
            int J = stoi(s);
            //cout << "Label " << k + 1 << " tem " << J << " conflitos." << endl;
            getline(f, s);
            stringstream ss(s); // stringstream to read the pairs
            // For each conflicts (J)
            for (int j = 0; j < J; j++) {
                int id;
                float distance;
                // read each pair (id, distance)
                ss >> id >> distance;
                w[id / P][id % P] += distance;
                // Define the conflict between xi,j and xt,u
                conflicts[i][k][id / P][id % P] = 1;
                conflicts[id / P][id % P][i][k] = 1;
                // cout << "id = " << id << ", Distance = " << distance << endl;
            }
            // Ignore this line
            getline(f, s);
        }
    }

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < P; j++) {
            // cout << "matriz[" << i << "][" << j << "] = " << w[i][j] << endl;
            temp = temp + to_string(w[i][j]) + " " + x + to_string(i) + to_string(j) + "\n+ ";

        }
    }

    // Remove the last " + " in the temp string
    temp.pop_back();
    temp.pop_back();
    temp.pop_back();

    for (int i = 0; i < N; i++) {
        temp = temp + " - " + z + to_string(i);
    }


    out = out + temp + "\nSubject To\n";
    temp = "";


    for (int i = 0; i < N; i++) {
        temp = temp + " c" + to_string(i) + ": ";
        for (int j = 0; j < P; j++) {
            temp = temp + x + to_string(i) + to_string(j) + " + ";
        }
        temp.pop_back();
        temp.pop_back();
        temp.pop_back();
        temp = temp + " = 1\n";
    }

    int constraint_id = N;
    out = out + temp;
    temp = "";
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < P; j++) {
            for (int t = 0; t < N; t++) {
                for (int u = 0; u < P; u++) {
                    if (conflicts[i][j][t][u] == 1 && i != t) {
                        temp = temp + " c" + to_string(constraint_id) + ": " + x + to_string(i) + to_string(j) + " + " + x + to_string(t) + to_string(u) + " - " + z + to_string(i) + " <= 1\n";
                        constraint_id += 1;
                    }
                }
            }

        }
    }
    out = out + temp;

    temp = "Binary\n";
    for (int i = 0; i < N; i++){
        for (int j = 0; j < P; j++){
            temp = temp + " x" + to_string(i) + to_string(j) + "\n";
        }
            temp = temp + " z" + to_string(i)+ "\n";
    }
    
    out = out + temp + "End";

    
    file_stream.open(lpFile);
    file_stream << out;
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













int main() {
    //long long currentTimeMs = getSystemTimeMsec();
    //forcing the slow to count the time
    // for (int i = 0; i < 10000; i++){
    //     for (int j = 0; j < 100000; j++){
    //         cout << "";
    //     }
    // }





    // write the LP file with file
    string file = "h2_w24_505_all";
    writeLpFile(file);


    // Load the LP file
    //  const char* file = "lp.lp";
     //Solve LP file
     //solveLpFile(file);

    

    




    //long long afterTime = getSystemTimeMsec();


    //std::cout << "\n\npreviousTimeMs: " << currentTimeMs << std::endl;
    //std::cout << "currentTimeMs: " << afterTime << std::endl;

    //std::cout << "total seconds: " << (afterTime - currentTimeMs) / 1000 << "seconds" << std::endl;

    getchar();  // wait for keyboard input
}


