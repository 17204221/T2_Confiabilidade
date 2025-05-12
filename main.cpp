#include <iostream>
#include <fstream>
#include <string>
#include <cmath> //pow
#include <limits>
#include <sstream>

#define TAM_ALFABETO 26
#define IC_PORTUGUES 0.07797
#define RESULTADO "resultado.txt"

using namespace std;

// carregar arquivo texto cifrado
string fileCifra(string file) {
    ifstream arquivo(file);
    string texto = "";
    string linha;

    if (!arquivo) {
        cerr << "Erro ao abrir o arquivo!" << endl;
        exit(1);
    }
    while (getline(arquivo, linha)) {
        for (int i = 0; i < linha.size(); i++) {
            char letra = linha[i];
            if (isalpha(letra)) {
                texto += toupper(letra);
            }
        }
    }
    return texto;
}

double frequenciaReferencia[TAM_ALFABETO] = {0.0};

// carregar arquivo com frequencias de letras
void fileFrequencia(string file) {
    ifstream arquivo(file);

    if (!arquivo) {
        cerr << "Erro ao abrir o arquivo de frequencias!" << endl;
        exit(1);
    }

    string linha;
    while (getline(arquivo, linha)) {
        char letra;
        double freq;
        stringstream ss(linha);
        ss >> letra;
        ss.ignore(1, ':');
        ss >> freq;

        frequenciaReferencia[toupper(letra) - 'A'] = freq;
    }
}

// calcula frequencia das letras do texto
void calculaFrequenciaLetras(string texto, int freq[TAM_ALFABETO]) {
    for (int i = 0; i < TAM_ALFABETO; i++) {
        freq[i] = 0;
    }

    for (int i = 0; i < texto.size(); i++) {
        freq[texto[i] - 'A']++;
    }
}

// calcula IC baseado em fi(fi-1)/N(N-1)
double calculaIC(string texto) {
    int freq[TAM_ALFABETO];
    calculaFrequenciaLetras(texto, freq);

    int N = texto.size();
    if (N <= 1) 
        return 0;

    double ic = 0.0;
    for (int i = 0; i < TAM_ALFABETO; i++) {
        ic += freq[i] * (freq[i] - 1);      // somatorio fi(fi-1) para cada letra
    }
    ic = ic / (N * (N - 1));        // IC = somatorio fi(fi-1) / N(N-1)

    return ic;
}

// estimar o tamanho da chave usando IC
int estimaTamanhoChave(string texto) {
    int melhorTamanho = 1;
    double menorDiferenca = numeric_limits<double>::max();

    for (int tamChave = 1; tamChave <= 20; tamChave++) {
        double somaIC = 0;

        for (int i = 0; i < tamChave; i++) {
            string segmento = "";
            for (int j = i; j < texto.size(); j += tamChave) {
                segmento += texto[j];
            }

            somaIC += calculaIC(segmento);
        }

        double mediaIC = somaIC / tamChave;
        double diferenca = abs(mediaIC - IC_PORTUGUES); // retorna modulo da diferenca

        if (diferenca < menorDiferenca) {
            menorDiferenca = diferenca;
            melhorTamanho = tamChave;
        }
    }

    cout << "Tamanho estimado da chave: " << melhorTamanho << endl;
    return melhorTamanho;
}

// calcula frequencia em porcentagem
void calcularFrequenciaPorcentagem(string texto, double freq[TAM_ALFABETO]) {
    int contagem[TAM_ALFABETO] = {0};
    int total = 0;

    for (int i = 0; i < texto.size(); i++) {
        contagem[texto[i] - 'A']++;
        total++;
    }

    for (int i = 0; i < TAM_ALFABETO; i++) {
        freq[i] = 100.0 * contagem[i] / total; // calculo porcentagem basica de frequencias
        //cout << "frequencias" << freq[i] << " ";
    }
}

// calcula distancia entre duas distribuicoes de frequencia
double calcularDistancia(double f1[TAM_ALFABETO], double f2[TAM_ALFABETO]) {
    double soma = 0;
    for (int i = 0; i < TAM_ALFABETO; i++) {
        soma += pow(f1[i] - f2[i], 2); // distancia euclidiana
    }
    return soma;
}

// Descriptografar uma letra apenas
char descriptografarLetra(char c, int deslocamento) {
    return (((c - 'A' - deslocamento + TAM_ALFABETO) % TAM_ALFABETO) + 'A');
}

// descriptografar todo o texto com a chave utilizando funcao para descriptografar letra a letra
string descriptografaTexto(string texto, string chave) {
    string resultado = "";
    int indiceChave = 0;

    for (int i = 0; i < texto.size(); i++) {
        char c = texto[i];
        int deslocamento = chave[indiceChave % chave.size()] - 'A';
        resultado += descriptografarLetra(c, deslocamento);
        indiceChave++;
    }
    return resultado;
}

// descobrir chave sabendo o tamanho ja calculado em estimaTamanhoChave
string descobreChave(string texto, int tamChave) {
    string chave = "";

    for (int i = 0; i < tamChave; i++) {
        string segmento = "";
        for (int j = i; j < texto.size(); j += tamChave) {
            segmento += texto[j];
        }

        double menorDistancia = numeric_limits<double>::max();
        int melhorDeslocamento = 0;

        for (int deslocamento = 0; deslocamento < TAM_ALFABETO; deslocamento++) {
            string textoDeslocado = "";
            for (int k = 0; k < segmento.size(); k++) {
                textoDeslocado += descriptografarLetra(segmento[k], deslocamento);
            }

            double freqSegmento[TAM_ALFABETO];
            calcularFrequenciaPorcentagem(textoDeslocado, freqSegmento);
            double dist = calcularDistancia(freqSegmento, frequenciaReferencia);

            if (dist < menorDistancia) {
                menorDistancia = dist;
                melhorDeslocamento = deslocamento;
            }
        }

        chave += (char)('A' + melhorDeslocamento);
    }

    return chave;
}

// salva o texto descriptografado em um arquivo passado como argumento
void salvarTexto(string texto, string file) {
    ofstream arquivo(file);

    if (!arquivo) {
        cerr << "Erro ao salvar o arquivo" << endl;
        exit(1);
    }

    arquivo << texto;
    cout << "Texto descriptografado salvo em: " << file << endl;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cout << "Uso: " << argv[0] << " arquivo_cifrado.txt frequencia.txt" << endl;
        return 1;
    }

    string textoCifrado = fileCifra(argv[1]);
    fileFrequencia(argv[2]);

    int tamanhoChave = estimaTamanhoChave(textoCifrado);
    string chave = descobreChave(textoCifrado, tamanhoChave);

    cout << "Chave encontrada: " << chave << endl;

    string textoDescriptografado = descriptografaTexto(textoCifrado, chave);
    salvarTexto(textoDescriptografado, RESULTADO);

    return 0;
}
