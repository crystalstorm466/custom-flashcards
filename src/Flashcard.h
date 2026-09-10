#pragma once

#include <QString>
#include <QJsonObject>

/**
 * @brief Represents a single flashcard with a front (question/prompt) and a back (answer).
 */
class Flashcard {
public:
    Flashcard();
    Flashcard(const QString& front, const QString& back);

    QString front() const;
    void setFront(const QString& front);

    QString back() const;
    void setBack(const QString& back);

    // Serialization to / from JSON
    QJsonObject toJson() const;
    static Flashcard fromJson(const QJsonObject& json);

private:
    QString m_front;
    QString m_back;
};
