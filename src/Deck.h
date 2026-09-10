#pragma once

#include "Flashcard.h"
#include <QList>
#include <QString>

/**
 * @brief Represents a collection of Flashcards, with support for saving/loading to JSON files.
 */
class Deck {
public:
    Deck(const QString& title = "My Deck");

    QString title() const;
    void setTitle(const QString& title);

    const QList<Flashcard>& cards() const;
    int count() const;
    const Flashcard& at(int index) const;

    void addCard(const Flashcard& card);
    bool removeCard(int index);
    void clear();

    // Export / Save to a JSON file
    bool saveToFile(const QString& filePath, QString* errorMessage = nullptr) const;

    // Import / Load: overwrite current deck with contents from file
    bool loadFromFile(const QString& filePath, QString* errorMessage = nullptr);

    // Import / Append: add cards from file into existing deck without overwriting
    bool appendFromFile(const QString& filePath, QString* errorMessage = nullptr);

private:
    QString m_title;
    QList<Flashcard> m_cards;
};
