#pragma once
#include <fstream>
#include <memory>
#include <string>


namespace Dicts
{
bool deleteWord(const std::string& word, const std::string dictname);
bool changeWord(const std::string& word, const std::string& newTranslation,
    const std::string dictname);
} /* Dicts */


class Dict
{
  private:
    bool mIs_open{false};
    std::shared_ptr<const std::string> mContent;
    std::string mFilename{""};
    std::string mName{""};
    bool mEnabled{true};

    bool is_open();

  public:
    Dict();
    Dict(const std::string& filename);
    Dict(const std::string& filename, int bonus, bool enabled);

    /// Fill dictionary with custom string, usefull for testing
    void fill(const std::string& content);

    /**
     * Load the file
     * @param  filename
     * @return          [description]
     */
    bool open(const std::string& filename);

    /**
     * Is dictionary enabled (thus loaded)?
     */
    bool is_enabled();

    /**
     * Tries to enable or disable dict. Also loads the file if neccessary.
     * @param  state desired state
     * @return       returns end state
     */
    bool enable(bool state = true);

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

    /**
     * Changes the word in dictionary.
     * If wordNew is empty (""), the translated word, remains the same.
     * If wordNew is not empty translated word is replaced.
     */
    void changeWord(const std::string& word, const std::string& newTranslation,
        const std::string& wordNew = "");


    void saveDictionary();

    const std::string& getFilename() const;
    const std::string& getName() const;
    std::shared_ptr<const std::string> getContens() const;
    long long getContensSize() const;

    int mBonus{0}; ///< Lower means higher
    bool mErrorState{false};
};
