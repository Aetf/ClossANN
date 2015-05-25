#ifndef LEARNTASK_H
#define LEARNTASK_H

#include "utils.h"

class ClossNet;
class UCWDataSet;
class LearnParam;
namespace OpenANN {
class StoppingCriteria;
}

using OpenANN::StoppingCriteria;

class LearnTask
{
public:
    LearnTask(const LearnParam &param);

    ClossNet &network();
    StoppingCriteria &stopCriteria();
    UCWDataSet &data();

protected:
    UCWDataSet *createDataSourceFromParam(const LearnParam &param);

private:
    unique_ptr<ClossNet> network_;
    unique_ptr<StoppingCriteria> stopCriteria_;
    unique_ptr<UCWDataSet> data_;
};

#endif // LEARNTASK_H
