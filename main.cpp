#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include "httplib.h"

struct Student {
    std::string name;
    std::string password;
    std::string absence;
    std::string exam1;
    std::string exam2;
    std::string exam3;
    std::string exam4;
};

// Helper to parse CSV lines
std::vector<std::string> parseCSVLine(const std::string& line) {
    std::vector<std::string> fields;
    std::stringstream ss(line);
    std::string field;
    while (std::getline(ss, field, ',')) {
        fields.push_back(field);
    }
    return fields;
}

int main() {
    // 1. Load CSV data into memory at startup
    std::ifstream file("students.csv");
    if (!file.is_open()) {
        std::cerr << "Error: Could not open students.csv.\n";
        return 1;
    }

    std::vector<Student> students;
    std::string line;
    std::getline(file, line); // Skip header

    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::vector<std::string> fields = parseCSVLine(line);
        Student s;
        s.name = (fields.size() > 0) ? fields[0] : "";
        s.password = (fields.size() > 1) ? fields[1] : "";
        s.absence = (fields.size() > 2) ? fields[2] : "";
        s.exam1 = (fields.size() > 3) ? fields[3] : "";
        s.exam2 = (fields.size() > 4) ? fields[4] : "";
        s.exam3 = (fields.size() > 5) ? fields[5] : "";
        s.exam4 = (fields.size() > 6) ? fields[6] : "";
        students.push_back(s);
    }
    file.close();

    httplib::Server svr;

    // 2. Serve the HTML Web Page
    svr.Get("/", [](const httplib::Request&, httplib::Response& res) {
        std::string html =
            "<html><body style='font-family:Arial; text-align:center; margin-top:50px;'>"
            "<h2>Student Record Lookup</h2>"
            "<form action='/lookup' method='GET'>"
            "<input type='password' name='pass' placeholder='Enter Password' style='padding:10px; font-size:16px;' required>"
            "<br><br>"
            "<button type='submit' style='padding:10px 20px; font-size:16px;'>View Grades</button>"
            "</form></body></html>";
        res.set_content(html, "text/html");
        });

    // 3. Handle the Password Lookup and Display Results
    svr.Get("/lookup", [&students](const httplib::Request& req, httplib::Response& res) {
        std::string password = req.get_param_value("pass");
        std::string resultHtml = "<h2>Results</h2>";

        bool found = false;
        for (const auto& s : students) {
            if (s.password == password) {
                found = true;
                resultHtml += "<p><b>Name:</b> " + (s.name.empty() ? "[Empty]" : s.name) + "</p>";
                resultHtml += "<p><b>Absence:</b> " + (s.absence.empty() ? "[Empty]" : s.absence) + "</p>";
                resultHtml += "<p><b>Exam 1:</b> " + (s.exam1.empty() ? "[Empty]" : s.exam1) + "</p>";
                resultHtml += "<p><b>Exam 2:</b> " + (s.exam2.empty() ? "[Empty]" : s.exam2) + "</p>";
                resultHtml += "<p><b>Exam 3:</b> " + (s.exam3.empty() ? "[Empty]" : s.exam3) + "</p>";
                resultHtml += "<p><b>Exam 4:</b> " + (s.exam4.empty() ? "[Empty]" : s.exam4) + "</p>";
                break;
            }
        }

        if (!found) {
            resultHtml += "<p style='color:red;'>Incorrect password or student not found.</p>";
        }

        resultHtml += "<br><a href='/'>Back</a>";
        res.set_content(resultHtml, "text/html");
        });

    std::cout << "Starting C++ Web Server on http://localhost:8080 ...\n";
    svr.listen("0.0.0.0", 8080);
    return 0;
}