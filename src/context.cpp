// Copyright (c) 2018 The OPCX developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "context.h"
#include "timedata.h"
#include "bootstrapmodel.h"
#include "autoupdatemodel.h"

#include <memory>
#include <stdexcept>

using namespace std;

static unique_ptr<CContext> context_;

void CreateContext()
{
    if (context_)
        throw runtime_error("context has already been initialized, revise your code");
    else
        context_.reset(new CContext);
}

void ReleaseContext()
{
    context_.reset();
}

CContext& GetContext()
{
    if (!context_)
        throw runtime_error("context is not initialized");
    else
        return *context_;
}

CContext::CContext()
{
    nStartupTime_ = GetAdjustedTime();
}

CContext::~CContext() {}

int64_t CContext::GetStartupTime() const
{
    return nStartupTime_;
}

void CContext::SetStartupTime(int64_t nTime)
{
    nStartupTime_ = nTime;
}

BootstrapModelPtr CContext::GetBootstrapModel()
{
    if (!bootstrapModel_)
        bootstrapModel_.reset(new BootstrapModel);

    return bootstrapModel_;
}

AutoUpdateModelPtr CContext::GetAutoUpdateModel()
{
    if (!autoupdateModel_)
        autoupdateModel_.reset(new AutoUpdateModel);

    return autoupdateModel_;
}
