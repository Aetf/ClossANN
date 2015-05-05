#include "ucwdataset.h"
#include "utils.h"
#include <OpenANN/Learner.h>
#include <OpenANN/Preprocessing.h>
#include <OpenANN/util/AssertionMacros.h>
#include <OpenANN/io/DirectStorageDataSet.h>
#include <QVariantMap>

using namespace OpenANN;
using Eigen::MatrixXd;
using Eigen::VectorXd;

UCWDataSet::UCWDataSet(DataSource source, int density, double maxDiameter)
    : DataSet()
    , inTrainingMode_(true)
    , trainingIn()
    , trainingOut()
    , testingIn()
    , testingOut()
    , trainingData(nullptr)
    , testingData(nullptr)
{
    switch (source) {
    case TwoSpirals:
        generateTwoSpirals(density, maxDiameter);
    case Moon:
        generateMoon(density, maxDiameter);
    }
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
    QVariantList prediction;
    for(int x = 0; x < 30; x++)
    {
        for(int y = 0; y < 30; y++)
        {
            double xx = x / 30.0;
            double yy = y / 30.0;
            Eigen::VectorXd in(2);
            in << xx, yy;
            Eigen::VectorXd out = learner(in);
            QVariantList point;
            point << xx << out(0, 0) << yy;
            prediction << QVariant(point);
        }
    }
    emit predictionUpdated(prediction);
}

bool UCWDataSet::inTrainingMode() const
{
    return inTrainingMode_;
}

void UCWDataSet::inTrainingMode(bool val)
{
    inTrainingMode_ = val;
}

void UCWDataSet::generateTwoSpirals(int density, double maxDiameter)
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
}

void UCWDataSet::generateMoon(int density, double maxDiameter)
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
}
