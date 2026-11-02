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

class FirstTest : public QObject {
    Q_OBJECT

private:
    bool myCondition() {
        return true;
    }

private slots:
    void initTestCase() {
        qDebug("called before everything else");
    }

    void myFirstTest() {
        QVERIFY(true);
        QCOMPARE(1, 1);
    }

    void mySecondTest() {
        QVERIFY(myCondition());
        QVERIFY(1 != 2);

        QString str = "hello";
        QCOMPARE(str.toUpper(), QString("HELLO"));
    }

    void cleanupTestCase() {
        qDebug("called after myFirstTest and mySecondTest");
    }
};

QTEST_MAIN(FirstTest)

#include "unitTests.moc"
