#include <stdio.h>
#include <stdlib.h>


FILE *f, *wf;
#define n_cities 10
#define cross 70
#define mut 10
#define generation_size 30

typedef struct Chromosome{

    int genotype[n_cities];
    int fenotype;

}Chromosomes;

void start_environment(int environment[n_cities][n_cities])
{

    for(int i = 0 ; i < n_cities ; ++i)
    {
        for(int j = 0 ; j < n_cities ; ++j)
        {
            environment[i][j] = 0;
        }

    }

}
void loading_environment(int environment[n_cities][n_cities], char *file_name)
{
    f = fopen(file_name,"r");
    if(!f)
    {
        printf("Error reading the file\n");
        return;
    }

    int cost;

    for(int i = 0 ; i < n_cities ; ++i)
    {
        for(int j = 0 ; j < n_cities ; ++j)
        {
            fscanf(f, "%d", &cost);
            environment[i][j] = cost;
        }
    }
}

void print_environment(int environment[n_cities][n_cities])
{

    printf("Environment\n\n");
    for(int i = 0 ; i < n_cities ; ++i)
    {
        for(int j = 0 ; j < n_cities ; ++j)
        {
            printf("%d ", environment[i][j]);
        }

        printf("\n");
    }
    printf("\n");
}

int factorial(int n)
{
    int fat = n;
    for(int i = n -1 ; i > 0 ; i--)
    {
        fat *= i;
    }
    return fat;
}

void start_generation(Chromosomes G[])
{
    for(int i = 0 ; i < generation_size ; i++)
    {
        G[i].fenotype = 0;
        for(int j = 0 ; j < n_cities ; j++)
        {
            G[i].genotype[j] = 0;
        }
    }
}

void print_generation(Chromosomes G[])
{
    printf("Generation\n\n");
    for(int i = 0 ; i < generation_size ; i++)
    {
        for(int j = 0 ; j < n_cities ; j++)
        {
            printf("%d ",G[i].genotype[j]);
        }

        printf("\t fenotype = %d\n",G[i].fenotype);
    }
    printf("\n");


}

void random_array(int a[])
{
    for(int i = 1 ; i < n_cities ; ++i)
    {
        a[i] = rand()%(n_cities-1) + 2;

        for(int j = 0 ; j < i ; ++j)
        {
            if(a[i] == a[j])
            {
                i--;
            }
        }
    }
}

void first_generation(Chromosomes G[])
{
    for(int i = 0 ; i < generation_size ; ++i)
    {
        G[i].genotype[0] = 1;
        random_array(G[i].genotype);
    }
}
int dist(int environment[n_cities][n_cities], int pos1, int pos2)
{
    if(pos1 > pos2)
        return environment[pos1][pos2];

    return environment[pos2][pos1];
}
void calculate_fenotype(Chromosomes G[], int environment[n_cities][n_cities])
{
    int pos1, pos2;
    for(int i = 0 ; i < generation_size ; ++i)
    {
        G[i].fenotype = 0;
        for(int j = 0 ; j < n_cities - 1 ; ++j)
        {
            pos1 = G[i].genotype[j] - 1;
            pos2 = G[i].genotype[j+1] - 1;
            G[i].fenotype += dist(environment,pos1,pos2);
        }
        //Distancia da ultima cidade a primeira
        G[i].fenotype += dist(environment, G[i].genotype[n_cities - 1] -1, G[i].genotype[0] - 1);
    }
}

void printing_file(int saves[3])
{
    wf = fopen("results.txt", "a+");
    fprintf(wf, "%d %d %d\n",saves[0], saves[1], saves[2]);
    fclose(wf);
}
void fitness(Chromosomes G[], int saves[3],int *best)
{
    //best
    saves[0] = G[0].fenotype;
    *best = 0;
    for(int i = 1 ; i < generation_size ; ++i)
    {
        if( G[i].fenotype < saves[0])
        {
            saves[0] = G[i].fenotype;
            *best = i;
        }
    }

    //average
    saves[1] = 0;
    for(int i = 0 ; i < generation_size ; ++i)
    {
        saves[1] += G[i].fenotype;
    }
    saves[1] = (int)(saves[1]/generation_size);

    //worse
    saves[2] = G[0].fenotype;
    for(int i = 1 ; i < generation_size ; ++i)
    {
        if(G[i].fenotype > saves[2])
            saves[2] = G[i].fenotype;
    }
}
int tournament(Chromosomes G[], int x, int y)
{
    if( G[x].fenotype < G[y].fenotype)
        return x;

    return y;
}

