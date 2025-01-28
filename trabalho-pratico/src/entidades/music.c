#include "../../include/entidades/music.h"

// Forma de enunciar a entidade música.
struct music {
    long int id;
    long int* artist_id;
    int num_artists;
    long int album_id;
    char* duration;
    char* genre;
    int year;
};

// Função para criar uma estrutura da entidade música parametrizada.
Music* createMusic(long int id, long int* artist_id, int num_artists, long int album_id, char* duration, char* genre, int year){
    Music* music = (Music*)malloc(sizeof(Music));
    if(!music){
        perror("erro ao alocar memória para a música.\n");
        exit(EXIT_FAILURE);
    }

    music -> id = id;

    music -> artist_id = malloc(num_artists * sizeof(long int));
    if(!music -> artist_id){
        perror("erro ao alocar memória para a lista de artistas.\n");
        free(music);
        exit(EXIT_FAILURE);
    }

    for(int i = 0; i < num_artists; i++){
        music -> artist_id[i] = artist_id[i];
    }
    music -> num_artists = num_artists;

    music -> album_id = album_id;

    music -> duration = malloc(strlen(duration) + 1);
    if(!music -> duration){
        perror("erro ao alocar memória para a duracao.\n");
        free(music -> artist_id);
        free(music);
        exit(EXIT_FAILURE);
    }
    strcpy(music -> duration, duration);
    
    music -> genre = malloc(strlen(genre) + 1);
    if(!music -> genre){
        perror("erro ao alocar memória para o género da música.\n");
        free(music -> artist_id);
        free(music -> duration);
        free(music);
        exit(EXIT_FAILURE);
    }
    strcpy(music -> genre, genre);

    music -> year = year;

    return music;
}

// GETTERS

// Função que retorna o ID da música
long int getMusicID(Music* m){
    return m->id;
}

// Função que retorna o array de IDs de artistas da música
long int* getMusicArtistIDs(Music* m){
    return m->artist_id;
}

// Função que retorna o número de artistas da música
int getMusicNumArtists(Music* m){
    return m->num_artists;
}

//Função que retorna o ID do album ao qual a música pertence
long int getMusicAlbumId(Music* m){
    return m->album_id;
}

// Função que retorna a duração da música
char* getMusicDuration(Music* m){
    return m->duration ? strdup(m->duration) : NULL;
}

// Função que retorna o gênero da música
char* getMusicGenre(Music* m){
    return m->genre ? strdup(m->genre) : NULL;
}

// Função que retorna o ano da música
int getMusicYear(Music* m){
    return m->year;
}

// SETTERS

// Função que altera o ID da música
void setMusicID(Music* m, long int newID){
    m->id = newID;
}

// Função que altera o array de ID's de artistas da música
void setMusicArtistIDs(Music* m, long int* newArtistID, int newNumArtists){
    if(m->artist_id){
        free(m->artist_id);
        m->artist_id = NULL;
    }        
    m->artist_id = (long int*)malloc(newNumArtists * sizeof(long int));
    if (m->artist_id == NULL){
        perror("Erro ao alocar memoria para o novo array de artistas da musica");
        exit(EXIT_FAILURE); 
    }
    memcpy(m->artist_id, newArtistID, newNumArtists * sizeof(long int));
    m->num_artists = newNumArtists;
}

// Função que altera o ID do album ao qual a música pertence.
void setMusicAlbumId(Music* m, long int newAlbumId){
    m->album_id = newAlbumId;
}

// Função que altera a duração da música
void setMusicDuration(Music* m, char* newDuration){
    if(m->duration){
        free(m->duration);
        m->duration = NULL;
    }        
    m->duration = strdup(newDuration);
    if(m->duration == NULL){
        perror("Erro ao alocar memoria para a nova duracao da musica");
        exit(EXIT_FAILURE);
    }
}

// Função que altera o género da música
void setMusicGenre(Music* m, char* newGenre){
    if(m->genre){
        free(m->genre);
        m->genre = NULL;
    }        
    m->genre = strdup(newGenre);
    if(m->genre == NULL){
        perror("Erro ao alocar memoria para o novo genero da musica");
        exit(EXIT_FAILURE);
    }
}

// Função que altera o ano da música
void setMusicYear(Music* m, int newYear){
    m->year = newYear;
}

// Função para libertar a memória de uma entidade do tipo música.
void freeMusic(Music* music){
    free(music -> artist_id);
    free(music -> duration);
    free(music -> genre);
    free(music);
}

// Função para libertar a memória de uma entidade do tipo música contida numa hash table
void freeMusicInTable(gpointer value){
    Music* music = (Music*)value;
    free(music -> artist_id);
    free(music -> duration);
    free(music -> genre);
    free(music);
}