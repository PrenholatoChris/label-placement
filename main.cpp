#include <iostream>
#include <fstream>     
#include <sstream>  
#include <vector>
#include <string>
#include <cstdio>
#include <algorithm>
#include <chrono>  // For time tracking
#include <cstdlib> // For atoi, atof
#include <filesystem>  

#define P 4

using namespace std;
using namespace std::chrono;

class Conflict {
    int n_x;
    int n_y;

    int p_x;
    int p_y;
    public: Conflict(int index_i, int index_j) {
        n_x = index_i / P;
        n_y = index_j / P;

        p_x = index_i % P;
        p_y = index_j % P;
    }
    
    int get_point(bool x=false) {
        if (x) {
            return this->n_x;
        }
        return this->n_y;
    }

    int get_pos(bool x = false) {
        if (x) {
            return this->p_x;
        }
        return this->p_y;
    }

    void print() {
        cout << "[" + to_string(n_x) + "," + to_string(p_x) + "]" + "![" + to_string(n_y) + "," + to_string(p_y) + "]" << endl;
    }

};

void print_conflicts(vector<Conflict*> conflicts) {
    int size = conflicts.size();
    for (int i = 0; i < size; i++)
    {
        Conflict* conflict = conflicts.at(i);
        conflict->print();

    }
}





class Solution {
public:
    static int N;
    static vector<Conflict*> conflicts;


    int FO;
    vector <int> sol;

    Solution(){
        for (int i = 0; i < this->N; i++){
            int pos = rand() % P;
            sol.push_back(pos);
        }
        totalConflicts();
    }

    Solution(vector <int> sol) {
        this->sol = sol;
        totalConflicts();
    }

    void print() {
        cout << "Sol (" + to_string(this->FO) +") = [" + to_string(this->sol[0]);
        for (int i = 1; i < this->N; i++)
        {
            cout << ", " + to_string(this->sol[i]);
        }
        cout << "]" << endl;

    }

    int totalConflicts(bool print = false) {
        int total = 0;
        for (int i = 0; i < this->conflicts.size(); i++){
            Conflict* conflict = this->conflicts.at(i);
            int n_x = conflict->get_point(true);
            int n_y = conflict->get_point();
            int p_x = conflict->get_pos(true);
            int p_y = conflict->get_pos();
            if (sol.at(n_x) == p_x and sol.at(n_y) == p_y){
                if (print) {
                    cout << "conflito entre:(" + to_string(n_x) + "," + to_string(p_x) + ") e (" + to_string(n_y) + "," + to_string(p_y) + ")" << endl;
                    /*break;*/
                }
                total++;
            }
        }
        this->FO = N - total;
        return total;
    }

};


// Comparator function
bool compareByFO(Solution* a, Solution* b) {
    return a->FO > b->FO;  // Correct comparison for highest to lowest FO
}

void insertSorted(vector<Solution*>& Pop, Solution* newSol, int maxPopSize) {
    // Ensure the population is sorted in descending order by FO before insertion

    auto it = upper_bound(Pop.begin(), Pop.end(), newSol, compareByFO);
    Pop.insert(it, newSol);  // Insert the new solution in the correct position

    // If population size exceeds the max allowed size, remove the solution with the lowest FO
    if (Pop.size() > maxPopSize) {
        //delete Pop.back();  // Free memory before removing
        Pop.pop_back();
    }
}

void insertSolinPop(vector<Solution*>& Pop, Solution* newSol, int maxPopSize) {
    
    Pop.push_back(newSol);

    sort(Pop.begin(), Pop.end(), compareByFO);

    if (Pop.size() > maxPopSize) {
        Pop.pop_back();
    }
    
}

vector<Solution*> createRandPop(int amount, int max_pop) {
    vector<Solution*> Pop;
    for (int i = 0; i < amount; i++){
        insertSolinPop(Pop, new Solution(), max_pop);
        //insertSorted(Pop, new Solution(), amount);
    }
    return Pop;
}


void print_vector(vector<int> v) {
    cout << "size=" + to_string(v.size()) << endl;
    cout << "[" + to_string(v.at(0));
    for (int i = 1; i < v.size(); i++) {
        cout << ", " + to_string(v.at(i));
    }
    cout << "]" << endl;
}





Solution* getSolFromPop(const vector<Solution*>& Pop, bool selectBest = false) {
    if (Pop.empty()) return nullptr;  // Safety check

    if (selectBest) {
        // Select a random solution from the top 10%
        int topPercentage = max(1, (int)(0.1 * Pop.size()));
        return Pop[rand() % topPercentage]; // Pop is already sorted
    }

    // Otherwise, return a completely random solution
    return Pop[rand() % Pop.size()];
}




vector<int> getHalfSol(int init, int N, Solution* sol) {
    vector<int> half;
    for (int i = init; i < N; i++){
        half.push_back(sol->sol.at(i));
    }
    return half;
}



void crossover(Solution* sol1, Solution* sol2, vector<int>& result) {
    int mid = (int)sol1->sol.size() / 2;
    vector<int> a, b;
    if (rand() % 2 == 0) {
        // Take the first half from sol1 and second half from sol2
        a = getHalfSol(0, mid, sol1);
        b = getHalfSol(mid, sol2->sol.size(), sol2);
    }
    else {
        // Take the first half from sol2 and second half from sol1
        a = getHalfSol(0, mid, sol2);
        b = getHalfSol(mid, sol1->sol.size(), sol1);
    }

    // Merge `a` and `b` into `result`
    result.clear();  // Ensure `result` is empty before inserting
    result.insert(result.end(), a.begin(), a.end());
    result.insert(result.end(), b.begin(), b.end());
}



