#include <algorithm>
#include <functional>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <vector>

/******************************************************************************
 *              set            unordered_set   map            unordered_map   *
 * insert       O(log n)       O(1)            O(log n)       O(1)            *
 * lookup       O(log n)       O(1)            O(log n)       O(1)            *
 * sorted       O(n)           O(n log n)      O(n)           O(n log n)      *
 ******************************************************************************/

/*
 * The two main options for storing student grades against their names is
 * map or unordered_map. unordered_map has the advantage that inserts/lookups
 * are constant time, while for map they are O(log n). However, returning all
 * data sorted by key and printing it is only linear time for map,
 * but linearithmic for unordered_map. For sorting according to an arbitrary
 * order it is O(n log n) for both.
 *
 * I have therefore chosen to store the data in an unordered_map, for the
 * faster constant time inserts, lookups, and the fact that printing sorted
 * data will usually take O(n log n) time in most cases anyway.
 *
 * Another option I considered was using a map and a vector, where the
 * vector would hold the grades and the map was from the student's name to
 * the index into the vector. However, this would require a reverse lookup
 * once we had sorted data; that is, if we sorted by maths grades, for example,
 * and index 7 had the highest, we need to search in the map for who index 7
 * was. Alternatively we could also store the name in the vector as well,
 * but this would provide no performance advantages over the solution I chose,
 * would be more complicated, and require extra memory.
 *
 * I also considered pre-computing the total grade for each student and
 * storing it in the map, but chose not to. Computing it on-the-fly
 * is very fast but storing it costs an extra O(n) space.
 */

typedef int Grade;
typedef std::array<Grade, 4> Grades;
typedef std::pair<std::string const, Grades> Student;
typedef std::unordered_map<std::string, Grades> Students;
typedef std::function<bool(Student const &, Student const &)> Order;
typedef std::function<bool(Student const &)> Predicate;

std::size_t
subject_map(std::string const &subject)
{
	if (subject == "Biology") {
		return 0;
	} else if (subject == "Chemistry") {
		return 1;
	} else if (subject == "Mathematics") {
		return 2;
	} else if (subject == "Physics") {
		return 3;
	}
	return -1;
}

auto
find_student(Students &students, std::string &student_name)
{
	auto iterator = students.find(student_name);
	if (iterator == students.end()) {
		/* student_name not found in map */
		auto pair = std::make_pair(student_name, Grades{0, 0, 0, 0});
		iterator = students.insert(pair).first;
	}
	return iterator;
}

void
build_table(Students &students, char const filename[])
{
	std::string student_name;
	std::string subject;
	Grade grade;
	std::ifstream file;
	file.open(filename);
	if (!file.good()) {
		throw std::exception();
	}
	for (;;) {
		file >> student_name;
		file >> subject;
		file >> grade;
		if (file.eof())
			break;
		auto iterator = find_student(students, student_name);
		iterator->second[subject_map(subject)] = grade;
	}
	/* File closed automatically when variable `file' goes out of scope. */
}

int
total_grade(Grades const &grades)
{
	int total = 0;
	total += grades[0];
	total += grades[1];
	total += grades[2];
	total += grades[3];
	return total;
}

void
print_data(std::vector<Student const *> order, bool total = false)
{
	for (auto iter : order) {
		std::cout << iter->first;
		std::cout << " " << iter->second[0];
		std::cout << " " << iter->second[1];
		std::cout << " " << iter->second[2];
		std::cout << " " << iter->second[3];
		if (total) {
			std::cout << " " << total_grade(iter->second);
		}
		std::cout << std::endl;
	}
}

auto
sorted_data(Students const &students, Order order)
{
	std::vector<Student const *> ptrs;
	for (auto iter = students.begin(); iter != students.end(); ++iter) {
		ptrs.push_back(&*iter);
	}
	std::sort(ptrs.begin(), ptrs.end(),
	          [&] (auto a, auto b) {
	              return order(*a, *b); });
	return ptrs;
}

void
print_students(Students const &students)
{
	auto order = sorted_data(students,
	    [&] (auto a, auto b) {
	        return a.first < b.first; });
	print_data(order);
}

void
print_subject(Students const &students, int subject)
{
	auto order = sorted_data(students,
	    [&] (auto a, auto b) {
	        return a.second[subject] > b.second[subject]; });
	print_data(order);
}

void
print_total(Students const &students)
{
	auto order = sorted_data(students,
	    [&] (auto a, auto b) {
	        return total_grade(a.second) > total_grade(b.second); });
	print_data(order, true);
}

void
print_filter(Students const &students, int subject, int grade)
{
	Predicate pred;
	if (subject == -1) {
		pred = [&] (auto a) {
		    return total_grade(a.second) > grade; };
	} else {
		pred = [&] (auto a) {
		    return a.second[subject] > grade; };
	}
	std::cout << std::count_if(students.begin(), students.end(), pred);
	std::cout << std::endl;
}

int
main(int argc, char *argv[])
{
	std::unordered_map<std::string, Grades> students;
	std::string input, command;
	if (argc != 2) {
		std::cerr << "Command line usage: task1 filename" << std::endl;
		return -1;
	}
	try {
		build_table(students, argv[1]);
	} catch (std::exception &e) {
		std::cerr << "Could not open file." << std::endl;
		return -1;
	}
	for (;;) {
		std::cout << "> ";
		std::getline(std::cin, input);
		auto sstream = std::istringstream(input);
		sstream >> command;
		if (command == "students") {
			print_students(students);
		} else if (command == "biology") {
			print_subject(students, 0);
		} else if (command == "chemistry") {
			print_subject(students, 1);
		} else if (command == "maths") {
			print_subject(students, 2);
		} else if (command == "physics") {
			print_subject(students, 3);
		} else if (command == "total") {
			print_total(students);
		} else if (command == "more-than") {
			int grade;
			std::string subject;
			sstream >> grade;
			sstream >> subject;
			if (subject == "biology") {
				print_filter(students, 0, grade);
			} else if (subject == "chemistry") {
				print_filter(students, 1, grade);
			} else if (subject == "maths") {
				print_filter(students, 2, grade);
			} else if (subject == "physics") {
				print_filter(students, 3, grade);
			} else if (subject == "total") {
				print_filter(students, -1, grade);
			} else {
				std::cerr << "command not understood" << std::endl;
			}
		} else if (command == "exit") {
			break;
		}else {
			std::cerr << "command not understood" << std::endl;
		}
	}
	return 0;
}
