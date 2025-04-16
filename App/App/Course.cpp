#include "Course.hpp"

Course::Course(const string& id, const string& title, const string& description)
	: id_(id),title_(title),description_(description),status_(Status::Draft){ }

const string& Course::getId() const {
	return id_;
}

const string& Course::getTitle() const {
    return title_;
}

const string& Course::getDescription() const {
    return description_;
}

Course::Status Course::getStatus() const {
    return status_;
}

const std::vector<string>& Course::getMaterials() const {
    return materials_;
}