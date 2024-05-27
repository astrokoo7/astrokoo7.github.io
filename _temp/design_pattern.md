#include <string>
#include <string>
#include <iostream>

class Model {
public:
    void setData(const std::string& data);
    std::string getData() const;

private:
    std::string data_;
};

void Model::setData(const std::string& data) {
    data_ = data;
}

std::string Model::getData() const {
    return data_;
}

class View {
public:
    void displayData(const std::string& data);
    std::string getUserInput();
};

void View::displayData(const std::string& data) {
    std::cout << "Data: " << data << std::endl;
}

std::string View::getUserInput() {
    std::string input;
    std::cout << "Enter data: ";
    std::getline(std::cin, input);
    return input;
}

class Controller {
public:
    Controller(Model& model, View& view);
    void handleUserInput();

private:
    Model& model_;
    View& view_;
};

Controller::Controller(Model& model, View& view)
    : model_(model), view_(view) {}

void Controller::handleUserInput() {
    std::string input = view_.getUserInput();
    model_.setData(input);
    view_.displayData(model_.getData());
}

int main() {
    Model model;
    View view;
    Controller controller(model, view);

    while (true) {
        controller.handleUserInput();
    }

    return 0;
}