void mutate(vector<int>& v, float mutation_rate) {
    for (int& gene : v) {
        if ((float)rand() / RAND_MAX < mutation_rate) { // Use `<` for better probability handling
            gene = rand() % P; // Generates a number between 0 and 3
        }
    }
}


void randGenetic(int AG, vector<Solution*>& Pop, int maxPopSize, int crossings, float mutation_rate, int teste) {
    Solution* bestSol = getSolFromPop(Pop, true);  // Best solution starts as a reference (no extra allocation)

    vector<int> crossVector;  // Preallocate crossover vector

    for (int ag = 0; ag < AG; ag++) {
        Solution* sol1 = getSolFromPop(Pop, true);
        Solution* sol2 = getSolFromPop(Pop);

        for (int i = 0; i < crossings; i++) {
            crossover(sol1, sol2, crossVector);  // Modify crossVector in-place

            // Mutation with proper probability
            mutate(crossVector, mutation_rate);

            Solution* crossSol = new Solution(crossVector);

            // Update best solution without allocating new memory
            if (crossSol->FO > bestSol->FO) {
                bestSol = crossSol;  // Just update pointer reference
            }

            insertSolinPop(Pop, crossSol, maxPopSize);  // Insert and maintain population sizes
        }

    }
}

void read_file(string fileName, int &N, vector<Conflict*> &conflicts) {

    //cout << "Reading " + fileName << endl;

    vector <int> instance(2, 0);

    ifstream f(fileName);

    if (!f.is_open()) {
        cerr << "Error opening the input file!";
        return ;
    }


    string s;
    getline(f, s);//primeira linha em branco
    getline(f, s);
    N = stoi(s);


    if (N == 0) {
        cerr << "N not filled!";
    }

    getline(f, s);
    int p = stoi(s);

    if (p != P) {
        exit;
    }

    //cout << "N:" + to_string(N) + " P:" + to_string(p) << endl;

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
                
                conflicts.push_back(new Conflict((i*P)+k, id));
            }
        }
    }
    return ;
}

//int index_x = 8;
//int index_y = 1;
//Conflict* teste = new Conflict(index_x, index_y, 25, 4);
//teste->printar_conflito();




int Solution::N = 0;  // Definition and initialization
vector<Conflict*> Solution::conflicts;// Definition and initialization








void saveBestSolutionToFile(Solution* best, const string& inputFilePath) {
    // Criar diretório "sol" se não existir
    //filesystem::create_directory("sol");

    // Extrair apenas o nome do arquivo sem o caminho
    // Encontrar a última ocorrência de '/'
    size_t lastSlash = inputFilePath.find_last_of("/\\");
   string fileName = (lastSlash == string::npos) ? inputFilePath : inputFilePath.substr(lastSlash + 1);


    // Substituir extensão ".dat" por ".sol"
    size_t pos = fileName.find(".dat");
    if (pos !=string::npos) {
        fileName.replace(pos, 4, ".sol");  // Troca ".dat" por ".sol"
    }
    else {
        fileName += ".sol";  // Caso não tenha ".dat", adiciona ".sol"
    }

    // Criar caminho final dentro da pasta "sol"
   string outputFilePath = "sol/" + fileName;

   ofstream outFile(outputFilePath);
    if (!outFile) {
       cerr << "Error while opening file for write!" <<endl;
        return;
    }

    // Escreve os dados no arquivo
    outFile << "FO = " << best->FO << "\n";
    outFile << "Solution:\n";
    for (int val : best->sol) {
        outFile << val << " ";
    }
    outFile << "\n";

    outFile.close();
   cout << "Solution saved in : " << outputFilePath <<endl;
}




int main(int argc, char* argv[]) {
    if (argc < 7) {
        cerr << "Usage: " << argv[0] << " <fileName> <total_generations> <initial_pop> <max_pop> <total_crossings> <mutation_rate> <time_limit>" << endl;
        return 1;
    }

    string fileName = argv[1];
    int total_generations = atoi(argv[2]);
    int initial_pop = atoi(argv[3]);
    int max_pop = atoi(argv[4]);
    int total_crossings = atoi(argv[5]);
    float mutation_rate = atof(argv[6]);
    int time_limit = atoi(argv[7]); // New parameter: Time limit in seconds
    bool limited = false;

    int N = 0;
    vector<Conflict*> conflicts;
    read_file(fileName, N, conflicts);

    Solution::N = N;
    Solution::conflicts = conflicts;

    vector<Solution*> Pop = createRandPop(initial_pop, max_pop);

    auto start_time = high_resolution_clock::now();
    auto current_time = high_resolution_clock::now();
    auto elapsed_time = duration_cast<seconds>(current_time - start_time).count();

    Solution* best = nullptr;
    //Use heuristic
    if (true) {

        for (int generation = 0; generation < total_generations; generation++) {
            randGenetic(1, Pop, max_pop, total_crossings, mutation_rate, generation);  // Run 1 generation at a time
        
            // Check elapsed time
            auto current_time = high_resolution_clock::now();
            auto elapsed_time = duration_cast<seconds>(current_time - start_time).count();

            if (elapsed_time >= time_limit) {
                limited = true;
                break;  // Stop execution if time is exceeded
            }
        }
        
        best = Pop.at(0);
        cout << best->FO << endl;

    }

    //print solution
    if (true) {
        best = Pop.at(0);
        if (limited) {
            cout << "Time limit reached (" << elapsed_time << "s)." << endl;
        }
        else {
            current_time = high_resolution_clock::now();
            elapsed_time = duration_cast<seconds>(current_time - start_time).count();
            cout << "Elapsed time (" << elapsed_time << "s)." << endl;
        }

        cout << "Best solution found : FO = " << best->FO << endl;

        // Chamar a função passando o nome do arquivo de entrada como parâmetro
        saveBestSolutionToFile(best, fileName);

    }

    return 0;
}



