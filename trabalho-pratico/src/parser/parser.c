#include "../../include/parser/parser.h"
#define MAX_FILENAME 1024
#define MAX_LINE 2048
#define MAX_QUERYLINE 150
typedef void (*EntityProcessor)(char* line, void* gestor, void* aux_dataX, void* aux_dataY);

// Função principal de parse que chama os módulos de parse individualmente.
void parse_all(char* path, GestorArtist* gestorArtist, GestorMusic* gestorMusic, GestorUser* gestorUser, GestorAlbum* gestorAlbum, GestorHistory* gestorHistory){
    //parse de artistas
    parse_artist(path, gestorArtist);
    printf("Artists parsed!\n");

    //parse de albúns
    parse_album(path, gestorAlbum, gestorArtist);
    printf("Albums parsed!\n");

    //parse de músicas
    parse_music(path, gestorMusic, gestorArtist, gestorAlbum);
    printf("Musics parsed!\n");

    //parse de utilizadores
    parse_user(path, gestorUser, gestorMusic);
    printf("Users parsed!\n");

    //parse de históricos
    parse_history(path, gestorHistory, gestorMusic, gestorArtist);
    printf("History parsed!\n");

}

// função que faz parse de um csv.
void parse_csv(const char* path, const char* filename, void* gestor, void* aux_dataX, void* aux_dataY, EntityProcessor process_line, int error_code) {
    FILE* file;
    char full_path[MAX_FILENAME];
    char line[MAX_LINE];
    char original_line[MAX_LINE];

    snprintf(full_path, MAX_FILENAME, "%s/%s", path, filename);

    file = fopen(full_path, "r");
    if(!file){
        perror("Erro ao abrir o ficheiro csv.\n");
        exit(EXIT_FAILURE);
    }

    // ignorar a primeira linha(cabeçalho).
    fgets(line, sizeof(line), file);

    while (fgets(line, sizeof(line), file) != NULL){
        strcpy(original_line, line);
        if(process_line){
            process_line(original_line, gestor, aux_dataX, aux_dataY);
        }else{
            writeErrors(original_line, error_code);
        }
    }

    fclose(file);
}

