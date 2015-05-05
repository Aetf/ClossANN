#ifndef LEARNTASK_H
#define LEARNTASK_H

#include "utils.h"

class ClossNet;
class UCWDataSet;
namespace OpenANN {
class StoppingCriteria;
}

using OpenANN::StoppingCriteria;

class LearnTask
{
public:
    LearnTask();

    ClossNet &network();
    StoppingCriteria &stopCriteria();
    UCWDataSet &data();

private:
    unique_ptr<ClossNet> network_;
    unique_ptr<StoppingCriteria> stopCriteria_;
    unique_ptr<UCWDataSet> data_;
};

#endif // LEARNTASK_H
