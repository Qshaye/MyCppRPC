#pragma once
#include <google/protobuf/service.h>
#include <string>

class MprpcController : public google::protobuf::RpcController
{
public:
    MprpcController();

    virtual void Reset();

    virtual bool Failed() const;

    // If Failed() is true, returns a human-readable description of the error.
    virtual std::string ErrorText() const;

    virtual void SetFailed(const std::string& reason);  

    // ///////
    virtual void StartCancel();
    virtual bool IsCanceled() const;
    virtual void NotifyOnCancel(google::protobuf::Closure* callback);
private:
    // rpc调用请求是否失败
    bool m_failed;
    // 错误信息
    std::string m_errmsg;
};