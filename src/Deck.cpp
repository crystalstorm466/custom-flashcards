#include "Deck.h"
#include "Flashcard.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

Deck::Deck(const QString& title)
    : m_title(title)
{
}

QString Deck::title() const {
    return m_title;
}

void Deck::setTitle(const QString& title) {
    m_title = title;
}

const QList<Flashcard>& Deck::cards() const {
    return m_cards;
}

int Deck::count() const {
    return m_cards.size();
}

const Flashcard& Deck::at(int index) const {
    return m_cards.at(index);
}

void Deck::addCard(Flashcard card) {
    if (card.getIndex() <= 0) {
        card.setIndex(m_cards.size() + 1);
    }
    m_cards.append(card);
}

bool Deck::removeCard(int index) {
    if (index >= 0 && index < m_cards.size()) {
        m_cards.removeAt(index);
        reindexCards();
        return true;
    }
    return false;
}

void Deck::reindexCards() {
    for (int i = 0; i < m_cards.size(); ++i) {
        m_cards[i].setIndex(i + 1);
    }
}

void Deck::clear() {
    m_cards.clear();
}

bool Deck::saveToFile(const QString& filePath, QString* errorMessage) const {
    QJsonObject rootObject;
    rootObject["title"] = m_title;

    QJsonArray cardsArray;
    for (const auto& card : m_cards) {
        cardsArray.append(card.toJson());
    }
    rootObject["cards"] = cardsArray;

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        if (errorMessage) {
            *errorMessage = file.errorString();
        }
        return false;
    }

    QJsonDocument doc(rootObject);
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
    return true;
}

bool Deck::loadFromFile(const QString& filePath, QString* errorMessage) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        if (errorMessage) {
            *errorMessage = file.errorString();
        }
        return false;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        if (errorMessage) {
            *errorMessage = parseError.errorString();
        }
        return false;
    }

    if (!doc.isObject()) {
        if (errorMessage) {
            *errorMessage = "Invalid JSON structure: root must be an object.";
        }
        return false;
    }

    QJsonObject root = doc.object();
    m_title = root.value("title").toString("Imported Deck");
    m_cards.clear();

    QJsonArray cardsArray = root.value("cards").toArray();
    for (const auto& cardVal : cardsArray) {
        if (cardVal.isObject()) {
            m_cards.append(Flashcard::fromJson(cardVal.toObject()));
        }
    }
    reindexCards();

    return true;
}

bool Deck::appendFromFile(const QString& filePath, QString* errorMessage) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        if (errorMessage) {
            *errorMessage = file.errorString();
        }
        return false;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        if (errorMessage) {
            *errorMessage = parseError.errorString();
        }
        return false;
    }

    if (!doc.isObject()) {
        if (errorMessage) {
            *errorMessage = "Invalid JSON structure: root must be an object.";
        }
        return false;
    }

    QJsonObject root = doc.object();
    QJsonArray cardsArray = root.value("cards").toArray();
    for (const auto& cardVal : cardsArray) {
        if (cardVal.isObject()) {
            m_cards.append(Flashcard::fromJson(cardVal.toObject()));
        }
    }
    reindexCards();

    return true;
}
