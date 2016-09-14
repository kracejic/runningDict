#pragma once
#include <fstream>
#include <string>

class Dict
{
  private:
    bool mIs_open{false};
    std::ifstream file;
    std::string mContents;
    std::string mFilename{""};
    bool mEnabled{true};

    bool is_open();

  public:
    Dict();
    Dict(std::string filename);
    Dict(std::string filename, int bonus, bool enabled);

    /**
     * Load the file
     * @param  filename
     * @return          [description]
     */
    bool open(std::string filename);

    /**
     * Is dictionary enabled (thus loaded)?
     */
    bool is_enabled();

    /**
     * Tries to enable or disable dict. Also loads the file if neccessary.
     * @param  state desired state
     * @return       returns end state
     */
    bool enable(bool state=true);

    /**
     * Flips enable state (if possible). Also loads the file if neccessary.
     * @return       returns end state
     */
    bool toogle_enable();

    /**
     * Reloads the content from dict file.
     */
    bool reload();


    /**
     * Add new word to dictionary.
     * This causes write to harddrive.
     *
     * @return             was this succesfull?
     */
    bool addWord(const std::string& word, const std::string& translation);


    void saveDictionary();

    const std::string &getFilename() const;
    const std::string &getContens() const;
    long long getContensSize() const;

    int mBonus{0}; ///< Lower means higher
    bool mErrorState{false};
};
