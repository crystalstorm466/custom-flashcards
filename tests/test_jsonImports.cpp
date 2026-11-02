#include "../src/vimBinds.h"
#include "../src/MainWindow.h"
#include "../src/Deck.h"


#include <QApplication>
#include <QKeyEvent>
#include <QStatusBar>
#include <QTabWidget>
#include <QListWidget>
#include <QLineEdit>
#include <QDebug>
#include <QtTest>
#include <qbenchmark.h>
#include <qjsonobject.h>
#include <random>

class DeckTest : public QObject {
    Q_OBJECT

private slots:

    void testInitalState() {
        Deck m_deck;

        m_deck.addCard(Flashcard("test question", "test answer"));
        QCOMPARE(m_deck.count(), 1);
    }

    void testLargeJsonImport() {
        const int CARD_LENGTH = 1000000;
        QTemporaryFile tempFile;
        QVERIFY(tempFile.open());

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> lengthDist(10, 50);

        const QString chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 ";
        QJsonObject rootObject;
        QJsonArray cardsArray;
        rootObject["title"] = "RNG stress test";
        for (int i = 0; i < CARD_LENGTH; ++i) {
            int frontLen = lengthDist(gen);
            int backLen = lengthDist(gen) * 3;

            QString randomFront;
            for (int j = 0; j < frontLen; ++j) {
                randomFront.append(chars[gen() % chars.length()]);
            }
            QString randomBack;
            for (int j = 0; j < backLen; ++j) {
                randomBack.append(chars[gen() % chars.length()]);
            }
            QJsonObject card;
            card["front"] = randomFront;
            card["back"] = randomBack;
            card["index"] = i;
            cardsArray.append(card);

        }
        rootObject["cards"] = cardsArray;

        QJsonDocument doc(rootObject);
        tempFile.write(doc.toJson());
        tempFile.close();

        Deck testDeck;

        QBENCHMARK {
            testDeck.loadFromFile(tempFile.fileName());
        }

        QCOMPARE(testDeck.count(), CARD_LENGTH);
    }
};

QTEST_MAIN(DeckTest)
#include "test_jsonImports.moc"
