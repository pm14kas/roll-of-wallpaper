#include <QString>
#include <QtTest>
#include <string>
#include "interval.h"
#include "parser.h"

class RowTest : public QObject
{
    Q_OBJECT

public:
    RowTest();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void testCaseIntervalCreation();
    void testCaseIntervalArithmetic();
    void testCaseIntervalString();


    void testCaseParserArithmetic();
    void testCaseParserFunctions();
};

RowTest::RowTest()
{
}

void RowTest::initTestCase()
{
}

void RowTest::cleanupTestCase()
{
}

void RowTest::testCaseIntervalCreation()
{
    QCOMPARE(Interval(5, 6), Interval(6, 5));
    QCOMPARE(Interval(4), Interval(4, 4));
}

void RowTest::testCaseIntervalArithmetic()
{
    QCOMPARE(Interval(5, 6) + Interval(8, 12), Interval(13, 18));
    QCOMPARE(Interval(5, 6) * Interval(8, 12), Interval(40, 72));
    QCOMPARE(Interval(-5, 6) * Interval(8, 12), Interval(-60, 72));
}

void RowTest::testCaseIntervalString()
{
    std::string holder1;
    Interval interval1 = Interval(499, 562);
    holder1 = interval1.toString();
    QCOMPARE(holder1, std::string("[499;562]"));
    QCOMPARE(Interval::fromString(holder1), interval1);

    std::string holder2;
    Interval interval2 = Interval(-45.658, 0.99983);
    holder2 = interval2.toString();
    QCOMPARE(holder2, std::string("[-45.658;0.99983]"));
    QCOMPARE(Interval::fromString(holder2), interval2);

    try {
        Interval interval3(std::string("[314;]"));
    }
    catch(Interval::Exception e){
        QCOMPARE(e, Interval::ExceptionParseStructure);
    }

    try {
        Interval interval4(std::string("[14+2;2test]"));
    }
    catch(Interval::Exception e){
        QCOMPARE(e, Interval::ExceptionParseNumber);
    }

}

void RowTest::testCaseParserArithmetic()
{
    Parser parserPlus("5+3+0");
    Parser parserMinus("5-3-2");
    Parser parserMultiply("2*(2+2)*2");
    Parser parserDivide("10/5");

    std::vector<KeyValue> varmap;

    QCOMPARE(parserPlus.calc(varmap), 8.0);
    QCOMPARE(parserMinus.calc(varmap), 0.0);
    QCOMPARE(parserMultiply.calc(varmap), 16.0);
    QCOMPARE(parserDivide.calc(varmap), 2.0);

}
void RowTest::testCaseParserFunctions()
{
    Parser parserSin("sin(pi/2)");
    Parser parserCos("cos(0)");
    Parser parserSaraev("e^abs(-5)");
    Parser parserSaraevCheck("e^5");

    std::vector<KeyValue> varmap;

    QCOMPARE(parserSin.calc(varmap), 1.0);
    QCOMPARE(parserCos.calc(varmap), 1.0);
    QCOMPARE(parserSaraev.calc(varmap), parserSaraevCheck.calc(varmap));
}


QTEST_APPLESS_MAIN(RowTest)

#include "tst_row-test.moc"
