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


class DeckTest : public QObject {
    Q_OBJECT

private slots:

    void testInitalState() {
        Deck m_deck;

        m_deck.addCard(Flashcard("test question", "test answer"));
        QCOMPARE(m_deck.count(), 1);
    }
};

QTEST_MAIN(DeckTest)
#include "test_jsonImports.moc"
