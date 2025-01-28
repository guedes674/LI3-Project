#include "../../include/parser/parser_history.h"
#define MAX_FILENAME 1024
#define MAX_LINE 2048

//função que dá parse ao ficheiro de histórico.
void parse_history(char* path, GestorHistory* gestorHistory, GestorMusic* gestorMusic, GestorArtist* gestorArtist){
    parse_csv(path, "history.csv", gestorHistory, gestorMusic, gestorArtist, process_history_line, 4);
}

//função que processa uma linha do hsitórico.
void process_history_line(char* line, void* gestor, void* aux_dataX, void* aux_dataY){
    GestorMusic* gestorMusic = (GestorMusic*)aux_dataX;
    GestorArtist* gestorArtist = (GestorArtist*)aux_dataY;
    GestorHistory* gestorHistory = (GestorHistory*)gestor;

    char original_line[MAX_LINE];
    strcpy(original_line, line);
    char* tmp_line = line;

    char *id_str, *user_id_str, *music_id_str, *timestamp, *duration_str, *platform_str;
    long int id, user_id, music_id;
    Platform platform;

    id_str = remove_aspas(strsep(&tmp_line, ";"));
    if (!id_str || strlen(id_str) == 0){
        writeErrors(original_line, 1);
        return;
    }
    id = strtol(id_str + 1, NULL, 10);

    user_id_str = remove_aspas(strsep(&tmp_line, ";"));
    music_id_str = remove_aspas(strsep(&tmp_line, ";"));
    timestamp = remove_aspas(strsep(&tmp_line, ";"));
    duration_str = remove_aspas(strsep(&tmp_line, ";"));
    platform_str = remove_aspas(strsep(&tmp_line, "\n"));

    //validações.
    if(!user_id_str || !music_id_str || !timestamp || !duration_str || !platform_str || isValidPlatform(platform_str) == 0){
        writeErrors(original_line, 4);
        free(id_str);
        free(user_id_str);
        free(music_id_str);
        free(timestamp);
        free(duration_str);
        free(platform_str);
        return;
    }
    //gerar a chave da semana.
    char* week_key = getWeekKey(timestamp);

    //conversões.
    user_id = strtol(user_id_str + 1, NULL, 10);
    music_id = strtol(music_id_str + 1, NULL, 10);
    platform = stringToPlatform(platform_str);
    int duration_int = duration_to_seconds(duration_str);

    History* h = createHistory(id, user_id, music_id, timestamp, duration_str, platform);
    addHistory(gestorHistory, h);
    
    Music* m = searchMusic(gestorMusic, music_id);
    // Dados utilizados para o calculo da receita de cada artista
    long int* artists_ids = getMusicArtistIDs(m);
    int num_artists = getMusicNumArtists(m);
    for(int i = 0; i < num_artists; i++){
        long int artist_id = artists_ids[i];
        Artist* a = searchArtist(gestorArtist, artist_id);
        ArtistType type = getArtistType(a);
        addArtistDurationWeek(gestorHistory, week_key, artist_id, duration_int, type);
        int count = getMusicReps(gestorArtist, artist_id);
        count++;
        updateMusicReps(gestorArtist, artist_id, count);
    }

    // Dados utilizados para o pre-calculo dos generos ouvidos de cada utilizador
    char* genre = getMusicGenre(m);

    addGenresListened(gestorHistory, user_id_str, genre);

    free(genre);
    free(id_str);
    free(user_id_str);
    free(music_id_str);
    free(timestamp);
    free(duration_str);
    free(platform_str);
    free(week_key);
}