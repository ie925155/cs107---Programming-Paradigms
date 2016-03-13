using namespace std;
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "imdb.h"

const char *const imdb::kActorFileName = "actordata";
const char *const imdb::kMovieFileName = "moviedata";

imdb::imdb(const string& directory)
{
  const string actorFileName = directory + "/" + kActorFileName;
  const string movieFileName = directory + "/" + kMovieFileName;

  actorFile = acquireFileMap(actorFileName, actorInfo);
  movieFile = acquireFileMap(movieFileName, movieInfo);
}

bool imdb::good() const
{
  return !( (actorInfo.fd == -1) ||
	    (movieInfo.fd == -1) );
}

static int32_t binarySearchMovie(const string& key, uint8_t *actorInfos, int p1,
  int p2)
{
  //cout << "key=" << key << endl;
  if(p1 > p2) return -1;
  uint32_t mid = (p1 + p2) / 2;
  //printf("mid=%s\n", (char*)actorInfos + *((uint32_t*)(actorInfos+4+4*mid)));
  //printf("strcmp=%d\n", strcmp(key.c_str(), (char*)actorInfos + *((uint32_t*)(actorInfos+4+4*mid))));
  //printf("strcmp=%d\n", strcmp((char*)actorInfos + *((uint32_t*)(actorInfos+4+4*mid)), key.c_str()));
  if(strcmp((char*)actorInfos + *((uint32_t*)(actorInfos+4+4*mid)), key.c_str())
    == 0) return mid;
  if(strcmp((char*)actorInfos + *((uint32_t*)(actorInfos+4+4*mid)),
    key.c_str()) > 0)
      return binarySearchMovie(key, actorInfos, p1, mid-1);
  else
      return binarySearchMovie(key, actorInfos, mid+1, p2);
}

static int32_t binarySearchPlayer(const film& key, uint8_t *movieInfos, int p1,
  int p2)
{
  //cout << "1.film.title=" << key.title << " 2.file.year=" << key.year << endl;
  if(p1 > p2) return -1;
  uint32_t mid = (p1 + p2) / 2;
  //printf("mid title=%s\n", (char*)movieInfos + *((uint32_t*)(movieInfos+4+4*mid)));
  int lengthOfTitle = strlen((char*)movieInfos + *((uint32_t*)(movieInfos+4+4*mid)))+1;
  //printf("lengthOfTitle=%d\n", lengthOfTitle);
  int year = 1900 + *(movieInfos + *((uint32_t*)(movieInfos+4+4*mid)) + lengthOfTitle);
  //cout << "year=" << year << endl;
  //printf("mid year=%d\n", movieInfos + *((uint32_t*)(movieInfos+4+4*mid)));
  //printf("strcmp=%d\n", strcmp(key.title.c_str(), (char*)movieInfos + *((uint32_t*)(movieInfos+4+4*mid))));
  //printf("strcmp=%d\n", strcmp((char*)movieInfos + *((uint32_t*)(movieInfos+4+4*mid)), key.title.c_str()));
  if(strcmp((char*)movieInfos + *((uint32_t*)(movieInfos+4+4*mid)), key.title.c_str())
    == 0 && year == key.year) return mid;
  if(strcmp((char*)movieInfos + *((uint32_t*)(movieInfos+4+4*mid)),
    key.title.c_str()) > 0)
      return binarySearchPlayer(key, movieInfos, p1, mid-1);
  else
      return binarySearchPlayer(key, movieInfos, mid+1, p2);
}
// you should be implementing these two methods right here...
bool imdb::getCredits(const string& player, vector<film>& films) const
{
  int32_t ret;
  uint8_t *actorInfos = (uint8_t*)actorFile;
  uint8_t *movieInfos = (uint8_t*)movieFile;
  uint32_t numberOfActors = *((uint32_t*)actorFile);
  ret = binarySearchMovie(player, actorInfos, 0, numberOfActors);
  if(ret == -1) return false;
  uint32_t playerIndex = *((uint32_t*)(actorInfos+4+4*ret));
  //printf("playerIndex=%u\n", playerIndex);
  uint8_t *playerInfo = actorInfos + playerIndex;
  //printf("player=%s\n", playerInfo);
  short movies;
  int32_t nameFieldLength = strlen((char*)playerInfo)+1;
  if(nameFieldLength%2 == 0){ // even
    movies = *((short*)(playerInfo + nameFieldLength));
  }else{ //add dummy byte
    movies = *((short*)(playerInfo + ++nameFieldLength));
  }
  int32_t moviesFieldLength = sizeof(short);
  moviesFieldLength += (nameFieldLength+moviesFieldLength) % 4;
  //cout << "movies=" << movies << endl;
  //cout << "moviesFieldLength=" << moviesFieldLength << endl;

  //index all actor's movies
  struct film movie;
  for(int i = 0 ; i < movies ; i++){
    uint32_t movieIndex = *((uint32_t*)(playerInfo + nameFieldLength +
      moviesFieldLength + 4*i));
    //printf("%p\n", playerInfo + nameFieldLength + moviesFieldLength + 4*i);
    //cout << "movieIndex=" << movieIndex << endl;
    //printf("%s\n", (char*)movieInfos + movieIndex);
    std::string str((char*)movieInfos + movieIndex);
    movie.title = str;
    int movieTitleLength = strlen((char*)movieInfos+movieIndex)+1;
    //printf("year=%d\n", 1900+*(movieInfos + movieIndex + movieTitleLength));
    movie.year = 1900+*(movieInfos + movieIndex + movieTitleLength);
    films.push_back(movie);
  }
  return true;
}

