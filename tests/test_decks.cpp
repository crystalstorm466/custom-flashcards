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
#include <QtCore>
#include <QtTest>
#include <qbenchmark.h>
#include <qjsonobject.h>
#include <qtestcase.h>
#include <random>
#include <QtPdf/QPdfDocument>

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

    void testEmptyDeck() {
        QTemporaryFile emptyFil;
        QTemporaryFile emptyPdfFil;
        MainWindow m_mainWin;

        QVERIFY(emptyFil.open());
        emptyFil.close();
        Deck emptyDeck;
        emptyDeck.loadFromFile(emptyFil.fileName());
        QCOMPARE(emptyDeck.count(), 0);
        QBENCHMARK { //try exporting empty deck to json
            emptyDeck.loadFromFile(emptyFil.fileName());
            m_mainWin.setDeckForTesting(emptyDeck);
            bool success = m_mainWin.onExportDecktoJson(emptyFil.fileName());
            QVERIFY(success);
        }
        QBENCHMARK { //try exporting empty deck to pdf
            m_mainWin.setDeckForTesting(emptyDeck);
            bool success = m_mainWin.exportPDFToFile(emptyPdfFil.fileName());
            QVERIFY(success);
            QPdfDocument pdfDoc;
            QCOMPARE(pdfDoc.load(emptyPdfFil.fileName()) == QPdfDocument::Error::None, false);
            int expectedPages = 0;
            qInfo() << "PDF should have " << expectedPages;
            QVERIFY(pdfDoc.pageCount() == 0);
            QCOMPARE(pdfDoc.pageCount(), expectedPages);

            QFileInfo fileInfo(emptyPdfFil.fileName());
            QCOMPARE(fileInfo.size() < 0, false);
        }
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
        QTemporaryFile tempPDFFile;
        QVERIFY(tempFile.open());
        QVERIFY(tempPDFFile.open());
        tempFile.write(createLargeDeck().toJson());
        tempFile.close();

        Deck testDeck;
        MainWindow m_mainWin;
        QBENCHMARK { //try exporting deck to json
            testDeck.loadFromFile(tempFile.fileName());
            m_mainWin.setDeckForTesting(testDeck);
            bool success = m_mainWin.onExportDecktoJson(tempFile.fileName());
            QVERIFY(success);
        }
        QBENCHMARK { //try exporting deck to pdf
            testDeck.loadFromFile(tempFile.fileName());
            m_mainWin.setDeckForTesting(testDeck);
            bool success = m_mainWin.exportPDFToFile(tempPDFFile.fileName());
            QVERIFY(success);
            QPdfDocument pdfDoc;
            QCOMPARE(pdfDoc.load(tempPDFFile.fileName()), QPdfDocument::Error::None );
            int expectedPages = (CARD_LENGTH + (6-1) ) / 6;
            qInfo() << "PDF should have " << expectedPages;
            QVERIFY(pdfDoc.pageCount() >= expectedPages);


            QFileInfo fileInfo(tempPDFFile.fileName());
            QVERIFY(fileInfo.exists());
            QVERIFY(fileInfo.size() > 0);
        }
    }

    void singleCard() {
        QTemporaryFile pdfFile;
        QTemporaryFile jsonFile;
        QVERIFY(pdfFile.open());
        QVERIFY(jsonFile.open());
        jsonFile.close();
        pdfFile.close();

        Deck testDeck;
        testDeck.addCard(Flashcard("Single card Q", "Single card A"));

        MainWindow m_main;
        m_main.setDeckForTesting(testDeck);

        bool success = m_main.exportPDFToFile(pdfFile.fileName());
        bool json_suc = m_main.onExportDecktoJson(jsonFile.fileName());
        QVERIFY(success);
        QVERIFY(json_suc);

        QPdfDocument pdfDoc;
        QCOMPARE(pdfDoc.load(pdfFile.fileName()), QPdfDocument::Error::None );
        QCOMPARE(pdfDoc.pageCount(), 1);
    }

    void testCorruptedjson() {
        QTemporaryFile corruptFile;
        QVERIFY(corruptFile.open());
        corruptFile.write("{ \"title\": \"Broken Deck\", \"cards\": [ { \"front\": \"unclosed } ]");
        corruptFile.close();

        Deck testDeck;
        bool loaded = testDeck.loadFromFile(corruptFile.fileName());
        QCOMPARE(loaded, false);
        QCOMPARE(testDeck.count(),0 );
    }
};

QTEST_MAIN(DeckTest)
#include "test_decks.moc"
