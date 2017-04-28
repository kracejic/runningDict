#pragma once
#include <fstream>
#include <future>
#include <memory>
#include <string>
#include <vector>

enum class ChangeType
{
    addWord,
    deleteWord,
    changeWord
};

struct Change
{
    ChangeType changeType;
    std::string word;
    std::string wordNew{""};
    std::string translation;

    Change(ChangeType _type, std::string _word, std::string _translation = "",
        std::string _wordNew = "")
        : changeType(_type)
        , word(_word)
        , wordNew(_wordNew)
        , translation(_translation){};
};

class Dict
{
  private:
    bool mIs_open{false};
    std::shared_ptr<const std::string> mContent;
    std::string mFilename{""};
    std::string mName{""};
    bool mEnabled{true};

    std::vector<Change> history;
    int revision{0};

  public:
    Dict();
    Dict(const std::string& filename, int bonus = 0, bool enabled = true);

    /// Fill dictionary with custom string, usefull for testing
    void fill(const std::string& content);

    bool hasWord(const std::string& word);

    /**
     * Add new word to dictionary.
     * This causes write to harddrive.
     * @return             was this succesfull?
     */
    bool addWord(const std::string& word, const std::string& translation);

    /**
     * Changes the word in dictionary.
     * If wordNew is empty (""), the translated word, remains the same.
     * If wordNew is not empty translated word is replaced.
     */
    bool changeWord(const std::string& word, const std::string& newTranslation,
        const std::string& wordNew = "");

    bool deleteWord(const std::string& word);


    bool open(const std::string& filename);
    bool reload();
    void saveDictionary();

    bool sync(const std::string& serverUrl);
    std::future<bool> deleteFromServer(const std::string& serverUrl);

    // content getters
    std::shared_ptr<const std::string> getContens() const;
    long long getContensSize() const;

    // getters, setters
    bool is_enabled();
    bool enable(bool state = true);
    bool toogle_enable();
    const std::string& getFilename() const;
    void setName(const std::string& name);
    const std::string& getName() const;

    int mBonus{0}; ///< Lower means higher
    bool mOnline{false};
    bool mReadOnly{false};
    bool mErrorState{false};

  private:
    bool _addWord(const std::string& word, const std::string& translation);
    bool _changeWord(const std::string& word, const std::string& newTranslation,
        const std::string& wordNew = "");
    bool _deleteWord(const std::string& word);

    bool synchronizeHistory(const std::string& serverUrl);
};
