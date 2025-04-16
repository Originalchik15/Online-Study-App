#pragma once 
#include <string>
#include <vector>
#include <stdexcept>

using std::string;

class Course {
public:
	enum class Status {Draft, UndreReview, Published, Archived, Deleted};

	Course(const string& id, const string& title, const string& description);

	const string& getId() const;
	const string& getTitle() const;
	const string& getDescription() const;
	Status getStatus() const;
	const std::vector<string>& getMaterials() const;

	void setTitle(const string& newTitle);
	void setDescription(const string& newDescription);
	void addMaterial(const string& materialPath);

	void submitForReviev();
	void approve();
	void reject();
	void archive();
	void restore();
	void remove();

private:
	string id_;
	string title_;
	string description_;
	std::vector<string> materials_;
	Status status_;
};