bool imdb::getCast(const film& movie, vector<string>& players) const
{
  int32_t ret;
  uint8_t *actorInfos = (uint8_t*)actorFile;
  uint8_t *movieInfos = (uint8_t*)movieFile;
  uint32_t numberOfMovies = *((uint32_t*)movieFile);
  ret = binarySearchPlayer(movie, movieInfos, 0, numberOfMovies);
  if(ret == -1) return false;
  uint32_t movieIndex = *((uint32_t*)(movieInfos+4+4*ret));
  uint8_t *movieInfo = movieInfos + movieIndex;
  int lengthOfTitle = strlen((char*)movieInfo) + 1;
  //printf("lengthOfTitle=%d\n", lengthOfTitle);
  int year = 1900 + *(movieInfo + lengthOfTitle);
  //printf("movie title=%s  year=%d\n", movieInfo, year);
  int lengthOfYear = sizeof(uint8_t);
  lengthOfYear += (lengthOfTitle + lengthOfYear) % 2;
  //printf("lengthOfYear=%d\n", lengthOfYear);
  int numberOfActorsPlayed = *((short*)(movieInfo + lengthOfTitle + lengthOfYear));
  //printf("numberOfActorsPlayed=%d\n", numberOfActorsPlayed);
  int lengthOfNumsActor = sizeof(short);
  lengthOfNumsActor += (lengthOfTitle + lengthOfYear + lengthOfNumsActor) % 4;

  for(int i = 0; i < numberOfActorsPlayed; i++){
    uint32_t playerIndex = *((uint32_t*)(movieInfo + lengthOfTitle +
      lengthOfYear + lengthOfNumsActor + 4*i));
    //printf("i=%d, playerIndex=%u\n", i, playerIndex);
    //printf("player=%s\n", (char*)actorInfos + playerIndex);
    std::string str((char*)actorInfos + playerIndex);
    players.push_back(str);
  }
  return true;
}

imdb::~imdb()
{
  releaseFileMap(actorInfo);
  releaseFileMap(movieInfo);
}

// ignore everything below... it's all UNIXy stuff in place to make a file look like
// an array of bytes in RAM..
const void *imdb::acquireFileMap(const string& fileName, struct fileInfo& info)
{
  struct stat stats;
  stat(fileName.c_str(), &stats);
  info.fileSize = stats.st_size;
  info.fd = open(fileName.c_str(), O_RDONLY);
  return info.fileMap = mmap(0, info.fileSize, PROT_READ, MAP_SHARED, info.fd, 0);
}

void imdb::releaseFileMap(struct fileInfo& info)
{
  if (info.fileMap != NULL) munmap((char *) info.fileMap, info.fileSize);
  if (info.fd != -1) close(info.fd);
}
