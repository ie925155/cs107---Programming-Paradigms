#include <vector>
#include <list>
#include <set>
#include <map>
#include <string>
#include <iostream>
#include <iomanip>
#include "imdb.h"
#include "path.h"
using namespace std;

/**
 * Using the specified prompt, requests that the user supply
 * the name of an actor or actress.  The code returns
 * once the user has supplied a name for which some record within
 * the referenced imdb existsif (or if the user just hits return,
 * which is a signal that the empty string should just be returned.)
 *
 * @param prompt the text that should be used for the meaningful
 *               part of the user prompt.
 * @param db a reference to the imdb which can be used to confirm
 *           that a user's response is a legitimate one.
 * @return the name of the user-supplied actor or actress, or the
 *         empty string.
 */

enum Direction { LEFT, RIGHT };

static string promptForActor(const string& prompt, const imdb& db, vector<film>& credits)
{
  string response;
  while (true) {
    cout << prompt << " [or <enter> to quit]: ";
    getline(cin, response);
    if (response == "") return "";
    if (db.getCredits(response, credits)) return response;
    cout << "We couldn't find \"" << response << "\" in the movie database. "
	 << "Please try again." << endl;
  }
}

static bool generateShortestPath(const imdb& db, string& source, string& target, Direction d)
{
  cout << "generateShortestPath start" << endl;
  cout << "source " << source << " target " << target << endl;
  list<path> partialPaths;
  set<string> previouslySeenActors;
  set<film> previouslySeenFilms;
  vector<film> credits;
  vector<string> casts;
  //for speed up
  map<string, vector<film> > cachedSearchedPlayer;
  map<string, vector<film> >::iterator it;
  map<film, vector<string> > cachedSearchedMovie;
  map<film, vector<string> >::iterator itt;
  if(d == RIGHT){
    string temp = source;
    source = target;
    target = temp;
  }
  path p(source);
  cout << "from " << p.getLastPlayer() << " side search" << endl;
  partialPaths.push_back(p);
  while(partialPaths.size() > 0 && partialPaths.front().getLength() <= 5){
    path p = partialPaths.front();
    partialPaths.pop_front();
    it = cachedSearchedPlayer.find(p.getLastPlayer());
    if(it == cachedSearchedPlayer.end()){
      db.getCredits(p.getLastPlayer(), credits);
      cachedSearchedPlayer.insert( pair<string, vector<film> >(p.getLastPlayer(), credits) );
    }else{ //use cached value
      cout << "use cached player value" << endl;
      credits = it->second;
    }
    for(vector<film>::iterator movie = credits.begin() ; movie != credits.end() ; ++movie){
      //cout << "movie=" << (*movie).title << endl;
      //the movie you've not seen before
      if(previouslySeenFilms.find(*movie) == previouslySeenFilms.end()){
         previouslySeenFilms.insert(*movie);
         itt = cachedSearchedMovie.find(*movie);
         if(itt == cachedSearchedMovie.end()){
           db.getCast(*movie, casts);
           cachedSearchedMovie.insert( pair<film, vector<string> >(*movie, casts) );
         }else{
           cout << "use cached movie value" << endl;
           casts = itt->second;
         }
         for(vector<string>::iterator costar = casts.begin() ; costar != casts.end() ; ++costar){
           //cout << "costar=" << *costar << endl;
           //the cast you've not seen before
           if(previouslySeenActors.find(*costar) == previouslySeenActors.end()){
             previouslySeenActors.insert(*costar);
             path p2 = p;
             p2.addConnection(*movie, *costar);
             if((*costar).compare(target) == 0){
               if(d == LEFT){
                 cout << p2 << endl;
               }else{
                 p2.reverse();
                 cout << "reverting" << endl;
                 cout << p2 << endl;
               }
               return true;
             }
             partialPaths.push_back(p2);
           }
         }
      } //end of if(previouslySeenFilms.find(*movie) == previouslySeenFilms.end())
    }
    credits.clear();
    casts.clear();
  } //end of while
  return false;
}
/**
 * Serves as the main entry point for the six-degrees executable.
 * There are no parameters to speak of.
 *
 * @param argc the number of tokens passed to the command line to
 *             invoke this executable.  It's completely ignored
 *             here, because we don't expect any arguments.
 * @param argv the C strings making up the full command line.
 *             We expect argv[0] to be logically equivalent to
 *             "six-degrees" (or whatever absolute path was used to
 *             invoke the program), but otherwise these are ignored
 *             as well.
 * @return 0 if the program ends normally, and undefined otherwise.
 */

int main(int argc, const char *argv[])
{
  imdb db(determinePathToData(argv[1])); // inlined in imdb-utils.h
  if (!db.good()) {
    cout << "Failed to properly initialize the imdb database." << endl;
    cout << "Please check to make sure the source files exist and that you have permission to read them." << endl;
    //exit(1);
  }

  vector<film> credits;
  int value1, value2;
  while (true) {
    string source = promptForActor("Actor or actress", db, credits);
    if (source == "") break;
    value1 = credits.size();
    credits.clear();
    string target = promptForActor("Another actor or actress", db, credits);
    if (target == "") break;
    value2 = credits.size();
    credits.clear();
    if (source == target) {
      cout << "Good one.  This is only interesting if you specify two different people." << endl;
    } else {
      // replace the following line by a call to your generateShortestPath routine...
      Direction d = (value1 >= value2) ? LEFT : RIGHT;
	    if(!generateShortestPath(db, source, target, d))
        cout << endl << "No path between those two people could be found." << endl << endl;
    }
  }

  cout << "Thanks for playing!" << endl;
  return 0;
}
