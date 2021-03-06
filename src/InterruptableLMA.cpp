#define OPENANN_LOG_NAMESPACE "LMA"

#include "InterruptableLMA.h"
#include <OpenANN/optimization/Optimizable.h>
#include <OpenANN/optimization/StoppingInterrupt.h>
#include <OpenANN/util/AssertionMacros.h>
#include <OpenANN/util/Random.h>
#include <OpenANN/util/OpenANNException.h>
#include <OpenANN/io/Logger.h>
#include <limits>

InterruptableLMA::InterruptableLMA()
    : opt(0), iteration(-1), n(-1)
{
}

InterruptableLMA::~InterruptableLMA()
{
}

int InterruptableLMA::currentIteration() const
{
    return iteration;
}

double InterruptableLMA::currentError() const
{
    return errorValues.mean();
}

void InterruptableLMA::setOptimizable(Optimizable& opt)
{
    this->opt = &opt;
}

void InterruptableLMA::setStopCriteria(const StoppingCriteria& stop)
{
    this->stop = stop;
}

void InterruptableLMA::optimize()
{
    OPENANN_CHECK(opt);
    OpenANN::StoppingInterrupt interrupt;
    while(step())
    {
        OPENANN_DEBUG << "Iteration #" << iteration
                      << ", training error = "
                      << OpenANN::FloatingPointFormatter(errorValues.mean(), 4);
        if(interrupt.isSignaled())
        {
            reset();
            break;
        }
    }
}

bool InterruptableLMA::step()
{
    OPENANN_CHECK(opt);
    if(iteration < 0)
        initialize();
    OPENANN_CHECK(n > 0);

    try
    {
        while(alglib_impl::minlmiteration(state.c_ptr(), &envState))
        {
            if(state.needfi)
            {
                for(unsigned i = 0; i < n; i++)
                    parameters(i) = state.x[i];
                opt->setParameters(parameters);
                for(unsigned i = 0; i < opt->examples(); i++)
                    state.fi[i] = opt->error(i);
                if(iteration != state.c_ptr()->repiterationscount)
                {
                    iteration = state.c_ptr()->repiterationscount;
                    opt->finishedIteration();
                    return true;
                }
                continue;
            }
            if(state.needfij)
            {
                for(unsigned i = 0; i < n; i++)
                    parameters(i) = state.x[i];
                opt->setParameters(parameters);
                for(int ex = 0; ex < opt->examples(); ex++)
                {
                    opt->errorGradient(ex, errorValues(ex), gradient);
                    state.fi[ex] = errorValues(ex);
                    for(unsigned d = 0; d < opt->dimension(); d++)
                        state.j[ex][d] = gradient(d);
                }
                if(iteration != state.c_ptr()->repiterationscount)
                {
                    iteration = state.c_ptr()->repiterationscount;
                    opt->finishedIteration();
                    return true;
                }
                continue;
            }
            if(state.xupdated)
                continue;
            throw alglib::ap_error("ALGLIB: error in 'minlmoptimize' (some "
                                   "derivatives were not provided?)");
        }
        alglib_impl::ae_state_clear(&envState);
    }
    catch(alglib_impl::ae_error_type)
    {
        throw OpenANN::OpenANNException(envState.error_msg);
    }

    reset();
    return false;
}

Eigen::VectorXd InterruptableLMA::result()
{
    OPENANN_CHECK(opt);
    if (iteration != -1)
        reset();
    opt->setParameters(optimum);
    return optimum;
}

std::string InterruptableLMA::name()
{
    std::stringstream stream;
    stream << "Levenberg-Marquardt Algorithm";
    return stream.str();
}

void InterruptableLMA::initialize()
{
    n = opt->dimension();

    // temporary vectors to avoid allocations
    parameters.resize(n);
    errorValues.resize(opt->examples());
    gradient.resize(n);

    xIn.setcontent(n, opt->currentParameters().data());

    // Initialize optimizer
    alglib::minlmcreatevj(opt->examples(), xIn, state);

    // Set convergence criteria
    double minimalSearchSpaceStep = stop.minimalSearchSpaceStep !=
                                    StoppingCriteria::defaultValue.minimalSearchSpaceStep ?
                                    stop.minimalSearchSpaceStep : 0.0;
    double minimalValueDifferences = stop.minimalValueDifferences !=
                                     StoppingCriteria::defaultValue.minimalValueDifferences ?
                                     stop.minimalValueDifferences : 0.0;
    int maximalIterations = stop.maximalIterations !=
                            StoppingCriteria::defaultValue.maximalIterations ?
                            stop.maximalIterations : 0;
    alglib::minlmsetcond(state, minimalSearchSpaceStep, minimalValueDifferences,
                         0.0, maximalIterations);

    // Initialize optimizer state
    alglib_impl::ae_state_init(&envState);
}

void InterruptableLMA::reset()
{
    // Read out results
    alglib::minlmreport report;
    alglib::minlmresults(state, xIn, report);

    // Set optimum
    optimum.resize(n);
    for(unsigned i = 0; i < n; i++)
        optimum(i) = xIn[i];
    opt->setParameters(optimum);

    // Log result
    OPENANN_DEBUG << "Terminated:";
    OPENANN_DEBUG << report.iterationscount << " iterations";
    OPENANN_DEBUG << report.nfunc << " function evaluations";
    OPENANN_DEBUG << report.njac << " Jacobi evaluations";
    OPENANN_DEBUG << "Error = " << opt->error();
    OPENANN_DEBUG << "Reason:";
    switch(report.terminationtype)
    {
    case 1:
        OPENANN_DEBUG << "Relative function improvement is below threshold.";
        break;
    case 2:
        OPENANN_DEBUG << "Relative step is below threshold.";
        break;
    case 4:
        OPENANN_DEBUG << "Gradient is below threshold.";
        break;
    case 5:
        OPENANN_DEBUG << "MaxIts steps was taken";
        break;
    case 7:
        OPENANN_DEBUG << "Stopping conditions are too stringent, "
                      << "further improvement is impossible.";
        break;
    default:
        OPENANN_DEBUG << "Unknown.";
    }

    iteration = -1;
}
