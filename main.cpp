#include <iostream>
#include <fstream>
#include <string> 
#include <list>
#include "matrix/dist/matrix.cpp"
#include "plot/pbPlots.hpp"
#include "plot/supportLib.hpp"

using namespace std;

int main(){
	bool success;
	StringReference *errorMessage = CreateStringReferenceLengthValue(0, L' ');
	RGBABitmapImageReference *imageReference = CreateRGBABitmapImageReference();

    int N,K;
    double L, deltaT, Θ0, Θn, k;

//PARTE1 (carregar todas as variaveis)
    L = 2;      //comprimento da barra
    N = 100;    //total de partes da barra 
    K = 5;      //divisão do tempo
    deltaT = 0.005; //intervalo de tempo em que é calculado as temperaturas
    Θ0 = 0;    //temperatura na ponta esquerda
    Θn = 500;   //temperatura na ponta direita
    k = -.08;       //condutividade termica do material da barra

//PARTE2 (calcular ∆x e φ)
    double deltaX, φ;
    deltaX = L/N;
    φ = (k*deltaT)/(deltaX*deltaX);     //k é o coeficiente de tempo para ∆t

//PARTE3 (determinar a matriz A)
    Matrix A(N-1,N-1);      //essa é a forma matricial da equação anterior de φ após fazer alguns rearanjos
    for(int i=0; i<N-1; i++){
        for(int j=0; j<N-1; j++){
            if(i == j){             //s linhas e colunas forem iguais, entao subtraimos 1 − 2φ
                A(i,j) = 1 - (2*φ);
            }else if((j == i+1) || (j == i-1)){    //preenche com φ valores proximos a i==j (assim como é na forma matricial)
                A(i,j) = φ;
            }else{
                A(i,j) = 0;             //preenche com zeros os demais espaços da matriz
            }
        }
    }

//PARTE4
//utilizaremos os indices de um vetor para compor a lista L; esses valores vao de 0 a K

//PARTE5
//INICIALIZANDO OS VALORES CONHECIDOS DE θ:
    Matrix Θ(N+1,K+1);              //matriz composta pelo total de partes da barra e divisao de tempo 
    for(int i=0; i<=K; i++){
        Θ(0,i) = Θ0;                //temperatura na ponta esquerda (fica ao topo da matriz)
        Θ(N,i) = Θn;                //temperatura da ponta direita (fica no fim da matriz)
    }
    for(int i = 1; i < N; i++){
        Θ(i,0) = 0;  
    }
    
    Matrix X(A.size_cols(), 1);

    int aux = -1;//aux serve como K, e é inicializado com -1, pois ele é incrementado no início do for, começando, assim, como 0

//PARTE7 (resolvendo o sistema: A · Θk + 1 = Θk)
    for(double i = 0; i <= K; i+=deltaT){
        if(i>=(aux+1)){
            aux++;
            cout <<aux<<endl;
            for(int j = 1; j < N; j++){                  
                X(j-1,0) = Θ(j,aux);          //X = θ^K, que seria o B do sistema Ax=B
            }
        }
	    X(0,0) += Θ0;
        X(N-2,0) += Θn;     //o primeiro e o último elemento de B são subtraídos de θ0 e θn, como demonstrado no PDF
        X = A.inverse()*X;  //set X
        for(int j = 1; j < N; j++){                  
           Θ(j,aux+1) =  X(j-1,0);          
        }
    }

//PARTE8 (plot de Θ^k)
    double ret[N+1];            //vetor que recebe todos os valores da matriz
    vector<vector<double>> ys;  //vetor de vetores da classe vector (vai armazenar os valores da matriz dividido pelo tempo K)
    for(int j = 0; j <=K; j++){
        for(int i = 0; i<=N; i++){
            ret[i] = Θ(i,j);
            cout << ret[i] << " ";
        }
        vector<double> retCopy(ret, ret+sizeof(ret)/sizeof(double));   //vetor da classe vector que irá copiar double ret
        ys.push_back(retCopy);  //valores das linhas da matriz Θ^k no eixo Y
        cout << endl;
    }
    vector<double> xs;      //eixo X recebe apenas os valores de N (total de partes da barra)
    for(int n=0; n<N+1; n++){
        xs.push_back(n);
    }

    vector<ScatterPlotSeries*> series;      //esse vetor armazena todas as series que deverão ser plotadas no grafico
    //essa quantidade é proporcional a divisao do tempo K

    for(int i=0; i<N+1; i++){
        ScatterPlotSeries *plot = GetDefaultScatterPlotSeriesSettings();
        plot->xs = &xs;
        plot->ys = &ys[i];              //variação dos valores em θ^K
        plot->linearInterpolation = true;
        plot->lineType = toVector(L"solid");
        plot->lineThickness = 2;
        plot->color = CreateRGBColor(0, 0, 1);
        series.push_back(plot);
    }
	ScatterPlotSettings *settings = GetDefaultScatterPlotSettings();
	settings->width = 600;
	settings->height = 400;
	settings->autoBoundaries = true;
	settings->autoPadding = true;
    for(int i=0; i<K; i++){     //divisao de tempo K define a quantidade de series no grafico
        settings->scatterPlotSeries->push_back(series[i]);
    }

	success = DrawScatterPlotFromSettings(imageReference, settings, errorMessage);
	if(success){
		vector<double> *pngdata = ConvertToPNG(imageReference->image);
		WriteToFile(pngdata, "Grafico.png");
		DeleteImage(imageReference->image);
	}else{
		cerr << "Error: ";
		for(wchar_t c : *errorMessage->string){
			cerr << c;
		}
		cerr << endl;
	}
	FreeAllocations();
	return success ? 0 : 1;
}
