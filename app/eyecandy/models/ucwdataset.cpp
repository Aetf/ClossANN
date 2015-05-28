#include "ucwdataset.h"
#include "utils/utils.h"
#include <OpenANN/Learner.h>
#include <OpenANN/Preprocessing.h>
#include <OpenANN/util/AssertionMacros.h>
#include <OpenANN/io/DirectStorageDataSet.h>
#include <QVariantMap>
#include <QDebug>
#include <QFile>

#define CSV_IO_NO_THREAD
#include "csv.h"

namespace csv = io;
using namespace OpenANN;
using Eigen::MatrixXd;
using Eigen::VectorXd;
using csv::CSVReader;

UCWDataSet::UCWDataSet(DataSource source)
    : DataSet()
    , inTrainingMode_(true)
    , trainingIn()
    , trainingOut()
    , testingIn()
    , testingOut()
    , trainingData(nullptr)
    , testingData(nullptr)
    , inputRange_()
    , outputRange_()
    , outputLabelCount_(2)
{
    switch (source) {
    case TwoSpirals:
        generateTwoSpirals();
        break;
    default:
        generateNone();
        break;
    }
}

void UCWDataSet::createInternalDataSet()
{
    trainingData = make_unique(new DirectStorageDataSet(&trainingIn, &trainingOut));
    testingData = make_unique(new DirectStorageDataSet(&testingIn, &testingOut));
}

UCWDataSet::~UCWDataSet()
{
}

DirectStorageDataSet &UCWDataSet::dataSet()
{
    return inTrainingMode() ? *trainingData : *testingData;
}

int UCWDataSet::samples()
{
    return dataSet().samples();
}

int UCWDataSet::inputs()
{
    return dataSet().inputs();
}

int UCWDataSet::outputs()
{
    return dataSet().outputs();
}

Eigen::VectorXd& UCWDataSet::getInstance(int i)
{
    return dataSet().getInstance(i);
}

Eigen::VectorXd& UCWDataSet::getTarget(int i)
{
    return dataSet().getTarget(i);
}

void UCWDataSet::finishIteration(Learner& learner)
{
}

range UCWDataSet::inputRange() const
{
    return inputRange_;
}

range UCWDataSet::outputRange() const
{
    return outputRange_;
}

int UCWDataSet::outputLabelCount() const
{
    return outputLabelCount_;
}

bool UCWDataSet::inTrainingMode() const
{
    return inTrainingMode_;
}

void UCWDataSet::inTrainingMode(bool val)
{
    inTrainingMode_ = val;
}

bool UCWDataSet::generateNone()
{
    trainingIn.resize(0, 2);
    trainingOut.resize(0, 1);
    testingIn.resize(0, 2);
    testingOut.resize(0, 1);
    inputRange_ = {0.0, 0.0};
    outputRange_ = {0.0, 0.0};
    outputLabelCount_ = 0;

    createInternalDataSet();
    return true;
}

bool UCWDataSet::generateTwoSpirals(int density, double maxDiameter)
{
    // Number of interior data points per spiral to generate
    const int points = 96 * density;

    trainingIn.resize(points + 1, 2);
    trainingOut.resize(points + 1, 1);
    testingIn.resize(points + 1, 2);
    testingOut.resize(points + 1, 1);
    int trIdx = 0;
    int teIdx = 0;

    for(int i = 0; i <= points; i++)
    {
        // Angle is based on the iteration * PI/16, divided by point density
        const double angle = i * M_PI / (16.0 * density);
        // Radius is the maximum radius * the fraction of iterations left
        const double radius = maxDiameter * (104 * density - i) / (104 * density);
        // x and y are based upon cos and sin of the current radius
        const double x = radius * cos(angle);
        const double y = radius * sin(angle);

        if(i == points)
        {
            trainingIn.row(trIdx) << x, y;
            trainingOut.row(trIdx) << 1.0;
            testingIn.row(trIdx) << -x, -y;
            testingOut.row(teIdx) << -1.0;
        }
        else if(i % 2 == 0)
        {
            OPENANN_CHECK_WITHIN(trIdx, 0, points);
            trainingIn.row(trIdx) << x, y;
            trainingOut.row(trIdx) << 1.0;
            trainingIn.row(trIdx + 1) << -x, -y;
            trainingOut.row(trIdx + 1) << -1.0;
            trIdx += 2;
        }
        else
        {
            OPENANN_CHECK_WITHIN(teIdx, 0, points);
            testingIn.row(teIdx) << x, y;
            testingOut.row(teIdx) << 1.0;
            testingIn.row(teIdx + 1) << -x, -y;
            testingOut.row(teIdx + 1) << -1.0;
            teIdx += 2;
        }
    }

    OpenANN::scaleData(testingIn, 0.0, 1.0);
    OpenANN::scaleData(trainingIn, 0.0, 1.0);

    inputRange_ = {0.0, 1.0};
    outputRange_ = {-1.0, 1.0};
    outputLabelCount_ = 2;

    createInternalDataSet();
    return true;
}

bool UCWDataSet::generateCSV(QString filePath)
{
    trainingIn.resize(200, 2);
    trainingOut.resize(200, 1);
    testingIn.resize(0, 2);
    testingOut.resize(0, 1);

    try
    {
        CSVReader<3> reader(filePath.toLocal8Bit().data());
        reader.set_header("x", "y", "z");

        // read in each line
        double x, y, z;
        int rowIndex = 0;
        while (reader.read_row(x, y, z))
        {
            trainingIn.row(rowIndex) << x, y;
            trainingOut.row(rowIndex) << z;
            rowIndex++;
        }

        OpenANN::scaleData(trainingIn, -1.5, 1.5);
        inputRange_ = {-1.5, 1.5};
        outputRange_ = {-1.0, 1.0};
        outputLabelCount_ = 2;
    }
    catch (csv::error::can_not_open_file e)
    {
        qDebug() << e.what();
        return generateNone();
    }

    createInternalDataSet();
    return true;
}
