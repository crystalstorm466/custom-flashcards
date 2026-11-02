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
#include <qtestcase.h>
#include <random>

class DeckTest : public QObject {
    Q_OBJECT

private:
    static constexpr int CARD_LENGTH = 10000;

    QJsonDocument createLargeDeck() {


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

        return QJsonDocument(rootObject);


    }

private slots:

    void testInitalState() {
        Deck m_deck;
        m_deck.addCard(Flashcard("test question", "test answer"));
        QCOMPARE(m_deck.count(), 1);
    }

    void testLargeJsonImport() {
        QTemporaryFile tempFile;
        QVERIFY(tempFile.open());
        tempFile.write(createLargeDeck().toJson());
        tempFile.close();

        Deck testDeck;
        QBENCHMARK {
            std::random_device rd;
            std::mt19937 gen(rd());
            testDeck.loadFromFile(tempFile.fileName());
            testDeck.removeCard(gen() % CARD_LENGTH);
            testDeck.removeCard(gen() % CARD_LENGTH);

            testDeck.reindexCards();

        }

        QCOMPARE(testDeck.count(), CARD_LENGTH-2);


    }

    void testRemoveJson() {
        QTemporaryFile tempFile;
        QVERIFY(tempFile.open());
        tempFile.write(createLargeDeck().toJson());
        tempFile.close();

        Deck testDeck;

        QBENCHMARK {
            testDeck.loadFromFile(tempFile.fileName());
            testDeck.clear();
            testDeck.loadFromFile(tempFile.fileName());
            testDeck.appendFromFile(tempFile.fileName());
        }
        QCOMPARE(testDeck.count(), CARD_LENGTH * 2);
    }

    void exportJson() {
        QTemporaryFile tempFile;
        QVERIFY(tempFile.open());
        tempFile.write(createLargeDeck().toJson());
        tempFile.close();

        Deck testDeck;
        MainWindow m_mainWin;
        QBENCHMARK {
            testDeck.loadFromFile(tempFile.fileName());
            m_mainWin.setDeckForTesting(testDeck);
            m_mainWin.onExportPDF();
        }
    }
};

QTEST_MAIN(DeckTest)
#include "test_jsonImports.moc"