// Função dá parse as queries.
void parse_queries(char* path, GestorUser* gestorUser, GestorMusic* gestorMusic, GestorArtist* gestorArtist, GestorAlbum* gestorAlbum, GestorHistory* gestorHistory, int measure_flag){
    char line[MAX_QUERYLINE];
    char* linePtr=NULL;
    FILE* queries;
    FILE* outputQ1;
    FILE* outputQ2;
    FILE* outputQ3;
    FILE* outputQ4;
    FILE* outputQ5;
    FILE* outputQ6;
    int command = 0; // Para escrita individual de ficheiros de output
    char outputPath[MAX_FILENAME]; // Caminho para ficheiros de output individuais

    
    char* user; // Utilizador para a query 1
    int nArtists = 0; // Nº de artistas para a query 2
    char* country; // País para a query 2
    Discography* disco = NULL; // Discografia para a query 2
    int ageMin = 0; // Idade mínima para a query 3
    int ageMax = 0; // Idade máxima para a query 3
    char* start_week;
    char* end_week;

    populateWeekTop10(gestorHistory);

    // Variaveis utilizadas no calculo do tempo de execução de cada query
    struct timespec query_start, query_end;
    double total_time_query1 = 0;
    double total_time_query2 = 0;
    double total_time_query3 = 0;
    double total_time_query4 = 0;
    double total_time_query5 = 0;
    //double total_time_query6 = 0;
    double query_elapsed;

    // Abertura ficheiro de input das queries
    queries = fopen(path, "r");
    if(!queries){
        perror("Erro ao abrir o ficheiro input das queries.\n");
        exit(EXIT_FAILURE);
    }

    // Discografia antes de resolver a Query2 
    disco = fillWithArtists(gestorArtist, disco);
    disco = updateArtistsDurationFromMusic(gestorMusic, disco);
 
    sortByDuration(&disco);
    // discografia pronta para a 2ª query

    // Leitura query a query
    while(fgets(line, sizeof(line), queries) != NULL){ 

        linePtr = line;
        // Atualização do path para o ficheiro de output da query 
        command++;
        snprintf(outputPath, MAX_FILENAME, "resultados/command%d_output.txt", command);

        
        /*// Para query 6
        if(getMusicWrap(gestorMusic)){
            freeMusicWrap(gestorMusic);
            createGestorMusicWrapped(gestorMusic);
        }
        else createGestorMusicWrapped(gestorMusic);*/

        char delimiter = getDelimiter(line); // obter o delimitador que irá ser utilizado no writing
        // Identificação da Query
        switch(line[0]){
            case '1':{
                // Criação do ficheiro de output da query para argumento da função query1
                outputQ1 = fopen(outputPath, "w");
                if(!outputQ1){
                    perror("Erro ao criar o ficheiro de output da query 1.\n");
                    exit(EXIT_FAILURE);
                }
                // Tratamento da linha para a 1ª Query
                strsep(&linePtr, " "); // Ignora o id da Query e o espaço
                user = strsep(&linePtr, "\n");

                if(measure_flag) clock_gettime(CLOCK_REALTIME, &query_start);
                query1(user, gestorUser, gestorArtist, delimiter, outputQ1);
                if(measure_flag){
                    clock_gettime(CLOCK_REALTIME, &query_end);
                    query_elapsed = (query_end.tv_sec - query_start.tv_sec) +
                                    (query_end.tv_nsec - query_start.tv_nsec) / 1e9;
                    total_time_query1 += query_elapsed;
                }
                // Processo completo, fechar ficheiro
                fclose(outputQ1);
                break;
            }

            case '2':{
                outputQ2 = fopen(outputPath, "w");
                if(!outputQ2){
                    perror("Erro ao criar o ficheiro de output da query 2.\n");
                    exit(EXIT_FAILURE);
                }

                strsep(&linePtr, " ");
                nArtists = atoi(strsep(&linePtr, " ")); // Numero de artistas
                country = remove_aspas(strsep(&linePtr, "\n")); // País sem aspas || NULL

                if(measure_flag) clock_gettime(CLOCK_REALTIME, &query_start);

                query2(nArtists, country, disco, delimiter, outputQ2); 

                if(measure_flag){
                    clock_gettime(CLOCK_REALTIME, &query_end);
                    query_elapsed = (query_end.tv_sec - query_start.tv_sec) +
                                    (query_end.tv_nsec - query_start.tv_nsec) / 1e9;
                    total_time_query2 += query_elapsed;
                }
                if(country != NULL){
                    free(country);
                }
                fclose(outputQ2);
                break;
            }

            case '3':{
                outputQ3 = fopen(outputPath, "w");
                if(!outputQ3){
                    perror("Erro ao criar o ficheiro de output da query 3.\n");
                    exit(EXIT_FAILURE);
                }

                strsep(&linePtr, " ");
                ageMin = atoi(strsep(&linePtr, " "));
                ageMax = atoi(strsep(&linePtr, "\n"));

                if(measure_flag) clock_gettime(CLOCK_REALTIME, &query_start);
                query3(ageMin, ageMax, gestorUser, delimiter, outputQ3);
                if(measure_flag){
                    clock_gettime(CLOCK_REALTIME, &query_end);
                    query_elapsed = (query_end.tv_sec - query_start.tv_sec) +
                                    (query_end.tv_nsec - query_start.tv_nsec) / 1e9;
                    total_time_query3 += query_elapsed;
                }
                fclose(outputQ3);
                break;
            }
            // No caso do big dataset demorar muito tempo, comentar a query 4 para o case '4'.
            case '4':{
                outputQ4 = fopen(outputPath, "w");
                if(!outputQ4){
                    perror("Erro ao criar o ficheiro de output da query 4.\n");
                    exit(EXIT_FAILURE);
                }
                strsep(&linePtr, " "); 

                start_week = verifyAndConvertWeekToKey(strsep(&linePtr, " "));
                end_week = verifyAndConvertWeekToKey(strsep(&linePtr, " "));
                if(measure_flag) clock_gettime(CLOCK_REALTIME, &query_start);
                query4(gestorHistory, start_week, end_week, delimiter, outputQ4);
                if(measure_flag){
                    clock_gettime(CLOCK_REALTIME, &query_end);
                    query_elapsed = (query_end.tv_sec - query_start.tv_sec) +
                                    (query_end.tv_nsec - query_start.tv_nsec) / 1e9;
                    total_time_query4 += query_elapsed;
                }
                free(start_week);
                free(end_week);
                fclose(outputQ4);
                break;
            }
                       
            case '5':{
                
                strsep(&linePtr, " "); 
                user = strsep(&linePtr, " ");
                int numRecommendations = atoi(strsep(&linePtr, "\n"));
                outputQ5 = fopen(outputPath, "w");
                if(!outputQ5){
                    perror("Erro ao criar o ficheiro de output da query 3.\n");
                    exit(EXIT_FAILURE);
                }
                if(measure_flag) clock_gettime(CLOCK_REALTIME, &query_start);
                query5(gestorHistory, user, numRecommendations, delimiter, outputQ5);
                if(measure_flag){
                    clock_gettime(CLOCK_REALTIME, &query_end);
                    query_elapsed = (query_end.tv_sec - query_start.tv_sec) +
                                    (query_end.tv_nsec - query_start.tv_nsec) / 1e9;
                    total_time_query5 += query_elapsed;
                }
                fclose(outputQ5);
                break;
            }
            case '6':{
                outputQ6 = fopen(outputPath, "w");
                if(!outputQ6){
                    perror("Erro ao criar o ficheiro de output da query 6.\n");
                    exit(EXIT_FAILURE);
                }

                genericOutputWriter(outputQ6, delimiter, NULL);
                fclose(outputQ6);
            }
            /*      
            case '6':{
                outputQ6 = fopen(outputPath, "w");
                if(!outputQ6){
                    perror("Erro ao criar o ficheiro de output da query 6.\n");
                    exit(EXIT_FAILURE);
                }

                // Tratamento da linha da 6ª Query
                strsep(&linePtr, " ");
                char* userQ6 = strsep(&linePtr, " "); // Id do utilizador
                char* yearQ6 = strsep(&linePtr, " "); // Ano
                char* listQ6 = strsep(&linePtr, "\n"); // argumento opcional
                if(listQ6 == NULL){
                    yearQ6[4]='\0';
                }
            
                if(measure_flag) clock_gettime(CLOCK_REALTIME, &query_start);

                // Execução da 6ª Query
                query6(userQ6,yearQ6,listQ6,gestorHistory,gestorMusic,delimiter,outputQ6);
                freeMusicWrap(gestorMusic);

                if(measure_flag){
                    clock_gettime(CLOCK_REALTIME, &query_end);
                    query_elapsed = (query_end.tv_sec - query_start.tv_sec) +
                                    (query_end.tv_nsec - query_start.tv_nsec) / 1e9;
                    total_time_query6 += query_elapsed;
                }

                fclose(outputQ6);
            }
            */
            
            default:
                continue;
        }

    }
    freeDiscography(disco);
    fclose(queries);

    // Para o modo de testes
    if(measure_flag){
        printf("============== Tempo de execução das queries ===============\n");
        printf("Tempo de execucao da Query 1: %.6f segundos\n", total_time_query1);
        printf("Tempo de execucao da Query 2: %.6f segundos\n", total_time_query2);
        printf("Tempo de execucao da Query 3: %.6f segundos\n", total_time_query3);
        printf("Tempo de execucao da Query 4: %.6f segundos\n", total_time_query4);
        printf("Tempo de execucao da Query 5: %.6f segundos\n", total_time_query5);
        //printf("Tempo de execucao da Query 6: %.6f segundos\n", total_time_query6);
        double total_time = (total_time_query1 + total_time_query2 + total_time_query3 + total_time_query4 + total_time_query5)/5;
        printf("Tempo medio de execucao de cada query: %.6f segundos\n", total_time);
    }
}