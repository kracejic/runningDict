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
};


struct Change
{
    ChangeType changeType;
    std::string word;
    std::string translation;

    Change(ChangeType _type, std::string _word, std::string _translation = "")
        : changeType(_type)
        , word(_word)
        , translation(_translation){};
};

class Dict
{
  private:
    std::shared_ptr<const std::string> mContent;
    std::string mFilename{""};
    std::string mName{""};
    int revision{0};

    std::vector<Change> history;

    bool mEnabled{true};
    bool mIsLoaded{false};
    bool mErrorState{false};
    bool mIsSynchronized{false};

  public:
    bool mReadOnly{false};
    bool mOnline{false};

    bool isReady()
    {
        return mIsLoaded && (mIsSynchronized || !mOnline) && !mErrorState;
    };


  public:
    Dict();
    Dict(const std::string& filename, int bonus = 0, bool enabled = true);

    /// Fill dictionary with custom string, usefull for testing
    void fill(const std::string& content);

    bool hasWord(const std::string& word) const;

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
    bool isEnabled();
    bool enable(bool state = true);
    bool isLoaded()
    {
        return mIsLoaded;
    };
    bool toogle_enable();
    void setFileName(const std::string& name);
    const std::string& getFilename() const;
    void setName(const std::string& name);
    const std::string& getName() const;
    int getRevision() const
    {
        return revision;
    };
    bool isDirty()
    {
        return history.size() > 0;
    };

    int mBonus{0}; ///< Lower means higher

    // Helper function for tests TODO wip
    bool operator==(const Dict& d) const;
    bool operator!=(const Dict& d) const;
    // std::string translationOfWord(const std::string& word) const;
    bool checkTranslationOfWord(const std::string& word, const std::string& translation) const;

  private:
    bool _addWord(const std::string& word, const std::string& translation);
    bool _deleteWord(const std::string& word);

    bool synchronizeHistory(const std::string& serverUrl);
    bool oneWayEqualityCheck(const Dict& d1, const Dict& d2) const;
};
