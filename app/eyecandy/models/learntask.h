#ifndef LEARNTASK_H
#define LEARNTASK_H

#include "utils/utils.h"
#include "models/learnparam.h"

class ClossNet;
class UCWDataSet;
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

    const LearnParam &parameters() const;

protected:
    UCWDataSet *createDataSourceFromParam(const LearnParam &param);

private:
    unique_ptr<ClossNet> network_;
    unique_ptr<StoppingCriteria> stopCriteria_;
    unique_ptr<UCWDataSet> data_;

    LearnParam param_;
};

#endif // LEARNTASK_H
