#include "Flashcard.h"

Flashcard::Flashcard()
    : m_front("")
    , m_back("")
{
}

Flashcard::Flashcard(const QString& front, const QString& back)
    : m_front(front)
    , m_back(back)
{
}

QString Flashcard::front() const {
    return m_front;
}

void Flashcard::setFront(const QString& front) {
    m_front = front;
}

QString Flashcard::back() const {
    return m_back;
}

void Flashcard::setBack(const QString& back) {
    m_back = back;
}

QJsonObject Flashcard::toJson() const {
    QJsonObject json;
    json["front"] = m_front;
    json["back"] = m_back;
    return json;
}

Flashcard Flashcard::fromJson(const QJsonObject& json) {
    Flashcard card;
    if (json.contains("front") && json["front"].isString()) {
        card.setFront(json["front"].toString());
    }
    if (json.contains("back") && json["back"].isString()) {
        card.setBack(json["back"].toString());
    }
    return card;
}