Chromosomes crossing(Chromosomes G[], int x, int y)
{
    Chromosomes A,B;
    A = G[x];
    B = G[y];

    int z, N,pos;
    if(n_cities % 2 == 0)
        z = n_cities / 2 - 1;
    else
        z = n_cities / 2;

    while(z != 0)
    {
        N = rand()%(n_cities - 1) + 1;
        for(int i = 0 ; i < n_cities ; i++)
        {
            if(B.genotype[i] == N)
            {
                pos = i;
                i = n_cities;
            }
        }
        for(int i = 0 ; i < n_cities ; i++)
        {
            if(A.genotype[i] == N)
            {
                A.genotype[i] = A.genotype[pos];
                A.genotype[pos] = N;
            }
        }
        z--;
    }

    return A;
}

void crossover(Chromosomes G[], Chromosomes G2[], int environment[n_cities][n_cities])
{
    int x,y,z;

    //nova populacao pos torneio entre cromosomos randomicos
    for(int i = 0 ; i < generation_size ; ++i)
    {
        x = rand()%generation_size;

        y = rand()%generation_size;


        z = tournament(G,x,y);

        G2[i]= G[z];
    }

    for(int i = 0 ; i < generation_size ; ++i)
    {
        x = i;
        y = rand()%generation_size;
        while(y == x)
            y = rand()%generation_size;

        z = rand()%101;
        if(z <= cross)
           G2[x] = crossing(G2,x,y);
    }

}

void mutation(Chromosomes G[])
{
    int x,y,z;
    for(int i = 0 ; i < generation_size ; ++i)
    {
        x = rand()%n_cities;
        y = rand()%n_cities;
        z = rand()%101;

        if(z <= mut)
        {
            int aux;
            aux = G[i].genotype[x];
            G[i].genotype[x] = G[i].genotype[y];
            G[i].genotype[y] = aux;
        }

    }
}
void elitism(Chromosomes G[], Chromosomes G2[],int saves[3], int *best)
{
    int x;
    fitness(G2, saves, &x);
    if(G2[x].fenotype < G[*best].fenotype)
    {
        *best = x;
    }
    else
    {
        G2[x] = G[*best];
    }
}

void copy(Chromosomes G[], Chromosomes G2[])
{
    for(int i = 0 ; i < generation_size ; i++)
    {
        G[i] = G2[i];
    }

}
int main()
{
    int generation = 1;
    int saves[3];
    int best;

    //int generation_size = (int) (factorial(n_cities) * 0.1);
    //while(generation_size % 2 != 0 )
      //  generation_size += 1;




    printf("Generation Size: %d\n\n");
    //iniciando o ambiente
    int environment[n_cities][n_cities];
    start_environment(environment);

    //Lendo arquivo com informacoes do ambiente
    static char *file_name = "C:/Users/daniel/Desktop/test2.txt";

    //Dando Load no ambiente
    loading_environment(environment, file_name);
    print_environment(environment);

    //inicializando o vetor de geracoes
    Chromosomes P[generation_size];

    start_generation(P);
    print_generation(P);

    first_generation(P);

    calculate_fenotype(P,environment);

    print_generation(P);

    fitness(P,saves,&best);

    printing_file(saves);

    //first step finished

    while(generation != 100)
    {

        Chromosomes P2[generation_size];
        start_generation(P2);

        crossover(P,P2,environment);
        mutation(P2);
        calculate_fenotype(P2,environment);
        elitism(P,P2,saves,&best);
        copy(P,P2);
        fitness(P,saves,&best);
        printing_file(saves);
        generation++;
    }

    return 0;
}
