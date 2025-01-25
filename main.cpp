#include <iostream>
#include <fstream>     
#include <sstream>  
#include <vector>
#include <string>
#include <cstdio>




 #include <ilcplex/cplex.h>
 #include <ilcplex/ilocplex.h>


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

    string datFile = "pfclp/" + file + ".dat";

    cout << datFile << endl;

    ifstream f(datFile);

    if (!f.is_open()) {
        cerr << "Error opening the input file!";
        return;
    }

    string lpFile = "lps/" + file + ".lp";
    cout << lpFile<< endl;

    string s;
    getline(f, s);//primeira linha em branco
    getline(f, s);
    int N = stoi(s);
    getline(f, s);
    int P = stoi(s);

    // Tenta abrir o arquivo
    ofstream file_stream(lpFile, ios::out | ios::trunc); // Abre para escrever do zero
    if (!file_stream.is_open()) {
        cerr << "Error opening the output file: " << lpFile << endl;
        return;
    }
    cout << "Reading File: " << datFile << "\nN: " << N << "\nP: " << P << endl;
    file_stream << "Maximize\n obj:";
    file_stream.close();

    vector<vector<vector<vector<int>>>> conflicts(N, vector<vector<vector<int>>>(P, vector<vector<int>>(N, vector<int>(P, 0))));

    for (int i = 0; i < N; i++) {
        for (int k = 0; k < P; k++) {
            getline(f, s);
            int J = stoi(s);
            getline(f, s);
            stringstream ss(s);
            for (int j = 0; j < J; j++) {
                int id;
                ss >> id;
                id -= 1;
                conflicts[i][k][id / P][id % P] = 1;
                conflicts[id / P][id % P][i][k] = 1;
            }
        }
    }

    int counter = 0; // Contador para monitorar o progresso

    // Maximization terms
    file_stream.open(lpFile, ios::out | ios::app); // Reabre o arquivo em modo append
    if (!file_stream.is_open()) {
        cerr << "Error opening the output file!";
        return;
    }
    
    
    //file_stream.seekp(-3, ios_base::cur); // Remove o �ltimo " + "
    for (int i = 0; i < N; i++) {
        file_stream << " - z" << i;
    }
    file_stream << " + " + to_string(N);

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
            if (j != P - 1) {
                file_stream << "x" << i << j << " + ";
            }
            else {
                file_stream << "x" << i << j;
            }
            if (++counter % 100000 == 0) {
                file_stream.close();
                file_stream.open(lpFile, ios::out | ios::app);
            }
        }
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



#include <ilcplex/ilocplex.h>
#include <iostream>
#include <string>
#include <exception>


#include "gurobi_c++.h"
#include <cmath>

int solveLpFileCplex(string lpFile) {
    string lpPath = "lps/" + lpFile + ".lp";
    try {
        IloEnv env;
        IloModel model(env);
        IloCplex cplex(env);

        IloObjective obj;
        IloNumVarArray var(env);
        IloRangeArray rng(env);

        // Define o limite de tempo para 1 hora (3600 segundos)
        cplex.setParam(IloCplex::Param::TimeLimit, 3600);

        // Importa o modelo
        cplex.importModel(model, lpPath.c_str(), obj, var, rng);
        cplex.extract(model);

        // Resolve o problema
        if (!cplex.solve()) {
            env.error() << "Failed to optimize LP" << std::endl;
            cerr << ("Failed to solve the problem.");
        }

        // Obter o Lower Bound (LB) e Upper Bound (UB) da função objetivo
        double lowerBound = cplex.getBestObjValue();
        double upperBound = cplex.getObjValue();

        // Calcula o Gap
        double gap = 0.0;
        if (lowerBound != 0.0) { // Evitar divisão por zero
            gap = ((upperBound - lowerBound) / std::abs(lowerBound)) * 100;
        }

        // Exibe os resultados
        env.out() << "Solution status = " << cplex.getStatus() << std::endl;
        env.out() << "Objective value (UB)  = " << upperBound << std::endl;
        env.out() << "Best bound (LB)       = " << lowerBound << std::endl;
        env.out() << "Gap                   = " << gap << "%" << std::endl;

        IloNumArray vals(env);
        cplex.getValues(vals, var);
        env.out() << "Solution status = " << cplex.getStatus() << std::endl;
        env.out() << "Solution value  = " << cplex.getObjValue() << std::endl;
        //env.out() << "Solution vector = " << vals << std::endl;

        string solPath = "sol/CPLEX/" + lpFile + ".sol";
        cplex.writeSolution(solPath.c_str());
        env.end();
        return 0; // Sucesso
    }
    catch (IloException& e) {
        std::cerr << "Error: " << e.getMessage() << std::endl;
        return 1; // Erro no CPLEX
    }
    catch (...) {
        std::cerr << "Unknown error occurred!" << std::endl;
        return 2; // Outro erro
    }
}


int solveLpFileGurobi(std::string lpFile) {
    std::string lpPath = "lps/" + lpFile + ".lp";

    try {
        // Inicializar o ambiente do Gurobi
        GRBEnv env = GRBEnv(true);
        env.start();

        // Criar um modelo vazio
        GRBModel model = GRBModel(env);

        // Importar o modelo LP
        model.read(lpPath);

        // Configurar o limite de tempo (1 hora = 3600 segundos)
        model.set(GRB_DoubleParam_TimeLimit, 3600);

        // Otimizar o modelo
        model.optimize();

        // Verificar o status da solução
        int status = model.get(GRB_IntAttr_Status);

        if (status == GRB_OPTIMAL || status == GRB_SUBOPTIMAL) {
            // Obter o valor do objetivo (Upper Bound)
            double upperBound = model.get(GRB_DoubleAttr_ObjVal);

            // Obter o melhor limite inferior (Lower Bound)
            double lowerBound = model.get(GRB_DoubleAttr_ObjBound);

            // Calcular o Gap
            double gap = 0.0;
            if (lowerBound != 0.0) { // Evitar divisão por zero
                gap = ((upperBound - lowerBound) / std::abs(lowerBound)) * 100;
            }

            // Exibir resultados
            std::cout << "Solution status = " << status << std::endl;
            std::cout << "Objective value (UB) = " << upperBound << std::endl;
            std::cout << "Best bound (LB)      = " << lowerBound << std::endl;
            std::cout << "Gap                  = " << gap << "%" << std::endl;

            // Salvar solução em um arquivo
            std::string solPath = "sol/GUROBI/" + lpFile + ".sol";
            model.write(solPath);

            return 0; // Sucesso
        }
        else {
            std::cerr << "Failed to optimize LP. Status code: " << status << std::endl;
            return 1; // Falha ao resolver o problema
        }
    }
    catch (GRBException& e) {
        std::cerr << "Gurobi Error Code: " << e.getErrorCode() << std::endl;
        std::cerr << e.getMessage() << std::endl;
        return 2; // Erro específico do Gurobi
    }
    catch (...) {
        std::cerr << "Unknown error occurred!" << std::endl;
        return 3; // Outro erro
    }
}












int main(int argc, char* argv[]) {
    //long long currentTimeMs = getSystemTimeMsec();
    //long long afterTime = getSystemTimeMsec();
    //std::cout << "Total time: " << (afterTime - currentTimeMs) / 1000 << "seconds" << std::endl;



    string fileName = argv[1];
    // write the LP file with .dat file
    //string fileName = "d1000_24";
    writeLpFile(fileName);


    //Solve LP file
    solveLpFileCplex(fileName);
    //solveLpFileGurobi(fileName);


    getchar();  // wait for keyboard input
    return 0;
}