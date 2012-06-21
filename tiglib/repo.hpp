#ifndef __TIGLIB_REPO_HPP_
#define __TIGLIB_REPO_HPP_

#include "gamedata.hpp"
#include "misc/lockfile.hpp"
#include <stdint.h>
#include "misc/jconfig.hpp"

namespace TigLib
{
  class Repo
  {
    std::string dir;
    std::string listFile, tigDir;
    Misc::JConfig conf;
    Misc::LockFile lock;
    TigLib::GameData data;

    int64_t lastTime;

  public:
    Misc::JConfig inst, news, rates;
    /* Find or establish a repository in the given location. An empty
       path means we should use the standard path for this OS. This
       will also look for repositories in legacy locations from older
       versions of Tiggit.

       Returns true on success, false if the repository is not usable
       (eg. if the path is not writable), or if no standard path was
       found.

       If it fails, you may prompt the user for a new location and try
       again.
     */
    bool findRepo(const std::string &where = "");

    /* Initialize repository. This includes creating a lock file and
       loading configuration. The function will also convert legacy
       data from older versions of Tiggit.

       Returns true on success, false if the locking failed.

       A 'false' return value usually either means that another
       process is using the repository, or that a process crashed
       while the repository was locked. The best course of action is
       to ask the user if they want to override the lock (call again
       with forceLock=true), and at the same time warn them about the
       possible consequences of this.

       Note that a forced lock may still fail under some
       circumstances.
     */
    bool initRepo(bool forceLock=false);

    /* Update all files from the net.
     */
    void fetchFiles();

    /* Load current game data.
     */
    void loadData();

    // Get the path of a file or directory within the repository. Only
    // valid after findRepo() has been invoked successfully.
    std::string getPath(const std::string &fname);

    // Get install dir for a game
    std::string getInstDir(const std::string &idname)
    { return getPath("games/" + idname); }

    // Main lookup list of all games
    const InfoLookup &getList() { return data.lookup; }

    // Use this to derive other ListBase structs from the main list.
    List::ListBase &baseList() { return data.allList; }

    // The last known TigEntry::addDate of our previous run.
    int64_t getLastTime() const { return lastTime; }

    // Get rating for a given game. Returns -1 if no rating is set.
    int getRating(const std::string &id);

    // Set rating, in range 0-5
    void setRating(const std::string &id, const std::string &urlname,
                   int rate);

    // Set new lastTime. Does NOT change the current lastTime field,
    // but instead stores the value in conf for our next run.
    void setLastTime(int64_t val);

    // Store install status for this game in the config files. Safe to
    // call from worker threads.
    void setInstallStatus(const std::string &idname, int status);

    // Notify us that a download has finished. Will update config
    // files and notify the server counter.
    void downloadFinished(const std::string &idname,
                          const std::string &urlname);
  };
}
#endif
