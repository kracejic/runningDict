#pragma once
#include <fstream>
#include <string>

class Dict
{
  private:
    bool mIs_open{false};
    std::string mContents;
    std::string mFilename{""};
    bool mEnabled{true};

    bool is_open();

  public:
    Dict();
    Dict(std::string filename);
    Dict(std::string filename, int bonus, bool enabled);

    /// Fill dictionary with custom string, usefull for testing
    void fill(std::string contents);

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

    bool hasWord(const std::string& word);
    void changeWord(const std::string& word, const std::string& newTranslation);


    void saveDictionary();

    const std::string &getFilename() const;
    const std::string &getContens() const;
    long long getContensSize() const;

    int mBonus{0}; ///< Lower means higher
    bool mErrorState{false};
};
