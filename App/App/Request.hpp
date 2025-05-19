#pragma once
#include <string>
#include <chrono>

class Request {
public:
    enum class Type { Create, Edit, Delete };
    enum class State { Pending, Approved, Rejected };

    Request(int id,
        const std::string& courseId,
        Type type,
        const std::string& newTitle,
        const std::string& newDescription,
        const std::string& requestedBy,
        State status,
        std::chrono::system_clock::time_point createdAt);

    // геттеры...
    int  getId()              const;
    const std::string& getCourseId()     const;
    Type getType()            const;
    const std::string& getNewTitle()     const;
    const std::string& getNewDescription() const;
    const std::string& getRequestedBy()  const;
    State getStatus()         const;
    std::chrono::system_clock::time_point getCreatedAt() const;

    void setStatus(State st);  // Approved или Rejected

private:
    int    id_;
    std::string courseId_;
    Type   type_;
    std::string newTitle_;
    std::string newDescription_;
    std::string requestedBy_;
    State  status_;
    std::chrono::system_clock::time_point createdAt_;
};
