#include "rpccontroller.h"


MprpcController::MprpcController() {
    m_failed = false;
    m_errmsg = "";
}

void MprpcController::Reset()
{
    m_failed = false;
    m_errmsg = "";
}

bool MprpcController::Failed() const
{
    return m_failed;
}

// If Failed() is true, returns a human-readable description of the error.
std::string MprpcController::ErrorText() const
{
    return m_errmsg;
}

void MprpcController::SetFailed(const std::string& reason)
{
    m_failed = true;
    m_errmsg = reason;
}

// ///////
bool MprpcController::IsCanceled() const
{
    return false;
}
void MprpcController::StartCancel() {}
void MprpcController::NotifyOnCancel(google::protobuf::Closure* callback) {}